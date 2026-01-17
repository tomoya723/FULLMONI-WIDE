# Issue #50: マスターワーニング機能 提案仕様書

## 概要

CANデータフィールドに設定された閾値を監視し、異常値を検出した場合にマスターワーニング（警告表示＋警告音）を発報する機能。

## 機能要件

### 1. 監視対象
- NUM1〜NUM6（水温、吸気温、油温、MAP、油圧、バッテリー電圧）
- REV（エンジン回転数）
- A/F値（空燃比）

### 2. 警告条件
- CANフィールドごとに上限閾値（`warn_high`）と下限閾値（`warn_low`）を設定可能
- `warn_enabled = 1` のフィールドのみ監視対象
- 値が上限を超えた場合、または下限を下回った場合にワーニングをアクティブ化
- 閾値が `---`（無効値: INT16_MIN = -32768）の場合、その方向の監視は無効

### 3. 警告表示
- 画面下部に赤いエリアが出現し警告を知らせる
- AppWizard変数 `ID_VAR_WARNING` で表示/非表示を制御（0=正常, 1=警告）
- AppWizard変数 `ID_RTEXT_WARNING` で警告テキストを設定（Firmware側から文字列指定）
- 警告テキストウィジェットに警告対象のパラメータ名と HIGH/LOW を表示
- 例: 「WATER HIGH」「OIL LOW」

### 4. 警告音
- 警告が OFF→ON になった瞬間に1回だけ警告音を再生
- `speaker_play_warning()` によるPCM再生
- 継続中は音を鳴らさない（表示のみ継続）

### 5. 警告解除条件
- 全ての監視対象が安全圏内に戻った場合に解除
- **ヒステリシス**を設けてチャタリング防止:
  - 上限警告の場合: 上限閾値の**5%少ない値**で解除 (例: 8000rpm警告 → 7600rpmで解除)
  - 下限警告の場合: 下限閾値の**5%多い値**で解除 (例: 60°C警告 → 63°Cで解除)
- 計算式:
  - 上限解除閾値 = `warn_high * 0.95`
  - 下限解除閾値 = `warn_low * 1.05`

## データ構造

### CAN_Field_t 拡張（param_storage.h）

```c
#define CAN_FIELD_NAME_MAX  8   /* 名前文字列最大長 (7文字+NULL) */
#define CAN_FIELD_UNIT_MAX  4   /* 単位文字列最大長 (3文字+NULL) */
#define CAN_WARN_DISABLED   INT16_MIN  /* 閾値無効値: -32768 */

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
    
    /* === 新規追加フィールド (18 bytes) === */
    char     name[CAN_FIELD_NAME_MAX];  /* 名前文字列 (例: "WATER", "OIL") */
    char     unit[CAN_FIELD_UNIT_MAX];  /* 単位文字列 (例: "C", "kPa", "rpm") */
    uint8_t  warn_enabled;          /* ワーニング有効: 0=無効, 1=有効 */
    uint8_t  reserved;              /* 予約 (アライメント用) */
    int16_t  warn_low;              /* 下限閾値 (これを下回るとワーニング) */
    int16_t  warn_high;             /* 上限閾値 (これを超えるとワーニング) */
} CAN_Field_t;  /* 合計: 30 bytes */
```

### メモリ使用量

| 項目 | 変更前 | 変更後 | 増分 |
|------|--------|--------|------|
| CAN_Field_t (1個) | 12 bytes | 30 bytes | +18 bytes |
| CAN_Field_t × 16 | 192 bytes | 480 bytes | +288 bytes |
| CAN_Config_t 合計 | 約220 bytes | 約508 bytes | +288 bytes |

※ CAN_Config_tは `.bss2` セクション (RAM2領域) に配置されており、メインRAM (48KB) への影響なし

### フィールドの説明
- `name[8]`: パラメータ名（警告表示用）- "WATER", "OIL", "INTAKE", "MAP", "PRESS", "BATT", "REV", "A/F"
- `unit[4]`: 単位文字列（英字のみ、最大3文字）- "C", "kPa", "rpm", "V", "afr"

### CAN設定バージョン
```c
#define CAN_CONFIG_VERSION  2   /* name, warningフィールド追加 */
```

## データフィールド定義例

```
データフィールド定義 (最大16個、使用例8個)
┌───┬────────┬──────┬────┬──────┬──────┬──────┬─────┬──────┬─────┬──────┬───────┬────┬────────┬────────┬────┐
│ # │ Name   │ Unit │ CH │ Byte │ Size │ Type │ End │ Var  │ Off │ Mul  │ Div   │ EN │ WarnLo │ WarnHi │ WE │
├───┼────────┼──────┼────┼──────┼──────┼──────┼─────┼──────┼─────┼──────┼───────┼────┼────────┼────────┼────┤
│ 0 │ REV    │ rpm  │ 1  │ 0    │ 2    │ U    │ B   │ REV  │ 0   │ 1000 │ 1000  │ ✓  │ ---    │ 8000   │ ✓  │
│ 1 │ MAP    │ kPa  │ 1  │ 4    │ 2    │ U    │ B   │ NUM4 │ 0   │ 1000 │ 10000 │ ✓  │ ---    │ ---    │    │
│ 2 │ INTAKE │ C    │ 1  │ 6    │ 2    │ S    │ B   │ NUM2 │ 0   │ 1000 │ 10000 │ ✓  │ -40    │ 80     │ ✓  │
│ 3 │ WATER  │ C    │ 2  │ 0    │ 2    │ S    │ B   │ NUM1 │ 0   │ 1000 │ 10000 │ ✓  │ 60     │ 110    │ ✓  │
│ 4 │ A/F    │ afr  │ 2  │ 2    │ 2    │ U    │ B   │ AF   │ 0   │ 147  │ 1000  │ ✓  │ 100    │ 180    │ ✓  │
│ 5 │ OIL    │ C    │ 3  │ 6    │ 2    │ S    │ B   │ NUM3 │ 0   │ 1000 │ 10000 │ ✓  │ 60     │ 130    │ ✓  │
│ 6 │ PRESS  │ kPa  │ 4  │ 0    │ 2    │ U    │ B   │ NUM5 │ 0   │ 1    │ 1000  │ ✓  │ 100    │ ---    │ ✓  │
│ 7 │ BATT   │ V    │ 4  │ 6    │ 2    │ U    │ B   │ NUM6 │ 0   │ 1000 │ 10000 │ ✓  │ 110    │ 160    │ ✓  │
└───┴────────┴──────┴────┴──────┴──────┴──────┴─────┴──────┴─────┴──────┴───────┴────┴────────┴────────┴────┘
```

