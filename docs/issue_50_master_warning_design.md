# Issue #50: マスターワーニング機能 設計書

## 概要

CANデータフィールドに設定された閾値を監視し、異常値を検出した場合にマスターワーニング（警告表示＋警告音）を発報する機能。

## 機能要件

### 1. 監視対象
- NUM1〜NUM6（水温、吸気温、油温、MAP、油圧、バッテリー電圧）
- REV（エンジン回転数）
- A/F値（空燃比）

### 2. 警告条件
- CANフィールドごとに上限閾値（`warn_high`）と下限閾値（`warn_low`）を設定可能
- 上限/下限それぞれ独立して有効/無効を設定可能（`warn_high_enabled`, `warn_low_enabled`）
- 値が上限を超えた場合（`warn_high_enabled = 1` かつ `値 > warn_high`）にHIGH警告
- 値が下限を下回った場合（`warn_low_enabled = 1` かつ `値 < warn_low`）にLOW警告
- 閾値が `---`（無効値: `-1e30f`）の場合、その方向の監視は無効

### 3. 閾値の単位
- **閾値は表示単位で設定**（ユーザーが画面で見る値と同じ）
- 例：A/F = 14.7 AFR で閾値 10.0〜18.0 AFR と設定

#### 背景：内部値と表示値の関係（AppWizardの仕様）
AppWizardの `APPW_SetVarData()` は**整数値のみ**受け付けます。
小数点表示は AppWizard の Decimal Mode（Mask: `###.#`）で実現しており、
内部値147が画面では14.7と表示されます。

| パラメータ | 内部値 | AppWizard Mask | 表示値 | 閾値設定 |
|------------|--------|----------------|--------|----------|
| A/F | 147 | `###.#` | 14.7 AFR | 10.0〜18.0 |
| OIL-P | 40 | `##.#` | 4.0 x100kPa | 1.5〜9.0 |
| BATT | 142 | `##.#` | 14.2 V | 11.0〜16.0 |

警告判定時は `get_field_current_value()` 関数で内部値を表示単位（÷10）に変換してから閾値と比較します。
これはAppWizardの整数制約に基づく設計です。
これにより、ユーザーは画面で見ている値と同じ単位で閾値を設定できます。

### 4. 警告表示
- 画面下部に赤いエリアが出現し警告を知らせる
- AppWizard変数 `ID_VAR_PRM` で表示/非表示を制御（0=正常, 1=警告）
- `TEXT_SetText()` で警告テキストを設定（Firmware側から直接制御）
- 警告テキストウィジェットに警告対象のパラメータ名、HIGH/LOW、および**ピーク値**を表示
- 例: 「OIL-T HIGH 120」「OIL-P LOW 1.0」
- **ピーク値追跡**: HIGH警告は最大値、LOW警告は最小値を追跡・表示
- **小数点対応**: `decimal_shift` に基づき0〜2桁の小数点表示
- **背景色**: HIGH/LOW 両方とも赤背景（0xFFFF0000）で統一
- **ラッチ機能**: 警告発生後、条件が解消しても10秒間は表示を継続（ラッチ中もピーク値更新）
- **メッセージ最大長**: 24バイト（`MASTER_WARNING_MSG_MAX`）

### 5. 警告音
- 警告が OFF→ON になった瞬間に1回だけ警告音を再生
- `speaker_play_warning()` によるPCM再生
- 継続中は音を鳴らさない（表示のみ継続）

### 6. 警告解除条件
- 全ての監視対象が安全圏内に戻った場合に解除
- **ヒステリシス**を設けてチャタリング防止:
  - 上限警告の場合: 上限閾値の**5%少ない値**で解除 (例: 8000rpm警告 → 7600rpmで解除)
  - 下限警告の場合: 下限閾値の**5%多い値**で解除 (例: 60°C警告 → 63°Cで解除)
- 計算式:
  - 上限解除閾値 = `warn_high * 0.95`
  - 下限解除閾値 = `warn_low * 1.05`

### 7. 起動時の警告抑制
- 電源投入後5秒間は警告を抑制
- エンジン始動前の不安定な値による誤警報を防止

## データ構造

### CAN_Field_t 拡張（param_storage.h）

