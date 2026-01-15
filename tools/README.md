# FULLMONI-WIDE Tools

開発支援ツール集

## check_ram_usage.ps1

### 概要

RX72N ファームウェアのRAM使用量を正確に計算・表示するPowerShellスクリプト。

`rx-elf-size`コマンドの`bss`列は各シンボルサイズの合計を表示するため、実際のリンカ配置とは異なる値になることがあります。このスクリプトはmapファイルから実際の配置アドレスを解析し、正確な使用量を表示します。

### 背景

2026年1月のIssue #65実装時に、`rx-elf-size`の出力（bss=120,000バイト）とリンカスクリプトのRAM容量（114,624バイト）の不一致により、メモリオーバーフローが発生していると誤認する問題がありました。

実際にはリンカは正しく領域内に配置しており（114,576バイト使用）、約48バイトの空きがありました。この経験から、正確なRAM使用量を確認するためのツールを作成しました。

### 機能

1. **メモリマップ表示**
   - Low RAM / High RAM の配置を視覚化
   - フレームバッファ1,2の位置とサイズを表示
   - BSS領域と未使用領域を明示

2. **BSS使用量の詳細**
   - 実際の開始・終了アドレス
   - 使用量・容量・空き容量
   - 使用率のパーセント表示

3. **メモリ境界チェック**
   - BSS開始がフレームバッファ2を侵害していないか
   - BSS終端がRAM領域を超えていないか
   - フレームバッファ1,2が重複していないか

4. **警告表示**
   - 空き < 256バイト: 赤色警告
   - 空き < 1024バイト: 黄色注意
   - 侵害検出時: エラー終了（exit code 1）

### 使用方法

#### VS Code タスクから実行

1. `Ctrl+Shift+P` → `Tasks: Run Task`
2. `RAM Usage Detail (GCC-RX)` を選択

#### コマンドラインから実行

```powershell
cd Firmware/HardwareDebug
powershell -ExecutionPolicy Bypass -File ../../tools/check_ram_usage.ps1
```

#### パラメータ

```powershell
# デフォルト（Firmware.map）
.\check_ram_usage.ps1

# 別のmapファイルを指定
.\check_ram_usage.ps1 -MapFile "path/to/other.map"
```

### 出力例

```
=== RX72N Internal RAM Memory Map ===

Low RAM (0x00000000 - 0x0007FFFF, 512KB):
  0x00000040 - 0x0006403F: Frame Buffer 2 (400 KB)
  0x00064040 - 0x0007FFFF: BSS Region    (112 KB)

High RAM (0x00800000 - 0x0087FFFF, 512KB):
  0x00800000 - 0x00863FFF: Frame Buffer 1 (400 KB)
  0x00864000 - 0x0087FFFF: Unused        (112 KB)

=== BSS Region Usage ===
BSS Start:    0x00064040
BSS End:      0x0007FFD0

Used:            114,576 bytes (100.0%)
Capacity:        114,624 bytes
Free:                 48 bytes

=== Memory Boundary Check ===
[OK] BSS start is after Frame Buffer 2 end
[OK] BSS end is within RAM region
[OK] Frame Buffers 1 and 2 are separate

WARNING: RAM is critically low! Less than 256 bytes free.
Consider using High RAM unused region (0x00864000-0x0087FFFF, ~112KB)
```

### RX72N メモリマップ

```
┌─────────────────────────────────────────────────────────────┐
│ Low RAM (0x00000000 - 0x0007FFFF, 512KB)                    │
├─────────────────────────────────────────────────────────────┤
│ 0x00000000 │ Reserved (64 bytes)                            │
│ 0x00000040 │ Frame Buffer 2 (emWin ダブルバッファ)          │
│            │ 800 x 256 x 2bytes = 400KB                     │
│ 0x00064040 │ BSS Region (グローバル変数)                    │
│            │ ~112KB (リンカスクリプトで定義)                │
│ 0x0007FFFF │ End of Low RAM                                 │
├─────────────────────────────────────────────────────────────┤
│ High RAM (0x00800000 - 0x0087FFFF, 512KB)                   │
├─────────────────────────────────────────────────────────────┤
│ 0x00800000 │ Frame Buffer 1 (emWin プライマリバッファ)      │
│            │ 800 x 256 x 2bytes = 400KB                     │
│ 0x00864000 │ Unused (将来の拡張用)                          │
│            │ ~112KB                                         │
│ 0x0087FFFF │ End of High RAM                                │
└─────────────────────────────────────────────────────────────┘
```

### 関連ファイル

| ファイル | 説明 |
|----------|------|
| `Firmware/src/linker_script.ld` | RAMリージョン定義 |
| `Firmware/src/qe_emwin_config.h` | フレームバッファアドレス定義 |
| `Firmware/HardwareDebug/Firmware.map` | リンカ出力マップファイル |

### rx-elf-size との違い

| ツール | 表示内容 | 用途 |
|--------|----------|------|
| `rx-elf-size` | 各セクションのシンボルサイズ合計 | クイックチェック |
| **check_ram_usage.ps1** | mapファイルからの実際の配置アドレス | **正確な確認** |

`rx-elf-size`の`bss`値はリンカ配置前の合計であり、アライメントやセクション配置により実際の使用量と異なることがあります。正確なRAM残量を確認する場合は本スクリプトを使用してください。

### トラブルシューティング

#### "Could not find _bss or _ebss in map file"

mapファイルが存在しないか、ビルドが完了していません。先にビルドを実行してください。

#### 侵害エラーが表示される場合

リンカスクリプトまたはフレームバッファ設定に問題があります。以下を確認：
- `linker_script.ld` の RAM 定義
- `qe_emwin_config.h` の `EMWIN_GUI_FRAME_BUFFER1/2` 定義

---

## fw_upload.py

ファームウェアアップロードツール（別途ドキュメント参照）
