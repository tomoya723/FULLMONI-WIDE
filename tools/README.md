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

## build_variants.ps1

### 概要

AppWizardバリアント（aw001, aw002）のファームウェアを自動ビルドするPowerShellスクリプト。
Junction + 動的subdir.mk生成でバリアントを切り替えてビルドします。

### 機能

1. **Junction自動切り替え** - `Firmware/aw` を各バリアントディレクトリにリンク
2. **subdir.mk動的生成** - AppWizardソースファイル用のMakefile断片を生成
3. **バイナリ生成** - objcopyで正しいセクション構成のbinファイル生成
4. **マニフェスト更新** - SHA256ハッシュとファイルサイズを自動更新

### 使用方法

```powershell
# 両バリアントをビルド
powershell -ExecutionPolicy Bypass -File tools/build_variants.ps1

# 特定バリアントのみ
powershell -ExecutionPolicy Bypass -File tools/build_variants.ps1 -Variants aw001

# バージョン指定
powershell -ExecutionPolicy Bypass -File tools/build_variants.ps1 -Version "0.1.2"
```

### パラメータ

| パラメータ | デフォルト | 説明 |
|-----------|-----------|------|
| `-Variants` | `@("aw001", "aw002")` | ビルドするバリアント |
| `-OutputDir` | `test-release` | 出力ディレクトリ |
| `-Version` | (なし) | バージョン番号（ファイル名に付加） |

### 出力ファイル

| ファイル | 説明 |
|----------|------|
| `Firmware_v{ver}_{variant}.bin` | ファームウェアバイナリ |
| `release-manifest.json` | SHA256/サイズ自動更新 |

### 技術詳細

#### セクション構成

objcopyで以下のセクションを抽出：
- `.firmware_header` - ブートローダー検証用ヘッダ (64バイト)
- `.text` - コード
- `.rvectors` - 割り込みベクタ
- `.rodata*` - 読み取り専用データ
- `.fvectors` - 固定ベクタ
- `.data` - 初期化データ

#### Junctionベースのビルド

```
Firmware/
  aw/  ← Junction (シンボリックリンク)
  aw001/  ← Standard Theme
  aw002/  ← Racing Theme
```

ビルド時に `aw/` を目的のバリアントにリンクし、
ソースコードは `#include "../aw/..."` で参照します。

### 注意事項

- Windows環境のみ対応（Junctionは管理者権限不要）
- `rx-elf-gcc` がPATHに含まれている必要あり
- e2 studioの `make.exe` パスがスクリプト内にハードコード

---

## switch_to_aw001.bat / switch_to_aw002.bat

### 概要

開発時にAppWizardバリアントを切り替えるためのバッチファイル。
e2 studio でのデバッグ前に実行することで、目的のバリアントをビルド対象にできます。

### 使用方法

**方法1: Explorer からダブルクリック**

- `tools/switch_to_aw001.bat` → aw001（青テーマ）に切り替え
- `tools/switch_to_aw002.bat` → aw002（赤テーマ）に切り替え

**方法2: e2 studio External Tools に登録**

1. Run → External Tools → External Tools Configurations
2. Program → New Configuration
3. 設定:
   - Name: `Switch to aw001`
   - Location: `C:\Windows\System32\cmd.exe`
   - Working Directory: `${project_loc:Firmware}`
   - Arguments: `/c "..\tools\switch_to_aw001.bat"`
4. Apply → Close

以降は Run → External Tools → Switch to aw001 で切り替え可能。

### 仕組み

Junctionを使用してフォルダのリンク先を切り替えます：

```
Firmware/aw → aw001  (switch_to_aw001.bat 実行後)
Firmware/aw → aw002  (switch_to_aw002.bat 実行後)
```

### 切り替え後の操作

1. e2 studio でプロジェクトを **Refresh**（F5）
2. **Clean** → **Build**
3. デバッグ実行

---

## fw_upload.py

ファームウェアアップロードツール（別途ドキュメント参照）

---

## capture_thumbnails.ps1

### 概要

GUISimulation（AppWizardシミュレータ）を自動起動し、LCD表示部分のスクリーンショットをキャプチャするPowerShellスクリプト。HOST Appのファームウェアカタログ用サムネイル生成に使用。

### 機能

1. **バリアント自動検出**
   - `Firmware/aw001`, `aw002`, `aw003`... を自動検出
   - `aw` リンクフォルダは除外（`^aw\d+$` パターンで判定）

2. **GUI_Lib自動セットアップ**
   - SEGGER AppWizardインストール先またはバックアップから自動コピー
   - ライセンス上の理由でgitにはコミットしない（.gitignoreで除外）

3. **GUISimulation.exe自動ビルド**
   - MSBuildを自動検出
   - Windows SDK 10.0 / PlatformToolset v145 で上書きビルド（vcxprojは変更しない）

4. **LCD領域キャプチャ**
   - 800x256ピクセルのLCD表示部分のみをキャプチャ
   - ウィンドウを強制的にフォアグラウンドに移動（AttachThreadInputトリック使用）

### 使用方法

```powershell
# バージョン指定でサムネイル生成
powershell -ExecutionPolicy Bypass -File tools/capture_thumbnails.ps1 -Version "0.1.2"
# → test-release/thumbnail_v0.1.2_aw001.png, thumbnail_v0.1.2_aw002.png ...

# バージョンなし
powershell -ExecutionPolicy Bypass -File tools/capture_thumbnails.ps1
# → test-release/thumbnail_aw001.png, thumbnail_aw002.png ...
```

### パラメータ

| パラメータ | デフォルト | 説明 |
|-----------|-----------|------|
| `-Version` | (なし) | サムネイルファイル名に付加するバージョン |
| `-OutputDir` | `test-release` | 出力先フォルダ |

### 前提条件

- Visual Studio (MSBuild) がインストールされていること
- SEGGER AppWizard がインストールされているか、GUI_Libのバックアップがあること
  - バックアップ場所: `%USERPROFILE%\Desktop\FULLMONI-WIDE_aw003\Firmware\aw001\Simulation\GUI_Lib`

### 注意事項

- **GUI_Lib, Exe, Output フォルダは .gitignore で除外済み**（SEGGER機密コード保護のため）
- スクリプト実行中はGUISimulationウィンドウが表示されます（自動的に閉じます）
- キャプチャ中に他のウィンドウを操作しないでください
