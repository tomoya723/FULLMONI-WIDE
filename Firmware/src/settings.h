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
    MODE_NORMAL = 0,    /* 通常動作（emWin描画有効） */
    MODE_PARAM  = 1     /* パラメータ変更モード（emWin停止） */
} SYSTEM_MODE;

/* グローバル変数宣言 */
extern volatile SYSTEM_MODE g_system_mode;
extern volatile uint8_t g_param_mode_active;
/* g_uart_rx_trigger は USB CDC化により削除 */

// emwin display resource - Junction経由で切り替え
// ビルド前に tools/build_variants.ps1 で aw Junctionを設定すること
#include "../aw/Source/Generated/Resource.h"
#include "../aw/Source/Generated/ID_SCREEN_Telltale.h"
#include "../aw/Source/Generated/ID_SCREEN_01a.h"
#include "../aw/Source/Generated/ID_SCREEN_01b.h"
#include "../aw/Source/Generated/ID_SCREEN_01c.h"

extern void sch_10ms();

#endif /* SETTINGS_H_ */
