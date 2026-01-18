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
 * 24C16: アドレスの上位3ビットはI2Cデバイスアドレスに含める
 * ============================================================ */
static bool eeprom_write(uint16_t addr, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint32_t timeout;
    uint8_t i2c_addr;

    /* 24C16: アドレスの上位3ビットをI2Cアドレスに含める */
    i2c_addr = EEPROM_I2C_ADDR | ((addr >> 8) & 0x07);

    /* 先頭1バイトはアドレス（下位8ビットのみ） */
    i2c_buf[0] = addr & 0xFF;

    /* データコピー */
    for (i = 0; i < len && i < 63; i++) {
        i2c_buf[1 + i] = data[i];
    }

    /* I2C送信 */
    I2C0_TX_END_FLG = 0;
    R_Config_RIIC0_Master_Send(i2c_addr, (void *)i2c_buf, 1 + len);
    
    /* タイムアウト付き待機 */
    timeout = 1000000;
    while (I2C0_TX_END_FLG == 0 && timeout > 0) {
        timeout--;
    }
    if (timeout == 0) {
        return false;
    }

    /* EEPROM書き込み待ち (約10ms) */
    for (volatile int wait = 0; wait < 100000; wait++);

    return true;
}

/* ============================================================
 * I2C EEPROM 読み込み
 * 24C16: アドレスの上位3ビットはI2Cデバイスアドレスに含める
 * ============================================================ */
static bool eeprom_read(uint16_t addr, uint8_t *data, uint16_t len)
{
    uint8_t i2c_addr;

    /* 24C16: アドレスの上位3ビットをI2Cアドレスに含める */
    i2c_addr = EEPROM_I2C_ADDR | ((addr >> 8) & 0x07);

    /* アドレス送信（下位8ビットのみ） */
    i2c_buf[0] = addr & 0xFF;

    I2C0_TX_END_FLG = 0;
    R_Config_RIIC0_Master_Send_Without_Stop(i2c_addr, (void *)i2c_buf, 1);
    while (I2C0_TX_END_FLG == 0);

    /* データ読み込み */
    I2C0_RX_END_FLG = 0;
    R_Config_RIIC0_Master_Receive(i2c_addr, (void *)data, len);
    while (I2C0_RX_END_FLG == 0);

    return true;
}

/* ============================================================
 * レガシー領域（0x0000）への保存
 * main.cの起動シーケンスで読み込まれる形式に【完全に】合わせる
 *
 * main.cでの読み込み:
 *   I2C_BUF[0,1] = {0x00, 0x00} をアドレスとして送信
 *   I2C_BUF2に12バイト読み込み
 *   wr_cnt = I2C_BUF2[0-3], sp_int = I2C_BUF2[4-7], tr_int = I2C_BUF2[8-11]
 *
 * main.cでの書き込み:
 *   I2C_WriteData[14] = {0x00, wr_cnt[4], sp_int[4], tr_int[4], 0x00}
 *   I2C_WriteData[0] = 0x00 (固定、アドレス上位)
 *   I2C_WriteData[1-4] = wr_cnt (※アドレス下位を上書きしている！)
 *   I2C_WriteData[5-8] = sp_int
 *   I2C_WriteData[9-12] = tr_int
 *   I2C_WriteData[13] = 0x00 (固定)
 *
 * ★重要: main.cはI2C_WriteData[1]にwr_cnt上位バイトを入れているため、
 *   wr_cnt < 0x01000000 の場合、実質アドレス0x0000に書き込まれ、
 *   EEPROMには [wr_cnt>>16, wr_cnt>>8, wr_cnt, sp_int..., tr_int..., 0x00] が保存される。
 *   これは読み込み時の解釈と一致しないバグだが、互換性のため同じ形式で保存する。
 * ============================================================ */
