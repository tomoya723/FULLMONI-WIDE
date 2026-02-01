/*
 * startup_image_write.c
 *
 * 起動画面書き込み機能
 * USB CDC経由でBMP画像をフラッシュメモリに書き込む
 *
 *  Created on: 2026/01/13
 *      Author: FULLMONI-WIDE Project
 *
 * 実装方針:
 * - RX72N 4MB Linearモードでは、フラッシュ書き込み中はコードフラッシュが
 *   読めなくなるため、書き込み関数をRAMから実行する必要がある
 * - Bootloaderと同様にRAM2領域に関数をコピーして実行
 * - 起動画像(acmtc)のアドレスは実行時に取得
 * - r_flash_rxは使用せず、完全独自実装（Bootloaderと同じ方式）
 */

#include "startup_image_write.h"
#include "param_console.h"
#include "usb_cdc.h"
#include "platform.h"
#include "r_smc_entry.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* LCD制御（バックライト） */
extern void LCD_FadeIN(void);
extern void LCD_FadeOUT(void);

/* SCI9 デバッグ出力用 (USB CDCと独立) */
extern MD_STATUS R_Config_SCI9_Serial_Send(uint8_t * const tx_buf, uint16_t tx_num);
extern void R_Config_SCI9_Start(void);
volatile bool s_sci9_tx_done = false;

/* USB送信カウンタ (usb_cdc.c) */
extern volatile uint32_t s_usb_write_success;
extern volatile uint32_t s_usb_write_errors;
extern volatile uint32_t s_usb_write_complete_count;

/* SCI9送信完了コールバック (Config_SCI9_user.cで設定) */
void sci9_debug_tx_callback(void)
{
    s_sci9_tx_done = true;
}

static void debug_print(const char *str)
{
    uint16_t len = strlen(str);
    s_sci9_tx_done = false;
    R_Config_SCI9_Serial_Send((uint8_t *)str, len);
    /* 送信完了を待つ */
    uint32_t timeout = 100000;
    while (!s_sci9_tx_done && timeout > 0) {
        timeout--;
    }
}

static char debug_buf[128];
static void debug_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(debug_buf, sizeof(debug_buf), fmt, args);
    va_end(args);
    debug_print(debug_buf);
}
/* AppWizardリソースヘッダ (acfmw_op0シンボル定義)
 * ../aw は Junction でaw001/aw002に切り替えられる */
#include "../aw/Source/Generated/Resource.h"

/* 起動画像シンボルのエイリアス（コード変更を最小化）*/
#define acmtc acfmw_op0

/* ============================================================
 * フラッシュ書き込み関連定義
 * ============================================================ */

/* FCUコマンド領域 */
#define FCU_CMD_AREA            ((volatile uint8_t *)0x007E0000UL)
#define FCU_CMD_AREA_WORD       ((volatile uint16_t *)0x007E0000UL)

/* FCUコマンド */
#define FCU_CMD_PROGRAM         0xE8
#define FCU_CMD_ERASE           0x20
#define FCU_CMD_FINAL           0xD0

/* タイムアウト (ループ回数) */
#define IMG_WAIT_MAX_ROM_WRITE  (0x000FFFFFUL)
#define IMG_WAIT_MAX_ERASE      (0x00FFFFFFUL)

/* RX72N コードフラッシュ情報 */
#define FLASH_CF_BLOCK_SIZE     (32 * 1024)  /* 32KB/ブロック */

/* RAM2領域 (フラッシュ操作関数を配置) */
#define RAM2_FUNC_AREA          (0x00800000UL)
#define RAM2_FUNC_SIZE          (4096)       /* 関数用4KB */

/* 受信バッファ */
static uint8_t rx_buffer[IMG_CHUNK_SIZE];
static uint8_t program_buffer[256] __attribute__((aligned(4)));

/*
 * プレフィックス保存用:
 * RAM2領域 (0x00800000〜) は8KB。
 * 前半4KBをフラッシュ操作関数に、後半4KBをプレフィックスデータに使用。
 */
#define RAM2_PREFIX_OFFSET   (4096)  /* 関数用に4KB確保 */
#define PREFIX_SAVE_MAX      (4096)  /* プレフィックス用に4KB */
#define RAM2_PREFIX_AREA     (RAM2_FUNC_AREA + RAM2_PREFIX_OFFSET)

