/*
 * master_warning.c
 *
 * マスターワーニング機能 (Issue #50)
 * CANフィールドの閾値監視と警告発報
 *
 *  Created on: 2026/01/18
 *      Author: FULLMONI-WIDE Project
 */

#include <string.h>
#include "master_warning.h"
#include "param_storage.h"
#include "dataregister.h"
#include "speaker.h"

/* ============================================================
 * 内部定数
 * ============================================================ */
/* 起動後の警告抑制期間（100ms単位、50 = 5秒）*/
#define STARTUP_DELAY_COUNT   50

/* 下限警告を有効にする最小エンジン回転数 */
#define MIN_RPM_FOR_LOW_WARNING  500

/* 複数警告表示切り替え間隔（100ms単位、5 = 0.5秒）*/
#define WARNING_ROTATE_INTERVAL  5

/* 警告ラッチ期間（100ms単位、100 = 10秒）*/
#define WARNING_LATCH_COUNT  100

/* 最大同時警告数 */
#define MAX_ACTIVE_WARNINGS  8

/* ============================================================
 * 内部変数
 * ============================================================ */
static bool s_warning_active = false;       /* 現在のワーニング状態 */
static bool s_warning_prev = false;         /* 前回のワーニング状態（立ち上がり検出用）*/
static int8_t s_warning_field_idx = -1;     /* 表示中のフィールドインデックス */
static MasterWarningType_t s_warning_type = WARN_TYPE_NONE;  /* 表示中の警告タイプ */
static char s_warning_message[MASTER_WARNING_MSG_MAX] = "";  /* 警告メッセージ */
static uint8_t s_startup_delay = STARTUP_DELAY_COUNT;  /* 起動後遅延カウンタ */

/* 複数警告管理用 */
static uint8_t s_active_warning_count = 0;  /* アクティブな警告数 */
static int8_t s_active_warnings[MAX_ACTIVE_WARNINGS];  /* 警告中のフィールドインデックス配列 */
static MasterWarningType_t s_active_warning_types[MAX_ACTIVE_WARNINGS];  /* 各警告のタイプ */
static uint8_t s_display_index = 0;         /* 現在表示中の警告インデックス */
static uint8_t s_rotate_counter = 0;        /* 表示切り替えカウンタ */
static bool s_message_changed = false;      /* メッセージ変更フラグ */

/* ラッチ機能用 */
static uint8_t s_latch_counter = 0;         /* ラッチカウンタ（警告解除後の表示継続用）*/
static uint8_t s_latched_warning_count = 0; /* ラッチ中の警告数 */
static int8_t s_latched_warnings[MAX_ACTIVE_WARNINGS];  /* ラッチ中の警告インデックス */
static MasterWarningType_t s_latched_warning_types[MAX_ACTIVE_WARNINGS];  /* ラッチ中の警告タイプ */

/* ============================================================
 * 内部関数
 * ============================================================ */

/**
 * @brief 10のべき乗を計算（小数点シフト用）
 * @param n べき指数 (0-3)
 * @return 10^n
 */
static float pow10_table[] = { 1.0f, 10.0f, 100.0f, 1000.0f };

/**
 * @brief CANフィールドの内部値を取得
 * @param target_var ターゲット変数インデックス
 * @return 内部値（g_CALC_dataから取得）
 */
static float get_field_internal_value(uint8_t target_var)
{
    switch (target_var) {
    case CAN_TARGET_REV:   return g_CALC_data.rev;
    case CAN_TARGET_AF:    return g_CALC_data.af;
    case CAN_TARGET_NUM1:  return g_CALC_data.num1;
    case CAN_TARGET_NUM2:  return g_CALC_data.num2;
    case CAN_TARGET_NUM3:  return g_CALC_data.num3;
    case CAN_TARGET_NUM4:  return g_CALC_data.num4;
    case CAN_TARGET_NUM5:  return g_CALC_data.num5;
    case CAN_TARGET_NUM6:  return g_CALC_data.num6;
    case CAN_TARGET_SPEED: return g_CALC_data.sp;
    default:               return 0.0f;
    }
}

/**
 * @brief CANフィールドの現在値を取得（表示単位、閾値比較用）
 * @param field CANフィールド設定
 * @return 表示値（decimal_shift適用後の物理値）
 * @note 閾値は表示単位（物理単位）で指定する
 */
