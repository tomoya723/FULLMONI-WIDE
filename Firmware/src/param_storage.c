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

    /* EEPROMから読み込み */
    eeprom_read(EEPROM_ADDR_PARAM, buf, PARAM_STORAGE_SIZE);

    loaded = (PARAM_Storage_t *)buf;

    /* CRC検証 (crc16フィールドを除いた部分) */
    calc_crc = param_calc_crc16(buf, PARAM_STORAGE_SIZE - sizeof(uint16_t));

    if (calc_crc == loaded->crc16) {
        /* CRC OK - パラメータをコピー */
        memcpy(&g_param, loaded, sizeof(PARAM_Storage_t));

        /* グローバル変数に同期 */
        sp_int = g_param.odo_pulse;
        tr_int = g_param.trip_pulse;

        return true;
    } else {
        /* CRCエラー - デフォルト値使用 */
        param_storage_init();
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

    /* 即時保存 */
    param_storage_save();
}

/* ============================================================
 * TRIPリセット
 * ============================================================ */
void param_storage_reset_trip(void)
{
    g_param.trip_pulse = sp_int;
    tr_int = g_param.trip_pulse;

    /* 即時保存 */
    param_storage_save();
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