/* ============================================================
 * フラッシュエラーコード (独自定義)
 * ============================================================ */
typedef enum {
    IMG_FLASH_OK = 0,
    IMG_FLASH_ERR_TIMEOUT,
    IMG_FLASH_ERR_LOCKED,
    IMG_FLASH_ERR_FAILURE
} img_flash_result_t;

/* ============================================================
 * RAM実行フラッシュ操作関数
 * (Bootloaderと同じ方式 - 実績あるコード)
 * ============================================================ */

/* 関数ポインタ型 */
typedef img_flash_result_t (*ram_flash_write_t)(uint32_t src, uint32_t dest, uint32_t bytes);
typedef img_flash_result_t (*ram_flash_erase_t)(uint32_t block_addr);

/* RAM関数ポインタ */
static ram_flash_write_t s_ram_flash_write = NULL;
static ram_flash_erase_t s_ram_flash_erase = NULL;

/*
 * フラッシュブロック消去の実装
 * この関数はRAM2にコピーされて実行される
 */
static img_flash_result_t __attribute__((noinline)) flash_erase_impl(uint32_t block_addr)
{
    volatile uint8_t *cmd_area = FCU_CMD_AREA;
    volatile int32_t wait_cnt;
    img_flash_result_t err = IMG_FLASH_OK;
    uint32_t psw_save;

    /* 割り込み禁止 */
    psw_save = __builtin_rx_mvfc(0);
    __builtin_rx_clrpsw('I');

    /* フラッシュ書き込み許可 */
    FLASH.FWEPROR.BYTE = 0x01;

    /* PEモード遷移 (コードフラッシュ) */
    FLASH.FENTRYR.WORD = 0xAA01;
    wait_cnt = 4;
    while (FLASH.FENTRYR.WORD != 0x0001) {
        if (--wait_cnt <= 0) {
            err = IMG_FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    /* FRDY待ち */
    wait_cnt = IMG_WAIT_MAX_ERASE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) {
            err = IMG_FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    /* 消去アドレス設定 */
    FLASH.FSADDR.LONG = block_addr;

    /* 消去コマンド発行 */
    *cmd_area = FCU_CMD_ERASE;
    *cmd_area = FCU_CMD_FINAL;

    /* 完了待ち */
    wait_cnt = IMG_WAIT_MAX_ERASE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) {
            *cmd_area = 0xB3;  /* 強制停止 */
            while (FLASH.FSTATR.BIT.FRDY != 1)
                ;
            err = IMG_FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    /* エラーチェック */
    if (FLASH.FASTAT.BIT.CMDLK != 0) {
        *cmd_area = 0xB3;
        while (FLASH.FSTATR.BIT.FRDY != 1)
            ;
        err = IMG_FLASH_ERR_LOCKED;
    }

exit_pe_mode:
    /* PEモード終了 */
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000)
        ;
    FLASH.FWEPROR.BYTE = 0x00;

    /* 割り込み復帰 */
    __builtin_rx_mvtc(0, psw_save);

    return err;
}
/* 終端マーカー */
static void flash_erase_impl_end(void) { }

/*
 * フラッシュ書き込みの実装
 * この関数はRAM2にコピーされて実行される
 * (Bootloaderのflash_write_implと同じ)
 */
static img_flash_result_t __attribute__((noinline)) flash_write_impl(uint32_t src_addr, uint32_t dest_addr, uint32_t num_bytes)
{
    volatile uint8_t *cmd_area = FCU_CMD_AREA;
    volatile uint16_t *cmd_area_word = FCU_CMD_AREA_WORD;
    uint32_t total_count = num_bytes >> 1;
    uint32_t write_cnt = 64;  /* 128 bytes / 2 = 64 words */
    uint16_t *src = (uint16_t *)src_addr;
    volatile int32_t wait_cnt;
    img_flash_result_t err = IMG_FLASH_OK;
    uint32_t psw_save;

    psw_save = __builtin_rx_mvfc(0);
    __builtin_rx_clrpsw('I');

    FLASH.FWEPROR.BYTE = 0x01;

    FLASH.FENTRYR.WORD = 0xAA01;
    wait_cnt = 4;
    while (FLASH.FENTRYR.WORD != 0x0001) {
        if (--wait_cnt <= 0) { err = IMG_FLASH_ERR_TIMEOUT; goto exit_pe_mode; }
    }

    wait_cnt = IMG_WAIT_MAX_ROM_WRITE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) { err = IMG_FLASH_ERR_TIMEOUT; goto exit_pe_mode; }
    }

    FLASH.FSADDR.LONG = dest_addr;
    *cmd_area = FCU_CMD_PROGRAM;
    *cmd_area = (uint8_t)write_cnt;

    while (total_count > 0) {
        *cmd_area_word = *src++;
        while (FLASH.FSTATR.BIT.DBFULL == 1);
        total_count--;
    }

    *cmd_area = FCU_CMD_FINAL;

    wait_cnt = IMG_WAIT_MAX_ROM_WRITE;
    while (FLASH.FSTATR.BIT.FRDY != 1) {
        if (--wait_cnt <= 0) {
            *cmd_area = 0xB3;
            while (FLASH.FSTATR.BIT.FRDY != 1);
            err = IMG_FLASH_ERR_TIMEOUT;
            goto exit_pe_mode;
        }
    }

    if (FLASH.FASTAT.BIT.CMDLK != 0) {
        *cmd_area = 0xB3;
        while (FLASH.FSTATR.BIT.FRDY != 1);
        err = IMG_FLASH_ERR_LOCKED;
    }

