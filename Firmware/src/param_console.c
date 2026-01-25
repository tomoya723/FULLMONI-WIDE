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
#include "startup_image_write.h"  /* 起動画像書き込み */
#include "can.h"  /* Issue #65: CAN受信フィルタ更新 */
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

/* Issue #65: CAN設定変更フラグ（exitでフィルタ更新が必要かどうか） */
static bool can_config_changed = false;

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
    param_console_print("  imgwrite          - Write startup image to flash\r\n");
    param_console_print("  imgread           - Read startup image from flash\r\n");
    param_console_print("  mtcinfo           - Show startup image info\r\n");
    param_console_print("  exit              - Exit parameter mode\r\n");
    param_console_print("\r\nCAN Configuration (Issue #65):\r\n");
    param_console_print("  can_list          - Show CAN configuration\r\n");
    param_console_print("  can_warning <0|1> - Enable/disable master warning\r\n");
    param_console_print("  can_sound <0|1>   - Enable/disable warning sound\r\n");
    param_console_print("  can_ch <n> <id> <en> - Set CAN channel (n=1-6)\r\n");
    param_console_print("  can_field <n> <ch> <byte> <len> <type> <end> <var> <off> <mul> <div>\r\n");
    param_console_print("            <name> <unit> <dec_shift> <warn_lo_en> <warn_lo> <warn_hi_en> <warn_hi>\r\n");
    param_console_print("                    - Set CAN field (n=0-15, dec_shift=AppWizard decimal)\r\n");
    param_console_print("  can_preset <name> - Apply preset (motec/link/aem)\r\n");
    param_console_print("  can_save          - Save CAN config to EEPROM\r\n");
    param_console_print("  can_load          - Load CAN config from EEPROM\r\n");
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
 * CAN設定コマンド (Issue #65)
 * ============================================================ */

/* ターゲット変数名テーブル */
static const char *target_var_names[] = {
    "REV", "AF", "NUM1", "NUM2", "NUM3", "NUM4", "NUM5", "NUM6", "SPEED"
};

/* CAN設定一覧表示 */
static void cmd_can_list(void)
{
    uint8_t i;

    param_console_print("\r\n=== CAN Configuration ===\r\n");
    param_console_printf("Version: %d, Preset: %d\r\n",
                        g_can_config.version, g_can_config.preset_id);
    param_console_printf("Warning: %s, Sound: %s\r\n",
                        g_can_config.warning_enabled ? "ON" : "OFF",
                        g_can_config.sound_enabled ? "ON" : "OFF");

    param_console_print("\r\n-- Channels --\r\n");
    for (i = 0; i < CAN_CHANNEL_MAX; i++) {
        param_console_printf("CH%d: ID=0x%03X, %s\r\n",
                            i + 1,
                            g_can_config.channels[i].can_id,
                            g_can_config.channels[i].enabled ? "ON" : "OFF");
    }

    param_console_print("\r\n-- Fields (16) --\r\n");
    param_console_print("No CH Byte Len Type End Var    Off  Mul   Div  Dsh Name    Unit WLo  Lo WHi    Hi\r\n");
    for (i = 0; i < CAN_FIELD_MAX; i++) {
        CAN_Field_t *f = &g_can_config.fields[i];

        const char *var_name = (f->target_var < 9) ? target_var_names[f->target_var] : "---";

        /* 閾値表示用文字列を準備 (float対応) */
        char lo_str[12], hi_str[12];
        if (f->warn_low <= CAN_WARN_DISABLED) {
            strcpy(lo_str, "---");
        } else {
            snprintf(lo_str, sizeof(lo_str), "%.2f", f->warn_low);
        }
        if (f->warn_high <= CAN_WARN_DISABLED) {
            strcpy(hi_str, "---");
        } else {
            snprintf(hi_str, sizeof(hi_str), "%.2f", f->warn_high);
        }

        /* 空のName/Unitは "-" を出力（パース用） */
        const char *name_out = (f->name[0] != '\0') ? f->name : "-";
        const char *unit_out = (f->unit[0] != '\0') ? f->unit : "-";

        param_console_printf("%2d %2d   %d   %d   %c    %c   %-5s %4d %5d %5d  %d  %-7s %-4s %c %6s %c %6s\r\n",
                            i, f->channel, f->start_byte, f->byte_count,
                            f->data_type ? 'S' : 'U',
                            f->endian ? 'L' : 'B',
                            var_name,
                            f->offset, f->multiplier, f->divisor,
                            f->decimal_shift,
                            name_out, unit_out,
                            f->warn_low_enabled ? 'Y' : 'N', lo_str,
                            f->warn_high_enabled ? 'Y' : 'N', hi_str);
    }
}
static void cmd_can_ch(uint8_t ch, uint16_t can_id, uint8_t enabled)
{
    /* Issue #65: 値が変わる場合のみフラグを立てる */
    if (ch >= 1 && ch <= CAN_CHANNEL_MAX) {
        CAN_RX_Channel_t *cur = &g_can_config.channels[ch - 1];
        if (cur->can_id != can_id || cur->enabled != (enabled ? 1 : 0)) {
            can_config_changed = true;
        }
    }

    if (can_config_set_channel(ch, can_id, enabled)) {
        param_console_printf("CH%d: ID=0x%03X, %s\r\n", ch, can_id, enabled ? "ON" : "OFF");
    } else {
        param_console_print("Error: Invalid parameters\r\n");
    }
}

/* CANフィールド設定 */
static void cmd_can_field(int argc, char *args[])
{
    if (argc < 10) {
        param_console_print("Usage: can_field <n> <ch> <byte> <len> <type> <end> <var> <off> <mul> <div>\r\n");
        param_console_print("                <name> <unit> <dec_shift> <warn_lo_en> <warn_lo> <warn_hi_en> <warn_hi>\r\n");
        return;
    }

    CAN_Field_t field;
    memset(&field, 0, sizeof(field));  /* 全フィールドをゼロ初期化 */

    uint8_t idx = (uint8_t)atoi(args[0]);
    field.channel = (uint8_t)atoi(args[1]);
    field.start_byte = (uint8_t)atoi(args[2]);
    field.byte_count = (uint8_t)atoi(args[3]);
    field.data_type = (uint8_t)atoi(args[4]);
    field.endian = (uint8_t)atoi(args[5]);
    field.target_var = (uint8_t)atoi(args[6]);
    field.offset = (int16_t)atoi(args[7]);
    field.multiplier = (uint16_t)atoi(args[8]);
    field.divisor = (uint16_t)atoi(args[9]);

    /* Issue #50: 拡張フィールド */
    if (argc >= 11) {
        strncpy(field.name, args[10], CAN_FIELD_NAME_MAX - 1);
        field.name[CAN_FIELD_NAME_MAX - 1] = '\0';
    }
    if (argc >= 12) {
        strncpy(field.unit, args[11], CAN_FIELD_UNIT_MAX - 1);
        field.unit[CAN_FIELD_UNIT_MAX - 1] = '\0';
    }
    if (argc >= 13) {
        field.decimal_shift = (uint8_t)atoi(args[12]);
    }
    if (argc >= 14) {
        field.warn_low_enabled = (uint8_t)atoi(args[13]);
    }
    if (argc >= 15) {
        field.warn_low = (float)atof(args[14]);
    } else {
        field.warn_low = CAN_WARN_DISABLED;
    }
    if (argc >= 16) {
        field.warn_high_enabled = (uint8_t)atoi(args[15]);
    }
    if (argc >= 17) {
        field.warn_high = (float)atof(args[16]);
    } else {
        field.warn_high = CAN_WARN_DISABLED;
    }

    /* Issue #65: channelが変わる場合のみフラグを立てる（フィルタに影響） */
    /* 注: フィールド設定はデータ解釈のみなのでフィルタ更新不要 */

    if (can_config_set_field(idx, &field)) {
        param_console_printf("Field %d set: ch=%d dec=%d wlo_en=%d lo=%.2f whi_en=%d hi=%.2f\r\n",
            idx, field.channel, field.decimal_shift, field.warn_low_enabled, field.warn_low,
            field.warn_high_enabled, field.warn_high);
    } else {
        param_console_print("Error: Invalid parameters\r\n");
    }
}

/* CANプリセット適用 */
static void cmd_can_preset(const char *name)
{
    uint8_t preset_id = CAN_PRESET_CUSTOM;

    if (strcmp(name, "motec") == 0 || strcmp(name, "motec_m100") == 0) {
        preset_id = CAN_PRESET_MOTEC_M100;
    } else if (strcmp(name, "link") == 0 || strcmp(name, "link_g4") == 0) {
        preset_id = CAN_PRESET_LINK_G4;
        param_console_print("Warning: LINK preset not yet implemented, using MoTeC\r\n");
        preset_id = CAN_PRESET_MOTEC_M100;
    } else if (strcmp(name, "aem") == 0 || strcmp(name, "aem_ems") == 0) {
        preset_id = CAN_PRESET_AEM_EMS;
        param_console_print("Warning: AEM preset not yet implemented, using MoTeC\r\n");
        preset_id = CAN_PRESET_MOTEC_M100;
    } else {
        param_console_print("Unknown preset. Available: motec, link, aem\r\n");
        return;
    }

    can_config_apply_preset(preset_id);
    param_console_printf("Applied preset: %s\r\n", name);
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
    } else if (strcmp(cmd, "imgwrite") == 0) {
        /* 起動画像書き込みモード */
        startup_image_write_mode();
    } else if (strcmp(cmd, "imgread") == 0) {
        /* 起動画像読み出しモード */
        startup_image_read_mode();
    } else if (strcmp(cmd, "mtcinfo") == 0) {
        /* 起動画像情報表示 */
        startup_image_show_info();
    } else if (strcmp(cmd, "debug_eeprom") == 0) {
        /* レガシー領域(0x0000)の生データを表示 */
        extern void debug_dump_legacy_eeprom(void);
        debug_dump_legacy_eeprom();
    } else if (strcmp(cmd, "wr_cnt_reset") == 0) {
        extern void param_storage_reset_wr_cnt(void);
        param_storage_reset_wr_cnt();
        param_console_print("wr_cnt reset to 0.\r\n");
    /* === CAN設定コマンド (Issue #65) === */
    } else if (strcmp(cmd, "can_list") == 0) {
        cmd_can_list();
    } else if (strcmp(cmd, "can_warning") == 0 && argc >= 2) {
        /* can_warning <0|1> - Enable/disable master warning */
        uint8_t enabled = (uint8_t)atoi(arg1);
        g_can_config.warning_enabled = enabled ? 1 : 0;
        can_config_changed = true;
        param_console_printf("Warning: %s\r\n", g_can_config.warning_enabled ? "ON" : "OFF");
    } else if (strcmp(cmd, "can_sound") == 0 && argc >= 2) {
        /* can_sound <0|1> - Enable/disable warning sound */
        uint8_t enabled = (uint8_t)atoi(arg1);
        g_can_config.sound_enabled = enabled ? 1 : 0;
        can_config_changed = true;
        param_console_printf("Sound: %s\r\n", g_can_config.sound_enabled ? "ON" : "OFF");
    } else if (strcmp(cmd, "can_ch") == 0 && argc >= 4) {
        /* can_ch <n> <id> <en> */
        uint8_t ch = (uint8_t)atoi(arg1);
        uint16_t can_id = (uint16_t)strtol(arg2, NULL, 0);  /* 0x対応 */
        uint8_t enabled = (uint8_t)atoi(arg3);
        cmd_can_ch(ch, can_id, enabled);
    } else if (strcmp(cmd, "can_field") == 0 && argc >= 7) {
        /* can_field <n> <ch> <byte> <len> <type> <end> <var> <off> <mul> <div> <name> <unit> <dec_shift> <wlo_en> <warn_lo> <whi_en> <warn_hi> */
        /* 最大17引数をパース */
        char *args[17] = {arg1, arg2, arg3, arg4, arg5, arg6, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
        /* 残りの引数を再パース */
        char extra_args[11][32];
        int extra_argc = sscanf(line, "%*s %*s %*s %*s %*s %*s %*s %31s %31s %31s %31s %31s %31s %31s %31s %31s %31s %31s",
               extra_args[0], extra_args[1], extra_args[2], extra_args[3],
               extra_args[4], extra_args[5], extra_args[6], extra_args[7], extra_args[8], extra_args[9], extra_args[10]);
        if (extra_argc >= 4) {
            int i;
            for (i = 0; i < extra_argc && i < 11; i++) {
                args[6 + i] = extra_args[i];
            }
            cmd_can_field(6 + extra_argc, args);
        } else {
            param_console_print("Error: Not enough arguments for can_field\r\n");
        }
    } else if (strcmp(cmd, "can_preset") == 0 && argc >= 2) {
        cmd_can_preset(arg1);
    } else if (strcmp(cmd, "can_save") == 0) {
        param_console_print("Saving CAN config to EEPROM... ");
        if (can_config_save()) {
            param_console_print("OK\r\n");
        } else {
            param_console_print("NG\r\n");
        }
    } else if (strcmp(cmd, "can_load") == 0) {
        param_console_print("Loading CAN config from EEPROM... ");
        if (can_config_load()) {
            param_console_print("OK\r\n");
        } else {
            param_console_print("NG (using defaults)\r\n");
        }
        /* 設定が変わったのでフィルタを更新 */
        can_update_rx_filters();
    } else if (strcmp(cmd, "exit") == 0) {
        /* Issue #65: CAN設定が変更された場合のみフィルタを更新 */
        if (can_config_changed) {
            can_update_rx_filters();
            can_config_changed = false;
        }
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
    can_config_changed = false;  /* Issue #65 */
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
