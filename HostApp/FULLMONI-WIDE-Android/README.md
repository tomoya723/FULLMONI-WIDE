# FULLMONI-WIDE Android Terminal

FULLMONI-WIDEファームウェアとUSB通信するためのAndroidアプリケーションです。

## 📱 概要

Windows版 FULLMONI-WIDE-Terminal と同等の機能を持つAndroid版アプリです。  
USB OTGケーブル経由でFULLMONI-WIDEデバイスと直接通信し、パラメータ設定やファームウェア更新が可能です。

**初リリース: v1.0.0**

### 主な特徴

- 📶 **USB CDC直接通信**: USB OTGケーブルでデバイスと直接接続
- 🎨 **Material 3 UI**: Windows版と統一されたダークテーマ
- ⚡ **リアルタイム操作**: パラメータ変更が即座に反映
- 🖼️ **起動画面カスタマイズ**: スマホの画像をデバイスに書き込み
- 📦 **ファームウェア更新**: OTA的にファームウェアを更新

---

## 📲 インストール方法

### APKから直接インストール

1. GitHub Releases から `FULLMONI-WIDE-Terminal-android.apk` をダウンロード
2. スマートフォンでダウンロードしたAPKファイルを開く
3. 「提供元不明のアプリ」のインストールを許可
4. インストールを完了

### ソースからビルド

後述の「ビルド方法」セクションを参照してください。

---

## 🔌 接続方法

### 必要なもの

| 機材 | 説明 |
|------|------|
| USB OTGケーブル/アダプタ | スマートフォンのUSB端子に接続 |
| USB Type-A to Micro-Bケーブル | FULLMONI-WIDEとの接続用 |
| FULLMONI-WIDEデバイス | ファームウェアv1.0.0以上 |
| Android端末 | Android 5.0以上、USB Host対応 |

### 接続手順

```
┌─────────────┐    USB OTG     ┌─────────────┐    USB      ┌─────────────┐
│             │    アダプタ     │             │   ケーブル   │             │
│  Android    │◄──────────────►│   USB OTG   │◄───────────►│ FULLMONI    │
│  スマホ     │                │   アダプタ   │             │  -WIDE      │
│             │                │             │             │             │
└─────────────┘                └─────────────┘             └─────────────┘
```

1. USB OTGアダプタをスマートフォンに接続
2. FULLMONI-WIDEをUSBケーブルでOTGアダプタに接続
3. アプリを起動
4. **USB接続許可ダイアログ**が表示されたら「OK」をタップ
5. Home画面の「**Connect Device**」ボタンをタップ
6. 右上のインジケーターが🟢（緑）になれば接続完了

### 接続状態の確認

| インジケーター | 状態 |
|---------------|------|
| 🔴 赤 | 未接続 |
| 🟢 緑 | 接続中 |

---

## 🏠 機能一覧

### Home（ホーム）

ダッシュボード画面です。接続状態と各機能へのクイックアクセスを提供します。

- **ヒーローバナー**: 接続状態、デバイス名を表示
  - 背景画像が5秒ごとにクロスフェードで切り替わります
- **Quick Access カード**: Status, Settings, CAN Config, Boot Screen, FW Update, About への直接リンク
- **接続ボタン**: Connect Device / Disconnect

### Status（ステータス）

車両のステータス情報を表示・管理します。

| 項目 | 説明 | 操作 |
|------|------|------|
| ODO | 総走行距離 (km) | 表示のみ |
| TRIP | トリップメーター (km) | リセット可能 |
| Date/Time | デバイスの日時 | スマホ時刻と同期可能 |

**操作方法**:
- 「Reset TRIP」→ TRIPをゼロにリセット
- 「Sync RTC」→ スマートフォンの現在時刻でデバイスの時計を同期

### Settings（設定）

車両パラメータを設定します。

#### タイヤサイズ
| パラメータ | 範囲 | 例 |
|-----------|------|-----|
| 幅 | 125-355 mm | 245 |
| 扁平率 | 25-80 % | 40 |
| リム径 | 12-22 インチ | 18 |

#### ギア比
- **プリセット選択**: MX-5, S2000, GR86等
- **手動入力**: 1〜6速 + ファイナルギア

