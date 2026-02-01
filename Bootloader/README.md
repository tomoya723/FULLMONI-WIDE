# RX72N USB CDC ブートローダー

USB CDC（仮想COMポート）経由でファームウェア更新を行うブートローダーです。
**自動消去 + Size prefix + ACK プロトコル** により、確実な転送を実現します。

> ⚠️ **警告**: Smart Configuratorでコード生成を実行しないでください。
> カスタム実装が上書きされ、動作不能になる可能性があります。

## 特徴

- **MCU**: RX72N (4MB Flash LINEAR MODE)
- **インターフェース**: USB0 CDC（仮想COMポート）
- **プロトコル**: 自動消去 + Size prefix (4byte LE) + ACK ('.') フロー制御
- **対話型コマンド**: なし（USB接続後、自動でアップデートモード）
- **メモリマップ**:
  - ブートローダー: 0xFFC00000 - 0xFFC1FFFF (128KB)
  - アプリケーション: 0xFFC20000 - 0xFFDFFFFF (1.75MB)
  - 起動画像領域: 0xFFE00000 - 0xFFFFFFFF (2MB, ファームウェア更新で保持)
- **Flash書き込み**: RAM2から実行（RX72N 4MB Linear Mode対応）

## 動作フロー

```
リセット
   ↓
ブートローダー起動
   ↓
強制アップデートフラグ確認（RAM2: 0x0087FFF0）
   ↓
┌─────────────────┬─────────────────┐
│ フラグあり      │ フラグなし      │
│ (0xDEADBEEF)    │                 │
│      ↓          │      ↓          │
│ フラグクリア    │ FWヘッダ検証    │
│      ↓          │ (0xFFC20000)    │
│ USB CDC初期化   │      ↓          │
│      ↓          │ ┌────┴────┐     │
│ 自動消去        │ │Magic  Magic│  │
│      ↓          │ │一致   不一致│ │
│ "READY" 送信    │ │↓       ↓   │  │
│      ↓          │ │アプリ  USB │  │
│ サイズ+データ   │ │起動   CDC  │  │
│ 受信待ち        │ │      初期化│  │
│                 │ │      自動 │  │
│                 │ │      消去 │  │
│                 │ │      ... │   │
│                 │ └─────────┘    │
└─────────────────┴─────────────────┘
```

### アプリケーション有効性判定

| 条件 | Magic値 @ 0xFFC20000 | 判定 | 動作 |
|------|----------------------|------|------|
| 正常なアプリ | `0x52584657` ("RXFW") | 有効 | アプリ起動 |
| Flash消去済み | `0xFFFFFFFF` | 無効 | アップデートモード |
| 書き込み途中 | 不定値 | 無効 | アップデートモード |

## ファームウェア転送プロトコル

### ストリーミングモード（高速転送）

```
PC                              Bootloader
│                                    │
│         （USB接続）                │
│                                    │ Flash自動消去
│                                    │ (0xFFC20000-0xFFDFFFFF)
│  <─────── "READY\r\n" ───────────│
│                                    │
│─────── Size (4byte LE) ─────────>  │
│  <─────── ACK ('.') ──────────────│
│                                    │
│─────── Data (16KB chunk) ────────> │
│─────── Data (16KB chunk) ────────> │ リングバッファに蓄積
│─────── Data (16KB chunk) ────────> │ → Flash書き込み
│        ... ストリーミング ...      │ （中間ACKなし）
│                                    │
│─────── 最終データ ────────────────>│
│                                    │ 全データ書き込み完了
│  <─── "Done!\r\n" ───────────────│
│                                    │ 自動リセット
```

### プロトコル詳細

| 項目 | 値 |
|------|-----|
| 準備完了メッセージ | `READY\r\n` |
| サイズプレフィックス | 4バイト、リトルエンディアン |
| データ送信単位 | 4KB（ホストアプリ側チャンクサイズ） |
| 転送モード | ストリーミング（中間ACKなし） |
| 初回ACK文字 | `.` (0x2E) - サイズ受信後のみ |
| 完了メッセージ | `Done!\r\n` |
| エラーメッセージ | `Write ERR\r\n` |
| 転送速度 | 約268 KB/s（1.27MB → 約4.6秒） |

## ファームウェア転送手順

### 1. BINファイル生成

Firmwareプロジェクトのビルド後、`Firmware.bin`が自動生成されます。

```
Firmware/HardwareDebug/Firmware.bin
```

### 2. Python スクリプトで転送（推奨）