### カラム説明
| カラム | 説明 | 値の範囲 |
|--------|------|----------|
| # | フィールドインデックス | 0-15 |
| Name | パラメータ名（警告表示用） | 最大7文字 |
| Unit | 単位文字列（英字のみ） | 最大3文字 |
| CH | CANチャンネル | 1-6, 0=無効 |
| Byte | 開始バイト位置 | 0-7 |
| Size | バイト数 | 1, 2, 4 |
| Type | データ型 | U=unsigned, S=signed |
| End | エンディアン | B=Big, L=Little |
| Var | 代入先変数 | REV, AF, NUM1-6, SPEED |
| Off | オフセット | -32768〜32767 |
| Mul | 乗算係数 (×1000) | 0-65535 |
| Div | 除算係数 (×1000) | 1-65535 |
| EN | フィールド有効 | ✓=有効, 空=無効 |
| WarnLo | 下限閾値 | 値 or --- (無効) |
| WarnHi | 上限閾値 | 値 or --- (無効) |
| WE | ワーニング有効 | ✓=有効, 空=無効 |

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
    |-- g_can_values[] 更新
    v
master_warning_check()
    |-- g_can_config.fields[] をスキャン
    |-- warn_enabled == 1 のフィールドを監視
    |-- 値 > warn_high または 値 < warn_low でワーニング検出
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

## HOSTアプリ連携

### コマンド拡張

#### can_field コマンド（拡張）
```
can_field <idx> <ch> <byte> <size> <type> <end> <var> <off> <mul> <div> <name> <unit> <warn_en> <warn_lo> <warn_hi>
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
| warn_en | ワーニング有効 | 0=無効, 1=有効 |
| warn_lo | 下限閾値 | 値 or -32768 (無効) |
| warn_hi | 上限閾値 | 値 or -32768 (無効) |

#### 設定例
```
# フィールド3: 水温 (CH2, byte0, 2bytes, signed, BigEndian, NUM1, 警告60-110℃)
can_field 3 2 0 2 1 0 2 0 1000 10000 WATER C 1 60 110

# フィールド0: 回転数 (CH1, byte0, 2bytes, unsigned, BigEndian, REV, 警告上限8000rpm)
can_field 0 1 0 2 0 0 0 0 1000 1000 REV rpm 1 -32768 8000

# フィールド6: 油圧 (CH4, byte0, 2bytes, unsigned, BigEndian, NUM5, 警告下限100kPa)
can_field 6 4 0 2 0 0 6 0 1 1000 PRESS kPa 1 100 -32768
```

※ `warn_lo = -32768` または `warn_hi = -32768` (INT16_MIN) はその方向の閾値無効を意味する

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
3. 警告メッセージ形式: `"<name> HIGH"` または `"<name> LOW"`

### 制約事項
- **aw001/aw002フォルダのファイルは編集禁止**
- AppWizardで変数・ウィジェット追加後、エクスポートされたファイルを使用

## 実装手順

### Phase 1: データ構造拡張
1. [ ] `param_storage.h` - `CAN_Field_t` 構造体に `name`, `warn_enabled`, `warn_low`, `warn_high` 追加
2. [ ] `CAN_CONFIG_VERSION` を 2 に更新
3. [ ] `can_config_init()` / プリセットでデフォルト値設定

### Phase 2: マスターワーニングモジュール
1. [ ] `master_warning.h` 新規作成
2. [ ] `master_warning.c` 新規作成
3. [ ] 警告判定ロジック実装
4. [ ] 警告音再生連携 (`speaker_play_warning()`)

### Phase 3: GUI連携
1. [ ] AppWizardで `ID_VAR_WARNING` 変数追加（aw001/aw002）
2. [ ] AppWizardで警告テキストウィジェット追加
3. [ ] `master_warning_update_display()` 実装

### Phase 4: HOSTアプリ連携
1. [ ] `host_command.c` - `can_field` コマンド拡張（name, warn_* パラメータ追加）
2. [ ] `can_field_get` コマンド追加
3. [ ] HostApp側の設定画面実装（Android/Terminal）

## 注意事項

1. **名前文字列**: 英数字および "/" のみ、最大7文字（+NULL終端で8バイト）
2. **単位文字列**: 英字のみ、最大3文字（+NULL終端で4バイト）
3. **EEPROM互換性**: CAN_CONFIG_VERSION更新により、旧設定は初期化される
4. **閾値無効値**: INT16_MIN (-32768) を使用、HostAppでは "---" と表示
5. **RAM2配置**: CAN_Config_t は `.bss2` セクションに配置、メインRAMへの影響なし
6. **複数警告**: 複数フィールドが同時に閾値超過した場合、最初に検出したものを表示

## 変更履歴

| 日付 | バージョン | 内容 |
|------|-----------|------|
| 2026/01/18 | 1.0 | 初版作成（提案仕様） |