#### 警告設定
| 項目 | 説明 | デフォルト |
|------|------|-----------|
| 水温低温警告 | この温度以下で警告 | 60°C |
| 水温高温警告 | この温度以上で警告 | 100°C |
| 燃料警告 | この残量以下で警告 | 10L |

#### シフトインジケーター
エンジン回転数に応じて5段階でLEDが点灯します。
各段階のRPM閾値を設定できます（例: 5000, 6000, 6500, 7000, 7500）。

**操作方法**:
1. 各項目を変更
2. 「**Save to Device**」ボタンで保存
3. 「Saved successfully」と表示されれば完了

### CAN Config（CAN設定）

CAN BUSからのデータ受信設定をカスタマイズします。

#### Master Warning
| 項目 | 説明 |
|------|------|
| Enable | マスターワーニング有効/無効 |
| Sound | 警告音の有効/無効 |

#### 受信チャンネル (6ch)
各チャンネルで受信するCAN IDを16進数で設定（例: `0x201`）

#### データフィールド (16個)
各フィールドに以下を設定:
- **変数**: Speed, RPM, WaterTemp, OilTemp, OilPress等
- **開始バイト**: CAN データ内の位置 (0-7)
- **バイト長**: 1, 2, or 4 bytes
- **エンディアン**: Big/Little
- **係数**: 生データに掛ける倍率
- **オフセット**: 係数適用後に加算する値
- **警告閾値**: この値を超えると警告（0=無効）

### Boot Screen（起動画面）

デバイス起動時に表示される画像をカスタマイズします。

#### 画像仕様
| 項目 | 値 |
|------|-----|
| 解像度 | 765 × 256 ピクセル |
| 形式 | BMP, PNG, JPEG, WebP |
| 内部形式 | RGB565 (16bit) |

#### 操作

| ボタン | 説明 |
|--------|------|
| **Select** | スマートフォンから画像を選択 |
| **Read** | デバイスから現在の起動画像を読み込み |
| **Save** | プレビュー画像をPNGファイルとして保存 |
| **Upload** | 選択した画像をデバイスに書き込み |

#### 書き込み手順
1. 「Select」で画像を選択（自動的に765×256にリサイズ）
2. プレビューで確認
3. 「Upload」をタップ
4. 進捗バーが100%になるまで待機（約30秒）
5. 「Upload completed」と表示されれば成功
6. デバイスを再起動して確認

#### 読み込み手順
1. 「Read」をタップ
2. 進捗バーが100%になるまで待機（約5秒）
3. プレビューに現在の起動画像が表示される
4. 「Save」で画像をファイルに保存可能

**注意**: 転送中は絶対に接続を切らないでください。

### FW Update（ファームウェア更新）

FULLMONI-WIDEのファームウェアを更新します。

#### 更新手順
1. 「Select .mot file」で更新ファイルを選択
2. ファイル情報（サイズ等）を確認
3. 「Start Update」をタップ
4. 進捗バーが100%になるまで待機
5. デバイスが自動的に再起動

**注意**: 
- 更新中は絶対に接続を切らないでください
- 電源が安定していることを確認してください
- 更新失敗時はBootloaderモードで復旧可能です

### About（情報）

アプリとデバイスの情報を表示します。

- アプリバージョン
- ファームウェアバージョン
- ライセンス情報
- GitHubリポジトリへのリンク

---

## 🛠️ 動作環境

### 対応Android
- **最小**: Android 5.0 (API Level 21)
- **推奨**: Android 10.0以上
- **必須**: USB Host (USB OTG) 対応

### USB Host対応の確認方法
1. 「USB Host Diagnostics」等のアプリをインストール
2. USB OTG対応と表示されればOK

多くのAndroid端末がUSB Hostに対応していますが、一部の廉価端末では非対応の場合があります。

---

## 🔧 ビルド方法

### 必要環境
- Android Studio Hedgehog (2023.1.1) 以降
- JDK 17
- Android SDK (API Level 34)

### ビルド手順

