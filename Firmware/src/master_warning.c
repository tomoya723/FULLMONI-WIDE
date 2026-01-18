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
#include <stdio.h>
#include "master_warning.h"
#include "param_storage.h"
#include "dataregister.h"
#include "speaker.h"

/* ============================================================
 * 内部定数
 * ============================================================ */
/* 起動後の警告抑制期間（100ms単位、30 = 3秒）*/
#define STARTUP_DELAY_COUNT   30

/* 下限警告を有効にする最小エンジン回転数 */
#define MIN_RPM_FOR_LOW_WARNING  500

/* 複数警告表示切り替え間隔（100ms単位、5 = 0.5秒）*/
#define WARNING_ROTATE_INTERVAL  5

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

/* ============================================================
 * 内部関数
 * ============================================================ */

/**
 * @brief CANフィールドの現在値を取得
 * @param target_var ターゲット変数インデックス
 * @return 現在値（整数）
 */
static int32_t get_field_current_value(uint8_t target_var)
{
    switch (target_var) {
    case CAN_TARGET_REV:
        return (int32_t)g_CALC_data.rev;
    case CAN_TARGET_AF:
        /* A/F値は既に10倍値 (例: 14.7 -> 147) */
        return (int32_t)g_CALC_data.af;
    case CAN_TARGET_NUM1:
        return (int32_t)g_CALC_data.num1;
    case CAN_TARGET_NUM2:
        return (int32_t)g_CALC_data.num2;
    case CAN_TARGET_NUM3:
        return (int32_t)g_CALC_data.num3;
    case CAN_TARGET_NUM4:
        return (int32_t)g_CALC_data.num4;
    case CAN_TARGET_NUM5:
        return (int32_t)g_CALC_data.num5;
    case CAN_TARGET_NUM6:
        /* バッテリー電圧は既に10倍値 (例: 14.2V -> 142) */
        return (int32_t)g_CALC_data.num6;
    case CAN_TARGET_SPEED:
        return (int32_t)g_CALC_data.sp;
    default:
        return 0;
    }
}

/**
 * @brief 警告メッセージを生成
 * @param field 対象フィールド
 * @param type 警告タイプ
 */
static void build_warning_message(const CAN_Field_t *field, MasterWarningType_t type)
{
    const char *type_str = (type == WARN_TYPE_HIGH) ? "HIGH" : "LOW";
    
    if (field->name[0] != '\0') {
        snprintf(s_warning_message, MASTER_WARNING_MSG_MAX, "%s %s", field->name, type_str);
    } else {
        snprintf(s_warning_message, MASTER_WARNING_MSG_MAX, "WARN %s", type_str);
    }
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
    int32_t current_value;
    int32_t current_rpm;
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
    current_rpm = get_field_current_value(CAN_TARGET_REV);
    check_low_warning = (current_rpm >= MIN_RPM_FOR_LOW_WARNING);
    
    /* 全フィールドをスキャンして、警告中のものをすべて収集 */
    for (i = 0; i < CAN_FIELD_MAX && new_warning_count < MAX_ACTIVE_WARNINGS; i++) {
        field = &g_can_config.fields[i];
        
        /* 無効なフィールドはスキップ */
        if (field->channel == 0) {
            continue;
        }
        
        /* ワーニングが無効なフィールドはスキップ */
        if (!field->warn_enabled) {
            continue;
        }
        
        /* 現在値を取得 */
        current_value = get_field_current_value(field->target_var);
        
        /* 上限チェック（常に有効）*/
        if (field->warn_high != CAN_WARN_DISABLED) {
            if (current_value > field->warn_high) {
                new_warnings[new_warning_count] = (int8_t)i;
                new_warning_types[new_warning_count] = WARN_TYPE_HIGH;
                new_warning_count++;
                continue;  /* 1フィールドにつき1警告まで（上限優先）*/
            }
        }
        
        /* 下限チェック（エンジン回転中のみ）*/
        if (check_low_warning && field->warn_low != CAN_WARN_DISABLED) {
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
        int8_t disp_idx = s_active_warnings[s_display_index];
        MasterWarningType_t disp_type = s_active_warning_types[s_display_index];
        
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
    } else {
        s_warning_active = false;
        s_warning_field_idx = -1;
        s_warning_type = WARN_TYPE_NONE;
        s_warning_message[0] = '\0';
        s_active_warning_count = 0;
        s_display_index = 0;
        s_rotate_counter = 0;
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
