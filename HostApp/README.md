# FULLMONI-WIDE Host Applications

FULLMONI-WIDEファームウェアと通信するホストアプリケーション集です。

## アプリケーション一覧

### FULLMONI-WIDE Terminal (Windows)

UARTターミナルアプリケーション。ファームウェアのパラメータコンソールと対話するためのGUIツールです。

- 詳細: [FULLMONI-WIDE-Terminal/README.md](FULLMONI-WIDE-Terminal/README.md)

### FULLMONI-WIDE Terminal (Android)

USB Serial通信によるモバイル版ターミナルアプリケーション。Jetpack Compose + Material 3で構築。

- 詳細: [FULLMONI-WIDE-Android/README.md](FULLMONI-WIDE-Android/README.md)

## 必要環境

- Windows 10/11
- .NET 8.0 SDK（開発・ビルド時）
- .NET 8.0 Desktop Runtime（実行時のみ必要な場合）

### .NET 8.0 SDKのインストール

以下のURLからダウンロード:
https://dotnet.microsoft.com/download/dotnet/8.0

## ビルド方法

各アプリケーションのディレクトリで以下を実行:

```powershell
dotnet build
```

## 発行（配布用実行ファイル作成）

```powershell
dotnet publish -c Release -r win-x64 --self-contained
```

これにより、.NET Runtimeがインストールされていない環境でも実行可能なスタンドアロン実行ファイルが作成されます。
