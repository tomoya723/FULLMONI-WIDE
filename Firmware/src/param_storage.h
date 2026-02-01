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
 * CAN設定定義 (Issue #65)
 * ============================================================ */
#define CAN_CHANNEL_MAX     6       /* CAN受信チャンネル数 */
#define CAN_FIELD_MAX       16      /* CANデータフィールド数 */

/* Issue #50: マスターワーニング用定数 */
#define CAN_FIELD_NAME_MAX  8       /* 名前文字列最大長 (7文字+NULL) */
#define CAN_FIELD_UNIT_MAX  8       /* 単位文字列最大長 (7文字+NULL) "x100kPa"対応 */
#define CAN_WARN_DISABLED   (-1e30f) /* 閾値無効値: float用 */

/* CAN受信チャンネル設定 */
typedef struct __attribute__((packed)) {
    uint16_t can_id;                /* 受信CAN ID (0x000-0x7FF) */
    uint8_t  enabled;               /* 有効/無効 (0:無効, 1:有効) */
    uint8_t  reserved;              /* 予約 (アライメント用) */
} CAN_RX_Channel_t;

/* CANデータフィールド定義 */
typedef struct __attribute__((packed)) {
    /* === 既存フィールド (12 bytes) === */
    uint8_t  channel;               /* 使用するCANチャンネル (1-6, 0=無効) */
    uint8_t  start_byte;            /* 開始バイト位置 (0-7) */
    uint8_t  byte_count;            /* バイト数 (1, 2, 4) */
    uint8_t  data_type;             /* データ型: 0=unsigned, 1=signed */
    uint8_t  endian;                /* エンディアン: 0=Big, 1=Little */
    uint8_t  target_var;            /* 代入先emWin変数ID (内部インデックス) */
    int16_t  offset;                /* オフセット値 (変換前に加算) */
    uint16_t multiplier;            /* 乗算係数 (x1000) 例: 1000=x1, 100=x0.1 */
    uint16_t divisor;               /* 除算係数 (x1000) 例: 1000=/1, 10000=/10 */
    /* === 新規追加フィールド (Issue #50: 24 bytes) === */
    char     name[CAN_FIELD_NAME_MAX];  /* 名前文字列 (例: "WATER", "OIL") */
    char     unit[CAN_FIELD_UNIT_MAX];  /* 単位文字列 (例: "C", "kPa", "rpm") */
    uint8_t  decimal_shift;         /* 小数点シフト量 (0=整数, 1=÷10, 2=÷100) AppWizard Maskに対応 */
    uint8_t  warn_low_enabled;      /* 下限ワーニング有効: 0=無効, 1=有効 */
    uint8_t  warn_high_enabled;     /* 上限ワーニング有効: 0=無効, 1=有効 */
    uint8_t  reserved;              /* 予約 (アライメント用) */
    float    warn_low;              /* 下限閾値 (表示単位、これを下回るとワーニング) */
    float    warn_high;             /* 上限閾値 (表示単位、これを超えるとワーニング) */
} CAN_Field_t;  /* 合計: 40 bytes (unit拡張により+4) */

/* CAN設定全体 (RAM2領域に配置) */
typedef struct __attribute__((packed)) {
    uint8_t          version;                       /* 設定バージョン */
    uint8_t          preset_id;                     /* プリセットID (0=カスタム) */
    uint8_t          warning_enabled;               /* マスターワーニング有効: 0=無効, 1=有効 */
    uint8_t          sound_enabled;                 /* 警告音有効: 0=無効, 1=有効 */
    CAN_RX_Channel_t channels[CAN_CHANNEL_MAX];     /* 受信チャンネル設定 */
    CAN_Field_t      fields[CAN_FIELD_MAX];         /* データフィールド定義 */
    uint16_t         crc16;                         /* CRCチェックサム */
} CAN_Config_t;

/* CAN設定サイズ */
#define CAN_CONFIG_SIZE     sizeof(CAN_Config_t)

/* CAN設定バージョン */
#define CAN_CONFIG_VERSION  8       /* OIL-P divisor修正 (1000→100) */

/* プリセットID */
#define CAN_PRESET_CUSTOM       0
#define CAN_PRESET_MOTEC_M100   1
#define CAN_PRESET_LINK_G4      2
#define CAN_PRESET_AEM_EMS      3

/* ターゲット変数インデックス (emWin ID_VAR_xxに対応) */
#define CAN_TARGET_REV      0       /* ID_VAR_REV: エンジン回転数 */
#define CAN_TARGET_AF       1       /* ID_VAR_AF: A/F比 */
#define CAN_TARGET_NUM1     2       /* ID_VAR_01: 水温 */
#define CAN_TARGET_NUM2     3       /* ID_VAR_02: 吸気温 */
#define CAN_TARGET_NUM3     4       /* ID_VAR_03: 油温 */
#define CAN_TARGET_NUM4     5       /* ID_VAR_04: MAP */
#define CAN_TARGET_NUM5     6       /* ID_VAR_05: 油圧 */
#define CAN_TARGET_NUM6     7       /* ID_VAR_06: バッテリー電圧 */
#define CAN_TARGET_SPEED    8       /* ID_VAR_SPEED: 車速 */
#define CAN_TARGET_NONE     255     /* 未割当 */

/* ============================================================
 * パラメータ構造体定義
 * ============================================================ */
typedef struct __attribute__((packed)) {
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

    /* === シフトインジケータ閾値 [rpm] === */
    uint16_t shift_rpm1;            /* 段階1開始 (青2灯) */
    uint16_t shift_rpm2;            /* 段階2開始 (青4灯) */
    uint16_t shift_rpm3;            /* 段階3開始 (緑6灯) */
    uint16_t shift_rpm4;            /* 段階4開始 (赤8灯) */
    uint16_t shift_rpm5;            /* 段階5開始 (白点滅) */

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
 * CAN設定関連関数 (Issue #65)
 * ============================================================ */

/**
 * @brief CAN設定を初期化（デフォルトプリセット適用）
 */
void can_config_init(void);

/**
 * @brief EEPROMからCAN設定を読み込み
 * @return true: 成功, false: CRCエラー（デフォルト値使用）
 */
bool can_config_load(void);

/**
 * @brief EEPROMにCAN設定を保存
 * @return true: 成功, false: 失敗
 */
bool can_config_save(void);

/**
 * @brief プリセットを適用
 * @param preset_id プリセットID (CAN_PRESET_xxx)
 */
void can_config_apply_preset(uint8_t preset_id);

/**
 * @brief CANチャンネル設定
 * @param ch チャンネル番号 (1-6)
 * @param can_id CAN ID
 * @param enabled 有効/無効
 * @return true: 成功, false: 無効なパラメータ
 */
bool can_config_set_channel(uint8_t ch, uint16_t can_id, uint8_t enabled);

/**
 * @brief CANフィールド設定
 * @param idx フィールドインデックス (0-15)
 * @param field フィールド定義
 * @return true: 成功, false: 無効なパラメータ
 */
bool can_config_set_field(uint8_t idx, const CAN_Field_t *field);

/* ============================================================
 * グローバル変数 (extern)
 * ============================================================ */
extern PARAM_Storage_t g_param;

/* CAN設定 (RAM2領域に配置) */
extern CAN_Config_t g_can_config __attribute__((section(".bss2")));

#endif /* PARAM_STORAGE_H_ */
