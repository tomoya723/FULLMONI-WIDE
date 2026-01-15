# Issue #65: CAN通信のカスタム化・HOSTアプリで設定

## 概要

現在のFirmware内部処理では、CAN ID、物理値への変換式、emWin変数への代入などが**固定記述（ハードコード）**されています。
本Issueでは、これらをHostAppを使ったパラメータ書き込み処理に変更し、**ユーザーカスタム化**を実現します。

---

## 1. 現状分析

### 1.1 現在のCAN ID設定 ([can.c#L79-L88](Firmware/src/can.c#L79-L88))

```c
// 固定値でCAN ID定義
#define RX_CANID_DEMO_INIT1    (0x3E8)    // MoTeC m100 #1
#define RX_CANID_DEMO_INIT2    (0x3E9)    // MoTeC m100 #2
#define RX_CANID_DEMO_INIT3    (0x3EA)    // MoTeC m100 #3
#define RX_CANID_DEMO_INIT4    (0x3EB)    // MoTeC m100 #4
#define RX_CANID_DEMO_INIT5    (0x3EC)    // MoTeC m100 #5
#define RX_CANID_DEMO_INIT6    (0x3ED)    // MoTeC m100 #6
```

**問題点**: MoTeC M100専用のCAN IDがハードコードされており、他のECU（LINK、HalTech、AEM等）には対応不可。

### 1.2 現在の物理値変換 ([dataregister.c#L73-L99](Firmware/src/dataregister.c#L73-L99))

```c
// バイト位置・変換式が固定
g_CALC_data.num1 = (float)((((unsigned int)rx_dataframe2.data[0]) << 8) + rx_dataframe2.data[1]) / 10.0;  // water Temp
g_CALC_data.num2 = (float)((((unsigned int)rx_dataframe1.data[6]) << 8) + rx_dataframe1.data[7]) / 10.0;  // IAT
g_CALC_data.num4 = (float)((((unsigned int)rx_dataframe1.data[4]) << 8) + rx_dataframe1.data[5]) * 0.1;   // MAP
g_CALC_data.af   = (float)((((unsigned int)rx_dataframe2.data[2]) << 8) + rx_dataframe2.data[3]) * 0.147; // A/F
g_CALC_data.rev  =        ((((unsigned int)rx_dataframe1.data[0]) << 8) + rx_dataframe1.data[1]);          // RPM
```

**問題点**:
- データのバイト位置が固定
- 変換係数（/10.0, *0.1, *0.147等）が固定
- 符号付き/符号なし、エンディアンの選択不可

### 1.3 現在のemWin変数代入 ([dataregister.c#L228-L252](Firmware/src/dataregister.c#L228-L252))

```c
APPW_SetVarData(ID_VAR_AF, g_CALC_data.af);
APPW_SetVarData(ID_VAR_REV, g_CALC_data.rev);
APPW_SetVarData(ID_VAR_01, g_CALC_data.num1);  // WaterTemp
APPW_SetVarData(ID_VAR_04, g_CALC_data_sm.num4); // MAP
APPW_SetVarData(ID_VAR_SPEED, g_CALC_data.sp * 1.06);
```

**問題点**: 表示項目と内部変数のマッピングが固定

---

## 2. 改修設計

### 2.1 新規パラメータ構造体

[param_storage.h](Firmware/src/param_storage.h) に以下を追加：

```c
/* CAN受信チャンネル設定（6ch分） */
typedef struct __attribute__((packed)) {
    uint16_t can_id;            /* 受信CAN ID (0x000-0x7FF) */
    uint8_t  enabled;           /* 有効/無効 */
    uint8_t  reserved;          /* 予約 */
} CAN_RX_Channel_t;

/* CANデータフィールド定義（最大16フィールド） */
typedef struct __attribute__((packed)) {
    uint8_t  channel;           /* 使用するCANチャンネル (1-6) */
    uint8_t  start_byte;        /* 開始バイト位置 (0-7) */
    uint8_t  byte_count;        /* バイト数 (1, 2, 4) */
    uint8_t  data_type;         /* データ型: 0=unsigned, 1=signed */
    uint8_t  endian;            /* エンディアン: 0=Big, 1=Little */
    uint8_t  target_var;        /* 代入先emWin変数ID */
    int16_t  offset;            /* オフセット値 (加算後) */
    uint16_t multiplier;        /* 乗算係数 (x1000) */
    uint16_t divisor;           /* 除算係数 (x1000) */
    uint16_t reserved;          /* 予約 */
} CAN_Field_t;

/* 拡張パラメータ構造体 */
typedef struct __attribute__((packed)) {
    /* ... 既存パラメータ ... */

    /* === CAN設定 === */
    CAN_RX_Channel_t can_channels[6];   /* 受信チャンネル設定 */
    CAN_Field_t can_fields[16];          /* データフィールド定義 */

    /* === チェックサム === */
    uint16_t crc16;
} PARAM_Storage_Extended_t;
```

