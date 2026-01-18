/*
 * master_warning.h
 *
 * マスターワーニング機能 (Issue #50)
 * CANフィールドの閾値監視と警告発報
 *
 *  Created on: 2026/01/18
 *      Author: FULLMONI-WIDE Project
 */

#ifndef MASTER_WARNING_H_
#define MASTER_WARNING_H_

#include <stdbool.h>
#include <stdint.h>

/* 警告メッセージ最大長 */
#define MASTER_WARNING_MSG_MAX  16  /* "WATER HIGH" + NULL */

/* 警告タイプ */
typedef enum {
    WARN_TYPE_NONE = 0,
    WARN_TYPE_LOW,      /* 下限値を下回った */
    WARN_TYPE_HIGH      /* 上限値を超えた */
} MasterWarningType_t;

/**
 * @brief マスターワーニングの初期化
 */
void master_warning_init(void);

/**
 * @brief マスターワーニングの状態を更新（毎フレーム呼び出し）
 * @note CANデータ処理後に呼び出すこと
 */
void master_warning_check(void);

/**
 * @brief マスターワーニングの現在状態を取得
 * @return true: ワーニングアクティブ, false: 正常
 */
bool master_warning_is_active(void);

/**
 * @brief AppWizardのワーニング表示を更新
 * @note GUIタスクから呼び出すこと
 */
void master_warning_update_display(void);

/**
 * @brief 現在の警告メッセージを取得
 * @return 警告メッセージ文字列（例: "WATER HIGH"）、警告なしの場合は空文字列
 */
const char* master_warning_get_message(void);

/**
 * @brief 警告フィールドインデックスを取得
 * @return 警告中のフィールドインデックス、警告なしの場合は-1
 */
int8_t master_warning_get_field_index(void);

/**
 * @brief 警告タイプを取得
 * @return 警告タイプ (WARN_TYPE_NONE/LOW/HIGH)
 */
MasterWarningType_t master_warning_get_type(void);

/**
 * @brief 警告メッセージが変更されたかチェック
 * @return true: メッセージが変更された（表示更新が必要）
 */
bool master_warning_message_changed(void);

/**
 * @brief アクティブな警告数を取得
 * @return 警告数
 */
uint8_t master_warning_get_count(void);

#endif /* MASTER_WARNING_H_ */
