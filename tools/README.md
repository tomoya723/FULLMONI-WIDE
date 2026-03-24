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
│ 0x00000040 │ Frame Buffer 2 (GLCDC ダブルバッファ)         │
│            │ 800 x 256 x 2bytes = 400KB                     │
│ 0x00064040 │ BSS Region (グローバル変数)                    │
│            │ ~112KB (リンカスクリプトで定義)                │
│ 0x0007FFFF │ End of Low RAM                                 │
├─────────────────────────────────────────────────────────────┤
│ High RAM (0x00800000 - 0x0087FFFF, 512KB)                   │
├─────────────────────────────────────────────────────────────┤
│ 0x00800000 │ Frame Buffer 1 (GLCDC プライマリバッファ)     │
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
| `Firmware/src/lv_port/lv_port_disp.c` | LVGLディスプレイドライバ（フレームバッファアドレス定義） |
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
- `lv_port_disp.c` のフレームバッファアドレス定義

---

## fw_upload.py

ファームウェアアップロードツール（別途ドキュメント参照）

---

## capture_thumbnails.ps1

> **ℹ️ 現在未使用**: AppWizard (SEGGER) から LVGL (EEZ Studio) への移行に伴い、このスクリプトは現在動作しません。サムネイル生成は別の方法で行ってください。

### 概要

GUISimulation（AppWizardシミュレータ）を自動起動し、LCD表示部分のスクリーンショットをキャプチャするスクリプト。

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

---

## release.ps1

### 概要

FULLMONI-WIDE のリリース作成を自動化するPowerShellスクリプト。

v1.0.0リリース時に発生した以下の3つのミスを防ぐために作成されました：

1. **バリアント間のオブジェクト混入** → `make clean` を毎回実行
2. **古いrelease-manifest.jsonのアップロード** → 毎回自動生成
3. **古いホストアプリのアップロード** → 毎回ビルド＋SHA256検証

### 機能

1. **test-release/ のクリーンアップ**
   - 古いビルド成果物を削除
   - スキップされたコンポーネントのファイルは保持

2. **ファームウェア全バリアントのビルド**
   - `build_variants.ps1` を呼び出し
   - 各バリアント間で `make clean` を実行（オブジェクト混入防止）
   - MOTファイルを test-release/ にコピー

3. **サムネイル画像の生成**
   - `capture_thumbnails.ps1` を呼び出し
   - GUISimulationから各バリアントのスクリーンショットを取得

4. **ホストアプリのビルド・パッケージ**
   - `dotnet publish` で自己完結型ビルド
   - ZIPファイルに圧縮
   - test-release/ にコピー

5. **release-manifest.json の生成**
   - 全ファイルのSHA256ハッシュを計算
   - バージョン情報、リリース日時を記録
   - 既存のBootloader情報を引き継ぎ

6. **整合性検証**
   - 生成したマニフェストと実ファイルを照合
   - SHA256不一致があればエラー終了

7. **GitHub Release 作成（オプション）**
   - `gh release create` でリリース作成
   - 全アセットをアップロード

### ハイブリッドバージョン管理

FirmwareとHostAppは独立したリリースサイクルを持てます：

- `-SkipFirmware`: FWビルドをスキップ、既存マニフェストのFW情報を引き継ぎ
- `-SkipHostApp`: HostAppビルドをスキップ、既存マニフェストのHostApp情報を引き継ぎ

これにより、片方だけ更新する「ホットフィックス」的なリリースが可能です。

### 使用方法

#### 通常リリース（FW + HostApp両方）

```powershell
# ビルドのみ（test-release/ に成果物を生成）
.\tools\release.ps1 -Version "1.0.1"

# ビルド + GitHubアップロード
.\tools\release.ps1 -Version "1.0.1" -Upload
```

#### FWのみリリース

```powershell
.\tools\release.ps1 -Version "1.0.1" -SkipHostApp -Upload
```

#### HostAppのみリリース

```powershell
.\tools\release.ps1 -Version "1.0.1" -SkipFirmware -SkipThumbnails -Upload
```

### パラメータ

| パラメータ | 必須 | 説明 |
|-----------|------|------|
| `-Version` | ✓ | リリースバージョン（例: "1.0.1"） |
| `-Upload` | | GitHub Releaseを作成してアップロード |
| `-SkipFirmware` | | FWビルドをスキップ（既存情報を引き継ぎ） |
| `-SkipThumbnails` | | サムネイル生成をスキップ |
| `-SkipHostApp` | | HostAppビルドをスキップ（既存情報を引き継ぎ） |

### 前提条件

- **ファームウェアビルド**: GCC-RXツールチェーン、make
- **サムネイル生成**: 現在未使用（AppWizardシミュレータ廃止）
- **ホストアプリビルド**: .NET 8.0 SDK
- **GitHubアップロード**: GitHub CLI (`gh`) がインストール・認証済み

### 出力先

```
test-release/
├── Firmware_v1.0.1.mot             # FW
├── Bootloader_v0.1.2.mot          # Bootloader（既存を維持）
├── FULLMONI-WIDE-Terminal-win-x64.zip  # HostApp
└── release-manifest.json          # マニフェスト
```
