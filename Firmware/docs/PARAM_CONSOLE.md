# FULLMONI-WIDE パラメータコンソール設計仕様書

## 1. 概要

SCI9（UART）経由でターミナルから内部パラメータを書き換え、EEPROM（RIIC0経由）に保存する機能。
emWinの画面描画と排他制御を行い、パラメータ変更モード時はLCD更新を停止する。

### 1.1 機能概要
- UARTターミナル経由でのパラメータ設定・確認
- EEPROM永続化（CRC16チェックサム付き）
- RTC日時設定
- ODO/TRIP管理
- emWinとの排他制御

---

## 2. システム構成

### 2.1 動作モード

| モード | 説明 | emWin | CAN |
|--------|------|-------|-----|
| **MODE_NORMAL** | 通常動作（メーター表示） | ✅ 有効 | ✅ 有効 |
| **MODE_PARAM** | パラメータ変更モード | ❌ 停止 | ✅ 有効 |

### 2.2 モード切替シーケンス

```
[通常モード]
    │
    │ SCI9受信割り込み（任意キー入力）
    ▼
┌─────────────────────────────────┐
│ 1. g_system_mode = MODE_PARAM  │
│ 2. g_param_mode_active = 1     │
│ 3. emWin停止（GUI_Exec停止）   │
│ 4. コンソールメニュー表示      │
└─────────────────────────────────┘
    │
    │ ターミナルコマンド処理
    ▼
┌─────────────────────────────────┐
│ exitコマンド実行               │
│ 1. g_system_mode = MODE_NORMAL │
│ 2. g_param_mode_active = 0     │
│ 3. emWin再開                   │
└─────────────────────────────────┘
```

---

## 3. ファイル構成

### 3.1 新規追加ファイル

| ファイル | 説明 |
|----------|------|
| `src/param_storage.h` | パラメータ構造体定義、EEPROM関数宣言 |
| `src/param_storage.c` | EEPROM読み書き、CRC16計算、ODO/TRIP管理 |
| `src/param_console.h` | コンソール関数宣言、バッファサイズ定義 |
| `src/param_console.c` | UARTコンソール処理、コマンドパーサー |

### 3.2 変更ファイル

| ファイル | 変更内容 |
|----------|----------|
| `src/settings.h` | `SYSTEM_MODE` enum追加、グローバル変数宣言 |
| `src/main.c` | モード切替ロジック、パラメータ初期化、SCI9受信開始 |
| `src/smc_gen/Config_SCI9/Config_SCI9_user.c` | 継続受信処理、リングバッファ連携 |
| `src/dataregister.h` | `data_setLCD50ms` プロトタイプ追加 |

---

## 4. パラメータ構造体

### 4.1 PARAM_Storage_t

```c
typedef struct {
    /* タイヤスペック */
    uint16_t tyre_width;            /* タイヤ幅 [mm] */
    uint16_t tyre_aspect;           /* 扁平率 [%] */
    uint16_t tyre_rim;              /* リム径 [inch] */

    /* ギア比 (x1000) */
    uint16_t gear_ratio[6];         /* 1-6速ギア比 */
    uint16_t final_gear_ratio;      /* ファイナルギア比 */

    /* 警告閾値 */
    int16_t water_temp_low;         /* 水温警告低 [℃] */
    int16_t water_temp_high;        /* 水温警告高 [℃] */
    uint16_t fuel_warn_level;       /* 燃料警告レベル [%] */

    /* 距離計 */
    uint32_t odo_pulse;             /* ODOパルス累積値 */
    uint32_t trip_pulse;            /* TRIPパルス基準値 */

    /* チェックサム */
    uint16_t crc16;                 /* CRC16チェックサム */
} PARAM_Storage_t;
```

### 4.2 デフォルト値

| パラメータ | デフォルト値 | 備考 |
|-----------|-------------|------|
| tyre_width | 195 | mm |
| tyre_aspect | 50 | % |
| tyre_rim | 15 | inch |
| gear_ratio[0] | 3760 | 3.760 |
| gear_ratio[1] | 2269 | 2.269 |
| gear_ratio[2] | 1645 | 1.645 |
| gear_ratio[3] | 1257 | 1.257 |
| gear_ratio[4] | 1000 | 1.000 |
| gear_ratio[5] | 843 | 0.843 |
| final_gear_ratio | 4300 | 4.300 |
| water_temp_low | 60 | ℃ |
| water_temp_high | 100 | ℃ |
| fuel_warn_level | 10 | % |

