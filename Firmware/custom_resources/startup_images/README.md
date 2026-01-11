# カスタムスタートアップ画像
# Custom Startup Images

このディレクトリは、カスタムスタートアップ画像を配置するための場所です。  
This directory is for placing custom startup images.

## 使い方 / Usage

1. **カスタムロゴ画像を準備 / Prepare Your Custom Logo**
   - サイズ: 765x256 ピクセル / Size: 765x256 pixels
   - 形式: BMP (16-bit color推奨) / Format: BMP (16-bit color recommended)

2. **このディレクトリに画像を配置 / Place Image in This Directory**
   ```
   custom_resources/startup_images/my_logo.bmp
   ```

3. **カスタマイズガイドに従う / Follow Customization Guide**
   ```
   詳細は docs/STARTUP_SCREEN_CUSTOMIZATION.md を参照
   See docs/STARTUP_SCREEN_CUSTOMIZATION.md for details
   ```

## ファイル命名規則 / File Naming Convention

- 英数字とアンダースコアのみ使用 / Use only alphanumeric characters and underscores
- 例 / Examples:
  - my_company_logo.bmp
  - custom_startup_2024.bmp
  - team_badge.bmp

## 画像仕様 / Image Specifications

| 項目 / Item | 値 / Value |
|-------------|------------|
| 幅 / Width | 765 pixels |
| 高さ / Height | 256 pixels |
| 色深度 / Color Depth | 16-bit (推奨 / recommended) |
| 形式 / Format | BMP |
| 推奨ファイルサイズ / Recommended File Size | ~570 KB |

## サンプル画像テンプレート / Sample Image Template

サンプルテンプレートが必要な場合は、以下のコマンドで元の画像を参照できます:  
If you need a sample template, you can reference the original image with:

```bash
# aw001のデフォルト画像 / Default image for aw001
cp ../aw001/Resource/Image/mtc.bmp ./template_mtc.bmp

# aw002のデフォルト画像 / Default image for aw002
cp ../aw002/Resource/Image/mtc.bmp ./template_mtc.bmp
```

## 注意事項 / Notes

- このディレクトリの画像は直接ビルドには含まれません
  Images in this directory are not directly included in the build
- AppWizardまたは手動でプロジェクトに統合する必要があります
  Must be integrated into the project via AppWizard or manually
- カスタマイズ方法の詳細は docs/STARTUP_SCREEN_CUSTOMIZATION.md を参照
  See docs/STARTUP_SCREEN_CUSTOMIZATION.md for detailed customization instructions
