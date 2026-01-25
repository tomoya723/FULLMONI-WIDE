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