exit_pe_mode:
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000);
    FLASH.FWEPROR.BYTE = 0x00;
    __builtin_rx_mvtc(0, psw_save);

    return err;
}
/* 終端マーカー */
static void flash_write_impl_end(void) { }

/*
 * RAM2にフラッシュ操作関数をコピー
 */
static void init_ram_flash_functions(void)
{
    uint32_t erase_size, write_size;
    uint8_t *src, *dst;
    uint32_t i;
    uint32_t offset;

    /* 消去関数のサイズ計算とコピー */
    erase_size = (uint32_t)flash_erase_impl_end - (uint32_t)flash_erase_impl;
    if (erase_size > RAM2_FUNC_SIZE / 2) {
        erase_size = RAM2_FUNC_SIZE / 2;
    }
    if (erase_size < 512) erase_size = 512;  /* 最小安全サイズ */

    src = (uint8_t *)flash_erase_impl;
    dst = (uint8_t *)RAM2_FUNC_AREA;
    for (i = 0; i < erase_size; i++) {
        dst[i] = src[i];
    }
    s_ram_flash_erase = (ram_flash_erase_t)RAM2_FUNC_AREA;

    /* 書き込み関数のサイズ計算とコピー */
    offset = (erase_size + 15) & ~15;  /* 16バイトアライン */
    write_size = (uint32_t)flash_write_impl_end - (uint32_t)flash_write_impl;
    if (write_size > RAM2_FUNC_SIZE - offset) {
        write_size = RAM2_FUNC_SIZE - offset;
    }
    if (write_size < 512) write_size = 512;

    src = (uint8_t *)flash_write_impl;
    dst = (uint8_t *)(RAM2_FUNC_AREA + offset);
    for (i = 0; i < write_size; i++) {
        dst[i] = src[i];
    }
    s_ram_flash_write = (ram_flash_write_t)(RAM2_FUNC_AREA + offset);
}

/* ============================================================
 * 受信ヘルパー関数
 * ============================================================ */

/*
 * 指定バイト数を受信 (タイムアウト付き)
 */
static bool receive_bytes(uint8_t *buf, uint32_t size, uint32_t timeout_ms)
{
    uint32_t received = 0;
    int ch;
    uint32_t loop_count = 0;
    const uint32_t max_loops = timeout_ms * 10000;  /* 概算ループ数 */

    while (received < size) {
        usb_cdc_process();  /* USB処理 */

        ch = usb_cdc_getchar();
        if (ch >= 0) {
            buf[received++] = (uint8_t)ch;
            loop_count = 0;  /* データ受信でリセット */
        } else {
            loop_count++;
            if (loop_count > max_loops) {
                return false;  /* タイムアウト */
            }
        }
    }

    return true;
}

