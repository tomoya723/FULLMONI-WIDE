/*
 * settings.h
 *
 *  Created on: 2025/04/20
 *      Author: tomoy
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>

/* ============================================================
 * システム動作モード定義
 * ============================================================ */
typedef enum {
    MODE_NORMAL = 0,    /* 通常動作 */
    MODE_PARAM  = 1     /* パラメータ変更モード */
} SYSTEM_MODE;

/* グローバル変数宣言 */
extern volatile SYSTEM_MODE g_system_mode;
extern volatile uint8_t g_param_mode_active;
/* g_uart_rx_trigger は USB CDC化により削除 */



extern void sch_10ms();

#endif /* SETTINGS_H_ */