`fw_upload.py` を使用して転送します。

```bash
cd Bootloader/tools
py fw_upload.py -p COM7 ..\..\Firmware\HardwareDebug\Firmware.bin
```

**出力例:**
```
Loading firmware: ..\..\Firmware\HardwareDebug\Firmware.bin
Firmware size: 1,256,840 bytes
Firmware header: OK (RXFW)
Opening COM7 at 115200 baud...
Sending 'U' command...
Waiting for 'Erase OK'...

Erase OK! Send size+firmware

Sending size: 1256840 bytes...
Size ACK received
Sending firmware data...
  102,400/1,256,840 bytes (8%) - 237.9 KB/s
  ...
  1,249,280/1,256,840 bytes (99%) - 230.6 KB/s

Transfer complete: 1,256,840 bytes in 5.3s (230.6 KB/s)

==================================================
Firmware upload successful!
Bootloader will reset and start new firmware.
==================================================
```

### 3. 転送手順（手動）

#### 方法A: Firmwareの`fwupdate`コマンドを使用（推奨）

1. USB接続してターミナルを開く
2. 任意のキーを押してパラメータモードに入る
3. `fwupdate` コマンドを入力
4. `yes` を入力して確認
5. 自動的にBootloaderが起動
6. `fw_upload.py` で転送
7. 転送完了後、自動リセット → アプリ起動

#### 方法B: Flash消去済み状態から

1. USB接続（COMポートが認識される）
2. ボードをリセット（または電源投入）
3. アプリが無効の場合、自動的にアップデートモードへ
4. `fw_upload.py` で転送
5. 転送完了後、自動リセット → アプリ起動

### 転送時間の目安

| ファイルサイズ | 転送時間 | 転送速度 |
|---------------|---------|----------|
| 1.27MB | 約4.6秒 | ~268 KB/s |

## トラブルシューティング

### COMポートが認識されない

- USBケーブルがデータ通信対応か確認
- デバイスマネージャーでCOMポート番号を確認
- 別のUSBポートを試す

### 転送が途中で止まる

- COMポート番号を確認
- ボーレートが115200になっているか確認
- `fw_upload.py` を使用しているか確認

### ベリファイNG

- BINファイルが正しく生成されているか確認
- Firmwareプロジェクトを再ビルド

### アップデートモードに入らない

- Firmwareの `fwupdate` コマンドを使用
- または、ファームウェアが壊れている場合は電源再投入で自動的にアップデートモードへ

## 技術詳細

### USB CDC ドライバ

| 項目 | 内容 |
|------|------|
| USB FIT | r_usb_basic v1.44 |
| CDC FIT | r_usb_pcdc v1.44 |
| VID/PID | Renesas デフォルト |

### RX72N 4MB Linear Mode対応

RX72Nの4MB Flashは単一バンク構成のため、Flashに書き込み中は同じFlashからの実行ができません。
この制限を回避するため、Flash書き込み関数をRAM2（0x00800000）にコピーして実行しています。

### リングバッファ

256KBのリングバッファを使用してFlash書き込み中のデータを保持します。
ACKベースのフロー制御と組み合わせることで、オーバーフローを防止します。

## ファイル構成

```
Bootloader/
├── src/
│   ├── main.c                 # エントリポイント
│   ├── main_fit_cdc.c         # USB CDCブートローダー本体
│   ├── boot_loader.c          # Flash書き込み処理
│   ├── boot_loader.h          # ヘッダ
│   └── linker_script.ld       # リンカスクリプト
├── tools/
│   └── fw_upload.py           # ファームウェア転送スクリプト
├── HardwareDebug/
│   ├── Bootloader.elf         # デバッグ用ELF
│   └── Bootloader.mot         # 書き込み用MOT
└── README.md                  # このファイル
```

## UART版からの変更点

本ブートローダーは元々UART（SCI9）版でしたが、USB CDC版に変更されました。

| 項目 | UART版 | USB CDC版 |
|------|--------|-----------|
| ハードウェア | SCI9 @ 115200bps | USB0 CDC |
| フロー制御 | XON/XOFF (0x11/0x13) | Size prefix + ACK |
| ドライバ | r_Config_SCI9 | r_usb_basic + r_usb_pcdc |

**変更されていない部分:**
- コマンド体系（U, B, S, R）
- メモリマップ
- ファームウェアヘッダ形式
- 強制アップデートフラグ（RAM2経由）
- 起動フロー

## ライセンス

BSD-3-Clause (Renesas Electronics Corporation)
