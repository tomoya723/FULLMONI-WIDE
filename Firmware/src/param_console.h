/*
 * param_console.h
 *
 * UARTターミナルコンソール機能
 * パラメータ変更モードのコマンド処理
 *
 *  Created on: 2026/01/06
 *      Author: FULLMONI-WIDE Project
 */

#ifndef PARAM_CONSOLE_H_
#define PARAM_CONSOLE_H_

#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * 定数定義
 * ============================================================ */

/* UARTリングバッファサイズ */
#define UART_RX_BUFFER_SIZE     256
#define UART_TX_BUFFER_SIZE     256

/* コマンドラインバッファサイズ */
#define CMD_LINE_SIZE           128

/* ============================================================
 * 関数プロトタイプ
 * ============================================================ */

/**
 * @brief コンソール初期化
 */
void param_console_init(void);

/**
 * @brief パラメータ変更モード開始時の処理
 *        ウェルカムメッセージ表示
 */
void param_console_enter(void);

/**
 * @brief コンソール処理（メインループから呼び出し）
 * @return true: モード継続, false: 通常モードへ戻る
 */
bool param_console_process(void);

/**
 * @brief UART受信データをリングバッファに追加
 *        （受信割り込みから呼び出し）
 * @param data 受信データ
 */
void param_console_rx_push(uint8_t data);

/**
 * @brief リングバッファにデータがあるか確認
 * @return true: データあり
 */
bool param_console_rx_available(void);

/**
 * @brief リングバッファからデータを取得
 * @return 受信データ (データなしの場合は0)
 */
uint8_t param_console_rx_pop(void);

/**
 * @brief コンソールに文字列を出力
 * @param str 出力文字列
 */
void param_console_print(const char *str);

/**
 * @brief コンソールにフォーマット文字列を出力
 * @param fmt フォーマット文字列
 * @param ... 引数
 */
void param_console_printf(const char *fmt, ...);

#endif /* PARAM_CONSOLE_H_ */
