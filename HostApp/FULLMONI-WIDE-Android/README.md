# FULLMONI-WIDE Android Terminal

FULLMONI-WIDEファームウェアとUSB通信するためのAndroidアプリケーションです。

## 概要

Windows版 FULLMONI-WIDE-Terminal と同等の機能を持つAndroid版アプリです。  
USB OTGケーブル経由でファームウェアと直接通信し、パラメータ設定やファームウェア更新が可能です。

## 機能

### 🏠 Home（ダッシュボード）
- ヒーローバナーによる接続状態表示
- クイックアクセスカードで各機能に素早くアクセス
- 接続デバイス情報の表示

### 📊 Status（ステータス）
- ODO/TRIP/DateTime のリアルタイム表示
- TRIP リセット機能
- RTC 同期機能（スマートフォンの時刻で同期）

### ⚙️ Settings（車両設定）
- **タイヤサイズ**: 幅/扁平率/リム径
- **ギア比**: 1〜6速およびファイナルギア（プリセット選択可能）
- **警告設定**: 水温低温/高温、燃料警告閾値
- **シフトインジケータ**: 5段階のシフトアップRPM設定

### 🔌 CAN Config（CAN設定）
- Master Warning設定（有効/サウンド）
- 6チャンネルの受信ID設定
- 16フィールドのデータ定義（変数割り当て、係数設定、警告閾値）

### 🖼️ Boot Screen（起動画面）
- 765×256ピクセルの画像選択とプレビュー
- デバイスへの書き込み/読み込み
- ファイルへの保存

### 📥 FW Update（ファームウェア更新）
- .motファイル選択
- 転送進捗表示
- 通信ログ表示

### ℹ️ About（情報）
- アプリバージョン情報
- ハードウェア情報
- ライセンス情報
- GitHubリンク

## 動作環境

- Android 5.0 (API Level 21) 以上
- USB Host機能対応端末
- USB OTGケーブル

## 技術スタック

- **言語**: Kotlin
- **UI**: Jetpack Compose + Material 3
- **USB通信**: usb-serial-for-android ライブラリ
- **アーキテクチャ**: MVVM
- **テーマ**: Windows版と統一されたダークテーマ（Cyan/Tealアクセント）

## プロジェクト構成

```
app/
├── src/main/
│   ├── java/com/fullmoni/terminal/
│   │   ├── MainActivity.kt
│   │   ├── ui/
│   │   │   ├── theme/
│   │   │   │   ├── Color.kt
│   │   │   │   ├── Theme.kt
│   │   │   │   └── Type.kt
│   │   │   ├── screens/
│   │   │   │   ├── MainScreen.kt
│   │   │   │   ├── HomeScreen.kt
│   │   │   │   ├── StatusScreen.kt
│   │   │   │   ├── SettingsScreen.kt
│   │   │   │   ├── CanConfigScreen.kt
│   │   │   │   ├── BootScreenScreen.kt
│   │   │   │   ├── FirmwareUpdateScreen.kt
│   │   │   │   ├── AboutScreen.kt
│   │   │   │   └── TerminalScreen.kt
│   │   │   └── components/
│   │   │       └── SharedComponents.kt
│   │   ├── viewmodel/
│   │   │   └── MainViewModel.kt
│   │   └── service/
│   │       └── UsbSerialService.kt
│   ├── res/
│   └── AndroidManifest.xml
├── build.gradle.kts
└── proguard-rules.pro
```

## ビルド方法

### 必要環境
- Android Studio Hedgehog (2023.1.1) 以降
- JDK 17
- Android SDK (API Level 34)

### ビルド手順
1. Android Studio でプロジェクトを開く
2. Gradle Sync を実行
3. Build > Make Project

### デバッグ実行
1. USB OTG対応のAndroid端末を接続
2. Run > Run 'app'

### エミュレーターで実機テスト（TCP Bridge）

Android エミュレーターではUSBデバイスにアクセスできませんが、**TCP Bridge** 機能を使うことで、PCに接続された実機FULLMONI-WIDEとエミュレーター上のアプリを通信させることができます。

#### 前提条件
- FULLMONI-WIDEがPCのCOMポート（例: COM19）に接続されている
- Android Emulatorが起動している
- ADBにパスが通っている（または `C:\Users\<user>\AppData\Local\Android\Sdk\platform-tools\adb.exe` を使用）

