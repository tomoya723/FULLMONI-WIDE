/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --bpp 1 --size 12 --no-compress --font x8y12pxDenkiChip.ttf --range 32-127 --format lvgl
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

#ifndef UI_FONT_UI_FONT_FONT_HU_DSMALL
#define UI_FONT_UI_FONT_FONT_HU_DSMALL 1
#endif

#if UI_FONT_UI_FONT_FONT_HU_DSMALL

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xff, 0xfc, 0xf0,

    /* U+0022 "\"" */
    0xb6, 0x80,

    /* U+0023 "#" */
    0x57, 0xfe, 0xa5, 0x7f, 0xea,

    /* U+0024 "$" */
    0x31, 0xff, 0xf8, 0x78, 0x7f, 0xfe, 0x30,

    /* U+0025 "%" */
    0xee, 0xbf, 0x8e, 0x71, 0xfd, 0x77,

    /* U+0026 "&" */
    0x38, 0xf9, 0xb3, 0x6f, 0xfb, 0xf3, 0x7f, 0x76,

    /* U+0027 "'" */
    0xfc,

    /* U+0028 "(" */
    0x36, 0x6c, 0xcc, 0xc6, 0x63,

    /* U+0029 ")" */
    0xc6, 0x63, 0x33, 0x36, 0x6c,

    /* U+002A "*" */
    0x33, 0xff, 0xde, 0xff, 0x30,

    /* U+002B "+" */
    0x30, 0xcf, 0xff, 0x30, 0xc0,

    /* U+002C "," */
    0xf8,

    /* U+002D "-" */
    0xff,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0xc, 0x31, 0x86, 0x30, 0xc6, 0x18, 0xc3, 0x0,

    /* U+0030 "0" */
    0x77, 0xf7, 0xbd, 0xef, 0x7b, 0xfb, 0x80,

    /* U+0031 "1" */
    0x3f, 0xf3, 0x33, 0x33, 0x33,

    /* U+0032 "2" */
    0xf7, 0xc6, 0x33, 0xbb, 0x98, 0xff, 0xc0,

    /* U+0033 "3" */
    0xff, 0xce, 0xe6, 0x38, 0xe3, 0xff, 0x80,

    /* U+0034 "4" */
    0xde, 0xf7, 0xbd, 0xfd, 0xe3, 0x18, 0xc0,

    /* U+0035 "5" */
    0xff, 0xf1, 0xef, 0x8c, 0x63, 0xff, 0x80,

    /* U+0036 "6" */
    0x77, 0xb1, 0xef, 0xef, 0x7b, 0xfb, 0x80,

    /* U+0037 "7" */
    0xff, 0xc6, 0x33, 0xb9, 0x8c, 0x63, 0x0,

    /* U+0038 "8" */
    0x77, 0xf7, 0xb7, 0x7f, 0x7b, 0xfb, 0x80,

    /* U+0039 "9" */
    0x77, 0xf7, 0xbd, 0xfd, 0xe3, 0x7b, 0x80,

    /* U+003A ":" */
    0xf0, 0xf,

    /* U+003B ";" */
    0xf0, 0xf, 0x80,

    /* U+003C "<" */
    0xc, 0xff, 0x30, 0xf0, 0xf0, 0xc0,

    /* U+003D "=" */
    0xff, 0xc1, 0xff, 0x80,

    /* U+003E ">" */
    0xc3, 0xc3, 0xc3, 0x3f, 0xcc, 0x0,

    /* U+003F "?" */
    0xf7, 0xc6, 0x33, 0xb9, 0x80, 0x63, 0x0,

    /* U+0040 "@" */
    0x38, 0x8a, 0x6d, 0x5a, 0xb5, 0x67, 0x20, 0x3c,

    /* U+0041 "A" */
    0x7b, 0xfc, 0xf3, 0xcf, 0xff, 0xf3, 0xcf, 0x30,

    /* U+0042 "B" */
    0xfb, 0xfc, 0xf3, 0xfb, 0xec, 0xf3, 0xff, 0xe0,

    /* U+0043 "C" */
    0x7f, 0xf1, 0x8c, 0x63, 0x18, 0xfb, 0xc0,

    /* U+0044 "D" */
    0xfb, 0xfc, 0xf3, 0xcf, 0x3c, 0xf3, 0xff, 0xe0,

    /* U+0045 "E" */
    0xff, 0xf1, 0x8f, 0xff, 0x18, 0xff, 0xc0,

    /* U+0046 "F" */
    0xff, 0xf1, 0x8f, 0xff, 0x18, 0xc6, 0x0,

    /* U+0047 "G" */
    0x7f, 0xfc, 0x30, 0xcf, 0x3c, 0xf3, 0xfd, 0xf0,

    /* U+0048 "H" */
    0xcf, 0x3c, 0xf3, 0xff, 0xfc, 0xf3, 0xcf, 0x30,

    /* U+0049 "I" */
    0xff, 0xff, 0xf0,

    /* U+004A "J" */
    0x33, 0x33, 0x33, 0x33, 0xfe,

    /* U+004B "K" */
    0xcf, 0x3d, 0xfe, 0xf3, 0xcf, 0xb7, 0xcf, 0x30,

    /* U+004C "L" */
    0xc6, 0x31, 0x8c, 0x63, 0x18, 0xff, 0xc0,

    /* U+004D "M" */
    0xc7, 0xdf, 0xff, 0xfd, 0x78, 0xf1, 0xe3, 0xc7,
    0x8c,

    /* U+004E "N" */
    0xfb, 0xfc, 0xf3, 0xcf, 0x3c, 0xf3, 0xcf, 0x30,

    /* U+004F "O" */
    0x7b, 0xfc, 0xf3, 0xcf, 0x3c, 0xf3, 0xfd, 0xe0,

    /* U+0050 "P" */
    0xfb, 0xfc, 0xf3, 0xcf, 0xff, 0xb0, 0xc3, 0x0,

    /* U+0051 "Q" */
    0x7b, 0xfc, 0xf3, 0xcf, 0x3c, 0xf6, 0xfd, 0xb0,

    /* U+0052 "R" */
    0xfb, 0xfc, 0xf3, 0xcf, 0xef, 0xb3, 0xcf, 0x30,

    /* U+0053 "S" */
    0x7f, 0xf1, 0x8f, 0x3c, 0x63, 0xff, 0x80,

    /* U+0054 "T" */
    0xff, 0xf3, 0xc, 0x30, 0xc3, 0xc, 0x30, 0xc0,

    /* U+0055 "U" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x3c, 0xf3, 0xfd, 0xe0,

    /* U+0056 "V" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x37, 0x9e, 0x30, 0xc0,

    /* U+0057 "W" */
    0xc7, 0x8f, 0x1e, 0x3c, 0x7a, 0xff, 0xff, 0xef,
    0x8c,

    /* U+0058 "X" */
    0xcf, 0x3c, 0xff, 0x79, 0xef, 0xf3, 0xcf, 0x30,

    /* U+0059 "Y" */
    0xcf, 0x3c, 0xf3, 0xfd, 0xe3, 0xc, 0x30, 0xc0,

    /* U+005A "Z" */
    0xff, 0xf0, 0xc7, 0x39, 0xce, 0x30, 0xff, 0xf0,

    /* U+005B "[" */
    0xfb, 0x6d, 0xb6, 0xdc,

    /* U+005C "\\" */
    0xc3, 0x6, 0x18, 0x30, 0xc1, 0x86, 0xc, 0x30,

    /* U+005D "]" */
    0xed, 0xb6, 0xdb, 0x7c,

    /* U+005E "^" */
    0x77, 0xf6,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0xdd, 0x80,

    /* U+0061 "a" */
    0xfb, 0xf0, 0xdf, 0xff, 0x3f, 0xdf,

    /* U+0062 "b" */
    0xc3, 0xf, 0xbf, 0xcf, 0x3c, 0xf3, 0xff, 0xe0,

    /* U+0063 "c" */
    0x7f, 0xf1, 0x8c, 0x63, 0xef,

    /* U+0064 "d" */
    0xc, 0x37, 0xff, 0xcf, 0x3c, 0xf3, 0xfd, 0xf0,

    /* U+0065 "e" */
    0x7b, 0xfc, 0xff, 0xff, 0xf, 0xdf,

    /* U+0066 "f" */
    0x37, 0x6f, 0xf6, 0x66, 0x66,

    /* U+0067 "g" */
    0x7f, 0xfc, 0xf3, 0xfd, 0xf0, 0xdf, 0x78,

    /* U+0068 "h" */
    0xc3, 0xf, 0xbf, 0xcf, 0x3c, 0xf3, 0xcf, 0x30,

    /* U+0069 "i" */
    0xf3, 0xff, 0xf0,

    /* U+006A "j" */
    0x6c, 0x36, 0xdb, 0x7f, 0x0,

    /* U+006B "k" */
    0xc3, 0xc, 0xf7, 0xfb, 0xcf, 0x3e, 0xdf, 0x30,

    /* U+006C "l" */
    0xff, 0xff, 0xf0,

    /* U+006D "m" */
    0xfe, 0xff, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb,

    /* U+006E "n" */
    0xfb, 0xfc, 0xf3, 0xcf, 0x3c, 0xf3,

    /* U+006F "o" */
    0x7b, 0xfc, 0xf3, 0xcf, 0x3f, 0xde,

    /* U+0070 "p" */
    0xfb, 0xfc, 0xf3, 0xcf, 0x7d, 0xb0, 0xc0,

    /* U+0071 "q" */
    0x7f, 0xfc, 0xf3, 0xcf, 0xb6, 0xc3, 0xc,

    /* U+0072 "r" */
    0xdf, 0xf9, 0x8c, 0x63, 0x18,

    /* U+0073 "s" */
    0x7f, 0xf1, 0xc7, 0x1f, 0xfe,

    /* U+0074 "t" */
    0xcc, 0xff, 0xcc, 0xcc, 0xf7,

    /* U+0075 "u" */
    0xcf, 0x3c, 0xf3, 0xcf, 0x3f, 0xdf,

    /* U+0076 "v" */
    0xcf, 0x3c, 0xf3, 0x79, 0xe3, 0xc,

    /* U+0077 "w" */
    0xc3, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xff, 0x66,

    /* U+0078 "x" */
    0xcf, 0x3d, 0xce, 0x73, 0xbc, 0xf3,

    /* U+0079 "y" */
    0xcf, 0x3c, 0xf3, 0xfd, 0xf0, 0xdf, 0x78,

    /* U+007A "z" */
    0xff, 0xce, 0xee, 0x63, 0xff,

    /* U+007B "{" */
    0x36, 0x66, 0xcc, 0x66, 0x63,

    /* U+007C "|" */
    0xff, 0xf0,

    /* U+007D "}" */
    0xc6, 0x66, 0x33, 0x66, 0x6c,

    /* U+007E "~" */
    0x6f, 0xfd, 0x80
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 64, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 48, .box_w = 2, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 6, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 11, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 18, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 24, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 32, .adv_w = 48, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 33, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 38, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 112, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 48, .adv_w = 112, .box_w = 6, .box_h = 6, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 53, .adv_w = 48, .box_w = 2, .box_h = 3, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 54, .adv_w = 80, .box_w = 4, .box_h = 2, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 55, .adv_w = 48, .box_w = 2, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 56, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 64, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 71, .adv_w = 96, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 76, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 83, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 90, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 97, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 104, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 132, .adv_w = 48, .box_w = 2, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 48, .box_w = 2, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 137, .adv_w = 112, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 143, .adv_w = 96, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 2},
    {.bitmap_index = 147, .adv_w = 112, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 153, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 160, .adv_w = 128, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 168, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 176, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 199, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 206, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 221, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 229, .adv_w = 48, .box_w = 2, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 237, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 245, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 252, .adv_w = 128, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 261, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 269, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 277, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 285, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 293, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 96, .box_w = 5, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 308, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 316, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 324, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 332, .adv_w = 128, .box_w = 7, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 341, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 357, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 64, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 369, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 377, .adv_w = 64, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 381, .adv_w = 96, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 7},
    {.bitmap_index = 383, .adv_w = 112, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 384, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 8},
    {.bitmap_index = 386, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 392, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 405, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 413, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 419, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 424, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 431, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 439, .adv_w = 48, .box_w = 2, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 442, .adv_w = 64, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 447, .adv_w = 112, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 455, .adv_w = 48, .box_w = 2, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 458, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 466, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 472, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 485, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 492, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 497, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 502, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 513, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 519, .adv_w = 144, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 527, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 533, .adv_w = 112, .box_w = 6, .box_h = 9, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 540, .adv_w = 96, .box_w = 5, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 545, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 550, .adv_w = 32, .box_w = 1, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 552, .adv_w = 80, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 557, .adv_w = 112, .box_w = 6, .box_h = 3, .ofs_x = 0, .ofs_y = 4}
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
const lv_font_t ui_font_ui_font_font_hu_dsmall = {
#else
lv_font_t ui_font_ui_font_font_hu_dsmall = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 12,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};

#endif /*#if UI_FONT_UI_FONT_FONT_HU_DSMALL*/