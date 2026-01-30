# FULLMONI-WIDE 開発環境構築ガイド

このドキュメントは、FULLMONI-WIDEファームウェア開発に必要なツール・環境の構築手順を説明します。

## 動作確認済み環境

### IDE・コンパイラ

| ツール | バージョン | 説明 |
|--------|-----------|------|
| **Renesas e² studio** | 2025-12 (25.12.0) | 統合開発環境 |
| | Build Id: R20251204-1223 | |
| **GCC for Renesas RX** | 25.12.0.v20251117 | C/C++コンパイラ |

### GUIフレームワーク

| ツール | バージョン | 説明 |
|--------|-----------|------|
| **SEGGER emWin** | 6.14a～6.14g | GUIライブラリ（Smart Configurator経由） |
| **SEGGER AppWizard** | V136a_632a | GUI リソース設計ツール |

### ターゲットハードウェア

| 項目 | 仕様 |
|------|------|
| **マイコン** | RX72N |
| **CPUコア** | RXv3 |
| **Flash** | 4MB（Linear Mode対応） |
| **RAM** | 1MB |
| **パッケージ** | LFQFP/176 (0.50mm ピッチ) |
| **デバッガ** | E2 Lite（推奨） |

---

## ダウンロードリンク

| ツール | リンク |
|--------|--------|
| **Renesas e² studio** | [https://www.renesas.com/ja/software-tool/e2studio-information-rx-family](https://www.renesas.com/ja/software-tool/e2studio-information-rx-family) |
| **GCC for Renesas RX** | [https://llvm-gcc-renesas.com/ja/rx-download-toolchains/](https://llvm-gcc-renesas.com/ja/rx-download-toolchains/) |
| **E2 Lite ドライバ** | [https://www.renesas.com/eu/en/software-tool/e2-emulator-rte0t00020kce00000r#downloads](https://www.renesas.com/eu/en/software-tool/e2-emulator-rte0t00020kce00000r#downloads) |
| **SEGGER AppWizard** | [https://www.segger.com/downloads/emwin/](https://www.segger.com/downloads/emwin/) |

---

## 環境構築手順

### 1. Renesas e² studio のインストール

1. [Renesas公式サイト](https://www.renesas.com/ja/software-tool/e2studio-information-rx-family)から **e² studio 2025-12** をダウンロード
2. インストーラを実行
3. インストール時に以下を選択：
   - **RX Family** を対象デバイスとして選択
   - **GCC for Renesas RX** をツールチェーンとして選択（後からでも追加可能）

### 2. GCC for Renesas RX のインストール

e² studio インストール時に選択しなかった場合：

1. [GCC-RX ダウンロードページ](https://llvm-gcc-renesas.com/ja/rx-download-toolchains/)から **GNURX 25.12.0.v20251117** をダウンロード
2. インストーラを実行
3. e² studio への統合：
   ```
   Window → Preferences → Renesas → Toolchain Management
   ```
   から GCC-RX を追加

### 3. E2 Lite デバッガドライバのインストール

1. [E2 Lite ダウンロードページ](https://www.renesas.com/eu/en/software-tool/e2-emulator-rte0t00020kce00000r#downloads)からUSBドライバをダウンロード
2. インストーラを実行
3. デバイスマネージャーで「Renesas E2 Lite」が認識されることを確認

### 4. SEGGER AppWizard のインストール（オプション）

GUI画面リソースを編集する場合：

1. [SEGGER AppWizard](https://www.segger.com/downloads/emwin/)から **V136a_632a** 以上をダウンロード
2. インストーラを実行
3. プロジェクト内の `.AppWizard` ファイル（例: `aw001/aw001.AppWizard`）をダブルクリックで開く

---

## プロジェクトのビルド

### e² studio でのビルド

1. e² studio を起動
2. `File` → `Import` → `Existing Projects into Workspace`
3. `Firmware` フォルダを選択してインポート
4. `Project` → `Build Project` (`Ctrl+B`)

### emWin コード生成

1. `.scfg` ファイル（`FULLMONI_WIDE_official.scfg`）をダブルクリック
2. Smart Configurator が起動
3. **Code Generation** をクリック

> ⚠️ **注意**: emWin (`r_emwin_rx`) は再配布禁止のため、本リポジトリには含まれていません。
> Smart Configurator から自分でコード生成を実行してください。

---

## Visual Studio Code での開発（オプション）

e² studio と並行して VSCode を使用することも可能です。
詳細は [README.md](../README.md#development-with-visual-studio-code) を参照してください。

---

## トラブルシューティング

### 「rx-elf-gcc not found」エラー

**原因**: GCC-RX のパスが認識されていない

**解決法**:
```
e² studio → Window → Preferences → Renesas → Toolchain Management
```
から GCC-RX のインストールパスを確認・追加

### Smart Configurator で emWin が見つからない

**原因**: `r_emwin_rx` がプロジェクトに追加されていない

**解決法**:
1. Smart Configurator → Components
2. `r_emwin_rx` を検索して **Add**
3. **Code Generation** を実行

### E2 Lite 接続エラー

**原因**: ドライバが正しくインストールされていない

**解決法**:
1. デバイスマネージャーで認識状況を確認
2. ドライバを再インストール
3. 別のUSBポートで試す

---

## 関連ドキュメント

- [README.md](../README.md) - プロジェクト概要
- [PARAM_CONSOLE.md](PARAM_CONSOLE.md) - パラメータコンソールの使い方
- [STARTUP_IMAGE_WRITE.md](STARTUP_IMAGE_WRITE.md) - 起動画像書き込み手順

---

**最終更新**: 2026年1月24日