### 2.2 CANフィールド定義の例（MoTeC M100デフォルト）

| フィールド | CH | Byte | Size | Type | 係数 | 変数 | 説明 |
|-----------|-----|------|------|------|------|------|------|
| 0 | 1 | 0-1 | 2 | U16 | /1 | VAR_REV | エンジン回転数 |
| 1 | 1 | 4-5 | 2 | U16 | /10 | VAR_04 | マニホールド圧 |
| 2 | 1 | 6-7 | 2 | S16 | /10 | VAR_02 | 吸気温度 |
| 3 | 2 | 0-1 | 2 | S16 | /10 | VAR_01 | 水温 |
| 4 | 2 | 2-3 | 2 | U16 | *0.147 | VAR_AF | A/F比 |
| ... | | | | | | | |

### 2.3 変換処理の汎用化

[dataregister.c](Firmware/src/dataregister.c) を以下のように変更：

```c
/**
 * @brief CANデータを物理値に変換（汎用処理）
 * @param field フィールド定義
 * @param rx_data 受信データフレーム配列
 * @return 変換後の物理値
 */
float can_field_to_physical(const CAN_Field_t *field, const can_frame_t *rx_data[])
{
    const can_frame_t *frame = rx_data[field->channel - 1];
    uint32_t raw = 0;

    // バイト抽出（エンディアン対応）
    if (field->endian == 0) {  // Big Endian
        for (int i = 0; i < field->byte_count; i++) {
            raw = (raw << 8) | frame->data[field->start_byte + i];
        }
    } else {  // Little Endian
        for (int i = field->byte_count - 1; i >= 0; i--) {
            raw = (raw << 8) | frame->data[field->start_byte + i];
        }
    }

    // 符号付き処理
    float value;
    if (field->data_type == 1) {  // Signed
        if (field->byte_count == 2 && raw > 32767) {
            value = (float)((int32_t)raw - 65536);
        } else if (field->byte_count == 1 && raw > 127) {
            value = (float)((int32_t)raw - 256);
        } else {
            value = (float)raw;
        }
    } else {
        value = (float)raw;
    }

    // 変換計算: (value + offset) * multiplier / divisor
    value = value + field->offset;
    value = value * field->multiplier / 1000.0f;
    value = value / (field->divisor / 1000.0f);

    return value;
}
```

---

## 3. HostApp側の改修

### 3.1 新規コマンド追加

[param_console.c](Firmware/src/param_console.c) に追加するコマンド：

| コマンド | 説明 | 例 |
|---------|------|-----|
| `can_ch <n> <id> <en>` | CANチャンネル設定 | `can_ch 1 0x3E8 1` |
| `can_field <n> <ch> <byte> <size> <type> <end> <var> <off> <mul> <div>` | フィールド定義 | `can_field 0 1 0 2 0 0 10 0 1000 1000` |
| `can_list` | CAN設定一覧表示 | `can_list` |
| `can_preset <name>` | プリセット適用 | `can_preset motec_m100` |

### 3.2 HostApp UI追加

[MainViewModel.cs](HostApp/FULLMONI-WIDE-Terminal/ViewModels/MainViewModel.cs) に「CAN設定」タブを追加：