static float get_field_display_value(const CAN_Field_t *field)
{
    float internal = get_field_internal_value(field->target_var);
    
    /* decimal_shiftに応じて変換 (0:そのまま, 1:÷10, 2:÷100, 3:÷1000) */
    if (field->decimal_shift > 0 && field->decimal_shift < 4) {
        return internal / pow10_table[field->decimal_shift];
    }
    return internal;
}

/* target_varに対応するデフォルト名 */
static const char * const s_target_var_names[] = {
    "REV",    /* CAN_TARGET_REV = 0 */
    "A/F",    /* CAN_TARGET_AF = 1 */
    "WATER",  /* CAN_TARGET_NUM1 = 2 */
    "IAT",    /* CAN_TARGET_NUM2 = 3 */
    "OIL-T",  /* CAN_TARGET_NUM3 = 4 */
    "MAP",    /* CAN_TARGET_NUM4 = 5 */
    "OIL-P",  /* CAN_TARGET_NUM5 = 6 */
    "BATT",   /* CAN_TARGET_NUM6 = 7 */
    "SPEED",  /* CAN_TARGET_SPEED = 8 */
};

/**
 * @brief 名前が有効かチェック（空でない、かつ英数字で始まる）
 * @param name 名前文字列
 * @return true: 有効, false: 無効
 */
