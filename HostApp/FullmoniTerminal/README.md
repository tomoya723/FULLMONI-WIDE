# FULLMONI-WIDE Terminal

FULLMONI-WIDEファームウェアとUART通信するためのWindowsホストアプリケーションです。

## 機能

- **シリアル通信**: COMポートを選択して115200bps（8N1）で接続
- **ターミナル表示**: ファームウェアからの応答をリアルタイム表示
- **コマンド入力**: テキストボックスからコマンドを送信
- **クイックコマンド**: よく使うコマンドをワンクリックで送信
- **コマンド履歴**: 上下矢印キーで過去のコマンドを呼び出し

## 対応コマンド（ファームウェア側）

| コマンド | 説明 |
|---------|------|
| `help` | コマンド一覧表示 |
| `list` | 現在のパラメータ一覧 |
| `set <id> <value>` | パラメータ設定 |
| `save` | EEPROMに保存 |
| `load` | EEPROMから読込 |
| `default` | デフォルト値にリセット |
| `rtc` | RTC時刻表示 |
| `rtc YY MM DD hh mm ss` | RTC時刻設定 |
| `odo` | オドメーター表示 |
| `odo_init <km>` | オドメーター初期化 |
| `trip` | トリップメーター表示 |
| `trip_reset` | トリップリセット |
| `fwupdate` | ファームウェアアップデートモード |
| `exit` | パラメータモード終了 |

## パラメータID

- タイヤサイズ: `tyre_width`, `tyre_aspect`, `tyre_rim`
- ギア比: `gear1` ~ `gear6`, `final`
- 警告設定: `water_low`, `water_high`, `fuel_warn`

## ビルド方法

### 必要環境
- .NET 8.0 SDK
- Visual Studio 2022 または VS Code

### ビルドコマンド
```bash
cd HostApp/FullmoniTerminal
dotnet build
```

### 実行
```bash
dotnet run
```

### 発行（スタンドアロン実行ファイル）
```bash
dotnet publish -c Release -r win-x64 --self-contained
```

## 使用方法

1. アプリを起動
2. COMポートを選択（ファームウェアが接続されているポート）
3. ボーレートは115200（デフォルト）
4. 「Connect」ボタンをクリック
5. ファームウェア側で任意のキーを押してパラメータモードに入る
6. コマンドを入力またはクイックコマンドをクリック

## 技術仕様

- **フレームワーク**: .NET 8.0 WPF
- **アーキテクチャ**: MVVM (CommunityToolkit.Mvvm)
- **シリアル通信**: System.IO.Ports
- **通信設定**: 115200bps, 8N1

## ライセンス

FULLMONI-WIDE Projectの一部として提供されます。