/*
 * ACK送信
 */
static void send_ack(void)
{
    uint8_t ack = IMG_ACK_CHAR;
    usb_cdc_send(&ack, 1);
}

/* ============================================================
 * 公開関数
 * ============================================================ */

/*
 * 起動画像情報表示
 */
void startup_image_show_info(void)
{
    uint32_t addr = (uint32_t)acmtc;
    int i;

    param_console_printf("=== Startup Image Info ===\r\n");
    param_console_printf("Symbol: acmtc\r\n");
    param_console_printf("Address: 0x%08lX\r\n", addr);
    param_console_printf("Max Size: %u bytes\r\n", STARTUP_IMAGE_MAX_SIZE);

    /* 先頭16バイト表示 */
    param_console_print("First 16 bytes:");
    for (i = 0; i < 16; i++) {
        param_console_printf(" %02X", acmtc[i]);
    }
    param_console_print("\r\n");

    /* BMP検証 */
    if (acmtc[0] == 0x42 && acmtc[1] == 0x4D) {
        param_console_print("Format: Valid BMP\r\n");
    } else {
        param_console_print("Format: Not BMP (may be compressed)\r\n");
    }
}

/* ============================================================
 * CRC16計算 (CRC-16-CCITT)
 * ============================================================ */
static uint16_t crc16_ccitt(const uint8_t *data, uint32_t len)
{
    uint16_t crc = 0xFFFF;
    uint32_t i;
    uint8_t j;

    for (i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/*
 * 起動画面読み出しモード
 * プロトコル (固定遅延): ACKなし、チャンク間に遅延を入れて確実に送信
 *   1. FW→Host: サイズ(4バイト)
 *   2. FW→Host: チャンク(256バイト) × 繰り返し（各チャンク後に遅延）
 *   3. FW→Host: CRC16(2バイト)
 */
void startup_image_read_mode(void)
{
    uint32_t total_size = STARTUP_IMAGE_MAX_SIZE;
    uint32_t sent_bytes = 0;
    uint8_t header[4];
    uint8_t crc_bytes[2];
    uint32_t i;
    uint32_t chunk_size;
    uint16_t crc;

    /* チャンクサイズ: 256バイト */
    #define READ_CHUNK_SIZE 256
    uint8_t tx_chunk[READ_CHUNK_SIZE];

    /* SCI9デバッグ出力を有効化 */
    R_Config_SCI9_Start();
    debug_printf("[imgread] Mode started\r\n");

    /* USB送信カウンタをリセット */
    s_usb_write_success = 0;
    s_usb_write_errors = 0;
    s_usb_write_complete_count = 0;

    /* USB CDCモードをACTIVEに設定 */
    usb_cdc_set_mode(USB_MODE_ACTIVE);

    /* RXバッファをクリア */
    usb_cdc_flush_rx();

    /* 1. サイズ送信 (4バイト, little-endian) */
    header[0] = (uint8_t)(total_size & 0xFF);
    header[1] = (uint8_t)((total_size >> 8) & 0xFF);
    header[2] = (uint8_t)((total_size >> 16) & 0xFF);
    header[3] = (uint8_t)((total_size >> 24) & 0xFF);
    usb_cdc_send(header, 4);
    usb_cdc_flush_tx();

    /* ホストがサイズを処理する時間を確保 */
    R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);

    /* 2. CRC計算しながらデータ送信 (チャンク単位 + 遅延) */
    crc = 0xFFFF;
    uint32_t last_debug_percent = 0;

    debug_printf("[imgread] Starting data send: %lu bytes\r\n", total_size);

    while (sent_bytes < total_size) {
        /* 今回送信するサイズ */
        if (sent_bytes + READ_CHUNK_SIZE <= total_size) {
            chunk_size = READ_CHUNK_SIZE;
        } else {
            chunk_size = total_size - sent_bytes;
        }

        /* データをコピー & CRC更新 */
        for (i = 0; i < chunk_size; i++) {
            tx_chunk[i] = acmtc[sent_bytes + i];
            /* CRC更新 */
            crc ^= (uint16_t)tx_chunk[i] << 8;
            for (uint8_t j = 0; j < 8; j++) {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc <<= 1;
                }
            }
        }

        /* チャンク送信 (64バイトずつ) + USB処理 */
        uint32_t chunk_sent = 0;
        while (chunk_sent < chunk_size) {
            uint32_t send_size = (chunk_size - chunk_sent > 64) ? 64 : (chunk_size - chunk_sent);
            usb_cdc_send(&tx_chunk[chunk_sent], send_size);
            usb_cdc_flush_tx();
            usb_cdc_process();  /* USBイベント処理 */
            R_BSP_SoftwareDelay(3, BSP_DELAY_MILLISECS);  /* 64バイトごとに3ms待機 (安定値) */
            chunk_sent += send_size;
        }

        sent_bytes += chunk_size;

        /* USB処理時間を確保 (バッファ詰まり防止のため多めに遅延) */
        usb_cdc_process();
        R_BSP_SoftwareDelay(5, BSP_DELAY_MILLISECS);

        /* デバッグ: 10%ごとにUARTに進捗出力 */
        uint32_t percent = (sent_bytes * 100) / total_size;
        if (percent >= last_debug_percent + 10) {
            debug_printf("[imgread] Sent: %lu/%lu (%lu%%)\r\n", sent_bytes, total_size, percent);
            last_debug_percent = percent;
        }
    }

    debug_printf("[imgread] Loop done. sent_bytes=%lu\r\n", sent_bytes);

    /* 最後のフラッシュを繰り返し確実に送信 */
    for (int flush_count = 0; flush_count < 30; flush_count++) {
        usb_cdc_flush_tx();
        usb_cdc_process();
        R_BSP_SoftwareDelay(50, BSP_DELAY_MILLISECS);
    }

    /* 3. CRC16送信 (2バイト, little-endian) */
    crc_bytes[0] = (uint8_t)(crc & 0xFF);
    crc_bytes[1] = (uint8_t)((crc >> 8) & 0xFF);
    debug_printf("[imgread] Sending CRC: 0x%04X\r\n", crc);
    usb_cdc_send(crc_bytes, 2);
    usb_cdc_flush_tx();

    /* 終端マーカー送信 (4バイト) - ホストが確実に転送完了を検出できるようにする */
    uint8_t end_marker[4] = {IMG_END_MARKER_0, IMG_END_MARKER_1, IMG_END_MARKER_2, IMG_END_MARKER_3};
    debug_printf("[imgread] Sending end marker...\r\n");
    usb_cdc_send(end_marker, 4);
    usb_cdc_flush_tx();

    /* Zero-Length Packetを送信してUSBバルク転送の終端を明示 */
    debug_printf("[imgread] Sending ZLP...\r\n");
    usb_cdc_send_zlp();

    /* 終端マーカーがホストに届くまで繰り返しフラッシュ */
    debug_printf("[imgread] Final flush (5 sec)...\r\n");
    for (int wait = 0; wait < 100; wait++) {
        usb_cdc_flush_tx();
        usb_cdc_process();
        R_BSP_SoftwareDelay(50, BSP_DELAY_MILLISECS);
    }

    debug_printf("[imgread] Complete!\r\n");
    debug_printf("[imgread] R_USB_Write: success=%lu, errors=%lu\r\n", s_usb_write_success, s_usb_write_errors);
    debug_printf("[imgread] WRITE_COMPLETE callbacks: %lu\r\n", s_usb_write_complete_count);
}