static bool is_valid_name(const char *name)
{
    if (name[0] == '\0') return false;
    /* 英大文字(A-Z)、英小文字(a-z)で始まるかチェック */
    char c = name[0];
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

/**
 * @brief 警告メッセージを生成
 * @param field 対象フィールド
 * @param type 警告タイプ
 * @note snprintfは非リエントラントのため、手動で文字列を構築
 */
static void build_warning_message(const CAN_Field_t *field, MasterWarningType_t type)
{
    const char *type_str = (type == WARN_TYPE_HIGH) ? "HIGH" : "LOW";
    const char *name_src;
    int pos = 0;

    /* 名前を決定: field->nameが有効なら使用、なければtarget_varからデフォルト名 */
    if (is_valid_name(field->name)) {
        name_src = field->name;
    } else if (field->target_var < sizeof(s_target_var_names) / sizeof(s_target_var_names[0])) {
        name_src = s_target_var_names[field->target_var];
    } else {
        name_src = "WARN";
    }

    /* 名前をコピー（最大7文字）*/
    while (*name_src && pos < MASTER_WARNING_MSG_MAX - 6) {  /* " HIGH\0" 用に6バイト確保 */
        s_warning_message[pos++] = *name_src++;
    }

    /* スペース追加 */
    s_warning_message[pos++] = ' ';

    /* タイプ文字列をコピー */
    while (*type_str && pos < MASTER_WARNING_MSG_MAX - 1) {
        s_warning_message[pos++] = *type_str++;
    }

    /* NULL終端 */
    s_warning_message[pos] = '\0';
}

/* ============================================================
 * 公開関数
 * ============================================================ */

/**
 * @brief マスターワーニングの初期化
 */
void master_warning_init(void)
{
    s_warning_active = false;
    s_warning_prev = false;
    s_warning_field_idx = -1;
    s_warning_type = WARN_TYPE_NONE;
    s_warning_message[0] = '\0';
    s_startup_delay = STARTUP_DELAY_COUNT;  /* 起動後遅延をリセット */

    /* 複数警告管理の初期化 */
    s_active_warning_count = 0;
    s_display_index = 0;
    s_rotate_counter = 0;
    s_message_changed = false;
    memset(s_active_warnings, -1, sizeof(s_active_warnings));

    /* ラッチ機能の初期化 */
    s_latch_counter = 0;
    s_latched_warning_count = 0;
    memset(s_latched_warnings, -1, sizeof(s_latched_warnings));
}

/**
 * @brief ヒステリシス付き解除閾値を計算
 * @param threshold 元の閾値
 * @param is_high_limit true=上限, false=下限
 * @return 解除閾値
 * @note 上限: 5%少ない値で解除, 下限: 5%多い値で解除
 */
static int32_t calc_release_threshold(int16_t threshold, bool is_high_limit)
{
    int32_t th = (int32_t)threshold;
    if (is_high_limit) {
        /* 上限: 5%少ない値で解除 (例: 8000 → 7600) */
        return (th * 95) / 100;
    } else {
        /* 下限: 5%多い値で解除 (例: 60 → 63) */
        return (th * 105) / 100;
    }
}

/**
 * @brief マスターワーニングの状態を更新（毎フレーム呼び出し）
 */
void master_warning_check(void)
{
    uint8_t i;
    const CAN_Field_t *field;
    float current_value;
    float current_rpm;
    bool check_low_warning;
    uint8_t new_warning_count = 0;
    int8_t new_warnings[MAX_ACTIVE_WARNINGS];
    MasterWarningType_t new_warning_types[MAX_ACTIVE_WARNINGS];

    s_message_changed = false;  /* メッセージ変更フラグをリセット */

    /* 起動後遅延中は警告チェックをスキップ（CANデータ安定待ち）*/
    if (s_startup_delay > 0) {
        s_startup_delay--;
        s_warning_active = false;
        s_active_warning_count = 0;
        return;
    }

    /* CAN設定が有効か確認（バージョンが正しいか）*/
    if (g_can_config.version != CAN_CONFIG_VERSION) {
        /* 未初期化または不正なデータ - 警告チェックをスキップ */
        s_warning_active = false;
        s_active_warning_count = 0;
        return;
    }

    /* エンジン回転数を取得（下限警告の有効化判定用）*/
    current_rpm = get_field_internal_value(CAN_TARGET_REV);
    check_low_warning = (current_rpm >= MIN_RPM_FOR_LOW_WARNING);

    /* 全フィールドをスキャンして、警告中のものをすべて収集 */
    for (i = 0; i < CAN_FIELD_MAX && new_warning_count < MAX_ACTIVE_WARNINGS; i++) {
        field = &g_can_config.fields[i];

        /* 無効なフィールドはスキップ */
        if (field->channel == 0) {
            continue;
        }

        /* 現在値を取得（表示単位に変換）*/
        current_value = get_field_display_value(field);

        /* 上限チェック（warn_high_enabled が有効な場合のみ）*/
        if (field->warn_high_enabled && field->warn_high > CAN_WARN_DISABLED) {
            if (current_value > field->warn_high) {
                new_warnings[new_warning_count] = (int8_t)i;
                new_warning_types[new_warning_count] = WARN_TYPE_HIGH;
                new_warning_count++;
                continue;  /* 1フィールドにつき1警告まで（上限優先）*/
            }
        }

        /* 下限チェック（warn_low_enabled が有効 かつ エンジン回転中のみ）*/
        if (field->warn_low_enabled && check_low_warning && field->warn_low > CAN_WARN_DISABLED) {
            if (current_value < field->warn_low) {
                new_warnings[new_warning_count] = (int8_t)i;
                new_warning_types[new_warning_count] = WARN_TYPE_LOW;
                new_warning_count++;
            }
        }
    }

    /* 警告状態を更新 */
    if (new_warning_count > 0) {
        /* 警告数が変わった場合、表示インデックスをリセット */
        if (new_warning_count != s_active_warning_count) {
            s_display_index = 0;
            s_rotate_counter = 0;
        }

        /* 表示インデックスの範囲チェック（安全対策）*/
        if (s_display_index >= new_warning_count) {
            s_display_index = 0;
        }

        /* 警告リストをコピー */
        s_active_warning_count = new_warning_count;
        for (i = 0; i < new_warning_count; i++) {
            s_active_warnings[i] = new_warnings[i];
            s_active_warning_types[i] = new_warning_types[i];
        }

        /* 表示切り替えタイマー処理（複数警告時のみ）*/
        if (s_active_warning_count > 1) {
            s_rotate_counter++;
            if (s_rotate_counter >= WARNING_ROTATE_INTERVAL) {
                s_rotate_counter = 0;
                s_display_index++;
                if (s_display_index >= s_active_warning_count) {
                    s_display_index = 0;
                }
                s_message_changed = true;  /* 表示切り替え時にフラグを立てる */
            }
        }

        /* 現在表示する警告のメッセージを生成 */
        /* 範囲チェックを再度実施（競合状態対策）*/
        if (s_display_index >= s_active_warning_count) {
            s_display_index = 0;
        }
        int8_t disp_idx = s_active_warnings[s_display_index];
        MasterWarningType_t disp_type = s_active_warning_types[s_display_index];

        /* フィールドインデックスの範囲チェック */
        if (disp_idx < 0 || disp_idx >= CAN_FIELD_MAX) {
            /* 不正なインデックス - 警告をスキップ */
            s_warning_active = false;
            s_active_warning_count = 0;
            return;
        }

        /* 前回と違う警告を表示する場合、または最初の警告の場合のみメッセージを更新 */
        if (!s_warning_active) {
            /* 新規警告発生時のみメッセージ生成 */
            field = &g_can_config.fields[disp_idx];
            build_warning_message(field, disp_type);
            s_warning_field_idx = disp_idx;
            s_warning_type = disp_type;
            s_message_changed = true;
        } else if (s_message_changed) {
            /* 複数警告の切り替え時のみメッセージ更新 */
            field = &g_can_config.fields[disp_idx];
            build_warning_message(field, disp_type);
            s_warning_field_idx = disp_idx;
            s_warning_type = disp_type;
        }

        s_warning_active = true;

        /* ラッチ用に現在の警告をコピー */
        s_latched_warning_count = new_warning_count;
        for (i = 0; i < new_warning_count; i++) {
            s_latched_warnings[i] = new_warnings[i];
            s_latched_warning_types[i] = new_warning_types[i];
        }
        s_latch_counter = WARNING_LATCH_COUNT;  /* ラッチカウンタをリセット */
    } else {
        /* アクティブな警告がない場合 */
        if (s_latch_counter > 0) {
            /* ラッチ期間中 - 前回の警告を継続表示 */
            s_latch_counter--;

            /* ラッチ中の警告リストを使用 */
            s_active_warning_count = s_latched_warning_count;
            for (i = 0; i < s_latched_warning_count; i++) {
                s_active_warnings[i] = s_latched_warnings[i];
                s_active_warning_types[i] = s_latched_warning_types[i];
            }

            /* 表示切り替えタイマー処理（複数警告時のみ）*/
            if (s_active_warning_count > 1) {
                s_rotate_counter++;
                if (s_rotate_counter >= WARNING_ROTATE_INTERVAL) {
                    s_rotate_counter = 0;
                    s_display_index++;
                    if (s_display_index >= s_active_warning_count) {
                        s_display_index = 0;
                    }
                    /* ラッチ中の切り替えでもメッセージを更新 */
                    if (s_display_index < s_active_warning_count) {
                        int8_t disp_idx = s_active_warnings[s_display_index];
                        if (disp_idx >= 0 && disp_idx < CAN_FIELD_MAX) {
                            field = &g_can_config.fields[disp_idx];
                            build_warning_message(field, s_active_warning_types[s_display_index]);
                            s_warning_field_idx = disp_idx;
                            s_warning_type = s_active_warning_types[s_display_index];
                        }
                    }
                }
            }

            s_warning_active = true;  /* ラッチ中は警告状態を維持 */
        } else {
            /* ラッチ期間終了 - 警告を解除 */
            s_warning_active = false;
            s_warning_field_idx = -1;
            s_warning_type = WARN_TYPE_NONE;
            s_warning_message[0] = '\0';
            s_active_warning_count = 0;
            s_latched_warning_count = 0;
            s_display_index = 0;
            s_rotate_counter = 0;
        }
    }
}

/**
 * @brief マスターワーニングの現在状態を取得
 */
bool master_warning_is_active(void)
{
    return s_warning_active;
}

/**
 * @brief AppWizardのワーニング表示を更新
 */
void master_warning_update_display(void)
{
    /* 前回状態を更新（立ち上がり検出用に保持）*/
    s_warning_prev = s_warning_active;

    /* Note: 警告音再生はdataregister.cで直接呼び出し */
}

/**
 * @brief 現在の警告メッセージを取得
 */
const char* master_warning_get_message(void)
{
    return s_warning_message;
}

/**
 * @brief 警告フィールドインデックスを取得
 */
int8_t master_warning_get_field_index(void)
{
    return s_warning_field_idx;
}

/**
 * @brief 警告タイプを取得
 */
MasterWarningType_t master_warning_get_type(void)
{
    return s_warning_type;
}

/**
 * @brief 警告メッセージが変更されたかチェック
 */
bool master_warning_message_changed(void)
{
    return s_message_changed;
}

/**
 * @brief アクティブな警告数を取得
 */
uint8_t master_warning_get_count(void)
{
    return s_active_warning_count;
}
