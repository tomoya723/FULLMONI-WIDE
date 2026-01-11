/*
 * param_console.c
 *
 * USB CDCターミナルコンソール機能
 * パラメータ変更モードのコマンド処理
 *
 *  Created on: 2026/01/06
 *      Author: FULLMONI-WIDE Project
 *  Modified: 2026/01/11 - SCI9 → USB CDC に変更
 */

#include "param_console.h"
#include "param_storage.h"
#include "r_smc_entry.h"
#include "usb_cdc.h"  /* USB CDC driver */
#include "platform.h"   /* iodefine.h (RTC access, SYSTEM for software reset) */
#include "firmware_version.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* LCD FadeOUT (TFTLCD102.c) */
extern void LCD_FadeOUT(void);

/* ============================================================
 * RTC レジスタアクセス (platform.h経由で iodefine.h を参照)
 * ============================================================ */

/* 強制アップデートフラグ (RAM2領域, Bootloaderと共有) */
/* RAM2末尾に配置 - BootloaderのFlash書き込み関数と衝突しないように */
#define BL_FORCE_UPDATE_ADDR    (0x0087FFF0UL)  /* RAM2 end - 16 bytes */
#define BL_FORCE_UPDATE_MAGIC   (0xDEADBEEFUL)

/* コマンドラインバッファ */
static char cmd_line[CMD_LINE_SIZE];
static uint16_t cmd_pos = 0;

/* TX送信用バッファ */
static char tx_buffer[UART_TX_BUFFER_SIZE];

/* モード終了フラグ */
static bool exit_flag = false;

/* ============================================================
 * 受信データ取得（USB CDCから直接取得）
 * ============================================================ */
static int console_getchar(void)
{
    return usb_cdc_getchar();
}

/* ============================================================
 * USB CDC出力（SCI9から変更）
 * ============================================================ */
void param_console_print(const char *str)
{
    usb_cdc_print(str);
}

void param_console_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(tx_buffer, UART_TX_BUFFER_SIZE, fmt, args);
    va_end(args);
    param_console_print(tx_buffer);
}

/* ============================================================
 * コマンドハンドラ
 * ============================================================ */

/* ヘルプ表示 */
static void cmd_help(void)
{
    param_console_print("\r\n=== FULLMONI-WIDE Parameter Console ===\r\n");
    param_console_print("Commands:\r\n");
    param_console_print("  help              - Show this help\r\n");
    param_console_print("  version           - Show firmware version\r\n");
    param_console_print("  list              - Show all parameters\r\n");
    param_console_print("  set <id> <value>  - Set parameter value\r\n");
    param_console_print("  save              - Save to EEPROM\r\n");
    param_console_print("  load              - Load from EEPROM\r\n");
    param_console_print("  default           - Reset to default values\r\n");
    param_console_print("  rtc               - Show current RTC time\r\n");
    param_console_print("  rtc YY MM DD hh mm ss - Set RTC time\r\n");
    param_console_print("  odo               - Show ODO value\r\n");
    param_console_print("  odo_init <km>     - Initialize ODO value\r\n");
    param_console_print("  trip              - Show TRIP value\r\n");
    param_console_print("  trip_reset        - Reset TRIP to 0\r\n");
    param_console_print("  fwupdate          - Erase flash and reboot to bootloader\r\n");
    param_console_print("  exit              - Exit parameter mode\r\n");
    param_console_print("\r\nParameter IDs:\r\n");
    param_console_print("  tyre_width, tyre_aspect, tyre_rim\r\n");
    param_console_print("  gear1-gear6, final\r\n");
    param_console_print("  water_low, water_high, fuel_warn\r\n");
    param_console_print("  shift_rpm1-shift_rpm5 (shift indicator thresholds)\r\n");
}

