/*
 * param_storage.c
 *
 * パラメータ保存・読み込み機能
 * EEPROM (RIIC0経由) へのパラメータ永続化
 *
 *  Created on: 2026/01/06
 *      Author: FULLMONI-WIDE Project
 */

#include "param_storage.h"
#include "r_smc_entry.h"
#include <string.h>

/* ============================================================
 * 外部変数参照 (main.cより)
 * ============================================================ */
extern volatile uint8_t I2C0_TX_END_FLG;
extern volatile uint8_t I2C0_RX_END_FLG;
extern volatile unsigned long sp_int, tr_int;

/* ============================================================
 * デフォルト値定義
 * ============================================================ */
static const PARAM_Storage_t PARAM_DEFAULT = {
    /* タイヤスペック (195/50R15) */
    .tyre_width   = 195,
    .tyre_aspect  = 50,
    .tyre_rim     = 15,

    /* ギア比 (現行dataregister.cより) */
    .gear_ratio   = {3760, 2269, 1645, 1257, 1000, 843},
    .final_gear_ratio = 4300,

    /* 警告閾値 */
    .water_temp_low  = 60,
    .water_temp_high = 100,
    .fuel_warn_level = 10,

    /* シフトインジケータ閾値 [rpm] */
    .shift_rpm1 = 5500,
    .shift_rpm2 = 6000,
    .shift_rpm3 = 6500,
    .shift_rpm4 = 7000,
    .shift_rpm5 = 7500,

    /* 距離計 */
    .odo_pulse  = 0,
    .trip_pulse = 0,

    /* CRC (初期値) */
    .crc16 = 0x0000
};

/* ============================================================
 * グローバル変数
 * ============================================================ */
PARAM_Storage_t g_param;

/* I2C送受信バッファ */
static uint8_t i2c_buf[64];

/* ============================================================
 * CRC16計算 (CCITT)
 * ============================================================ */
uint16_t param_calc_crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;

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

/* ============================================================
 * I2C EEPROM 書き込み (ページ単位)
 * ============================================================ */
static bool eeprom_write(uint16_t addr, const uint8_t *data, uint16_t len)
{
    uint16_t i;

    /* 先頭2バイトはアドレス */
    i2c_buf[0] = (addr >> 8) & 0xFF;
    i2c_buf[1] = addr & 0xFF;

    /* データコピー */
    for (i = 0; i < len && i < 62; i++) {
        i2c_buf[2 + i] = data[i];
    }

    /* I2C送信 */
    I2C0_TX_END_FLG = 0;
    R_Config_RIIC0_Master_Send(EEPROM_I2C_ADDR, (void *)i2c_buf, 2 + len);
    while (I2C0_TX_END_FLG == 0);

    /* EEPROM書き込み待ち (約5ms) */
    for (volatile int wait = 0; wait < 50000; wait++);

    return true;
}

/* ============================================================
 * I2C EEPROM 読み込み
 * ============================================================ */
static bool eeprom_read(uint16_t addr, uint8_t *data, uint16_t len)
{
    /* アドレス送信 */
    i2c_buf[0] = (addr >> 8) & 0xFF;
    i2c_buf[1] = addr & 0xFF;

    I2C0_TX_END_FLG = 0;
    R_Config_RIIC0_Master_Send_Without_Stop(EEPROM_I2C_ADDR, (void *)i2c_buf, 2);
    while (I2C0_TX_END_FLG == 0);

    /* データ読み込み */
    I2C0_RX_END_FLG = 0;
    R_Config_RIIC0_Master_Receive(EEPROM_I2C_ADDR, (void *)data, len);
    while (I2C0_RX_END_FLG == 0);

    return true;
}

/* ============================================================
 * レガシー領域（0x0000）への保存
 * main.cの起動シーケンスで読み込まれる形式に【完全に】合わせる
 * main.cの形式: I2C_WriteData[14] で14バイト送信
 *   [0]=0x00(アドレス上位、固定), [1-4]=wr_cnt, [5-8]=sp_int, [9-12]=tr_int, [13]=0x00
 * ============================================================ */
