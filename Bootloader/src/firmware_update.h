/*
 * firmware_update.h
 * RX72N Bootloader - Firmware Update Functions
 */

#ifndef FIRMWARE_UPDATE_H
#define FIRMWARE_UPDATE_H

#include <stdint.h>
#include <stdbool.h>
#include "r_flash_rx_if.h"

/* 定数定義 */
#define APP_START_ADDR          0xFFC20000  /* アプリケーション開始アドレス */
#define APP_MAX_SIZE            0x3E0000    /* アプリケーション最大サイズ (約3.8MB) */
#define FLASH_BLOCK_SIZE        0x8000      /* フラッシュブロックサイズ (32KB) */

/* MOTファイル受信バッファ */
#define MOT_LINE_MAX_LENGTH     256
#define MOT_BUFFER_SIZE         (128 * 1024)  /* 128KB受信バッファ */

/* 戻り値 */
typedef enum {
    FW_UPDATE_OK = 0,
    FW_UPDATE_ERROR_TIMEOUT,
    FW_UPDATE_ERROR_FORMAT,
    FW_UPDATE_ERROR_CRC,
    FW_UPDATE_ERROR_FLASH,
    FW_UPDATE_ERROR_SIZE
} fw_update_result_t;

/* 関数プロトタイプ */
uint32_t crc32_calculate(const uint8_t *data, uint32_t length);
fw_update_result_t firmware_update_process(void);
bool mot_receive_line(char *buffer, uint32_t timeout_ms);
bool flash_erase_application_area(void);
bool flash_write_data(uint32_t address, const uint8_t *data, uint32_t length);

#endif /* FIRMWARE_UPDATE_H */