/* パラメータ一覧表示 */
static void cmd_list(void)
{
    param_console_print("\r\n=== Current Parameters ===\r\n");
    param_console_printf("Tyre: %d/%d R%d\r\n",
                        g_param.tyre_width, g_param.tyre_aspect, g_param.tyre_rim);
    param_console_printf("Gear1: %.3f  Gear2: %.3f  Gear3: %.3f\r\n",
                        g_param.gear_ratio[0] / 1000.0f,
                        g_param.gear_ratio[1] / 1000.0f,
                        g_param.gear_ratio[2] / 1000.0f);
    param_console_printf("Gear4: %.3f  Gear5: %.3f  Gear6: %.3f\r\n",
                        g_param.gear_ratio[3] / 1000.0f,
                        g_param.gear_ratio[4] / 1000.0f,
                        g_param.gear_ratio[5] / 1000.0f);
    param_console_printf("Final: %.3f\r\n",
                        g_param.final_gear_ratio / 1000.0f);
    param_console_printf("Water Temp Warning: %d - %d C\r\n",
                        g_param.water_temp_low, g_param.water_temp_high);
    param_console_printf("Fuel Warning: %d %%\r\n",
                        g_param.fuel_warn_level);
    param_console_printf("Shift RPM: %d / %d / %d / %d / %d\r\n",
                        g_param.shift_rpm1, g_param.shift_rpm2,
                        g_param.shift_rpm3, g_param.shift_rpm4, g_param.shift_rpm5);
    param_console_printf("ODO: %lu km  TRIP: %.1f km\r\n",
                        param_storage_get_odo_km(),
                        param_storage_get_trip_km10() / 10.0f);
}