```powershell
# リポジトリをクローン
git clone https://github.com/tomoya723/FULLMONI-WIDE.git
cd FULLMONI-WIDE/HostApp/FULLMONI-WIDE-Android

# Gradleラッパーでビルド
$env:JAVA_HOME = "C:\Program Files\Android\Android Studio\jbr"
.\gradlew.bat assembleDebug
```

APKは `app/build/outputs/apk/debug/app-debug.apk` に生成されます。

### リリースビルド

```powershell
.\gradlew.bat assembleRelease
```

**注意**: リリースビルドにはキーストアの設定が必要です。

---

## 🧪 エミュレーターでのテスト（TCP Bridge）

Android エミュレーターではUSBデバイスにアクセスできませんが、**TCP Bridge** 機能を使うことで、PCに接続された実機FULLMONI-WIDEとエミュレーター上のアプリを通信させることができます。

この機能は**エミュレーター実行時のみ**表示されます。

### 仕組み

```
┌─────────────────┐     TCP      ┌─────────────────┐    Serial    ┌─────────────┐
│ Android         │◄────────────►│ com_bridge.ps1  │◄────────────►│ FULLMONI    │
│ Emulator        │  127.0.0.1   │ (PC)            │   COM19      │ -WIDE       │
│ (127.0.0.1:9999)│   :9999      │                 │   115200bps  │ (実機)      │
└─────────────────┘              └─────────────────┘              └─────────────┘
         ▲
         │ adb reverse tcp:9999 tcp:9999
         ▼
┌─────────────────┐
│ PC TCP:9999     │
│ (Listener)      │
└─────────────────┘
```

### 手順

**1. TCPブリッジを起動**
```powershell
cd tools
.\com_bridge.ps1 -ComPort COM19
```

**2. ADB Reverseを設定**
```powershell
adb reverse tcp:9999 tcp:9999
```

**3. APKをインストール＆起動**
```powershell
adb install -r app\build\outputs\apk\debug\app-debug.apk
adb shell am start -n com.fullmoni.terminal/.MainActivity
```

**4. アプリ内で接続**
- Home画面の「**TCP Bridge (Real)**」ボタンをタップ

---

## ❓ トラブルシューティング

### 接続関連

| 問題 | 原因 | 解決方法 |
|------|------|----------|
| Connect Deviceを押しても反応しない | USBデバイスが認識されていない | USB接続を確認、OTGアダプタを確認 |
| USB許可ダイアログが出ない | 既に許可済み/デバイス未認識 | USBを抜き差し、アプリを再起動 |
| 接続後すぐ切断される | シリアル設定の不一致 | デバイス側のボーレートを確認 |
| 「Device not found」エラー | USBデバイスが見つからない | 他のUSBデバイスを外して再試行 |

### Boot Screen関連

| 問題 | 原因 | 解決方法 |
|------|------|----------|
| Upload中にタイムアウト | 通信エラー | 再接続して再試行 |
| 画像が正しく表示されない | サイズが不正 | 765×256の画像を使用 |
| Readで空データが返る | デバイスに画像がない | 一度Uploadしてから再試行 |

### FW Update関連

| 問題 | 原因 | 解決方法 |
|------|------|----------|
| 更新開始しない | .motファイルが不正 | 正しいファームウェアファイルを選択 |
| 更新中にエラー | 通信切断 | 再接続してBootloaderモードで復旧 |
| 更新後起動しない | ファームウェア破損 | Bootloaderモードでファームウェアを再書込 |

---

## 📂 プロジェクト構成