```c
#define CAN_FIELD_NAME_MAX  8   /* 名前文字列最大長 (7文字+NULL) */
#define CAN_FIELD_UNIT_MAX  8   /* 単位文字列最大長 (7文字+NULL) - "x100kPa"対応 */
#define CAN_WARN_DISABLED   (-1e30f)  /* 閾値無効値 (float型) */

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

    /* === 新規追加フィールド (28 bytes) === */
    uint8_t  decimal_shift;         /* 小数シフト: 0=整数, 1=÷10, 2=÷100, 3=÷1000 */
    uint8_t  reserved;              /* 予約 */
    char     name[CAN_FIELD_NAME_MAX];  /* 名前文字列 (例: "WATER", "OIL-P") */
    char     unit[CAN_FIELD_UNIT_MAX];  /* 単位文字列 (例: "C", "x100kPa", "rpm") */
    uint8_t  warn_low_enabled;      /* 下限ワーニング有効: 0=無効, 1=有効 */
    uint8_t  warn_high_enabled;     /* 上限ワーニング有効: 0=無効, 1=有効 */
    float    warn_low;              /* 下限閾値 (表示単位、これを下回るとワーニング) */
    float    warn_high;             /* 上限閾値 (表示単位、これを超えるとワーニング) */
} CAN_Field_t;  /* 合計: 40 bytes */
```

### メモリ使用量

| 項目 | 変更前 | 変更後 | 増分 |
|------|--------|--------|------|
| CAN_Field_t (1個) | 12 bytes | 40 bytes | +28 bytes |
| CAN_Field_t × 16 | 192 bytes | 640 bytes | +448 bytes |
| CAN_Config_t 合計 | 約220 bytes | 約668 bytes | +448 bytes |

※ CAN_Config_tは `.bss2` セクション (RAM2領域) に配置されており、メインRAM (48KB) への影響なし

### フィールドの説明
- `decimal_shift`: 小数シフト値（0=整数, 1=÷10, 2=÷100, 3=÷1000）- AppWizard Maskとの対応
- `name[8]`: パラメータ名（警告表示用）- "WATER", "OIL-T", "OIL-P", "INTAKE", "MAP", "BATT", "REV", "A/F"
- `unit[8]`: 単位文字列（最大7文字）- "C", "x100kPa", "rpm", "V", "AFR"
- `warn_low_enabled`: 下限警告の有効/無効フラグ
- `warn_high_enabled`: 上限警告の有効/無効フラグ
- `warn_low`: 下限閾値（**表示単位**で設定）
- `warn_high`: 上限閾値（**表示単位**で設定）

### CAN設定バージョン
```c
#define CAN_CONFIG_VERSION  7   /* warning_enabled, sound_enabled フラグ追加 */
```

### バージョン履歴
- VERSION 1: 初期バージョン
- VERSION 2: name, unit, warn_enabled フィールド追加
- VERSION 3: warn_enabled を warn_low_enabled/warn_high_enabled に分離
- VERSION 4: warn_low/warn_high を int16_t から float に変更
- VERSION 5: decimal_shift フィールド追加（内部値→表示値変換を汎用化）
- VERSION 6: unit フィールド拡張（4→8バイト、"x100kPa" 対応）
- VERSION 7: warning_enabled, sound_enabled フラグ追加（個別警告ON/OFF対応）

## データフィールド定義例（MoTeC M400プリセット）

```
データフィールド定義 (最大16個)
┌───┬────────┬──────┬────┬──────┬──────┬──────┬─────┬──────┬─────┬──────┬───────┬─────┬────────┬────┬────────┬────┐
│ # │ Name   │ Unit │ CH │ Byte │ Size │ Type │ End │ Var  │ Off │ Mul  │ Div   │ Dec │ WarnLo │ Lo │ WarnHi │ Hi │
├───┼────────┼──────┼────┼──────┼──────┼──────┼─────┼──────┼─────┼──────┼───────┼─────┼────────┼────┼────────┼────┤
│ 0 │ REV    │ rpm  │ 1  │ 0    │ 2    │ U    │ B   │ REV  │ 0   │ 1000 │ 1000  │ 0   │ ---    │    │ 8000.0 │ ✓  │
│ 1 │ MAP    │ kPa  │ 1  │ 4    │ 2    │ U    │ B   │ NUM4 │ 0   │ 1000 │ 10000 │ 0   │ ---    │    │ ---    │    │
│ 2 │ INTAKE │ C    │ 1  │ 6    │ 2    │ S    │ B   │ NUM2 │ 0   │ 1000 │ 10000 │ 0   │ -40.0  │ ✓  │ 80.0   │ ✓  │
│ 3 │ WATER  │ C    │ 2  │ 0    │ 2    │ S    │ B   │ NUM1 │ 0   │ 1000 │ 10000 │ 0   │ 60.0   │ ✓  │ 110.0  │ ✓  │
│ 4 │ A/F    │ afr  │ 2  │ 2    │ 2    │ U    │ B   │ AF   │ 0   │ 147  │ 1000  │ 1   │ 10.0   │ ✓  │ 18.0   │ ✓  │
│ 5 │ OIL-T  │ C    │ 3  │ 6    │ 2    │ S    │ B   │ NUM3 │ 0   │ 1000 │ 10000 │ 0   │ 60.0   │ ✓  │ 130.0  │ ✓  │
│ 6 │ OIL-P  │x100kPa│ 4  │ 0    │ 2    │ U    │ B   │ NUM5 │ 0   │ 1    │ 1000  │ 1   │ 1.5    │ ✓  │ 9.0    │ ✓  │
│ 7 │ BATT   │ V    │ 4  │ 6    │ 2    │ U    │ B   │ NUM6 │ 0   │ 1000 │ 10000 │ 1   │ 11.0   │ ✓  │ 16.0   │ ✓  │
└───┴────────┴──────┴────┴──────┴──────┴──────┴─────┴──────┴─────┴──────┴───────┴─────┴────────┴────┴────────┴────┘
```