/* パラメータ設定 */
static void cmd_set(const char *id, const char *value)
{
    int val = atoi(value);

    if (strcmp(id, "tyre_width") == 0) {
        g_param.tyre_width = val;
        param_console_printf("tyre_width = %d\r\n", val);
    } else if (strcmp(id, "tyre_aspect") == 0) {
        g_param.tyre_aspect = val;
        param_console_printf("tyre_aspect = %d\r\n", val);
    } else if (strcmp(id, "tyre_rim") == 0) {
        g_param.tyre_rim = val;
        param_console_printf("tyre_rim = %d\r\n", val);
    } else if (strcmp(id, "gear1") == 0) {
        g_param.gear_ratio[0] = val;
        param_console_printf("gear1 = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "gear2") == 0) {
        g_param.gear_ratio[1] = val;
        param_console_printf("gear2 = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "gear3") == 0) {
        g_param.gear_ratio[2] = val;
        param_console_printf("gear3 = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "gear4") == 0) {
        g_param.gear_ratio[3] = val;
        param_console_printf("gear4 = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "gear5") == 0) {
        g_param.gear_ratio[4] = val;
        param_console_printf("gear5 = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "gear6") == 0) {
        g_param.gear_ratio[5] = val;
        param_console_printf("gear6 = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "final") == 0) {
        g_param.final_gear_ratio = val;
        param_console_printf("final = %.3f\r\n", val / 1000.0f);
    } else if (strcmp(id, "water_low") == 0) {
        g_param.water_temp_low = val;
        param_console_printf("water_low = %d\r\n", val);
    } else if (strcmp(id, "water_high") == 0) {
        g_param.water_temp_high = val;
        param_console_printf("water_high = %d\r\n", val);
    } else if (strcmp(id, "fuel_warn") == 0) {
        g_param.fuel_warn_level = val;
        param_console_printf("fuel_warn = %d\r\n", val);
    } else if (strcmp(id, "shift_rpm1") == 0) {
        g_param.shift_rpm1 = val;
        param_console_printf("shift_rpm1 = %d\r\n", val);
    } else if (strcmp(id, "shift_rpm2") == 0) {
        g_param.shift_rpm2 = val;
        param_console_printf("shift_rpm2 = %d\r\n", val);
    } else if (strcmp(id, "shift_rpm3") == 0) {
        g_param.shift_rpm3 = val;
        param_console_printf("shift_rpm3 = %d\r\n", val);
    } else if (strcmp(id, "shift_rpm4") == 0) {
        g_param.shift_rpm4 = val;
        param_console_printf("shift_rpm4 = %d\r\n", val);
    } else if (strcmp(id, "shift_rpm5") == 0) {
        g_param.shift_rpm5 = val;
        param_console_printf("shift_rpm5 = %d\r\n", val);
    } else {
        param_console_printf("Unknown parameter: %s\r\n", id);
    }
}

/* RTC表示 */
static void cmd_rtc_show(void)
{
    uint8_t yy = RTC.RYRCNT.WORD & 0xFF;
    uint8_t mm = RTC.RMONCNT.BYTE;
    uint8_t dd = RTC.RDAYCNT.BYTE;
    uint8_t hh = RTC.RHRCNT.BYTE & 0x3F;
    uint8_t mi = RTC.RMINCNT.BYTE;
    uint8_t ss = RTC.RSECCNT.BYTE;

    param_console_printf("RTC: 20%02x/%02x/%02x %02x:%02x:%02x\r\n",
                        yy, mm, dd, hh, mi, ss);
}

/* RTC設定 */
static void cmd_rtc_set(int yy, int mm, int dd, int hh, int mi, int ss)
{
    /* RTC停止 */
    RTC.RCR2.BIT.START = 0;
    while (RTC.RCR2.BIT.START != 0);

    /* BCD形式で設定 */
    RTC.RYRCNT.WORD  = ((yy / 10) << 4) | (yy % 10);
    RTC.RMONCNT.BYTE = ((mm / 10) << 4) | (mm % 10);
    RTC.RDAYCNT.BYTE = ((dd / 10) << 4) | (dd % 10);
    RTC.RHRCNT.BYTE  = ((hh / 10) << 4) | (hh % 10);
    RTC.RMINCNT.BYTE = ((mi / 10) << 4) | (mi % 10);
    RTC.RSECCNT.BYTE = ((ss / 10) << 4) | (ss % 10);

    /* RTC再開 */
    RTC.RCR2.BIT.START = 1;
    while (RTC.RCR2.BIT.START != 1);

    param_console_printf("RTC set: 20%02d/%02d/%02d %02d:%02d:%02d\r\n",
                        yy, mm, dd, hh, mi, ss);
}

/* ODO表示 */
static void cmd_odo_show(void)
{
    extern volatile unsigned long sp_int;
    param_console_printf("ODO: %lu km (pulse: %lu)\r\n",
                        param_storage_get_odo_km(), sp_int);
}

/* ODO初期化 */
static void cmd_odo_init(uint32_t km)
{
    param_storage_set_odo(km);
    param_console_printf("ODO initialized: %lu km\r\n", km);
}

/* TRIP表示 */
static void cmd_trip_show(void)
{
    param_console_printf("TRIP: %.1f km\r\n",
                        param_storage_get_trip_km10() / 10.0f);
}

/* TRIPリセット */
static void cmd_trip_reset(void)
{
    param_storage_reset_trip();
    param_console_print("TRIP reset: 0.0 km\r\n");
}

/* ============================================================
 * Firmware Update (Bootloaderへの強制リブート)
 * ============================================================ */

/* 強制アップデートフラグ (RSTSR2レジスタを利用、Bootloaderと共有) */
/* RSTSR2の未使用ビット7を強制更新フラグとして使用（リセット後も保持） */
#define BL_FORCE_UPDATE_BIT     (0x80)  /* RSTSR2 bit7 */

/* 確認入力を待つ (ブロッキング) */
static bool wait_for_confirmation(const char *expected, uint32_t timeout_sec)
{
    char input[16] = {0};
    uint8_t pos = 0;
    uint32_t timeout_cnt = 0;
    int ch;

    while (timeout_cnt < timeout_sec * 100) {  /* 10ms x 100 = 1sec */
        /* USB CDCポーリング */
        usb_cdc_process();

        ch = console_getchar();
        if (ch >= 0) {
            if (ch == '\r' || ch == '\n') {
                param_console_print("\r\n");
                input[pos] = '\0';
                return (strcmp(input, expected) == 0);
            } else if (ch == '\b' || ch == 0x7F) {
                if (pos > 0) {
                    pos--;
                    param_console_print("\b \b");
                }
            } else if (ch >= 0x20 && ch < 0x7F && pos < sizeof(input) - 1) {
                input[pos++] = (char)ch;
                char echo[2] = {(char)ch, '\0'};
                param_console_print(echo);
            }
            timeout_cnt = 0;  /* 入力があったらタイムアウトリセット */
        } else {
            R_BSP_SoftwareDelay(10, BSP_DELAY_MILLISECS);
            timeout_cnt++;
        }
    }

    param_console_print("\r\n(timeout)\r\n");
    return false;
}

/* ソフトウェアリセット実行 */
static void perform_software_reset(void)
{
    /* USB送信バッファフラッシュ待機 */
    R_BSP_SoftwareDelay(50, BSP_DELAY_MILLISECS);

    /* USB周辺機能を停止 */
    usb_cdc_shutdown();

    /* 割り込み禁止 */
    __builtin_rx_clrpsw('I');

    /* ソフトウェアリセット */
    SYSTEM.PRCR.WORD = 0xA502;  /* プロテクト解除 */
    SYSTEM.SWRR = 0xA501;       /* ソフトウェアリセット実行 */

    while (1);  /* リセット待ち */
}

/* fwupdateコマンド */
static void cmd_fwupdate(void)
{
    param_console_print("\r\n");
    param_console_print("======================================\r\n");
    param_console_print("  *** FIRMWARE UPDATE MODE ***\r\n");
    param_console_print("  This will reboot to bootloader\r\n");
    param_console_print("  and enter firmware update mode.\r\n");
    param_console_print("\r\n");
    param_console_print("  WARNING: The device will not function\r\n");
    param_console_print("  until new firmware is programmed!\r\n");
    param_console_print("======================================\r\n");
    param_console_print("Type 'yes' to confirm (10sec timeout): ");

    /* 確認入力待ち */
    if (!wait_for_confirmation("yes", 10)) {
        param_console_print("Aborted.\r\n");
        return;
    }

    /* 強制アップデートフラグをセット (RAM2領域) */
    param_console_print("\r\nSetting update flag...\r\n");
    volatile uint32_t *force_flag = (volatile uint32_t *)BL_FORCE_UPDATE_ADDR;
    *force_flag = BL_FORCE_UPDATE_MAGIC;

    /* LCDバックライト消灯 */
    param_console_print("LCD FadeOUT...\r\n");
    LCD_FadeOUT();

    /* ブートローダーへリブート */
    param_console_print("Rebooting to bootloader...\r\n");
    perform_software_reset();

    /* ここには到達しない */
}

/* ============================================================
 * コマンドパーサー
 * ============================================================ */
static void parse_command(const char *line)
{
    char cmd[32] = {0};
    char arg1[32] = {0};
    char arg2[32] = {0};
    char arg3[32] = {0};
    char arg4[32] = {0};
    char arg5[32] = {0};
    char arg6[32] = {0};

    /* コマンド解析 */
    int argc = sscanf(line, "%31s %31s %31s %31s %31s %31s %31s",
                      cmd, arg1, arg2, arg3, arg4, arg5, arg6);

    if (argc < 1 || strlen(cmd) == 0) {
        return;
    }

    /* コマンド実行 */
    if (strcmp(cmd, "help") == 0) {
        cmd_help();
    } else if (strcmp(cmd, "version") == 0) {
        param_console_printf("VERSION %d.%d.%d\r\n",
            FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH);
    } else if (strcmp(cmd, "list") == 0) {
        cmd_list();
    } else if (strcmp(cmd, "set") == 0 && argc >= 3) {
        cmd_set(arg1, arg2);
    } else if (strcmp(cmd, "save") == 0) {
        param_console_print("Saving to EEPROM... ");
        if (param_storage_save()) {
            param_console_print("OK\r\n");
        } else {
            param_console_print("NG\r\n");
        }
    } else if (strcmp(cmd, "load") == 0) {
        param_console_print("Loading from EEPROM... ");
        if (param_storage_load()) {
            param_console_print("OK\r\n");
        } else {
            param_console_print("NG (using defaults)\r\n");
        }
    } else if (strcmp(cmd, "default") == 0) {
        param_storage_reset_default();
        param_console_print("Parameters reset to default.\r\n");
    } else if (strcmp(cmd, "rtc") == 0) {
        if (argc >= 7) {
            cmd_rtc_set(atoi(arg1), atoi(arg2), atoi(arg3),
                       atoi(arg4), atoi(arg5), atoi(arg6));
        } else {
            cmd_rtc_show();
        }
    } else if (strcmp(cmd, "odo") == 0) {
        cmd_odo_show();
    } else if (strcmp(cmd, "odo_init") == 0 && argc >= 2) {
        cmd_odo_init(atol(arg1));
    } else if (strcmp(cmd, "trip") == 0) {
        cmd_trip_show();
    } else if (strcmp(cmd, "trip_reset") == 0) {
        cmd_trip_reset();
    } else if (strcmp(cmd, "fwupdate") == 0) {
        cmd_fwupdate();
    } else if (strcmp(cmd, "exit") == 0) {
        param_console_print("Exiting parameter mode...\r\n");
        exit_flag = true;
    } else {
        param_console_printf("Unknown command: %s\r\n", cmd);
        param_console_print("Type 'help' for command list.\r\n");
    }
}

/* ============================================================
 * コンソール初期化
 * ============================================================ */
void param_console_init(void)
{
    cmd_pos = 0;
    exit_flag = false;
    memset(cmd_line, 0, CMD_LINE_SIZE);
}

/* ============================================================
 * パラメータモード開始
 * ============================================================ */
void param_console_enter(void)
{
    param_console_init();
    param_console_print("\r\n");
    param_console_print("========================================\r\n");
    param_console_print("  FULLMONI-WIDE Parameter Console\r\n");
    param_console_print("  Type 'help' for command list.\r\n");
    param_console_print("  Type 'exit' to return to normal mode.\r\n");
    param_console_print("========================================\r\n");
    param_console_print("\r\n> ");
}

/* ============================================================
 * コンソール処理（メインループから呼び出し）
 * ============================================================ */
bool param_console_process(void)
{
    int ch;

    /* USB CDCから受信データ処理 */
    while ((ch = console_getchar()) >= 0) {

        if (ch == '\r' || ch == '\n') {
            /* コマンド確定 */
            param_console_print("\r\n");
            cmd_line[cmd_pos] = '\0';

            if (cmd_pos > 0) {
                parse_command(cmd_line);
            }

            cmd_pos = 0;
            memset(cmd_line, 0, CMD_LINE_SIZE);

            if (!exit_flag) {
                param_console_print("> ");
            }
        } else if (ch == '\b' || ch == 0x7F) {
            /* バックスペース */
            if (cmd_pos > 0) {
                cmd_pos--;
                param_console_print("\b \b");
            }
        } else if (ch >= 0x20 && ch < 0x7F) {
            /* 通常文字 */
            if (cmd_pos < CMD_LINE_SIZE - 1) {
                cmd_line[cmd_pos++] = (char)ch;
                /* エコーバック */
                char echo[2] = {(char)ch, '\0'};
                param_console_print(echo);
            }
        }
    }

    /* モード継続判定 */
    return !exit_flag;
}