#### 手順

**ターミナル1: TCPブリッジを起動**
```powershell
cd tools
.\com_bridge.ps1 -ComPort COM19
```
出力例:
```
=== COM to TCP Bridge ===
COM Port: COM19
TCP Port: 9999

[OK] Serial port opened
[OK] TCP listener started on port 9999

Waiting for connection...
```

**ターミナル2: ADB Reverseを設定してアプリをインストール**
```powershell
# エミュレーターのポート転送を設定（これにより127.0.0.1:9999がPCの9999に転送される）
adb reverse tcp:9999 tcp:9999

# APKをビルド＆インストール
cd HostApp\FULLMONI-WIDE-Android
.\gradlew assembleDebug
adb install -r app\build\outputs\apk\debug\app-debug.apk

# アプリを起動
adb shell am start -n com.fullmoni.terminal/.MainActivity
```

**アプリ内で接続**
1. アプリのHomeScreen で **「TCP Bridge (Real)」** ボタンをタップ
2. ターミナル1に `[CONNECTED]` が表示される
3. 実機のパラメータが読み込まれ、UIが接続状態になる

#### 動作確認ログ例
```
[CONNECTED] Client connected from 127.0.0.1:54423
[TX->DEVICE] 
[RX<-DEVICE] ========================================
  FULLMONI-WIDE Parameter Console
========================================
>
[TX->DEVICE] version
[RX<-DEVICE] VERSION 1.0.0
>
[TX->DEVICE] list
[RX<-DEVICE] === Current Parameters ===
ODO: 184692 km  TRIP: 114.7 km
```

#### トラブルシューティング

| 問題 | 解決方法 |
|------|----------|
| `Access to port 'COM19' was denied` | 前のブリッジプロセスがポートを掴んでいる。PowerShellを全て閉じて再試行 |
| 接続後すぐに切断される | `adb reverse tcp:9999 tcp:9999` を再実行 |
| ボタンを押してもログが出ない | APKが古い可能性。再ビルド＆インストール |
| エミュレーターが見つからない | `adb devices` で確認。`emulator-5554` が表示されるか確認 |

#### 仕組み

```
┌─────────────────┐     TCP      ┌─────────────────┐    Serial    ┌─────────────┐
│ Android         │◄────────────►│ com_bridge.ps1  │◄────────────►│ FULLMONI    │
│ Emulator        │  127.0.0.1   │ (PC)            │   COM19      │ -WIDE       │
│ (127.0.0.1:9999)│   :9999      │                 │   115200bps  │ (実機)      │
└─────────────────┘              └─────────────────┘              └─────────────┘
         ▲
         │ adb reverse tcp:9999 tcp:9999
         │ (エミュレーター内の127.0.0.1:9999をPCの9999に転送)
         ▼
┌─────────────────┐
│ PC TCP:9999     │
│ (Listener)      │
└─────────────────┘
```

#### 自動テストスクリプト

`tools/tcp_bridge_test.ps1` を使用すると、上記の手順を自動化できます:

```powershell
cd tools
.\tcp_bridge_test.ps1
```

## 使用方法

1. USB OTGケーブルでFULLMONI-WIDEデバイスとスマートフォンを接続
2. アプリを起動
3. USB接続許可ダイアログで「許可」をタップ
4. ナビゲーションドロワーを開き「Connect」ボタンをタップ
5. 接続状態がグリーンに変わったらHomeを選択
6. 各機能カードをタップして必要な画面に移動

## 対応コマンド

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

## Windows版との対応

| Windows機能 | Android対応 |
|------------|------------|
| Home | ✅ 実装済み |
| Status | ✅ 実装済み |
| Settings | ✅ 実装済み |
| CAN Config | ✅ 実装済み |
| Boot Screen | ✅ 実装済み |
| FW Update | ✅ 実装済み |
| About | ✅ 実装済み |
| Terminal | ✅ 実装済み |

## ライセンス

MIT License

## 依存ライブラリ

- [usb-serial-for-android](https://github.com/mik3y/usb-serial-for-android) - Apache License 2.0
- Jetpack Compose - Apache License 2.0
- Material 3 - Apache License 2.0
