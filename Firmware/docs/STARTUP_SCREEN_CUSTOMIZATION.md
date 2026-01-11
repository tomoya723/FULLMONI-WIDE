# スタートアップ画面カスタマイズガイド
# Startup Screen Customization Guide

## 概要 / Overview

FULLMONI-WIDEのスタートアップ画面（起動時のロゴ画面）をカスタマイズする方法を説明します。  
This guide explains how to customize the FULLMONI-WIDE startup screen (boot logo screen).

デフォルトでは、起動時に「MTC」ロゴ（mtc.bmp）が1.5秒間表示されます。  
By default, the "MTC" logo (mtc.bmp) is displayed for 1.5 seconds at startup.

## スタートアップ画面の仕様 / Startup Screen Specifications

- **画面ID**: ID_SCREEN_00
- **表示時間**: 1500ms (設定可能)
- **画像サイズ**: 765x256 ピクセル
- **形式**: BMP (emWin対応形式)
- **画像リソース**: awXXX/Resource/Image/mtc.bmp

## カスタマイズ方法 / Customization Methods

### 方法1: emWin AppWizardを使用 (推奨) / Method 1: Using emWin AppWizard (Recommended)

この方法は、GUIツールを使用するため最も安全で推奨されます。  
This method is the safest and most recommended as it uses GUI tools.

#### 手順 / Steps

**1. カスタム画像を準備 / Prepare Custom Image**

```
推奨サイズ: 765x256 ピクセル
推奨形式: BMP (16-bit color)
Recommended size: 765x256 pixels
Recommended format: BMP (16-bit color)
```

画像編集ソフト（GIMP、Photoshopなど）でカスタムロゴを作成します。  
Create a custom logo using image editing software (GIMP, Photoshop, etc.).

**2. e² studioでAppWizardを開く / Open AppWizard in e² studio**

```
1. e² studioでプロジェクトを開く / Open project in e² studio
2. Firmware/awXXX/awXXX.AppWizard をダブルクリック
   Double-click Firmware/awXXX/awXXX.AppWizard
   (例: Firmware/aw001/aw001.AppWizard or Firmware/aw002/aw002.AppWizard)
3. AppWizard エディタが起動 / AppWizard editor launches
```

**3. 画像リソースを追加 / Add Image Resource**

```
1. AppWizard の左ペインで "Resource" → "Images" を選択
   Select "Resource" → "Images" in the left pane of AppWizard
2. 右クリックして "Add" を選択
   Right-click and select "Add"
3. カスタム画像ファイルを選択 (例: my_logo.bmp)
   Select your custom image file (e.g., my_logo.bmp)
4. 画像に名前を付ける (例: custom_logo)
   Name the image (e.g., custom_logo)
5. "OK" をクリック
   Click "OK"
```

**4. スタートアップ画面で画像を変更 / Change Image in Startup Screen**

```
1. AppWizard の左ペインで "Screens" → "ID_SCREEN_00" を選択
   Select "Screens" → "ID_SCREEN_00" in the left pane
2. 画面レイアウトで "ID_IMAGE_01" を選択
   Select "ID_IMAGE_01" in the screen layout
3. 右側の Properties パネルで "Bitmap" を探す
   Find "Bitmap" in the Properties panel on the right
4. ドロップダウンから新しい画像 (custom_logo) を選択
   Select the new image (custom_logo) from the dropdown
5. 必要に応じて画像の位置とサイズを調整
   Adjust image position and size as needed
```

**5. 表示時間を変更 (オプション) / Change Display Duration (Optional)**

```
1. ID_SCREEN_00 内で "ID_TIMER_00" を選択
   Select "ID_TIMER_00" within ID_SCREEN_00
2. Properties パネルで "Period" を見つける
   Find "Period" in the Properties panel
3. 値を変更 (ミリ秒単位、デフォルト: 1500)
   Change the value (in milliseconds, default: 1500)
```

**6. コード生成と再ビルド / Generate Code and Rebuild**

```
1. AppWizard で "File" → "Generate Code" を選択
   Select "File" → "Generate Code" in AppWizard
2. または AppWizard を閉じると自動的に生成される
   Or it will be generated automatically when you close AppWizard
3. e² studio でプロジェクトをビルド
   Build the project in e² studio
```

### 方法2: 生成ファイルを直接編集 (上級者向け) / Method 2: Direct File Editing (Advanced)

**警告**: この方法はAppWizardでコード再生成すると上書きされます。  
**Warning**: This method will be overwritten when regenerating code with AppWizard.

#### 手順 / Steps

**1. 画像ファイルを配置 / Place Image Files**

カスタム画像を以下のディレクトリに配置:  
Place your custom image in the following directory:

```
Firmware/awXXX/Resource/Image/
例: Firmware/aw001/Resource/Image/custom_logo.bmp
```

**2. 画像をC配列に変換 / Convert Image to C Array**

emWinのBitmap Converterを使用して、BMPをC配列に変換:  
Use emWin's Bitmap Converter to convert BMP to C array:

