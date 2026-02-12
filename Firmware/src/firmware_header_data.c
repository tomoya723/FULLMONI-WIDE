/*
 * Firmware Header Data Definition
 * RX72N/RX72T FULLMONI-WIDE Application
 *
 * このファイルはアプリケーションのファームウェアヘッダ実体を定義します。
 * .firmware_headerセクションに配置され、0xFFC20000（アプリ領域先頭）に配置されます。
 *
 * ビルドフロー:
 * 1. 通常ビルド (CRC32とsizeはプレースホルダー)
 * 2. post-buildスクリプトでアプリケーション領域のCRC32とサイズを計算
 * 3. ELFファイル内のヘッダ領域を実際の値で更新
 * 4. 最終HEX/MOTファイル生成
 */

#include <stdint.h>

/* Firmware Header構造体定義 (bootloader依存を排除) */
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint32_t size;
    uint32_t crc32;
    uint32_t load_address;
    uint32_t entry_point;
    uint32_t reserved[10];
} firmware_header_t;

/* マジックナンバー "RXFW" */
#define FIRMWARE_MAGIC  0x52584657  /* "RXFW" */

/* バージョン定義 (共通ヘッダから取得) */
#include "firmware_version.h"

/* バリアントID (build_variants.ps1が生成、存在しない場合はunknown) */
#if __has_include("variant_id.h")
  #include "variant_id.h"
#else
  #define BUILD_VARIANT_STRING "VARIANT:unknown"
#endif

/* バリアント識別文字列 (.rodataに配置 - ビルド検証用) */
__attribute__((used))
const char g_variant_string[] = BUILD_VARIANT_STRING;

/* エントリポイント (アプリケーションのリセットハンドラ) */
extern void PowerON_Reset(void);

/* ファームウェアヘッダ実体 (.firmware_headerセクションに配置) */
__attribute__((section(".firmware_header")))
const firmware_header_t firmware_header = {
    .magic = FIRMWARE_MAGIC,
    .version = (FW_VERSION_MAJOR << 16) | (FW_VERSION_MINOR << 8) | FW_VERSION_PATCH,
    .size = 0,  /* TODO: post-buildで更新 */
    .crc32 = 0xFFFFFFFF,  /* TODO: post-buildで更新 */
    .load_address = 0xFFC20000,
    .entry_point = (uint32_t)&PowerON_Reset,
    .reserved = {0}
};

/*
 * Post-Buildスクリプト仕様:
 *
 * 1. アプリケーション領域のサイズ計算:
 *    - 開始: 0xFFC20000 (.textセクション開始)
 *    - 終了: 最後のセクション終端
 *    - firmware_sizeフィールドを更新
 *
 * 2. CRC32計算:
 *    - 対象: 0xFFC20000 から firmware_size バイト
 *    - firmware_crc32フィールドを更新
 *
 * 3. タイムスタンプ設定:
 *    - build_timestampフィールドにUnix時間を設定
 *
 * 4. 更新方法:
 *    - ELFファイルの.firmware_headerセクションを直接書き換え
 *    - HEX/MOTファイルを再生成
 */