static void save_legacy_eeprom(void)
{
    uint8_t legacy_buf[14];
    extern volatile unsigned long wr_cnt;  /* main.cの書き込みカウンタを参照 */

    /* main.cと【完全に同じ形式】: 14バイト送信 */
    legacy_buf[0]  = 0x00;  /* アドレス上位 (main.cのI2C_WriteData[0]と同じ) */
    legacy_buf[1]  = (wr_cnt >> 24) & 0xFF;
    legacy_buf[2]  = (wr_cnt >> 16) & 0xFF;
    legacy_buf[3]  = (wr_cnt >>  8) & 0xFF;
    legacy_buf[4]  = (wr_cnt      ) & 0xFF;
    legacy_buf[5]  = (sp_int >> 24) & 0xFF;
    legacy_buf[6]  = (sp_int >> 16) & 0xFF;
    legacy_buf[7]  = (sp_int >>  8) & 0xFF;
    legacy_buf[8]  = (sp_int      ) & 0xFF;
    legacy_buf[9]  = (tr_int >> 24) & 0xFF;
    legacy_buf[10] = (tr_int >> 16) & 0xFF;
    legacy_buf[11] = (tr_int >>  8) & 0xFF;
    legacy_buf[12] = (tr_int      ) & 0xFF;
    legacy_buf[13] = 0x00;  /* main.cのI2C_WriteData[13]と同じ */

    /* I2C送信 (14バイト: main.cと同じ) */
    I2C0_TX_END_FLG = 0;
    R_Config_RIIC0_Master_Send(EEPROM_I2C_ADDR, (void *)legacy_buf, 14);
    while (I2C0_TX_END_FLG == 0);

    /* EEPROM書き込み待ち (約5ms) */
    for (volatile int wait = 0; wait < 50000; wait++);
}

/* ============================================================
 * パラメータ初期化
 * ============================================================ */
void param_storage_init(void)
{
    memcpy(&g_param, &PARAM_DEFAULT, sizeof(PARAM_Storage_t));
}

/* ============================================================
 * EEPROMからパラメータを読み込み
 * ============================================================ */
bool param_storage_load(void)
{
    uint8_t buf[PARAM_STORAGE_SIZE];
    uint16_t calc_crc;
    PARAM_Storage_t *loaded;

    /* 現在のsp_int/tr_intを保存（旧形式で読み込み済みの場合に備える） */
    uint32_t saved_sp_int = sp_int;
    uint32_t saved_tr_int = tr_int;

    /* EEPROMから読み込み */
    eeprom_read(EEPROM_ADDR_PARAM, buf, PARAM_STORAGE_SIZE);

    loaded = (PARAM_Storage_t *)buf;

    /* CRC検証 (crc16フィールドを除いた部分) */
    calc_crc = param_calc_crc16(buf, PARAM_STORAGE_SIZE - sizeof(uint16_t));

    if (calc_crc == loaded->crc16) {
        /* CRC OK - パラメータをコピー */
        memcpy(&g_param, loaded, sizeof(PARAM_Storage_t));

        /*
         * ODO/TRIPは旧形式（0x0000）を正とする
         * 新形式に保存されている値は使用しない（互換性のため）
         * sp_int/tr_intは変更しない
         */
        g_param.odo_pulse = saved_sp_int;
        g_param.trip_pulse = saved_tr_int;

        return true;
    } else {
        /* CRCエラー - デフォルト値使用（但しODO/TRIPは旧形式の値を維持） */
        param_storage_init();

        /* 旧形式で読み込んだODO/TRIPがあれば復元 */
        g_param.odo_pulse = saved_sp_int;
        g_param.trip_pulse = saved_tr_int;

        return false;
    }
}

/* ============================================================
 * EEPROMにパラメータを保存
 * ============================================================ */
bool param_storage_save(void)
{
    uint8_t buf[PARAM_STORAGE_SIZE];

    /* グローバル変数から同期 */
    g_param.odo_pulse = sp_int;
    g_param.trip_pulse = tr_int;

    /* CRC計算 */
    g_param.crc16 = param_calc_crc16((uint8_t *)&g_param,
                                     PARAM_STORAGE_SIZE - sizeof(uint16_t));

    /* バッファにコピー */
    memcpy(buf, &g_param, PARAM_STORAGE_SIZE);

    /* EEPROMに書き込み (分割書き込み: 32バイト単位) */
    uint16_t offset = 0;
    uint16_t remaining = PARAM_STORAGE_SIZE;
    uint16_t chunk;

    while (remaining > 0) {
        chunk = (remaining > 32) ? 32 : remaining;
        eeprom_write(EEPROM_ADDR_PARAM + offset, buf + offset, chunk);
        offset += chunk;
        remaining -= chunk;
    }

    return true;
}

