# FULLMONI-WIDE Android Terminal

FULLMONI-WIDEファームウェアとUSB通信するためのAndroidアプリケーションです。

## 概要

Windows版 FULLMONI-WIDE-Terminal のAndroid移植版として、USB OTGケーブル経由でファームウェアと直接通信します。

## 機能

### ターミナル機能
- **USB CDC ACM通信**: USB-UARTブリッジ経由で115200bps（8N1, XON/XOFF）で接続
- **ターミナル表示**: ファームウェアからの応答をリアルタイム表示
- **コマンド入力**: テキストボックスからコマンドを送信
- **クイックコマンド**: よく使うコマンドをワンタップで送信

### パラメータ設定GUI
- **タイヤサイズ設定**: 幅/扁平率/リム径を入力
- **ギア比設定**: 1〜6速およびファイナルギア
- **警告設定**: 水温低温/高温、燃料警告閾値
- **シフトインジケータ設定**: 5段階のシフトアップRPM設定

## 動作環境

- Android 5.0 (API Level 21) 以上
- USB Host機能対応端末
- USB OTGケーブル

## 技術スタック

- **言語**: Kotlin
- **UI**: Jetpack Compose
- **USB通信**: usb-serial-for-android ライブラリ
- **アーキテクチャ**: MVVM

## プロジェクト構成

```
app/
├── src/main/
│   ├── java/com/fullmoni/terminal/
│   │   ├── MainActivity.kt
│   │   ├── ui/
│   │   │   ├── theme/
│   │   │   ├── screens/
│   │   │   │   ├── TerminalScreen.kt
│   │   │   │   └── ParameterScreen.kt
│   │   │   └── components/
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

## 使用方法

1. USB OTGケーブルでFULLMONI-WIDEデバイスとスマートフォンを接続
2. アプリを起動
3. USB接続許可ダイアログで「許可」をタップ
4. 「接続」ボタンをタップ
5. ターミナル画面でコマンドを入力して通信

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

## ライセンス

MIT License

## 依存ライブラリ

- [usb-serial-for-android](https://github.com/mik3y/usb-serial-for-android) - Apache License 2.0
