/*
 * startup_config.h
 *
 *  Created on: 2026/01/11
 *      Author: FULLMONI-WIDE Project
 *
 *  Purpose: Startup screen configuration
 *           This file provides a centralized configuration for startup screen settings.
 */

#ifndef STARTUP_CONFIG_H_
#define STARTUP_CONFIG_H_

#include <stdint.h>

/* ============================================================
 * スタートアップ画面設定
 * Startup Screen Settings
 * ============================================================ */

/*
 * スタートアップ画面の表示時間 (ミリ秒)
 * Startup screen display duration (milliseconds)
 *
 * この値は ID_SCREEN_00 の ID_TIMER_00 期間として使用されます
 * This value is used as the ID_TIMER_00 period in ID_SCREEN_00
 *
 * デフォルト: 1500ms
 * Default: 1500ms
 *
 * 注意: AppWizard で生成されたコードを変更する場合は、この設定値を
 *      ID_SCREEN_00.c の _aSetup 配列内の APPW_SET_PROP_PERIOD に
 *      手動で適用する必要があります。
 *
 * Note: When modifying AppWizard-generated code, this value must be
 *       manually applied to the APPW_SET_PROP_PERIOD in the _aSetup
 *       array in ID_SCREEN_00.c
 */
#define STARTUP_SCREEN_DURATION_MS (1500)

/* ============================================================
 * カスタムスタートアップ画像の設定方法
 * How to Configure Custom Startup Image
 * ============================================================
 *
 * スタートアップ画像をカスタマイズするには:
 * To customize the startup image:
 *
 * 1. カスタム画像を用意 (765x256 BMP形式推奨)
 *    Prepare a custom image (765x256 BMP format recommended)
 *
 * 2. emWin AppWizard で画像をインポート
 *    Import the image in emWin AppWizard:
 *    - awXXX.AppWizard ファイルを開く / Open awXXX.AppWizard file
 *    - Resource → Images → Add で画像追加 / Add image via Resource → Images → Add
 *    - 画像に名前を付ける (例: custom_logo) / Name the image (e.g., custom_logo)
 *
 * 3. ID_SCREEN_00 で画像を変更
 *    Change the image in ID_SCREEN_00:
 *    - Screens → ID_SCREEN_00 を開く / Open Screens → ID_SCREEN_00
 *    - ID_IMAGE_01 を選択 / Select ID_IMAGE_01
 *    - Properties で Bitmap を新しい画像に変更 / Change Bitmap to new image in Properties
 *
 * 4. コード生成して再ビルド
 *    Generate code and rebuild:
 *    - Code Generation 実行 / Execute Code Generation
 *    - プロジェクトをビルド / Build the project
 *
 * または、生成済みファイルを直接編集する場合:
 * Or, to edit generated files directly:
 *
 * 1. Firmware/awXXX/Resource/Image/ にカスタム画像ファイルを配置
 *    Place custom image files in Firmware/awXXX/Resource/Image/
 *    - custom_logo.bmp (ソース画像 / source image)
 *    - custom_logo.c (生成されたC配列 / generated C array)
 *
 * 2. Firmware/awXXX/Source/Generated/Resource.h に画像を宣言
 *    Declare the image in Firmware/awXXX/Source/Generated/Resource.h:
 *    extern GUI_CONST_STORAGE unsigned char acCustomLogo[];
 *
 * 3. Firmware/awXXX/Source/Generated/ID_SCREEN_00.c の _aSetup 配列を編集
 *    Edit the _aSetup array in Firmware/awXXX/Source/Generated/ID_SCREEN_00.c:
 *    { ID_IMAGE_01, APPW_SET_PROP_SBITMAP, { ARG_VP(0, acCustomLogo), ARG_V(image_size), } },
 *
 * 注意: 生成ファイルの直接編集は推奨されません。AppWizard での再生成時に上書きされます。
 * Note: Direct editing of generated files is not recommended as they will be overwritten on regeneration.
 */

/* ============================================================
 * スタートアップ画像リソース定義
 * Startup Image Resource Definitions
 * ============================================================ */

/*
 * デフォルトスタートアップ画像
 * Default startup images
 *
 * aw001: acmtc (MTC logo)
 * aw002: acmtc (MTC logo)
 *
 * カスタム画像を追加する場合は、上記の手順に従ってください
 * To add custom images, follow the instructions above
 */

#endif /* STARTUP_CONFIG_H_ */
