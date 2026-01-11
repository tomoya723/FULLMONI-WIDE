# スタートアップ画面リソース分離・カスタム設定化 実装サマリー
# Startup Screen Resource Separation and Customization - Implementation Summary

## 実装概要 / Overview

このPRは、FULLMONI-WIDEのスタートアップ画面（起動時のロゴ画面）をカスタマイズできるようにするための実装です。  
This PR implements customization support for the FULLMONI-WIDE startup screen (boot logo).

**Issue**: #始動画面リソース分離・カスタム設定化

## 実装内容 / Implementation Details

### 1. 設定ファイルの作成 / Configuration File Creation

**ファイル**: `Firmware/src/startup_config.h`

- スタートアップ画面の表示時間を定義
- カスタマイズ手順を詳しく記載
- ビルド時設定の中心的な管理場所

### 2. 包括的なドキュメント / Comprehensive Documentation

**ファイル**: `Firmware/docs/STARTUP_SCREEN_CUSTOMIZATION.md`

- スタートアップ画面カスタマイズの完全ガイド
- 2つの方法を説明:
  - **方法1**: emWin AppWizard使用（推奨）
  - **方法2**: 生成ファイル直接編集（上級者向け）
- トラブルシューティングガイド付き
- 日本語・英語の両言語対応

### 3. カスタムリソース用ディレクトリ構造 / Custom Resources Directory Structure

**ディレクトリ**: `Firmware/custom_resources/startup_images/`

- ユーザーのカスタム画像を配置する専用ディレクトリ
- README.md で使い方を説明
- .gitignore でユーザー画像を除外（テンプレートは除く）
- .gitkeep でディレクトリ構造を保持

### 4. ヘルパースクリプト / Helper Scripts

**ファイル**: `Firmware/customize_startup.sh`

- カスタム画像の準備を簡単にするスクリプト
- 画像ファイルをカスタムディレクトリにコピー
- 画像情報を検証（ImageMagick使用時）
- 次のステップを案内

**使用例**:
```bash
cd Firmware
./customize_startup.sh my_logo.bmp aw001
```

### 5. 検証スクリプト / Validation Script

**ファイル**: `Firmware/validate_startup_config.sh`

- すべての設定ファイルの存在を確認
- ディレクトリ構造を検証
- ビルド設定をチェック
- カラー出力で結果を表示

**使用例**:
```bash
cd Firmware
./validate_startup_config.sh
```

### 6. ドキュメントの更新 / Documentation Updates

**更新ファイル**:
- `Firmware/README.md` - クイックスタートセクション追加
- `Firmware/src/main.c` - startup_config.hへの参照コメント追加

## 技術的な設計判断 / Technical Design Decisions

### なぜランタイム設定ではなくビルド時設定？ / Why Build-Time Configuration?

1. **emWinの制約**: AppWizardで生成されたコードは静的な画像参照を使用
2. **フラッシュメモリ効率**: 不要な画像をビルドに含めない
3. **既存アーキテクチャとの互換性**: 既存のビルドシステムに適合

### 分離されたリソースの利点 / Benefits of Resource Separation

1. **カスタマイズの容易さ**: 専用ディレクトリで画像を管理
2. **バージョン管理**: ユーザー画像はGitから除外
3. **明確な文書化**: ステップバイステップのガイド
4. **後方互換性**: 既存のビルドに影響なし

## ファイル構造 / File Structure

```
Firmware/
├── src/
│   ├── startup_config.h          [新規] 設定ヘッダ
│   └── main.c                     [更新] 参照コメント追加
├── docs/
│   └── STARTUP_SCREEN_CUSTOMIZATION.md  [新規] カスタマイズガイド
├── custom_resources/
│   └── startup_images/            [新規] カスタム画像ディレクトリ
│       ├── README.md
│       ├── .gitignore
│       └── .gitkeep
├── customize_startup.sh           [新規] ヘルパースクリプト
├── validate_startup_config.sh     [新規] 検証スクリプト
└── README.md                      [更新] クイックスタート追加
```

## 使用方法 / Usage

### クイックスタート / Quick Start

1. **カスタム画像を準備** (765x256 BMP推奨)
   ```bash
   # 画像を用意
   # Prepare your image (e.g., my_logo.bmp)
   ```

2. **ヘルパースクリプトを実行**
   ```bash
   cd Firmware
   ./customize_startup.sh my_logo.bmp aw001
   ```

3. **emWin AppWizardで統合**
   - e² studioで aw001.AppWizard を開く
   - Resource → Images → Add から画像追加
   - ID_SCREEN_00 で画像を変更
   - コード生成してビルド

### 検証 / Validation

```bash
cd Firmware
./validate_startup_config.sh
```

## 互換性 / Compatibility

- ✅ 既存のビルドシステムと互換性あり
- ✅ aw001とaw002の両方をサポート
- ✅ e² studio と VS Code の両方で動作
- ✅ 既存のスタートアップ画面（mtc.bmp）は保持
- ✅ 後方互換性を維持

## テスト結果 / Test Results

- ✅ 全ファイルが正しく配置
- ✅ 検証スクリプトがすべてのチェックに合格
- ✅ ヘルパースクリプトが実行可能
- ✅ ドキュメントが完全

## 今後の拡張可能性 / Future Extensibility

この実装により、以下の拡張が容易になります:
This implementation makes the following extensions easier:

1. **複数のスタートアップ画像**: 条件分岐でロゴを変更
2. **アニメーション対応**: 複数フレームの画像シーケンス
3. **設定ツール**: GUI設定ツールの追加
4. **テンプレートギャラリー**: サンプルロゴの提供

## 関連ドキュメント / Related Documentation

- [スタートアップ画面カスタマイズガイド](docs/STARTUP_SCREEN_CUSTOMIZATION.md)
- [Firmware README](README.md)
- [設定ヘッダ](src/startup_config.h)

## ライセンス / License

このコードはプロジェクトのライセンス（MIT License）に従います。  
This code follows the project's license (MIT License).

---

**注意**: AppWizardで生成されるファイル（`Source/Generated/`）は直接編集を避け、  
可能な限りAppWizard経由で変更することを推奨します。  
**Note**: Avoid directly editing AppWizard-generated files (`Source/Generated/`).  
Use AppWizard for modifications whenever possible.