**Dec (decimal_shift)**: AppWizard Mask設定と対応。0=整数表示、1=÷10（"###.#"）、2=÷100（"##.##"）

### 閾値設定の注意点

閾値は**表示単位で設定**します。内部値と表示値が異なるパラメータでも、
ユーザーは画面で見ている値と同じ数値を設定すればOKです。

| パラメータ | 表示単位 | 内部値の例 | 表示値 | 閾値設定例 |
|------------|----------|------------|--------|------------|
| A/F | AFR | 147 | 14.7 | 10.0〜18.0 |
| OIL-P | x100kPa | 40 | 4.0 | 1.5〜9.0 |
| BATT | V | 145 | 14.5 | 9.0〜16.0 |
| 水温等 | °C | 95 | 95 | 60.0〜110.0 |

### カラム説明
| カラム | 説明 | 値の範囲 |
|--------|------|----------|
| # | フィールドインデックス | 0-15 |
| Name | パラメータ名（警告表示用） | 最大7文字 |
| Unit | 単位文字列 | 最大7文字 |
| CH | CANチャンネル | 1-6, 0=無効 |
| Byte | 開始バイト位置 | 0-7 |
| Size | バイト数 | 1, 2, 4 |
| Type | データ型 | U=unsigned, S=signed |
| End | エンディアン | B=Big, L=Little |
| Var | 代入先変数 | REV, AF, NUM1-6, SPEED |
| Off | オフセット | -32768〜32767 |
| Mul | 乗算係数 (×1000) | 0-65535 |
| Div | 除算係数 (×1000) | 1-65535 |
| Dec | 小数シフト (AppWizard Mask対応) | 0=整数, 1=÷10, 2=÷100 |
| WarnLo | 下限閾値 (float) | 値 or --- (無効: -1e30f) |
| Lo | 下限警告有効 | ✓=有効, 空=無効 |
| WarnHi | 上限閾値 (float) | 値 or --- (無効: -1e30f) |
| Hi | 上限警告有効 | ✓=有効, 空=無効 |

## API設計

### master_warning.h

```c
#ifndef MASTER_WARNING_H_
#define MASTER_WARNING_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief マスターワーニングの初期化
 */
void master_warning_init(void);

/**
 * @brief マスターワーニングの状態を更新（毎フレーム呼び出し）
 * @note CANデータ処理後に呼び出すこと
 */
void master_warning_check(void);

/**
 * @brief マスターワーニングの現在状態を取得
 * @return true: ワーニングアクティブ, false: 正常
 */
bool master_warning_is_active(void);

/**
 * @brief AppWizardのワーニング表示を更新
 * @note GUIタスクから呼び出すこと
 */
void master_warning_update_display(void);

/**
 * @brief 現在の警告メッセージを取得
 * @return 警告メッセージ文字列（例: "WATER HIGH"）、警告なしの場合は空文字列
 */
const char* master_warning_get_message(void);

/**
 * @brief 警告フィールドインデックスを取得
 * @return 警告中のフィールドインデックス、警告なしの場合は-1
 */
int8_t master_warning_get_field_index(void);

#endif /* MASTER_WARNING_H_ */
```

## シーケンス

