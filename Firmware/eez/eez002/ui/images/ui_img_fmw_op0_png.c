/*
 * ui_img_fmw_op0_png.c  - HAND MAINTAINED (not regenerated from SLS)
 *
 * 起動画像ディスクリプタ
 *
 * ピクセルデータは Flash 固定アドレス 0xFFE00000 に配置される。
 * データ内容は imgwrite コマンド (tools/imgwrite.py) で書き込まれる。
 * ビルドにピクセル配列を含めないことで ROM 肥大化・アドレス衝突を防ぐ。
 *
 * 書き込み前のフラッシュ内容 (消去状態 0xFF または前回書き込み) を表示するため、
 * 初回起動時は画面が白またはノイズになる場合がある。
 *
 * LVGL image format: LV_IMG_CF_TRUE_COLOR (RGB565, little-endian)
 *   Width : 765 pixels
 *   Height: 256 pixels
 *   Size  : 765 * 256 * 2 = 391,680 bytes
 */

#include "../ui.h"

/* ピクセルデータは imgwrite が書き込む固定アドレスを直接参照。
 * 画像データは 12バイトヘッダ (BM magic + image info) の後にRGB565ピクセルが続く。
 * Terminal/Androidアプリが書き込むデータはこの形式のため、ヘッダをスキップする。 */
#define IMAGE_HEADER_SIZE     16
#define STARTUP_IMAGE_PIXEL_ADDR  ((const uint8_t *)(0xFFE20000UL + IMAGE_HEADER_SIZE))

const lv_img_dsc_t ui_img_fmw_op0_png = {
    .header.always_zero = 0,
    .header.w           = 765,
    .header.h           = 256,
    .data_size          = 765u * 256u * 2u,
    .header.cf          = LV_IMG_CF_TRUE_COLOR,
    .data               = STARTUP_IMAGE_PIXEL_ADDR
};
