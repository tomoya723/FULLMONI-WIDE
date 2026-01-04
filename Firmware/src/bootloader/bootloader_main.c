/*
 * Minimal Bootloader Main (SCI9 Test)
 * RX72N FULLMONI-WIDE
 */

#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"

/* Smart Configurator API */
extern void R_Config_SCI9_Create(void);
extern void R_Config_SCI9_Start(void);
extern void R_Config_SCI9_Serial_Send(uint8_t * const tx_buf, uint16_t tx_num);

/* アプリケーション開始アドレス */
#define APP_START_ADDRESS   0xFFC20000UL

/* 関数プロトタイプ */
typedef void (*app_entry_t)(void);

/* 送信完了フラグ（割り込みハンドラでセット） */
volatile bool g_uart_tx_done = false;

void bootloader_main(void)
{
    volatile uint32_t counter = 0;
    uint8_t test_msg[] = "BOOT\r\n";

    /* クロック設定を読み取る（デバッガで確認するため） */
    volatile uint32_t sckcr = SYSTEM.SCKCR.LONG;
    volatile uint16_t sckcr3 = SYSTEM.SCKCR3.WORD;

    /* UART初期化（Smart Configurator） */
    R_Config_SCI9_Create();
    R_Config_SCI9_Start();

    /* テストメッセージ送信 */
    g_uart_tx_done = false;
    R_Config_SCI9_Serial_Send(test_msg, sizeof(test_msg) - 1);
    while (!g_uart_tx_done);  /* 送信完了待ち */

    /* 単純な無限ループ */
    while (1) {
        counter++;
        if (counter > 10000000) {
            counter = 0;
            uint8_t msg[] = "TEST\r\n";
            g_uart_tx_done = false;
            R_Config_SCI9_Serial_Send(msg, sizeof(msg) - 1);
            while (!g_uart_tx_done);
        }
    }
}