```
[メインループ / CANタスク]
    |
    v
CAN受信データ処理
    |-- g_CALC_data 更新 (内部値、一部10倍)
    v
master_warning_check()
    |-- 起動後5秒は警告抑制
    |-- g_can_config.fields[] をスキャン
    |-- get_field_current_value() で現在値取得
    |   |-- A/F, NUM5(OIL-P), NUM6(BATT) は ÷10 して表示単位に変換
    |-- warn_high_enabled==1 かつ 値 > warn_high でHIGH警告
    |-- warn_low_enabled==1 かつ 値 < warn_low でLOW警告
    |-- s_warning_active = true, s_warning_field_idx 記録
    v
[GUIタスク]
    |
    v
master_warning_update_display()
    |-- 前回状態と比較（立ち上がり検出）
    |-- 立ち上がり: speaker_play_warning() 呼び出し
    |-- APPW_SetVarData(ID_VAR_WARNING, s_warning_active)
    |-- APPW_SetText() で警告メッセージ設定 (例: "WATER HIGH")
    v
[次フレームへ]
```

### 内部値から表示値への変換（get_field_current_value関数）

```c
static float get_field_current_value(uint8_t target_var) {
    float value = 0.0f;
    switch (target_var) {
        case CAN_TARGET_AF:   value = g_CALC_data.af / 10.0f; break;    // AFR
        case CAN_TARGET_NUM5: value = g_CALC_data.num5 / 10.0f; break;  // OIL-P x100kPa
        case CAN_TARGET_NUM6: value = g_CALC_data.num6 / 10.0f; break;  // BATT V
        default:              value = (float)get_raw_value(target_var); break;
    }
    return value;
}
```

## HOSTアプリ連携

### コマンド拡張

#### can_field コマンド（拡張）
```
can_field <idx> <ch> <byte> <size> <type> <end> <var> <off> <mul> <div> <name> <unit> <warn_lo_en> <warn_hi_en> <warn_lo> <warn_hi>
```

#### パラメータ詳細
| パラメータ | 説明 | 例 |
|------------|------|-----|
| idx | フィールドインデックス | 0-15 |
| ch | CANチャンネル | 1-6, 0=無効 |
| byte | 開始バイト | 0-7 |
| size | バイト数 | 1, 2, 4 |
| type | データ型 | 0=unsigned, 1=signed |
| end | エンディアン | 0=Big, 1=Little |
| var | ターゲット変数 | 0-8 (REV, AF, NUM1-6, SPEED) |
| off | オフセット | -32768〜32767 |
| mul | 乗算係数 | 0-65535 |
| div | 除算係数 | 1-65535 |
| name | 名前文字列 | 最大7文字 |
| unit | 単位文字列（英字のみ） | 最大3文字 |
| warn_lo_en | 下限警告有効 | 0=無効, 1=有効 |
| warn_hi_en | 上限警告有効 | 0=無効, 1=有効 |
| warn_lo | 下限閾値 (float) | 値 or -1e30 (無効) |
| warn_hi | 上限閾値 (float) | 値 or -1e30 (無効) |

#### 設定例
```
# フィールド3: 水温 (CH2, byte0, 2bytes, signed, BigEndian, NUM1, 下限60℃/上限110℃警告)
can_field 3 2 0 2 1 0 2 0 1000 10000 WATER C 1 1 60.0 110.0

# フィールド0: 回転数 (CH1, byte0, 2bytes, unsigned, BigEndian, REV, 上限8000rpm警告のみ)
can_field 0 1 0 2 0 0 0 0 1000 1000 REV rpm 0 1 -1e30 8000.0

# フィールド6: 油圧 (CH4, byte0, 2bytes, unsigned, BigEndian, NUM5, 下限1.5/上限9.0 x100kPa)
can_field 6 4 0 2 0 0 6 0 1 1000 OIL-P x100 1 1 1.5 9.0
```

※ `warn_lo = -1e30` または `warn_hi = -1e30` はその方向の閾値無効を意味する

### 読み出しコマンド
```
can_field_get <idx>
```
レスポンス:
```
can_field <idx> <ch> <byte> <size> <type> <end> <var> <off> <mul> <div> <name> <unit> <warn_en> <warn_lo> <warn_hi>
```

## AppWizard連携

### 必要な変数（ID_VAR_xxx）
| 変数名 | 説明 | 値 |
|--------|------|-----|
| ID_VAR_WARNING | 警告状態 | 0=正常, 1=警告 |

### 必要なウィジェット
| ウィジェット | 説明 |
|--------------|------|
| ID_TEXT_WARNING | 警告メッセージ表示テキスト |

### 表示制御
1. `ID_VAR_WARNING == 1` のとき、警告テキストウィジェットを表示
2. `ID_VAR_WARNING == 0` のとき、警告テキストウィジェットを非表示
3. 警告メッセージ形式: `"<name> HIGH <peak>"` または `"<name> LOW <peak>"`
   - 例: `"OIL-T HIGH 120"`, `"OIL-P LOW 1.0"`
   - ピーク値は小数点対応（decimal_shiftに基づく）