---

## 5. UARTプロトコル

### 5.1 通信設定

| 項目 | 設定値 |
|------|--------|
| ボーレート | 115200 bps |
| データビット | 8 bit |
| パリティ | なし |
| ストップビット | 1 bit |
| フロー制御 | なし |

### 5.2 コマンド一覧

| コマンド | 説明 | EEPROM |
|----------|------|--------|
| `help` | コマンド一覧表示 | - |
| `list` | 全パラメータ表示 | - |
| `set <id> <value>` | パラメータ設定 | - |
| `save` | EEPROMに保存 | ✅ Write |
| `load` | EEPROMから読み込み | ✅ Read |
| `default` | デフォルト値復元（ODO/TRIP維持） | - |
| `rtc` | 現在日時表示 | - |
| `rtc YY MM DD hh mm ss` | RTC日時設定 | - |
| `odo` | ODO値表示 | - |
| `odo_init <km>` | ODO初期化 | ✅ Write |
| `trip` | TRIP値表示 | - |
| `trip_reset` | TRIPリセット（0km） | ✅ Write |
| `exit` | 通常モードへ戻る | - |

### 5.3 パラメータID一覧

| ID | 対象 | 値の形式 |
|----|------|----------|
| `tyre_width` | タイヤ幅 | mm (整数) |
| `tyre_aspect` | 扁平率 | % (整数) |
| `tyre_rim` | リム径 | inch (整数) |
| `gear1` ~ `gear6` | ギア比 | x1000 (例: 3760 = 3.760) |
| `final` | ファイナルギア | x1000 |
| `water_low` | 水温警告低 | ℃ (整数) |
| `water_high` | 水温警告高 | ℃ (整数) |
| `fuel_warn` | 燃料警告 | % (整数) |

### 5.4 使用例

```
> help
=== FULLMONI-WIDE Parameter Console ===
Commands:
  help              - Show this help
  list              - Show all parameters
  ...

> list
=== Current Parameters ===
Tyre: 195/50 R15
Gear1: 3.760  Gear2: 2.269  Gear3: 1.645
Gear4: 1.257  Gear5: 1.000  Gear6: 0.843
Final: 4.300
Water Temp Warning: 60 - 100 C
Fuel Warning: 10 %
ODO: 184578 km  TRIP: 0.0 km

> set tyre_width 205
tyre_width = 205

> rtc 26 01 06 10 30 00
RTC set: 2026/01/06 10:30:00

> trip_reset
TRIP reset: 0.0 km

> save
Saving to EEPROM... OK

> exit
Exiting parameter mode...
```

---

## 6. EEPROMメモリマップ

| アドレス | サイズ | 内容 |
|----------|--------|------|
| 0x0000-0x000B | 12 | 既存ODO/TRIP（互換用） |
| 0x0010-0x003F | 48 | PARAM_Storage_t |

### 6.1 CRC16

- アルゴリズム: CRC-CCITT (0x1021)
- 初期値: 0xFFFF
- 対象: 構造体全体（crc16フィールドを除く）

---

## 7. ODO/TRIP計算

### 7.1 変換係数

```
1 km = 2548 パルス (PULSE_PER_KM)
```

### 7.2 計算式

```c
ODO [km] = sp_int / PULSE_PER_KM
TRIP [km] = (sp_int - tr_int) / PULSE_PER_KM
```

### 7.3 TRIPリセット

TRIPを0にするには、`tr_int = sp_int` に設定する。

```c
void param_storage_reset_trip(void)
{
    tr_int = sp_int;           // 現在のODOパルス値をTRIP基準に
    g_param.trip_pulse = sp_int;
    param_storage_save();      // EEPROM保存
}
```

---

## 8. 注意事項

### 8.1 emWin排他制御

- パラメータモード中は `GUI_Exec1()` と `APPW_Exec()` をスキップ
- CAN処理 (`main_CAN()`) は継続動作

### 8.2 defaultコマンド

- ODO/TRIPは**維持される**（誤操作防止）
- タイヤ、ギア比、警告閾値のみリセット

### 8.3 EEPROM書き込み

- `save`, `odo_init`, `trip_reset` 実行時のみ書き込み
- 書き込み回数を抑制するため、即時保存は限定的

---

## 9. 改版履歴

| 日付 | バージョン | 内容 |
|------|-----------|------|
| 2026/01/06 | 1.0 | 初版作成 |
