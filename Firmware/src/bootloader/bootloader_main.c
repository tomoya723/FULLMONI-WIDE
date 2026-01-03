/*
 * Minimal Bootloader Main (SCI9 Test)
 * RX72N FULLMONI-WIDE
 */

#include <stdint.h>
#include <stdbool.h>
#include "uart_driver.h"

/* アプリケーション開始アドレス */
#define APP_START_ADDRESS   0xFFC20000UL

/* 関数プロトタイプ */
typedef void (*app_entry_t)(void);

void bootloader_main(void)
{
    /* UART初期化 */
    uart_init();
    
    /* 起動メッセージ */
    uart_puts("\r\n");
    uart_puts("========================================\r\n");
    uart_puts("  RX72N FULLMONI-WIDE Bootloader\r\n");
    uart_puts("  SCI9 UART Test (Smart Configurator)\r\n");
    uart_puts("========================================\r\n");
    uart_puts("\r\n");
    uart_puts("Booting application...\r\n\r\n");
    
    /* 短い待機 */
    for (volatile int i = 0; i < 1000000; i++);
    
    /* アプリケーションのリセットベクタを読み取る */
    volatile uint32_t *reset_vector = (volatile uint32_t *)0xFFFFFFFC;
    app_entry_t app_entry = (app_entry_t)(*reset_vector);
    
    /* アプリケーションへジャンプ */
    app_entry();
    
    /* ここには到達しない */
    while (1);
}