### 制約事項
- **aw001/aw002フォルダのファイルは編集禁止**
- AppWizardで変数・ウィジェット追加後、エクスポートされたファイルを使用

## 実装手順

### Phase 1: データ構造拡張 ✅完了
1. [x] `param_storage.h` - `CAN_Field_t` 構造体に `name`, `warn_low_enabled`, `warn_high_enabled`, `warn_low`, `warn_high` 追加
2. [x] `CAN_CONFIG_VERSION` を 4 に更新
3. [x] `can_config_init()` / プリセットでデフォルト値設定（表示単位で設定）

### Phase 2: マスターワーニングモジュール ✅完了
1. [x] `master_warning.h` 新規作成
2. [x] `master_warning.c` 新規作成
3. [x] 警告判定ロジック実装（get_field_current_value で表示単位に変換）
4. [x] 警告音再生連携 (`speaker_play_warning()`)
5. [x] 起動後5秒間の警告抑制

### Phase 3: GUI連携 ✅完了
1. [x] AppWizardで `ID_VAR_WARNING` 変数追加（aw001/aw002）
2. [x] AppWizardで警告テキストウィジェット追加
3. [x] `master_warning_update_display()` 実装

### Phase 4: HOSTアプリ連携 ✅完了
1. [x] `param_console.c` - `can_field` コマンド拡張（name, warn_* パラメータ追加）
2. [x] `can_list` コマンドで16フィールド全表示
3. [x] HostApp側の設定画面実装（WPF Terminal）

## 注意事項

1. **名前文字列**: 英数字および "-", "/" のみ、最大7文字（+NULL終端で8バイト）
2. **単位文字列**: 英数字のみ、最大7文字（+NULL終端で8バイト）- "x100kPa" 対応
3. **EEPROM互換性**: CAN_CONFIG_VERSION更新により、旧設定は初期化される
4. **閾値無効値**: `-1e30f` を使用、HostAppでは "---" と表示
5. **閾値の単位**: **表示単位で設定**（画面に表示される値と同じ）
6. **RAM2配置**: CAN_Config_t は `.bss2` セクションに配置、メインRAMへの影響なし
7. **複数警告**: 複数フィールドが同時に閾値超過した場合、最初に検出したものを表示
8. **起動抑制**: 電源投入後5秒間は警告を抑制（エンジン始動前の誤警報防止）

## 変更履歴

| 日付 | バージョン | 内容 |
|------|-----------|------|
| 2026/01/18 | 1.0 | 初版作成（提案仕様） |
| 2026/01/19 | 2.0 | 実装完了、設計書を最新に更新 |
|            |     | - warn_enabled を warn_low_enabled/warn_high_enabled に分離 |
|            |     | - warn_low/warn_high を int16_t から float に変更 |
|            |     | - CAN_CONFIG_VERSION を 4 に更新 |
|            |     | - 閾値は表示単位で設定、内部で変換 |
|            |     | - 起動後5秒間の警告抑制を追加 |
| 2026/01/25 | 3.0 | decimal_shift フィールド追加 |
|            |     | - CAN_CONFIG_VERSION を 5 に更新 |
|            |     | - CAN_Field_t サイズ: 34 bytes → 36 bytes |
|            |     | - 内部値→表示値変換を汎用化（ハードコード削除） |
| 2026/01/25 | 4.0 | unit フィールド拡張、警告表示改善 |
|            |     | - CAN_CONFIG_VERSION を 6 に更新 |
|            |     | - CAN_Field_t サイズ: 36 bytes → 40 bytes |
|            |     | - unit フィールド: 4→8バイト（"x100kPa" 対応） |
|            |     | - HIGH/LOW 両方とも赤背景に統一 |
|            |     | - 警告表示 10秒ラッチ機能追加 |
|            |     | - 警告解除時の緑背景フラッシュ修正 |
| 2026/01/25 | 5.0 | 警告メッセージにピーク値表示機能追加 |
|            |     | - CAN_CONFIG_VERSION を 7 に更新 |
|            |     | - 警告メッセージに数値表示（例: "OIL-T HIGH 120"） |
|            |     | - ピーク値追跡（HIGH警告は最大値、LOW警告は最小値） |
|            |     | - 小数点対応（decimal_shift に基づく0〜2桁） |
|            |     | - MASTER_WARNING_MSG_MAX を 16→24 バイトに拡張 |
|            |     | - ラッチ期間中もピーク値を継続更新 |
|            |     | - 複数警告の3x2レイアウト対応 |