/*
 * 起動画面書き込みモード
 */
void startup_image_write_mode(void)
{
    uint32_t total_size;
    uint32_t received_bytes = 0;
    uint32_t acmtc_addr;
    uint32_t aligned_addr;
    uint32_t prefix_size;   /* アライメント境界からacmtc開始までのオフセット */
    uint32_t blocks_needed;
    uint32_t i;
    img_flash_result_t result;
    uint32_t chunk_size;
    uint32_t program_offset;
    uint32_t last_percent = 0;
    uint32_t write_addr;

    param_console_print("\r\n");
    param_console_print("=== Startup Image Write Mode ===\r\n");

    acmtc_addr = (uint32_t)acmtc;
    param_console_printf("Target address: 0x%08lX\r\n", acmtc_addr);
    param_console_print("Waiting for size (4 bytes)...\r\n");

    /* RAM2にフラッシュ操作関数をコピー */
    init_ram_flash_functions();

    /* 受信バッファをクリア（ゴミデータ排除） */
    usb_cdc_flush_rx();

    /* 1. サイズ受信 (4バイト, little-endian) */
    if (!receive_bytes((uint8_t *)&total_size, 4, IMG_TIMEOUT_MS)) {
        param_console_print("ERR: Timeout receiving size\r\n");
        return;
    }

    param_console_printf("Received size: %lu bytes\r\n", total_size);

    /* サイズ検証 */
    if (total_size < 1000 || total_size > STARTUP_IMAGE_MAX_SIZE) {
        param_console_printf("ERR: Invalid size %lu (max: %lu)\r\n", total_size, (uint32_t)STARTUP_IMAGE_MAX_SIZE);
        param_console_print("Note: Image must be 765x256 24bit BMP\r\n");
        return;
    }

    /* フラッシュ操作中の画面乱れを防ぐため、バックライトを即座に消灯 */
    LCD_FadeOUT();

    /* ACK送信 */
    send_ack();

    /* 2. アドレス計算とプレフィックス保存 */
    /* フラッシュ消去は32KB境界から始める必要がある */
    aligned_addr = acmtc_addr & ~(FLASH_CF_BLOCK_SIZE - 1);  /* 32KB境界にアライン */
    prefix_size = acmtc_addr - aligned_addr;  /* 境界からacmtcまでのオフセット */

    param_console_printf("Aligned address: 0x%08lX\r\n", aligned_addr);
    param_console_printf("Prefix offset: %lu bytes\r\n", prefix_size);

    /* プレフィックス部分のデータを保存（消去で失われるため） */
    /* RAM2領域の後半を使用 */
    if (prefix_size > 0 && prefix_size <= PREFIX_SAVE_MAX) {
        param_console_print("Saving prefix data...\r\n");
        memcpy((void *)RAM2_PREFIX_AREA, (void *)aligned_addr, prefix_size);
    } else if (prefix_size > PREFIX_SAVE_MAX) {
        param_console_printf("ERR: Prefix too large (%lu > %d)\r\n", prefix_size, PREFIX_SAVE_MAX);
        return;
    }

    /* 消去が必要なブロック数を計算（acmtc終端まで）*/
    uint32_t end_addr = acmtc_addr + total_size;
    blocks_needed = (end_addr - aligned_addr + FLASH_CF_BLOCK_SIZE - 1) / FLASH_CF_BLOCK_SIZE;

    param_console_printf("Erasing %lu blocks...\r\n", blocks_needed);

    for (i = 0; i < blocks_needed; i++) {
        uint32_t block_addr = aligned_addr + (i * FLASH_CF_BLOCK_SIZE);
        param_console_printf("  Block %lu (0x%08lX)... ", i, block_addr);

        result = s_ram_flash_erase(block_addr);
        if (result != IMG_FLASH_OK) {
            param_console_printf("FAIL (err=%d)\r\n", result);
            param_console_print("ERR: Flash erase failed\r\n");
            return;
        }
        param_console_print("OK\r\n");
    }

    /* 3. プレフィックス部分を復元 (RAM2領域に保存したデータ) */
    /* 注意: フラッシュ書き込みは128バイト境界からのみ可能 */
    /* 128バイト境界までのみ復元し、残りは画像データと一緒に書き込む */
    uint32_t prefix_restore_size = (prefix_size / 128) * 128;  /* 128バイト境界まで切り捨て */
    uint32_t prefix_remainder = prefix_size - prefix_restore_size;  /* 残り（0〜127バイト） */

    if (prefix_restore_size > 0) {
        param_console_print("Restoring prefix data...\r\n");

        /* 128バイト単位で書き込み（完全なブロックのみ） */
        uint32_t prefix_written = 0;
        uint8_t *prefix_src = (uint8_t *)RAM2_PREFIX_AREA;
        while (prefix_written < prefix_restore_size) {
            memcpy(program_buffer, prefix_src + prefix_written, 128);

            result = s_ram_flash_write((uint32_t)program_buffer, aligned_addr + prefix_written, 128);
            if (result != IMG_FLASH_OK) {
                param_console_printf("ERR: Prefix restore failed at 0x%08lX (err=%d)\r\n",
                                     aligned_addr + prefix_written, result);
                return;
            }

            prefix_written += 128;
        }
    }

    /* 消去完了後にACK送信 (Windows Terminal側が待機している) */
    send_ack();

    /* 4. データ受信と書き込み */
    /* 128バイト境界にアラインした書き込み開始アドレス */
    uint32_t aligned_write_addr = acmtc_addr & ~127UL;
    uint32_t write_padding = acmtc_addr - aligned_write_addr;  /* 先頭パディングバイト数 */

    param_console_print("Receiving and writing data...\r\n");
    param_console_printf("  Write start: 0x%08lX (padding: %lu)\r\n", aligned_write_addr, write_padding);

    /* 最初の書き込みブロックの準備 */
    /* 先頭のパディングバイトは、プレフィックスの残りデータで埋める */
    memset(program_buffer, 0xFF, sizeof(program_buffer));
    if (write_padding > 0 && prefix_remainder > 0) {
        /* プレフィックスの残り部分をコピー */
        uint8_t *prefix_src = (uint8_t *)RAM2_PREFIX_AREA;
        memcpy(program_buffer, prefix_src + prefix_restore_size, write_padding);
    }
    program_offset = write_padding;  /* パディング分だけオフセット */
    write_addr = aligned_write_addr;

    while (received_bytes < total_size) {
        /* チャンクサイズ計算 */
        uint32_t remaining = total_size - received_bytes;
        chunk_size = (remaining < IMG_CHUNK_SIZE) ? remaining : IMG_CHUNK_SIZE;

        /* チャンク受信 */
        if (!receive_bytes(rx_buffer, chunk_size, IMG_TIMEOUT_MS)) {
            param_console_printf("ERR: Timeout at offset %lu\r\n", received_bytes);
            return;
        }

        /* プログラムバッファに追加 */
        for (i = 0; i < chunk_size; i++) {
            program_buffer[program_offset++] = rx_buffer[i];

            /* 128バイト溜まったら書き込み (Bootloaderと同じ単位) */
            if (program_offset >= 128) {
                /* RAM関数で書き込み */
                result = s_ram_flash_write((uint32_t)program_buffer, write_addr, 128);
                if (result != IMG_FLASH_OK) {
                    param_console_printf("ERR: Write failed at 0x%08lX (err=%d)\r\n", write_addr, result);
                    return;
                }

                write_addr += 128;
                program_offset = 0;
                memset(program_buffer, 0xFF, sizeof(program_buffer));
            }
        }

        received_bytes += chunk_size;

        /* ACK送信 */
        send_ack();

        /* 進捗表示 (10%ごと) */
        uint32_t percent = received_bytes * 100 / total_size;
        if (percent >= last_percent + 10) {
            param_console_printf("  %lu%% (%lu/%lu bytes)\r\n", percent, received_bytes, total_size);
            last_percent = percent;
        }
    }

    /* 残りデータを書き込み */
    if (program_offset > 0) {
        /* 128バイト境界に切り上げ */
        uint32_t write_size = 128;  /* 最小書き込み単位 */

        /* 残りは0xFFでパディング済み */
        result = s_ram_flash_write((uint32_t)program_buffer, write_addr, write_size);
        if (result != IMG_FLASH_OK) {
            param_console_printf("ERR: Final write failed at 0x%08lX (err=%d)\r\n", write_addr, result);
            return;
        }
    }

    /* 5. 完了 → 自動再起動 */
    param_console_print("\r\nDone!\r\n");
    param_console_printf("Written %lu bytes to 0x%08lX\r\n", total_size, acmtc_addr);
    param_console_print("Restarting device...\r\n");

    /* USB送信バッファフラッシュ待機 */
    R_BSP_SoftwareDelay(100, BSP_DELAY_MILLISECS);

    /* USB周辺機能を停止 */
    usb_cdc_shutdown();

    /* 割り込み禁止 */
    __builtin_rx_clrpsw('I');

    /* ソフトウェアリセット */
    SYSTEM.PRCR.WORD = 0xA502;  /* プロテクト解除 */
    SYSTEM.SWRR = 0xA501;       /* ソフトウェアリセット実行 */

    while (1);  /* リセット待ち */
}
