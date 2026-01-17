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

/* AppWizard includes */
#include "Application.h"
#include "Resource.h"

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
 * @brief マスターワーニングの状態を更新（毎フレーム呼び出し）
 */
void master_warning_check(void)
{
    uint8_t i;
    const CAN_Field_t *field;
    int32_t current_value;
    bool found_warning = false;
    
    /* 全フィールドをスキャン */
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
    
    /* AppWizard変数の更新は、ID_VAR_WARNINGが定義されていれば有効化 */
#ifdef ID_VAR_WARNING
    APPW_SetVarData(ID_VAR_WARNING, s_warning_active ? 1 : 0);
#endif
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