```
app/
├── src/main/
│   ├── java/com/fullmoni/terminal/
│   │   ├── MainActivity.kt              # エントリーポイント
│   │   ├── model/
│   │   │   └── ReleaseManifest.kt       # リリースマニフェストモデル
│   │   ├── ui/
│   │   │   ├── theme/                   # テーマ定義
│   │   │   │   ├── Color.kt
│   │   │   │   ├── Theme.kt
│   │   │   │   └── Type.kt
│   │   │   ├── screens/                 # 各画面
│   │   │   │   ├── MainScreen.kt        # メインナビゲーション
│   │   │   │   ├── HomeScreen.kt        # ホーム
│   │   │   │   ├── StatusScreen.kt      # ステータス
│   │   │   │   ├── SettingsScreen.kt    # 設定
│   │   │   │   ├── CanConfigScreen.kt   # CAN設定
│   │   │   │   ├── BootScreenScreen.kt  # 起動画面
│   │   │   │   ├── FirmwareUpdateScreen.kt # FW更新
│   │   │   │   ├── AboutScreen.kt       # About
│   │   │   │   └── TerminalScreen.kt    # ターミナル
│   │   │   └── components/
│   │   │       └── SharedComponents.kt  # 共通コンポーネント
│   │   ├── viewmodel/
│   │   │   └── MainViewModel.kt         # ビューモデル
│   │   └── service/
│   │       ├── UsbSerialService.kt      # USB通信
│   │       ├── StartupImageService.kt   # 起動画像転送
│   │       ├── FirmwareUpdateService.kt # FW更新
│   │       └── AppWizardImageConverter.kt # 画像変換
│   ├── res/
│   │   ├── drawable-nodpi/              # 背景画像
│   │   │   ├── fm2.png
│   │   │   └── fm3.png
│   │   └── values/
│   └── AndroidManifest.xml
├── build.gradle.kts
└── proguard-rules.pro
```

---

## 📋 対応コマンド

Windows版と同一のコマンドセットに対応しています。

| コマンド | 説明 |
|---------|------|
| `help` | コマンド一覧表示 |
| `version` | ファームウェアバージョン表示 |
| `list` | 現在のパラメータ一覧 |
| `set <id> <value>` | パラメータ設定 |
| `save` | EEPROMに保存 |
| `load` | EEPROMから読込 |
| `default` | デフォルト値にリセット |
| `imgwrite` | 起動画像書き込みモード |
| `imgread` | 起動画像読み取りモード |
| `fwupdate` | ファームウェア更新モード |

---

## 🔄 Windows版との機能対応

| 機能 | Windows | Android | 備考 |
|------|---------|---------|------|
| Home | ✅ | ✅ | 背景画像切替対応 |
| Status | ✅ | ✅ | RTC同期対応 |
| Settings | ✅ | ✅ | プリセット対応 |
| CAN Config | ✅ | ✅ | 16フィールド対応 |
| Boot Screen | ✅ | ✅ | Read/Write/Save対応 |
| FW Update | ✅ | ✅ | .motファイル対応 |
| About | ✅ | ✅ | |
| Terminal | ✅ | ✅ | 開発用 |

---

## 📜 技術仕様

### USB通信
- **ライブラリ**: usb-serial-for-android
- **ボーレート**: 115200 bps
- **データビット**: 8
- **パリティ**: なし
- **ストップビット**: 1
- **フロー制御**: なし

### 起動画像転送プロトコル

**書き込み (imgwrite)**:
1. `imgwrite\r\n` コマンド送信
2. `IMGWRITE_READY` 応答待ち
3. サイズ送信 (4バイト、リトルエンディアン)
4. データ送信 (256バイトチャンク、ACK応答待ち)
5. CRC-16 送信
6. 終端マーカー `0xED 0x0F 0xAA 0x55`
7. `IMGWRITE_OK` 応答待ち

**読み込み (imgread)**:
1. `imgread\r\n` コマンド送信
2. `IMGREAD_READY` 応答待ち
3. サイズ受信 (4バイト)
4. データ受信 (ストリーミング)
5. CRC-16 受信・検証
6. 終端マーカー確認

---

## 📄 ライセンス

MIT License

## 📚 依存ライブラリ

| ライブラリ | ライセンス | 用途 |
|-----------|-----------|------|
| [usb-serial-for-android](https://github.com/mik3y/usb-serial-for-android) | Apache 2.0 | USB CDC通信 |
| Jetpack Compose | Apache 2.0 | UI フレームワーク |
| Material 3 | Apache 2.0 | デザインシステム |
| Kotlinx Serialization | Apache 2.0 | JSON パース |

---

## 🔗 関連リンク

- [FULLMONI-WIDE GitHub](https://github.com/tomoya723/FULLMONI-WIDE)
- [Windows Terminal README](../FULLMONI-WIDE-Terminal/README.md)
- [Firmware README](../../Firmware/README.md)
- [リリース手順書](../../docs/release_procedure.md)
