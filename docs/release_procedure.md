# FULLMONI-WIDE リリース手順書

ファームウェアとホストアプリのリリース発行手順をまとめたドキュメントです。

## 概要

リリース発行は以下の流れで行います：

1. バージョン番号の更新
2. ファームウェアバリアントのビルド
3. サムネイル画像の生成
4. マニフェストファイルの更新
5. ローカルテスト
6. Git タグ作成・プッシュ
7. GitHub Release 発行

---

## 1. バージョン番号の更新

### 1.1 firmware_header_data.c

```c
// Firmware/src/firmware_header_data.c
#define FW_VERSION_MAJOR    0
#define FW_VERSION_MINOR    1
#define FW_VERSION_PATCH    3  // ← インクリメント
```

### 1.2 WPF Terminal App

```xml
<!-- HostApp/FULLMONI-WIDE-Terminal/MainWindow.xaml -->
<!-- AssemblyInfo.cs のバージョンも更新 -->
```

---

## 2. ファームウェアバリアントのビルド

### 2.1 自動ビルドスクリプトの実行

```powershell
cd C:\Users\tomoy\git\FULLMONI-WIDE

# 両バリアント（aw001, aw002）をビルド
powershell -ExecutionPolicy Bypass -File tools/build_variants.ps1 -Version "0.1.3"
```

### 2.2 出力確認

```
test-release/
  Firmware_v0.1.3_aw001.bin  ← Standard Theme
  Firmware_v0.1.3_aw002.bin  ← Racing Theme
```

### 2.3 ビルド検証

各バイナリの先頭4バイトが `WFXR`（リトルエンディアンの "RXFW"）であることを確認：

```powershell
$bytes = [System.IO.File]::ReadAllBytes("test-release/Firmware_v0.1.3_aw001.bin")
[System.Text.Encoding]::ASCII.GetString($bytes[0..3])
# 出力: WFXR
```

---

## 3. サムネイル画像の生成

### 3.1 AppWizard Simulation の実行

各バリアントの Simulation を実行し、スクリーンショットをキャプチャします。

#### aw001 (Standard Theme)

1. `Firmware/aw001/Simulation/Simulation_VS2015_2017_Lib.sln` を Visual Studio で開く
2. ビルド・実行
3. LCD領域（800×256）をキャプチャ
4. `test-release/thumbnail_aw001.png` として保存

#### aw002 (Racing Theme)

1. `Firmware/aw002/Simulation/Simulation_VS2015_2017_Lib.sln` を Visual Studio で開く
2. ビルド・実行
3. LCD領域（800×256）をキャプチャ
4. `test-release/thumbnail_aw002.png` として保存

### 3.2 サムネイル仕様

| 項目 | 値 |
|------|-----|
| 解像度 | 800 × 256 ピクセル |
| 形式 | PNG |
| 内容 | LCD表示領域のみ（ウィンドウ枠を除く） |

---

## 4. マニフェストファイルの更新

### 4.1 release-manifest.json

`test-release/release-manifest.json` を更新します。

```json
{
    "schemaVersion": 1,
    "version": "0.1.3",
    "releaseDate": "2026-02-15",
    "releaseNotes": "https://github.com/tomoya723/FULLMONI-WIDE/releases/tag/v0.1.3",
    "firmware": {
        "minimumBootloaderVersion": "1.0.0",
        "variants": [
            {
                "id": "aw001",
                "name": "Standard (Blue Theme)",
                "description": "標準テーマ（青系）- デフォルト",
                "file": "Firmware_v0.1.3_aw001.bin",
                "thumbnail": "thumbnail_aw001.png",
                "size": 956206,
                "sha256": "..."
            },
            {
                "id": "aw002",
                "name": "Racing (Red Theme)",
                "description": "レーシングテーマ（赤系）",
                "file": "Firmware_v0.1.3_aw002.bin",
                "thumbnail": "thumbnail_aw002.png",
                "size": 916782,
                "sha256": "..."
            }
        ]
    }
}
```

### 4.2 SHA256 ハッシュの計算

