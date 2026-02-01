# USBブートモード経由のフラッシュ書き込みガイド

e2liteを使わずに、USBブートモード経由でRenesas Flash Programmerを使ってフラッシュする方法をまとめます。

## 概要

RX72NにはMCU内蔵のUSBブートモードがあり、これを利用してe2liteなしでフラッシュ書き込みが可能です。

## 必要なファイル

| ファイル | 用途 | 生成方法 |
|---------|------|----------|
| `Bootloader.mot` | ブートローダー | ビルド時に自動生成 |
| `Firmware_noofs.mot` | ファームウェア（OFS/ベクタ除外） | タスクで生成 |
| `Firmware.bin` | USB CDC経由アップデート用 | ビルド時に自動生成 |
| `Firmware.elf` | e2liteデバッグ用 | ビルド時に自動生成 |

### 注意: `Firmware.mot` は使用しない

通常生成される `Firmware.mot` には以下のセクションが含まれており、`Bootloader.mot` とアドレスが衝突するため、**Renesas Flash Programmerでの同時書き込みには使用できません**：

| セクション | アドレス | 問題 |
|-----------|---------|------|
| `.ofs1`〜`.ofs8` | 0xFE7F5D00〜0xFE7F5D70 | OFS設定（Bootloaderと重複） |
| `.exvectors` | 0xFFFFFF80 | 例外ベクタ（Bootloaderと重複） |

## Firmware_noofs.mot の生成

### 方法1: VSCodeタスク（推奨）

1. `Ctrl+Shift+P` → `Tasks: Run Task`
2. `Build + Generate MOT (GCC-RX)` を選択

または個別に:
1. `Build (GCC-RX)` でビルド
2. `Generate Firmware_noofs.mot` でMOT生成

### 方法2: コマンドライン

```powershell
rx-elf-objcopy "Firmware/HardwareDebug/Firmware.elf" -O srec -I elf32-rx-be-ns --remove-section=.ofs1 --remove-section=.ofs2 --remove-section=.ofs3 --remove-section=.ofs4 --remove-section=.ofs5 --remove-section=.ofs6 --remove-section=.ofs7 --remove-section=.ofs8 --remove-section=.exvectors "Firmware/HardwareDebug/Firmware_noofs.mot"
```

## 書き込み手順

### 初回セットアップ / 完全リカバリ

1. **ボードをUSBブートモードで起動**
   - MD端子を適切に設定（ハードウェア依存）
   - USBケーブルで接続

2. **Renesas Flash Programmerで書き込み**
   - ファイルを追加:
     - `Bootloader/HardwareDebug/Bootloader.mot`
     - `Firmware/HardwareDebug/Firmware_noofs.mot`
   - 「スタート」をクリック

3. **ブートモード解除してリセット**
   - MD端子を通常モードに戻す
   - ボードをリセット

### 通常のファームウェア更新

Bootloaderが書き込まれた後は、USB CDC経由で更新可能：

```powershell
py tools/fw_upload.py COM番号 Firmware/HardwareDebug/Firmware.bin
```

## トラブルシューティング

### エラー: データが既に存在します (Address: 0xFE7F5D00)

**原因**: `Firmware.mot`（OFS含む）を使っている

**解決**: `Firmware_noofs.mot` を使用する

### エラー: データが既に存在します (Address: 0xFFFFFF80)

**原因**: `Firmware.mot`（exvectors含む）を使っている

**解決**: `Firmware_noofs.mot` を使用する

### エラー: デバイスのメモリ範囲外にデータが存在します (Address: 0x00864000)

**原因**: リンカスクリプトで `.bss2` に `AT>RAM2` が設定されている

**解決**: リンカスクリプトで `.bss2` を `(NOLOAD)` 属性に変更し、`AT>RAM2` を削除

```ld
/* 修正前 */
.bss2 :
{
    ...
} >RAM2 AT>RAM2

/* 修正後 */
.bss2 (NOLOAD) :
{
    ...
} >RAM2
```

## ファイル用途まとめ

| 操作 | 使用ファイル |
|------|-------------|
| e2liteでデバッグ | `Firmware.elf` |
| USB CDC更新 (fw_upload.py) | `Firmware.bin` |
| Renesas Flash Programmer（初回） | `Bootloader.mot` + `Firmware_noofs.mot` |

## 関連ファイル

- `Bootloader/README.md` - Bootloaderの詳細仕様
- `tools/fw_upload.py` - USB CDC転送スクリプト
- `.vscode/tasks.json` - ビルドタスク定義

## 変更履歴

- 2026-02-01: 初版作成