```
1. emWin Bitmap Converter を起動
   Launch emWin Bitmap Converter
2. カスタム画像を開く
   Open your custom image
3. "File" → "Export as C file" を選択
   Select "File" → "Export as C file"
4. ファイルを custom_logo.c として保存
   Save file as custom_logo.c
5. Firmware/awXXX/Resource/Image/ に配置
   Place in Firmware/awXXX/Resource/Image/
```

**3. Resource.h に画像を宣言 / Declare Image in Resource.h**

`Firmware/awXXX/Source/Generated/Resource.h` を編集:  
Edit `Firmware/awXXX/Source/Generated/Resource.h`:

```c
/* Images セクション内に追加 / Add in Images section */
extern GUI_CONST_STORAGE unsigned char acCustomLogo[];
```

**4. ID_SCREEN_00.c の画像参照を変更 / Change Image Reference in ID_SCREEN_00.c**

`Firmware/awXXX/Source/Generated/ID_SCREEN_00.c` の `_aSetup` 配列を編集:  
Edit the `_aSetup` array in `Firmware/awXXX/Source/Generated/ID_SCREEN_00.c`:

変更前 / Before:
```c
{ ID_IMAGE_01,  APPW_SET_PROP_SBITMAP,  { ARG_VP(0, acmtc),
                                          ARG_V(587830), } },
```

変更後 / After:
```c
{ ID_IMAGE_01,  APPW_SET_PROP_SBITMAP,  { ARG_VP(0, acCustomLogo),
                                          ARG_V(<画像サイズ>), } },
```

**5. ビルドパスの設定 / Configure Build Path**

e² studio でカスタム画像の .c ファイルがビルドに含まれるよう設定:  
Configure e² studio to include your custom image .c file in the build:

```
1. プロジェクトを右クリック → "Properties"
   Right-click project → "Properties"
2. "C/C++ Build" → "Settings" → "Tool Settings"
3. "GNU Compiler for RX" → "Source" でリソースフォルダを確認
   Confirm resource folders in "GNU Compiler for RX" → "Source"
4. 必要に応じて追加
   Add if necessary
```

## 設定ファイル / Configuration File

スタートアップ画面の設定は以下のファイルで一元管理されています:  
Startup screen settings are centrally managed in the following file:

```
Firmware/src/startup_config.h
```

このファイルには、表示時間とカスタマイズの手順が記載されています。  
This file contains display duration settings and customization instructions.

## トラブルシューティング / Troubleshooting

### 画像が表示されない / Image Not Displayed

1. **画像形式を確認 / Check Image Format**
   - emWin対応のBMP形式を使用
   - Use emWin-compatible BMP format

2. **画像サイズを確認 / Check Image Size**
   - 765x256ピクセル推奨
   - 765x256 pixels recommended

3. **ビルドエラーを確認 / Check Build Errors**
   - コンソールでエラーメッセージを確認
   - Check error messages in the console

### AppWizardで画像が表示されない / Image Not Shown in AppWizard

1. **画像パスを確認 / Check Image Path**
   - Resource/Image/ に配置されているか確認
   - Confirm it's placed in Resource/Image/

2. **AppWizardを再起動 / Restart AppWizard**
   - ファイルを閉じて再度開く
   - Close and reopen the file

### ビルドエラー: undefined reference to 'acCustomLogo' / Build Error: undefined reference to 'acCustomLogo'

1. **C配列ファイルがビルドに含まれているか確認**
   **Confirm C array file is included in build**
   - custom_logo.c がプロジェクトに追加されているか
   - Check if custom_logo.c is added to the project

2. **外部宣言を確認 / Check External Declaration**
   - Resource.h に正しく宣言されているか
   - Confirm correct declaration in Resource.h

## ベストプラクティス / Best Practices

1. **AppWizardを使用 / Use AppWizard**
   - 可能な限りAppWizardを使用してカスタマイズ
   - Customize using AppWizard whenever possible

2. **バックアップ / Backup**
   - カスタマイズ前に元のファイルをバックアップ
   - Backup original files before customization

3. **バージョン管理 / Version Control**
   - カスタム画像はGitで管理
   - Manage custom images with Git

4. **サイズ最適化 / Size Optimization**
   - 画像サイズを最小限に抑える（フラッシュ容量節約）
   - Minimize image size (to save flash memory)

## 関連ファイル / Related Files

```
Firmware/src/startup_config.h          - 設定ファイル / Configuration file
Firmware/awXXX/aw.AppWizard            - AppWizardプロジェクト / AppWizard project
Firmware/awXXX/Resource/Image/         - 画像リソース / Image resources
Firmware/awXXX/Source/Generated/       - 生成コード / Generated code
```

## 参考資料 / References

- [SEGGER emWin Documentation](https://www.segger.com/products/user-interface/emwin/)
- [Renesas e² studio User Guide](https://www.renesas.com/us/en/document/mat/e-studio-users-manual)
- [FULLMONI-WIDE Firmware README](../README.md)
