/*******************************************************************************
 * Size: 18 px
 * Bpp: 1
 * Opts: --bpp 1 --size 18 --no-compress --font x14y24pxHeadUpDaisy.ttf --range 32-127 --format lvgl
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif

#ifndef UI_FONT_UI_FONT_FONT_HU_DMID
#define UI_FONT_UI_FONT_FONT_HU_DMID 1
#endif

#if UI_FONT_UI_FONT_FONT_HU_DMID

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xff, 0xc3,

    /* U+0022 "\"" */
    0xde, 0xf7, 0xb0,

    /* U+0023 "#" */
    0x31, 0x31, 0x31, 0xff, 0x63, 0x63, 0x63, 0x63,
    0xff, 0xc6, 0xc6, 0xc6,

    /* U+0024 "$" */
    0x18, 0x18, 0xff, 0xd8, 0xd8, 0x7f, 0x1b, 0x1b,
    0xdb, 0x7e, 0x18, 0x18,

    /* U+0025 "%" */
    0xf1, 0x79, 0x3c, 0x9e, 0x8e, 0xc1, 0xfd, 0x1e,
    0x8f, 0xc3, 0x0,

    /* U+0026 "&" */
    0x3c, 0x16, 0xb, 0x7, 0x3, 0x87, 0xff, 0x31,
    0x98, 0x7a, 0x19, 0xc0,

    /* U+0027 "'" */
    0xff,

    /* U+0028 "(" */
    0x1b, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x31, 0x86,
    0xc,

    /* U+0029 ")" */
    0xc3, 0x86, 0x31, 0x8c, 0x63, 0x18, 0xc6, 0x37,
    0x60,

    /* U+002A "*" */
    0x30, 0xb3, 0xb2, 0x7c, 0x38, 0x38, 0x7e, 0xb3,
    0x30, 0x30,

    /* U+002B "+" */
    0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18,

    /* U+002C "," */
    0xef, 0x80,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xc0,

    /* U+002F "/" */
    0x3, 0x6, 0x6, 0x6, 0xc, 0x8, 0x18, 0x38,
    0x30, 0x30, 0xe0, 0xc0, 0xc0,

    /* U+0030 "0" */
    0x3c, 0xe7, 0xc3, 0xc7, 0xcb, 0xcb, 0xd3, 0xe3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+0031 "1" */
    0x67, 0x38, 0xc6, 0x31, 0x8c, 0x63, 0x19, 0xf0,

    /* U+0032 "2" */
    0x3c, 0xe7, 0x3, 0x3, 0x3, 0x6, 0x8, 0x18,
    0x60, 0x60, 0xe0, 0xff,

    /* U+0033 "3" */
    0xff, 0x7, 0xe, 0xc, 0x18, 0x8, 0x7, 0x3,
    0x3, 0xc3, 0x66, 0x3c,

    /* U+0034 "4" */
    0x6, 0xe, 0x16, 0x16, 0xe6, 0xc6, 0xc6, 0xc6,
    0xff, 0x6, 0x6, 0x6,

    /* U+0035 "5" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0x7, 0x3,
    0x3, 0xc3, 0x66, 0x3c,

    /* U+0036 "6" */
    0x3c, 0x66, 0xc3, 0xc0, 0xc0, 0xfe, 0xe7, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+0037 "7" */
    0xff, 0x3, 0x3, 0x3, 0x7, 0xc, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18,

    /* U+0038 "8" */
    0x3c, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0x66, 0xc3,
    0xc3, 0xc3, 0xe7, 0x3c,

    /* U+0039 "9" */
    0x3c, 0x66, 0xc3, 0xc3, 0xc3, 0xe7, 0x7f, 0x3,
    0x3, 0xc3, 0x66, 0x3c,

    /* U+003A ":" */
    0xc0, 0xc,

    /* U+003B ";" */
    0xc0, 0x35, 0xc0,

    /* U+003C "<" */
    0x1, 0x83, 0x82, 0x2, 0x3, 0x2, 0x3, 0x0,
    0x80, 0x60, 0x8, 0x4, 0x1, 0x0, 0x70, 0xc,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0xc0, 0x60, 0x10, 0x8, 0xc, 0x2, 0x3, 0x2,
    0x6, 0x8, 0x8, 0x10, 0x60, 0xc0,

    /* U+003F "?" */
    0x7c, 0xc7, 0x3, 0x3, 0x3, 0x6, 0xc, 0x18,
    0x0, 0x0, 0x0, 0x18,

    /* U+0040 "@" */
    0x3e, 0x71, 0xf0, 0x7b, 0xbc, 0x5e, 0xef, 0x57,
    0xae, 0xde, 0x60, 0xd8, 0xc7, 0xc0,

    /* U+0041 "A" */
    0x18, 0x28, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xff,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0042 "B" */
    0xfc, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc6, 0xc3,
    0xc3, 0xc3, 0xc7, 0xfc,

    /* U+0043 "C" */
    0x3c, 0x66, 0xc3, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc3, 0x66, 0x3c,

    /* U+0044 "D" */
    0xf8, 0xc6, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc2, 0xc4, 0xf8,

    /* U+0045 "E" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xff,

    /* U+0046 "F" */
    0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0047 "G" */
    0x3f, 0xe0, 0xc0, 0xc0, 0xc0, 0xcf, 0xc3, 0xc3,
    0xc3, 0xc3, 0x63, 0x3f,

    /* U+0048 "H" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0049 "I" */
    0xfc, 0xc3, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0x3f,

    /* U+004A "J" */
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+004B "K" */
    0xc3, 0xc6, 0xc8, 0xd8, 0xe0, 0xe0, 0xe0, 0xd0,
    0xc8, 0xc8, 0xc6, 0xc3,

    /* U+004C "L" */
    0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xff,

    /* U+004D "M" */
    0xc3, 0xe7, 0xeb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+004E "N" */
    0xc3, 0xe3, 0xe3, 0xdb, 0xcb, 0xc7, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+004F "O" */
    0x3c, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+0050 "P" */
    0xfc, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0,
    0xc0, 0xc0, 0xc0, 0xc0,

    /* U+0051 "Q" */
    0x3c, 0x73, 0xb0, 0xd8, 0x6c, 0x36, 0x1b, 0xd,
    0xe6, 0xdb, 0x65, 0x19, 0x87, 0x70,

    /* U+0052 "R" */
    0xfc, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xf0, 0xd0,
    0xd8, 0xd8, 0xc6, 0xc3,

    /* U+0053 "S" */
    0x3c, 0xe7, 0xc0, 0xc0, 0xe0, 0x7c, 0x7, 0x3,
    0x3, 0xc3, 0x66, 0x3c,

    /* U+0054 "T" */
    0xff, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18,

    /* U+0055 "U" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x66, 0x3c,

    /* U+0056 "V" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3, 0x66, 0x28, 0x18,

    /* U+0057 "W" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xdb, 0xeb, 0xe7, 0xc3,

    /* U+0058 "X" */
    0xc3, 0xc3, 0xc3, 0xc7, 0x7a, 0x38, 0x38, 0x6e,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0059 "Y" */
    0xc3, 0xc3, 0xc3, 0xe7, 0x66, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18,

    /* U+005A "Z" */
    0xff, 0x3, 0x3, 0x7, 0xa, 0x8, 0x10, 0x60,
    0xc0, 0xc0, 0xc0, 0xff,

    /* U+005B "[" */
    0xfc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xf0,

    /* U+005C "\\" */
    0xc0, 0xc0, 0xc0, 0xe0, 0x30, 0x30, 0x38, 0x18,
    0x8, 0xc, 0x6, 0x6, 0x7,

    /* U+005D "]" */
    0xf3, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xf0,

    /* U+005E "^" */
    0x18, 0x28, 0x46, 0xc3,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xc9, 0x80,

    /* U+0061 "a" */
    0xfe, 0x7, 0x3, 0x3, 0xff, 0xc3, 0xc3, 0xc3,
    0x7f,

    /* U+0062 "b" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xc7, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc6, 0xfc,

    /* U+0063 "c" */
    0x3c, 0x66, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x66,
    0x3c,

    /* U+0064 "d" */
    0x3, 0x3, 0x3, 0x3f, 0xe3, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0x63, 0x3f,

    /* U+0065 "e" */
    0x3c, 0xe7, 0xc3, 0xc3, 0xff, 0xc0, 0xc0, 0x60,
    0x3f,

    /* U+0066 "f" */
    0x7, 0xc, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x18,

    /* U+0067 "g" */
    0x3f, 0x63, 0xc3, 0xc3, 0xc3, 0xc3, 0xe3, 0x7f,
    0x3, 0x3, 0x7, 0xfe,

    /* U+0068 "h" */
    0xc0, 0xc0, 0xc0, 0xfc, 0xc6, 0xc3, 0xc3, 0xc3,
    0xc3, 0xc3, 0xc3, 0xc3,

    /* U+0069 "i" */
    0x30, 0x0, 0x3c, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0x3f,

    /* U+006A "j" */
    0x6, 0x0, 0x0, 0xf0, 0x60, 0xc1, 0x83, 0x6,
    0xc, 0x18, 0x30, 0x61, 0xff, 0x0,

    /* U+006B "k" */
    0xc0, 0xc0, 0xc0, 0xc3, 0xce, 0xc8, 0xc8, 0xd8,
    0xe8, 0xc4, 0xc2, 0xc3,

    /* U+006C "l" */
    0xf0, 0xc3, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xc3,
    0xf,

    /* U+006D "m" */
    0xfe, 0xda, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb,
    0xdb,

    /* U+006E "n" */
    0xfc, 0xc6, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3,
    0xc3,

    /* U+006F "o" */
    0x3c, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66,
    0x3c,

    /* U+0070 "p" */
    0xfc, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc6,
    0xfc, 0xc0, 0xc0, 0xc0,

    /* U+0071 "q" */
    0x3f, 0xe3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x63,
    0x3f, 0x3, 0x3, 0x3,

    /* U+0072 "r" */
    0xfc, 0xc6, 0xc3, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
    0xc0,

    /* U+0073 "s" */
    0x7e, 0xc3, 0xc0, 0xc0, 0x7f, 0x3, 0x3, 0xc3,
    0x7e,

    /* U+0074 "t" */
    0x60, 0x60, 0x60, 0xff, 0x60, 0x60, 0x60, 0x60,
    0x60, 0x60, 0x30, 0x1f,

    /* U+0075 "u" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x63,
    0x3f,

    /* U+0076 "v" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x28,
    0x18,

    /* U+0077 "w" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xeb, 0xe7,
    0xc3,

    /* U+0078 "x" */
    0xc3, 0x42, 0x2c, 0x38, 0x38, 0x38, 0x44, 0x42,
    0xc3,

    /* U+0079 "y" */
    0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x63, 0x1f,
    0x7, 0x6, 0xc, 0xf8,

    /* U+007A "z" */
    0xff, 0x7, 0x6, 0xe, 0x18, 0x60, 0x40, 0xc0,
    0xff,

    /* U+007B "{" */
    0x3c, 0xc3, 0xc, 0x30, 0xc7, 0x38, 0x70, 0xc3,
    0xc, 0x30, 0xc3, 0xc0,

    /* U+007C "|" */
    0xff, 0xff, 0xff, 0xff, 0xf0,

    /* U+007D "}" */
    0xf0, 0xc3, 0xc, 0x30, 0xc3, 0x87, 0x38, 0xc3,
    0xc, 0x30, 0xcf, 0x0,

    /* U+007E "~" */
    0x79, 0xa4, 0xb3, 0xc0
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 168, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 168, .box_w = 2, .box_h = 12, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 168, .box_w = 5, .box_h = 4, .ofs_x = 3, .ofs_y = 8},
    {.bitmap_index = 7, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 19, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 31, .adv_w = 168, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 42, .adv_w = 168, .box_w = 9, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 54, .adv_w = 168, .box_w = 2, .box_h = 4, .ofs_x = 4, .ofs_y = 8},
    {.bitmap_index = 55, .adv_w = 168, .box_w = 5, .box_h = 14, .ofs_x = 4, .ofs_y = -1},
    {.bitmap_index = 64, .adv_w = 168, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 73, .adv_w = 168, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 83, .adv_w = 168, .box_w = 8, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 90, .adv_w = 168, .box_w = 3, .box_h = 3, .ofs_x = 3, .ofs_y = -2},
    {.bitmap_index = 92, .adv_w = 168, .box_w = 8, .box_h = 1, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 93, .adv_w = 168, .box_w = 2, .box_h = 1, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 94, .adv_w = 168, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 119, .adv_w = 168, .box_w = 5, .box_h = 12, .ofs_x = 3, .ofs_y = 0},
    {.bitmap_index = 127, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 139, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 151, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 175, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 187, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 211, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 223, .adv_w = 168, .box_w = 2, .box_h = 7, .ofs_x = 4, .ofs_y = 3},
    {.bitmap_index = 225, .adv_w = 168, .box_w = 2, .box_h = 9, .ofs_x = 4, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 168, .box_w = 9, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 244, .adv_w = 168, .box_w = 8, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 249, .adv_w = 168, .box_w = 8, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 263, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 168, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 337, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 373, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 385, .adv_w = 168, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 394, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 406, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 418, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 454, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 168, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 492, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 504, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 516, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 540, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 552, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 564, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 576, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 588, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 600, .adv_w = 168, .box_w = 4, .box_h = 15, .ofs_x = 5, .ofs_y = -1},
    {.bitmap_index = 608, .adv_w = 168, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 621, .adv_w = 168, .box_w = 4, .box_h = 15, .ofs_x = 2, .ofs_y = -1},
    {.bitmap_index = 629, .adv_w = 168, .box_w = 8, .box_h = 4, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 633, .adv_w = 168, .box_w = 8, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 168, .box_w = 3, .box_h = 3, .ofs_x = 4, .ofs_y = 11},
    {.bitmap_index = 636, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 645, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 657, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 666, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 678, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 687, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 699, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 711, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 723, .adv_w = 168, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 732, .adv_w = 168, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 746, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 758, .adv_w = 168, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 767, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 776, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 785, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 794, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 806, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 818, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 827, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 836, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 848, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 857, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 866, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 875, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 884, .adv_w = 168, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 896, .adv_w = 168, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 905, .adv_w = 168, .box_w = 6, .box_h = 15, .ofs_x = 3, .ofs_y = -1},
    {.bitmap_index = 917, .adv_w = 168, .box_w = 2, .box_h = 18, .ofs_x = 4, .ofs_y = -3},
    {.bitmap_index = 922, .adv_w = 168, .box_w = 6, .box_h = 15, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 934, .adv_w = 168, .box_w = 9, .box_h = 3, .ofs_x = 1, .ofs_y = 5}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
};

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t ui_font_ui_font_font_hu_dmid = {
#else
lv_font_t ui_font_ui_font_font_hu_dmid = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 18,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -4,
    .underline_thickness = 2,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};

#endif /*#if UI_FONT_UI_FONT_FONT_HU_DMID*/