/* ============================================================
 * デフォルト値にリセット
 * ============================================================ */
void param_storage_reset_default(void)
{
    /* ODO/TRIPは維持してデフォルト値をコピー */
    uint32_t saved_odo = g_param.odo_pulse;
    uint32_t saved_trip = g_param.trip_pulse;

    memcpy(&g_param, &PARAM_DEFAULT, sizeof(PARAM_Storage_t));

    g_param.odo_pulse = saved_odo;
    g_param.trip_pulse = saved_trip;
}

/* ============================================================
 * ODO値を初期化（km指定）
 * ============================================================ */
void param_storage_set_odo(uint32_t km)
{
    g_param.odo_pulse = km * PULSE_PER_KM;
    sp_int = g_param.odo_pulse;

    /* TRIP基準値も更新 */
    g_param.trip_pulse = g_param.odo_pulse;
    tr_int = g_param.trip_pulse;

    /* 即時保存（新フォーマット） */
    param_storage_save();

    /* レガシー領域（0x0000）にも保存 - 起動時に読まれるため必須 */
    save_legacy_eeprom();
}

/* ============================================================
 * TRIPリセット
 * TRIP = (sp_int - tr_int) なので、tr_int = sp_int にすることでTRIP=0
 * ============================================================ */
void param_storage_reset_trip(void)
{
    /* 現在のODOパルス値をTRIP基準値に設定 → TRIP = 0 */
    tr_int = sp_int;
    g_param.trip_pulse = sp_int;

    /* 即時保存（save内で再同期されるのでtr_intを先に設定） */
    param_storage_save();
}

/* ============================================================
 * 書き込みカウンタをリセット
 * ============================================================ */
void param_storage_reset_wr_cnt(void)
{
    extern volatile unsigned long wr_cnt;
    wr_cnt = 0;
    save_legacy_eeprom();
}

/* ============================================================
 * ODO値取得 [km]
 * ============================================================ */
uint32_t param_storage_get_odo_km(void)
{
    return sp_int / PULSE_PER_KM;
}

/* ============================================================
 * TRIP値取得 [km * 10]
 * ============================================================ */
uint32_t param_storage_get_trip_km10(void)
{
    return ((sp_int - tr_int) * 10) / PULSE_PER_KM;
}

/* ============================================================
 * デバッグ用：レガシーEEPROM(0x0000)の内容をダンプ
 * ============================================================ */
extern void param_console_printf(const char *fmt, ...);

void debug_dump_legacy_eeprom(void)
{
    uint8_t buf[16];
    uint32_t legacy_sp_int, legacy_tr_int, legacy_wr_cnt;

    /* レガシー領域を読み込み (アドレス0x0000から12バイト) */
    eeprom_read(0x0000, buf, 12);

    /* パース (ビッグエンディアン) */
    legacy_wr_cnt = ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
                    ((uint32_t)buf[2] << 8)  | (uint32_t)buf[3];
    legacy_sp_int = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16) |
                    ((uint32_t)buf[6] << 8)  | (uint32_t)buf[7];
    legacy_tr_int = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[9] << 16) |
                    ((uint32_t)buf[10] << 8) | (uint32_t)buf[11];

    param_console_printf("=== Legacy EEPROM (0x0000) ===\r\n");
    param_console_printf("Raw: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
           buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
           buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);
    param_console_printf("wr_cnt: %lu\r\n", legacy_wr_cnt);
    param_console_printf("sp_int: %lu (= %lu km)\r\n", legacy_sp_int, legacy_sp_int / PULSE_PER_KM);
    param_console_printf("tr_int: %lu (= %lu km)\r\n", legacy_tr_int, legacy_tr_int / PULSE_PER_KM);
    param_console_printf("=== Current RAM ===\r\n");
    param_console_printf("sp_int: %lu (= %lu km)\r\n", sp_int, sp_int / PULSE_PER_KM);
    param_console_printf("tr_int: %lu (= %lu km)\r\n", tr_int, tr_int / PULSE_PER_KM);
    param_console_printf("g_param.odo_pulse:  %lu (= %lu km)\r\n", g_param.odo_pulse, g_param.odo_pulse / PULSE_PER_KM);
    param_console_printf("g_param.trip_pulse: %lu (= %lu km)\r\n", g_param.trip_pulse, g_param.trip_pulse / PULSE_PER_KM);}
