# FULLMONI-WIDE Bootloader - クイックスタート

このガイドに従えば、5分でフラッシュ書き込みまで完了できます。

## 前提条件

- [ ] e2 studioがインストール済み
- [ ] Python 3.xがインストール済み
- [ ] E1/E2/E20エミュレータが接続済み
- [ ] ターゲットボードの電源ON

## ステップ1: ブートローダービルド（初回のみ）

```powershell
cd C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\HardwareDebug
.\build_bootloader.ps1
```

**期待される出力:**
```
✓ Bootloader build successful
  bootloader.elf: 35,196 bytes
  bootloader.mot: 8,852 bytes
```

## ステップ2: アプリケーションビルド

e2 studioで:
1. プロジェクトを開く
2. **Ctrl+B** (ビルド)
3. ビルド完了まで待機（約30秒）

**期待される出力:**
```
text: 1,214,577 bytes
✓ Build successful
```

## ステップ3: 統合MOTファイル生成

```powershell
cd C:\Users\tomoy\git\FULLMONI-WIDE\Firmware\HardwareDebug
.\generate_combined_mot.ps1
```

**期待される出力:**
```
✓ 統合MOTファイル生成成功!
  ファイル名: combined.mot
```

もしPythonエラーが出た場合、手動実行:
```powershell
python ..\..\tools\merge_mot_files.py combined.mot bootloader_build\bootloader.mot Firmware.mot
```

## ステップ4: フラッシュ書き込み

### 方法A: Renesas Flash Programmer（推奨）

1. **Renesas Flash Programmer起動**
2. **Connect Settings:**
   - Microcontroller: `R5F572NN`
   - Tool: `E1` / `E2` / `E20`（使用中のもの）
3. **Connect** をクリック
4. **Browse...** → `combined.mot` を選択
5. **Erase:** `Full chip erase` ☑
6. **Program:** `On` ☑
7. **Verify:** `On` ☑
8. **Start** をクリック
9. 完了まで待機（約60秒）
10. **Disconnect**

### 方法B: e2 studio Debugger

1. **Run → Debug Configurations...**
2. 新規設定作成
3. **Debugger タブ:**
   - Target Device: `R5F572NNDxBD`
4. **"Debug"** をクリック
5. デバッガが起動したら **Terminate**

## ステップ5: 動作確認

### USB-UART接続（オプション）

1. **ターミナルソフト設定:**
   - ポート: `COM3`（環境により異なる）
   - ボーレート: `115200`
   - データビット: `8`
   - パリティ: `None`
   - ストップビット: `1`

2. **ターゲットボード電源OFF → ON**

3. **期待されるログ:**
```
[BOOTLOADER] RX72N/RX72T Bootloader v1.0.0
[BOOTLOADER] Checking firmware header at 0xFFC10000...
[BOOTLOADER]   Magic: OK (0x52584657)
[BOOTLOADER]   Version: 1.0.0
[BOOTLOADER]   Size: 1237453 bytes
[BOOTLOADER]   CRC32: Calculating...
[BOOTLOADER]   CRC32: OK (0x12345678)
[BOOTLOADER] Firmware valid. Jumping to application...
[BOOTLOADER] Entry point: 0xFFC20XXX
```

4. **アプリケーション起動確認**
   - ディスプレイに画面表示
   - 正常動作

### エミュレータのみ（UARTなし）

1. ターゲットボード電源OFF → ON
2. ディスプレイに画面表示されればOK

## トラブルシューティング（最小限）

### "Connection failed"
→ エミュレータのUSB接続を確認

### "Python not found"
→ Pythonインストール: https://www.python.org/downloads/

### "CRC32 mismatch"
→ 手動でpost-build実行:
```powershell
cd Firmware\HardwareDebug
python post_build.py Firmware.elf
```
その後、ステップ3から再実行

### アプリケーションが起動しない
→ 両方のMOTファイルが書き込まれているか確認
→ フルチップ消去してから再書き込み

## ファームウェア更新テスト

1. **ターゲットボード電源ON**
2. **5秒以内に`U`キーを送信**（UART経由）
3. **更新モード確認:**
```
[BOOTLOADER] Update mode activated
[BOOTLOADER] Waiting for firmware data...
```
4. **更新実行:**
```powershell
cd tools
python update_firmware.py COM3 ..\Firmware\HardwareDebug\Firmware.mot
```

## 完了！

これで、ブートローダー対応のFULLMONI-WIDEが完成しました。

詳細情報:
- ビルド: [BUILD_GUIDE.md](Firmware/HardwareDebug/BUILD_GUIDE.md)
- フラッシュ書き込み: [FLASH_PROGRAMMING_GUIDE.md](Firmware/HardwareDebug/FLASH_PROGRAMMING_GUIDE.md)
- 完全仕様: [README_BOOTLOADER.md](README_BOOTLOADER.md)