```powershell
# aw001
(Get-FileHash "test-release/Firmware_v0.1.3_aw001.bin" -Algorithm SHA256).Hash.ToLower()

# aw002
(Get-FileHash "test-release/Firmware_v0.1.3_aw002.bin" -Algorithm SHA256).Hash.ToLower()
```

### 4.3 ファイルサイズの確認

```powershell
Get-ChildItem "test-release/*.bin" | Select-Object Name, Length
```

---

## 5. ローカルテスト

### 5.1 Terminal アプリでのテスト

1. **WPF Terminal アプリを起動**
2. **FW Update タブを開く**
3. **LOCAL TEST モードでマニフェストを読み込む**
   - `test-release/release-manifest.json` を参照
4. **各バリアントの表示を確認**
   - サムネイルが正しく表示されるか
   - バージョン、サイズが正しいか
5. **ダウンロード・転送テスト**
   - チェックサム検証が成功するか
   - 実機への転送・フラッシュが成功するか
   - 再起動後に正しく動作するか

### 5.2 動作確認チェックリスト

- [ ] aw001 ファームウェアが正常に起動する
- [ ] aw002 ファームウェアが正常に起動する
- [ ] CAN受信が正常に動作する
- [ ] 各種警告表示が正常に動作する
- [ ] USB CDC 通信が正常に動作する

---

## 6. Git タグ作成・プッシュ

### 6.1 変更をコミット

```powershell
git add Firmware/src/firmware_header_data.c
git add docs/
git commit -m "release: v0.1.3 準備"
```

### 6.2 タグ作成

```powershell
git tag -a v0.1.3 -m "Release v0.1.3

- 新機能: XXX
- バグ修正: YYY
- 改善: ZZZ"
```

### 6.3 プッシュ

```powershell
git push origin main
git push origin v0.1.3
```

---

## 7. GitHub Release 発行

### 7.1 リリースアセットの準備

以下のファイルを GitHub Release にアップロード：

| ファイル | 説明 |
|----------|------|
| `Firmware_v0.1.3_aw001.bin` | Standard Theme ファームウェア |
| `Firmware_v0.1.3_aw002.bin` | Racing Theme ファームウェア |
| `thumbnail_aw001.png` | Standard Theme サムネイル |
| `thumbnail_aw002.png` | Racing Theme サムネイル |
| `release-manifest.json` | マニフェストファイル |
| `FULLMONI-WIDE-Terminal-win-x64.zip` | Windows Terminal アプリ（任意） |

### 7.2 リリースノート例

```markdown
## FULLMONI-WIDE v0.1.3

### 新機能
- オンライン配信機能の追加
- 複数テーマバリアントのサポート

### バグ修正
- XXX の修正

### 改善
- YYY の改善

### ファームウェアバリアント
| バリアント | テーマ | サイズ |
|-----------|--------|--------|
| aw001 | Standard (Blue) | 956 KB |
| aw002 | Racing (Red) | 917 KB |

### アップデート方法
1. FULLMONI-WIDE Terminal アプリを起動
2. FW Update タブを選択
3. お好みのテーマを選択してダウンロード
4. 転送完了後、デバイスが自動的に再起動します
```

---

## トラブルシューティング

### ビルドエラー: undefined reference to 'acfmw_op0'

**原因**: `startup_image_write.c` が古い aw002 パスをハードコードしている

**解決**: Junction パス `../aw/` を使用するように修正済み（2026-02-01）

### チェックサム検証失敗

**原因**: `release-manifest.json` の SHA256 が古い

**解決**:
```powershell
(Get-FileHash "ファイル.bin" -Algorithm SHA256).Hash.ToLower()
```
で再計算してマニフェストを更新

### バイナリの Magic が不正

**原因**: objcopy で `.firmware_header` セクションが先頭に来ていない

**解決**: `build_variants.ps1` は `cmd /c` 経由で objcopy を実行（ワイルドカード問題回避済み）

---

## 関連ドキュメント

- [tools/README.md](../tools/README.md) - ビルドスクリプトの詳細
- [Firmware/README.md](../Firmware/README.md) - ファームウェアビルド手順
- [HostApp/README.md](../HostApp/README.md) - Terminal アプリの詳細