static void save_legacy_eeprom(void)
{
    uint8_t legacy_buf[13];  // 24C16: 1バイトアドレス + 12バイトデータ
    extern volatile unsigned long wr_cnt;  /* main.cの書き込みカウンタを参照 */

    /*
     * 24C16用フォーマット:
     * legacy_buf[0] = アドレス (0x00)
     * legacy_buf[1-4] = wr_cnt (4バイト)
     * legacy_buf[5-8] = sp_int (4バイト)
     * legacy_buf[9-12] = tr_int (4バイト)
     */
    legacy_buf[0]  = 0x00;  /* アドレス (24C16は1バイトアドレス) */
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

    /* I2C送信 (13バイト: 1バイトアドレス + 12バイトデータ) */
    I2C0_TX_END_FLG = 0;
    R_Config_RIIC0_Master_Send(EEPROM_I2C_ADDR, (void *)legacy_buf, 13);
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
    /* Note: can_config_init()はmain.cでInit_CAN()の前に呼ばれる */
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

    /* EEPROMに書き込み (分割書き込み: 16バイト単位 - 24C16ページサイズ) */
    uint16_t offset = 0;
    uint16_t remaining = PARAM_STORAGE_SIZE;
    uint16_t chunk;

    while (remaining > 0) {
        chunk = (remaining > 16) ? 16 : remaining;
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

/* ============================================================
 * CAN設定 (Issue #65)
 * ============================================================ */

/* CAN設定グローバル変数 (RAM2領域に配置) */
CAN_Config_t g_can_config __attribute__((section(".bss2")));

/* EEPROMアドレス (CAN設定用) */
#define EEPROM_ADDR_CAN_CONFIG  0x0100  /* CAN設定領域開始 */

/* MoTeC M100 プリセット (デフォルト) */
static const CAN_Config_t CAN_PRESET_MOTEC = {
    .version = CAN_CONFIG_VERSION,
    .preset_id = CAN_PRESET_MOTEC_M100,
    .reserved = 0,
    .channels = {
        { .can_id = 0x3E8, .enabled = 1, .reserved = 0 },  /* CH1: MoTeC #1 */
        { .can_id = 0x3E9, .enabled = 1, .reserved = 0 },  /* CH2: MoTeC #2 */
        { .can_id = 0x3EA, .enabled = 1, .reserved = 0 },  /* CH3: MoTeC #3 */
        { .can_id = 0x3EB, .enabled = 1, .reserved = 0 },  /* CH4: MoTeC #4 */
        { .can_id = 0x3EC, .enabled = 1, .reserved = 0 },  /* CH5: MoTeC #5 */
        { .can_id = 0x3ED, .enabled = 0, .reserved = 0 },  /* CH6: 未使用 */
    },
    .fields = {
        /* CH1 (0x3E8): RPM(0-1), TPS(2-3), MAP(4-5), IAT(6-7) */
        { .channel = 1, .start_byte = 0, .byte_count = 2, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_REV, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "REV", .unit = "rpm", .warn_enabled = 1, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = 8000 },
        { .channel = 1, .start_byte = 4, .byte_count = 2, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NUM4, .offset = 0, .multiplier = 1000, .divisor = 10000,
          .name = "MAP", .unit = "kPa", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },  /* MAP /10 */
        { .channel = 1, .start_byte = 6, .byte_count = 2, .data_type = 1, .endian = 0,
          .target_var = CAN_TARGET_NUM2, .offset = 0, .multiplier = 1000, .divisor = 10000,
          .name = "INTAKE", .unit = "C", .warn_enabled = 1, .reserved = 0,
          .warn_low = -40, .warn_high = 80 },  /* IAT /10 signed */
        /* CH2 (0x3E9): ECT(0-1), AFR(2-3) */
        { .channel = 2, .start_byte = 0, .byte_count = 2, .data_type = 1, .endian = 0,
          .target_var = CAN_TARGET_NUM1, .offset = 0, .multiplier = 1000, .divisor = 10000,
          .name = "WATER", .unit = "C", .warn_enabled = 1, .reserved = 0,
          .warn_low = 60, .warn_high = 110 },  /* WaterTemp /10 signed */
        { .channel = 2, .start_byte = 2, .byte_count = 2, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_AF, .offset = 0, .multiplier = 147, .divisor = 1000,
          .name = "A/F", .unit = "afr", .warn_enabled = 1, .reserved = 0,
          .warn_low = 100, .warn_high = 180 },     /* A/F *0.147 */
        /* CH3 (0x3EA): OilTemp(6-7) */
        { .channel = 3, .start_byte = 6, .byte_count = 2, .data_type = 1, .endian = 0,
          .target_var = CAN_TARGET_NUM3, .offset = 0, .multiplier = 1000, .divisor = 10000,
          .name = "OIL", .unit = "C", .warn_enabled = 1, .reserved = 0,
          .warn_low = 60, .warn_high = 130 },  /* OilTemp /10 signed */
        /* CH4 (0x3EB): OilPressure(0-1), BattV(6-7) */
        { .channel = 4, .start_byte = 0, .byte_count = 2, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NUM5, .offset = 0, .multiplier = 1, .divisor = 1000,
          .name = "PRESS", .unit = "kPa", .warn_enabled = 1, .reserved = 0,
          .warn_low = 1, .warn_high = CAN_WARN_DISABLED },     /* OilP *0.001, warn: 1=100kPa */
        { .channel = 4, .start_byte = 6, .byte_count = 2, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NUM6, .offset = 0, .multiplier = 1000, .divisor = 10000,
          .name = "BATT", .unit = "V", .warn_enabled = 1, .reserved = 0,
          .warn_low = 110, .warn_high = 160 },  /* BattV /10 */
        /* 残りは無効 */
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
        { .channel = 0, .start_byte = 0, .byte_count = 0, .data_type = 0, .endian = 0,
          .target_var = CAN_TARGET_NONE, .offset = 0, .multiplier = 1000, .divisor = 1000,
          .name = "", .unit = "", .warn_enabled = 0, .reserved = 0,
          .warn_low = CAN_WARN_DISABLED, .warn_high = CAN_WARN_DISABLED },
    },
    .crc16 = 0
};

/* ============================================================
 * CAN設定初期化
 * ============================================================ */
void can_config_init(void)
{
    memcpy(&g_can_config, &CAN_PRESET_MOTEC, sizeof(CAN_Config_t));
    /* CRCは保存時に計算 */
}

/* ============================================================
 * EEPROMからCAN設定を読み込み
 * ============================================================ */
bool can_config_load(void)
{
    uint8_t buf[CAN_CONFIG_SIZE];
    uint16_t crc_calc, crc_stored;

    /* EEPROM読み込み */
    if (!eeprom_read(EEPROM_ADDR_CAN_CONFIG, buf, CAN_CONFIG_SIZE)) {
        can_config_init();
        return false;
    }

    /* バージョンチェック */
    if (buf[0] != CAN_CONFIG_VERSION) {
        can_config_init();
        return false;
    }

    /* CRCチェック */
    crc_calc = param_calc_crc16(buf, CAN_CONFIG_SIZE - 2);
    /* Little Endian: 下位バイトが先 */
    crc_stored = buf[CAN_CONFIG_SIZE - 2] | (buf[CAN_CONFIG_SIZE - 1] << 8);

    if (crc_calc != crc_stored) {
        can_config_init();
        return false;
    }

    /* 設定を適用 */
    memcpy(&g_can_config, buf, CAN_CONFIG_SIZE);
    return true;
}

/* ============================================================
 * EEPROMにCAN設定を保存
 * ============================================================ */
bool can_config_save(void)
{
    uint16_t crc;
    uint8_t *buf = (uint8_t *)&g_can_config;

    /* バージョンを確実に設定 */
    g_can_config.version = CAN_CONFIG_VERSION;

    /* CRC計算 */
    crc = param_calc_crc16(buf, CAN_CONFIG_SIZE - 2);
    g_can_config.crc16 = crc;

    /* EEPROM書き込み (分割して書き込み) - 24C16のページサイズは16バイト */
    uint16_t offset = 0;
    uint16_t remaining = CAN_CONFIG_SIZE;
    while (remaining > 0) {
        uint16_t chunk = (remaining > 16) ? 16 : remaining;
        if (!eeprom_write(EEPROM_ADDR_CAN_CONFIG + offset, buf + offset, chunk)) {
            return false;
        }
        offset += chunk;
        remaining -= chunk;
    }

    return true;
}

/* ============================================================
 * プリセット適用
 * ============================================================ */
void can_config_apply_preset(uint8_t preset_id)
{
    switch (preset_id) {
    case CAN_PRESET_MOTEC_M100:
    default:
        memcpy(&g_can_config, &CAN_PRESET_MOTEC, sizeof(CAN_Config_t));
        break;
    /* 将来のプリセット追加用 */
    /*
    case CAN_PRESET_LINK_G4:
        memcpy(&g_can_config, &CAN_PRESET_LINK, sizeof(CAN_Config_t));
        break;
    */
    }
}

/* ============================================================
 * CANチャンネル設定
 * ============================================================ */
bool can_config_set_channel(uint8_t ch, uint16_t can_id, uint8_t enabled)
{
    if (ch < 1 || ch > CAN_CHANNEL_MAX) {
        return false;
    }
    if (can_id > 0x7FF) {
        return false;
    }

    g_can_config.channels[ch - 1].can_id = can_id;
    g_can_config.channels[ch - 1].enabled = enabled ? 1 : 0;
    g_can_config.preset_id = CAN_PRESET_CUSTOM;  /* カスタムに変更 */

    return true;
}

/* ============================================================
 * CANフィールド設定
 * ============================================================ */
bool can_config_set_field(uint8_t idx, const CAN_Field_t *field)
{
    if (idx >= CAN_FIELD_MAX) {
        return false;
    }
    if (field->channel > CAN_CHANNEL_MAX) {
        return false;
    }
    if (field->start_byte > 7) {
        return false;
    }
    if (field->byte_count != 1 && field->byte_count != 2 && field->byte_count != 4 && field->byte_count != 0) {
        return false;
    }

    memcpy(&g_can_config.fields[idx], field, sizeof(CAN_Field_t));
    g_can_config.preset_id = CAN_PRESET_CUSTOM;  /* カスタムに変更 */

    return true;
}
