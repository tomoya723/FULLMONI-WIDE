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
 * 内部変数
 * ============================================================ */
static bool s_warning_active = false;       /* 現在のワーニング状態 */
static bool s_warning_prev = false;         /* 前回のワーニング状態（立ち上がり検出用）*/
static int8_t s_warning_field_idx = -1;     /* 警告中のフィールドインデックス */
static MasterWarningType_t s_warning_type = WARN_TYPE_NONE;  /* 警告タイプ */
static char s_warning_message[MASTER_WARNING_MSG_MAX] = "";  /* 警告メッセージ */

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
        /* A/F値は10倍して整数化 (例: 14.7 -> 147) */
        return (int32_t)(g_CALC_data.af * 10.0f);
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
        /* バッテリー電圧は10倍して整数化 (例: 14.2V -> 142) */
        return (int32_t)(g_CALC_data.num6 * 10.0f);
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
    bool found_warning = false;
    
    /* CAN設定が有効か確認（バージョンが正しいか）*/
    if (g_can_config.version != CAN_CONFIG_VERSION) {
        /* 未初期化または不正なデータ - 警告チェックをスキップ */
        s_warning_active = false;
        return;
    }
    
    /* 現在警告中のフィールドがある場合、まずそれをチェック（ヒステリシス適用）*/
    if (s_warning_active && s_warning_field_idx >= 0) {
        field = &g_can_config.fields[s_warning_field_idx];
        current_value = get_field_current_value(field->target_var);
        
        /* 現在の警告タイプに応じた解除判定 */
        if (s_warning_type == WARN_TYPE_HIGH) {
            /* 上限警告中: 5%少ない値を下回ったら解除 */
            int32_t release_th = calc_release_threshold(field->warn_high, true);
            if (current_value > release_th) {
                found_warning = true;  /* まだ警告継続 */
            }
        } else if (s_warning_type == WARN_TYPE_LOW) {
            /* 下限警告中: 5%多い値を上回ったら解除 */
            int32_t release_th = calc_release_threshold(field->warn_low, false);
            if (current_value < release_th) {
                found_warning = true;  /* まだ警告継続 */
            }
        }
    }
    
    /* 警告継続中でなければ、全フィールドをスキャン（新規警告検出）*/
    if (!found_warning) {
        for (i = 0; i < CAN_FIELD_MAX; i++) {
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
        
        /* 上限チェック */
        if (field->warn_high != CAN_WARN_DISABLED) {
            if (current_value > field->warn_high) {
                found_warning = true;
                s_warning_field_idx = (int8_t)i;
                s_warning_type = WARN_TYPE_HIGH;
                build_warning_message(field, WARN_TYPE_HIGH);
                break;  /* 最初の警告で抜ける */
            }
        }
        
        /* 下限チェック */
        if (field->warn_low != CAN_WARN_DISABLED) {
            if (current_value < field->warn_low) {
                found_warning = true;
                s_warning_field_idx = (int8_t)i;
                s_warning_type = WARN_TYPE_LOW;
                build_warning_message(field, WARN_TYPE_LOW);
                break;  /* 最初の警告で抜ける */
            }
        }
        }
    }
    
    /* 警告状態を更新 */
    if (found_warning) {
        s_warning_active = true;
    } else {
        s_warning_active = false;
        s_warning_field_idx = -1;
        s_warning_type = WARN_TYPE_NONE;
        s_warning_message[0] = '\0';
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
    /* 立ち上がり検出（OFF→ON） */
    if (s_warning_active && !s_warning_prev) {
        /* 警告音を1回再生 */
        speaker_play_warning();
    }
    
    /* 前回状態を更新 */
    s_warning_prev = s_warning_active;
    
    /* Note: AppWizard変数の更新はdataregister.cから行う */
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