```
┌─────────────────────────────────────────────────────────────┐
│ [タイヤ/ギア] [警告設定] [シフトRPM] [CAN設定] [更新]      │
├─────────────────────────────────────────────────────────────┤
│ ■ プリセット: [MoTeC M100 ▼] [LINK G4+ ▼] [カスタム]      │
├─────────────────────────────────────────────────────────────┤
│ ■ 受信チャンネル                                            │
│   CH1: ID [0x3E8] [✓]   CH2: ID [0x3E9] [✓]               │
│   CH3: ID [0x3EA] [✓]   CH4: ID [0x3EB] [✓]               │
│   CH5: ID [0x3EC] [✓]   CH6: ID [0x3ED] [ ]               │
├─────────────────────────────────────────────────────────────┤
│ ■ データフィールド定義                                       │
│   ┌──────┬────┬─────┬─────┬──────┬────────┬───────┐        │
│   │ 表示 │ CH │Byte │Size │ Type │ 係数   │ 変数  │        │
│   ├──────┼────┼─────┼─────┼──────┼────────┼───────┤        │
│   │ RPM  │ 1  │ 0   │ 2   │ U16  │ x1     │VAR_REV│        │
│   │ 水温 │ 2  │ 0   │ 2   │ S16  │ /10    │VAR_01 │        │
│   │ ...  │    │     │     │      │        │       │        │
│   └──────┴────┴─────┴─────┴──────┴────────┴───────┘        │
├─────────────────────────────────────────────────────────────┤
│ [読込] [書込] [保存] [デフォルト]                           │
└─────────────────────────────────────────────────────────────┘
```

---

## 4. 実装フェーズ

### Phase 1: Firmware側基盤整備
1. `param_storage.h` に CAN設定構造体追加
2. `param_storage.c` にデフォルト値・CRC計算追加
3. `param_console.c` にCAN設定コマンド追加

### Phase 2: CAN処理の汎用化
1. `dataregister.c` に汎用変換関数追加
2. `can.c` のCAN ID初期化をパラメータ参照に変更
3. 固定記述の変換処理を汎用関数呼び出しに置換

### Phase 3: HostApp対応
1. CAN設定用ViewModelプロパティ追加
2. CAN設定UIの追加（XAML）
3. プリセット機能実装（MoTeC, LINK, AEM等）

### Phase 4: テスト・検証
1. 既存MoTeC設定での動作確認
2. 新規プリセット（LINK等）での動作確認
3. カスタム設定の保存・読込テスト

---

## 5. ECUプリセット例

### 5.1 MoTeC M100/M400/M800

| CH | CAN ID | フィールド |
|----|--------|-----------|
| 1 | 0x3E8 | RPM(0-1), TPS(2-3), MAP(4-5), IAT(6-7) |
| 2 | 0x3E9 | ECT(0-1), AFR(2-3), FUEL(4-5), IGN(6-7) |
| 3 | 0x3EA | OILT(0-1), OILP(2-3), FUELT(4-5) |
| 4 | 0x3EB | VSS(0-1), GEAR(2), ... |

### 5.2 LINK G4+/G4X

| CH | CAN ID | フィールド |
|----|--------|-----------|
| 1 | 0x3E0 | RPM(0-1), MAP(2-3), TPS(4-5), ECT(6-7) |
| 2 | 0x3E1 | IAT(0-1), AFR(2-3), OILP(4-5), ... |

### 5.3 AEM EMS / Infinity

| CH | CAN ID | フィールド |
|----|--------|-----------|
| 1 | 0x5F0 | RPM(0-1), TPS(2-3), MAP(4-5), AFR(6-7) |
| 2 | 0x5F1 | ECT(0-1), IAT(2-3), ... |

---

## 6. メモリ影響評価

| 項目 | サイズ | 備考 |
|------|--------|------|
| CAN_RX_Channel_t × 6 | 24 bytes | チャンネル設定 |
| CAN_Field_t × 16 | 224 bytes | フィールド定義 |
| 合計追加 | ~250 bytes | EEPROM/RAM |

現在のEEPROM使用量に余裕があるため、問題なし。

---

## 7. 後方互換性

- **EEPROMバージョン番号**を導入し、新旧パラメータを区別
- 旧パラメータ読み込み時は**MoTeCデフォルト**を適用
- HostAppは古いFirmwareでも基本機能は動作

---

## 8. 参考リンク

- [MoTeC CAN Protocol](https://www.motec.com.au/support/)
- [LINK G4+ CAN Setup](https://www.linkecu.com/)
- [現在のparam_console.c](Firmware/src/param_console.c)
- [現在のparam_storage.h](Firmware/src/param_storage.h)

---

## 変更履歴

| 日付 | バージョン | 内容 |
|------|-----------|------|
| 2026/01/15 | 1.0 | 初版作成 |
