/*
 * param_storage.h
 *
 * パラメータ保存・読み込み機能
 * EEPROM (RIIC0経由) へのパラメータ永続化
 *
 *  Created on: 2026/01/06
 *      Author: FULLMONI-WIDE Project
 */

#ifndef PARAM_STORAGE_H_
#define PARAM_STORAGE_H_

#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * パラメータ構造体定義
 * ============================================================ */
typedef struct {
    /* === タイヤスペック === */
    uint16_t tyre_width;            /* タイヤ幅 [mm] */
    uint16_t tyre_aspect;           /* 扁平率 [%] */
    uint16_t tyre_rim;              /* リム径 [inch] */

    /* === ギア比 (x1000) === */
    uint16_t gear_ratio[6];         /* 1-6速ギア比 */
    uint16_t final_gear_ratio;      /* ファイナルギア比 */

    /* === 警告閾値 === */
    int16_t water_temp_low;         /* 水温警告低 [℃] */
    int16_t water_temp_high;        /* 水温警告高 [℃] */
    uint16_t fuel_warn_level;       /* 燃料警告レベル [%] */

    /* === 距離計 === */
    uint32_t odo_pulse;             /* ODOパルス累積値 */
    uint32_t trip_pulse;            /* TRIPパルス基準値 */

    /* === チェックサム === */
    uint16_t crc16;                 /* CRC16チェックサム */
} PARAM_Storage_t;

/* 構造体サイズ */
#define PARAM_STORAGE_SIZE  sizeof(PARAM_Storage_t)

/* パルス→km変換係数 (1km = 2548パルス) */
#define PULSE_PER_KM        2548

/* EEPROMアドレス */
#define EEPROM_ADDR_LEGACY      0x0000  /* 既存ODO/TRIP領域 (互換用) */
#define EEPROM_ADDR_PARAM       0x0010  /* パラメータ領域開始 */
#define EEPROM_I2C_ADDR         0x50    /* EEPROM I2Cアドレス */

/* ============================================================
 * 関数プロトタイプ
 * ============================================================ */

/**
 * @brief パラメータ初期化（デフォルト値設定）
 */
void param_storage_init(void);

/**
 * @brief EEPROMからパラメータを読み込み
 * @return true: 成功, false: CRCエラー（デフォルト値使用）
 */
bool param_storage_load(void);

/**
 * @brief EEPROMにパラメータを保存
 * @return true: 成功, false: 失敗
 */
bool param_storage_save(void);

/**
 * @brief パラメータをデフォルト値にリセット
 */
void param_storage_reset_default(void);

/**
 * @brief ODO値を初期化（km指定）
 * @param km ODO値 [km]
 */
void param_storage_set_odo(uint32_t km);

/**
 * @brief TRIPをリセット（現在ODO値を基準に）
 */
void param_storage_reset_trip(void);

/**
 * @brief 現在のODO値を取得 [km]
 * @return ODO値
 */
uint32_t param_storage_get_odo_km(void);

/**
 * @brief 現在のTRIP値を取得 [km * 10]
 * @return TRIP値 (小数点1桁)
 */
uint32_t param_storage_get_trip_km10(void);

/**
 * @brief CRC16計算
 * @param data データポインタ
 * @param len データ長
 * @return CRC16値
 */
uint16_t param_calc_crc16(const uint8_t *data, uint16_t len);

/* ============================================================
 * グローバル変数 (extern)
 * ============================================================ */
extern PARAM_Storage_t g_param;

#endif /* PARAM_STORAGE_H_ */
