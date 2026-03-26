/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --no-compress --font x8y12pxDenkiChip.ttf --range 32-127 --format lvgl
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
    0xff, 0xff, 0xff, 0xe0, 0x7f, 0xc0,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x25, 0xff, 0xff, 0xf2, 0x44, 0x89, 0x7f, 0xff,
    0xfc, 0x90,

    /* U+0024 "$" */
    0x38, 0x38, 0x3f, 0xff, 0xff, 0xf0, 0x30, 0xf,
    0xf, 0xff, 0xfe, 0xfe, 0x38,

    /* U+0025 "%" */
    0xf7, 0xf7, 0xff, 0xfe, 0x1e, 0x7e, 0x78, 0x7f,
    0xe9, 0xef, 0xef,

    /* U+0026 "&" */
    0x1e, 0x3f, 0x9f, 0xce, 0xe7, 0x77, 0xff, 0xff,
    0xdf, 0xe7, 0x7f, 0xde, 0x6f, 0x30,

    /* U+0027 "'" */
    0xff, 0xf0,

    /* U+0028 "(" */
    0x18, 0xdc, 0xe7, 0x73, 0x9c, 0xe7, 0x1c, 0xe1,
    0x8c,

    /* U+0029 ")" */
    0xe7, 0x1c, 0xe7, 0xc, 0x63, 0x18, 0xdc, 0xee,
    0x70,

    /* U+002A "*" */
    0x18, 0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xe7,

    /* U+002B "+" */
    0x38, 0x38, 0xff, 0xff, 0xff, 0x38, 0x38, 0x38,

    /* U+002C "," */
    0xff, 0xc0,

    /* U+002D "-" */
    0xff, 0xfe,

    /* U+002E "." */
    0xff, 0x80,

    /* U+002F "/" */
    0x7, 0x7, 0x7, 0xe, 0xe, 0xe, 0x18, 0x18,
    0x70, 0x70, 0x70, 0xe0, 0xe0, 0xe0,

    /* U+0030 "0" */
    0x3c, 0x7b, 0xff, 0x7e, 0xfd, 0xfb, 0xf7, 0xef,
    0xdf, 0xbf, 0xff, 0xe7, 0x80,

    /* U+0031 "1" */
    0x1c, 0x7f, 0xff, 0xfc, 0x71, 0xc7, 0x1c, 0x71,
    0xc7, 0x1c, 0x70,

    /* U+0032 "2" */
    0xfd, 0xfb, 0xf8, 0x70, 0xe1, 0xc7, 0x9e, 0xfd,
    0xe3, 0x87, 0xff, 0xff, 0xc0,

    /* U+0033 "3" */
    0xff, 0xff, 0xf8, 0xf7, 0x8e, 0x1c, 0x3c, 0x18,
    0x3c, 0x3f, 0xff, 0xff, 0x0,

    /* U+0034 "4" */
    0xef, 0xdf, 0xbf, 0x7e, 0xfd, 0xfb, 0xff, 0x7e,
    0xfc, 0x38, 0x70, 0xe1, 0xc0,

    /* U+0035 "5" */
    0xff, 0xff, 0xff, 0xf, 0x9f, 0x3f, 0x87, 0xe,
    0x1c, 0x3f, 0xff, 0xff, 0x0,

    /* U+0036 "6" */
    0x3c, 0x7b, 0xf7, 0xf, 0xdf, 0xbf, 0xf7, 0xef,
    0xdf, 0xbf, 0xff, 0xe7, 0x80,

    /* U+0037 "7" */
    0xff, 0xff, 0xf8, 0x30, 0x60, 0xc7, 0x9e, 0x38,
    0x70, 0xe1, 0xc3, 0x87, 0x0,

    /* U+0038 "8" */
    0x3c, 0x7b, 0xff, 0x7e, 0xfd, 0xcf, 0x7f, 0xff,
    0xdf, 0xbf, 0xf3, 0xc7, 0x80,

    /* U+0039 "9" */
    0x3c, 0x7b, 0xff, 0x7e, 0xfd, 0xfb, 0xff, 0xfe,
    0x7c, 0x39, 0xf3, 0xe7, 0x80,

    /* U+003A ":" */
    0xff, 0x80, 0x0, 0xff, 0x80,

    /* U+003B ";" */
    0xff, 0x80, 0x0, 0xff, 0xc0,

    /* U+003C "<" */
    0x7, 0x7, 0x1f, 0xf8, 0xf8, 0xe0, 0xf8, 0x1f,
    0x1f, 0x7,

    /* U+003D "=" */
    0xff, 0xff, 0xf8, 0xf, 0xff, 0xff, 0x80,

    /* U+003E ">" */
    0xe0, 0xe0, 0xf8, 0x1f, 0x1f, 0x7, 0x1f, 0xf8,
    0xf8, 0xe0,

    /* U+003F "?" */
    0xf9, 0xf3, 0xf8, 0x70, 0xe1, 0xcf, 0xbc, 0x78,
    0xe0, 0x3, 0x87, 0xe, 0x0,

    /* U+0040 "@" */
    0x1e, 0xf, 0x8, 0x55, 0xf8, 0xec, 0x96, 0x4b,
    0x25, 0x92, 0xc7, 0x88, 0x3, 0xe1, 0xf0,

    /* U+0041 "A" */
    0x3e, 0x3e, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xff,
    0xff, 0xff, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+0042 "B" */
    0xfe, 0xfe, 0xff, 0xe7, 0xe7, 0xe7, 0xfe, 0xfe,
    0xfe, 0xe7, 0xe7, 0xff, 0xff, 0xfe,

    /* U+0043 "C" */
    0x3e, 0x7f, 0xff, 0xe, 0x1c, 0x38, 0x70, 0xe1,
    0xc3, 0x87, 0xff, 0xe7, 0xc0,

    /* U+0044 "D" */
    0xfe, 0xfe, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xff, 0xff, 0xfe,

    /* U+0045 "E" */
    0xff, 0xff, 0xff, 0xe, 0x1c, 0x3f, 0xff, 0xff,
    0xc3, 0x87, 0xff, 0xff, 0xc0,

    /* U+0046 "F" */
    0xff, 0xff, 0xff, 0xe, 0x1c, 0x3f, 0xff, 0xff,
    0xc3, 0x87, 0xe, 0x1c, 0x0,

    /* U+0047 "G" */
    0x7f, 0x7f, 0xff, 0xe0, 0xe0, 0xe0, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xff, 0xff, 0x7f,

    /* U+0048 "H" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0xff,
    0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+0049 "I" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xc0,

    /* U+004A "J" */
    0x39, 0xce, 0x73, 0x9c, 0xe7, 0x39, 0xcf, 0xff,
    0x78,

    /* U+004B "K" */
    0xe7, 0xe7, 0xe7, 0xef, 0xee, 0xfe, 0xf8, 0xf8,
    0xf8, 0xfe, 0xef, 0xe7, 0xe7, 0xe7,

    /* U+004C "L" */
    0xe1, 0xc3, 0x87, 0xe, 0x1c, 0x38, 0x70, 0xe1,
    0xc3, 0x87, 0xff, 0xff, 0xc0,

    /* U+004D "M" */
    0xe3, 0xf1, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xaf,
    0xc7, 0xe3, 0xf1, 0xf8, 0xfc, 0x7e, 0x3f, 0x1c,

    /* U+004E "N" */
    0xfe, 0xfe, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+004F "O" */
    0x3e, 0x3e, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xff, 0xff, 0x3e,

    /* U+0050 "P" */
    0xfe, 0xfe, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xff,
    0xff, 0xfe, 0xe0, 0xe0, 0xe0, 0xe0,

    /* U+0051 "Q" */
    0x7e, 0x7e, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xee, 0xff, 0xff, 0x77,

    /* U+0052 "R" */
    0xfe, 0xfe, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xfe,
    0xfe, 0xfe, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+0053 "S" */
    0x7d, 0xff, 0xf0, 0xc3, 0xf, 0x9f, 0x7c, 0x71,
    0xff, 0xfb, 0xe0,

    /* U+0054 "T" */
    0xff, 0xff, 0xff, 0x38, 0x38, 0x38, 0x38, 0x38,
    0x38, 0x38, 0x38, 0x38, 0x38, 0x38,

    /* U+0055 "U" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xff, 0x3e, 0x3e,

    /* U+0056 "V" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0x26, 0x3e, 0x3e, 0x3e, 0x18, 0x18,

    /* U+0057 "W" */
    0xe3, 0xf1, 0xf8, 0xfc, 0x7e, 0x3f, 0x1f, 0x8f,
    0xd7, 0xeb, 0xff, 0xff, 0xfe, 0xff, 0x7f, 0x1c,

    /* U+0058 "X" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0x3e, 0x3e, 0x3e,
    0x3e, 0xff, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+0059 "Y" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0x3e,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18,

    /* U+005A "Z" */
    0xff, 0xff, 0xff, 0x7, 0xf, 0xf, 0x1e, 0x78,
    0xf0, 0xf0, 0xe0, 0xff, 0xff, 0xff,

    /* U+005B "[" */
    0xff, 0xee, 0xee, 0xee, 0xee, 0xee, 0xff,

    /* U+005C "\\" */
    0xe0, 0xe0, 0xe0, 0x70, 0x70, 0x70, 0x18, 0x18,
    0xe, 0xe, 0xe, 0x7, 0x7, 0x7,

    /* U+005D "]" */
    0xff, 0x77, 0x77, 0x77, 0x77, 0x77, 0xff,

    /* U+005E "^" */
    0x3d, 0xff, 0xff, 0x70,

    /* U+005F "_" */
    0xff, 0xff,

    /* U+0060 "`" */
    0xef, 0xf7,

    /* U+0061 "a" */
    0xfe, 0xff, 0xff, 0x7, 0x7f, 0x7f, 0xff, 0xe7,
    0xff, 0xff, 0x7f,

    /* U+0062 "b" */
    0xe0, 0xe0, 0xe0, 0xfe, 0xff, 0xff, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xff, 0xfe, 0xfe,

    /* U+0063 "c" */
    0x3f, 0xff, 0xff, 0xe, 0x1c, 0x38, 0x70, 0xfe,
    0x7c, 0xf8,

    /* U+0064 "d" */
    0x7, 0x7, 0x7, 0x7f, 0xff, 0xff, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xff, 0x7f, 0x7f,

    /* U+0065 "e" */
    0x3e, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xff, 0xe0,
    0xff, 0x3f, 0x3f,

    /* U+0066 "f" */
    0x39, 0xde, 0xef, 0xff, 0xee, 0x73, 0x9c, 0xe7,
    0x38,

    /* U+0067 "g" */
    0x3f, 0xff, 0xff, 0xe7, 0xe7, 0xff, 0x3f, 0x3f,
    0x7, 0x3f, 0x3f, 0x3e,

    /* U+0068 "h" */
    0xe0, 0xe0, 0xe0, 0xfe, 0xfe, 0xff, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,

    /* U+0069 "i" */
    0xff, 0x81, 0xff, 0xff, 0xff, 0xc0,

    /* U+006A "j" */
    0x77, 0x70, 0x7, 0x77, 0x77, 0x77, 0xff, 0xe0,

    /* U+006B "k" */
    0xe0, 0xe0, 0xe0, 0xe7, 0xef, 0xef, 0xfe, 0xf8,
    0xf8, 0xf8, 0xfe, 0xef, 0xe7, 0xe7,

    /* U+006C "l" */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xc0,

    /* U+006D "m" */
    0xff, 0xdf, 0xfb, 0xff, 0xf7, 0x7e, 0xef, 0xdd,
    0xfb, 0xbf, 0x77, 0xee, 0xfd, 0xdf, 0xbb, 0x80,

    /* U+006E "n" */
    0xfe, 0xfe, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xe7, 0xe7, 0xe7,

    /* U+006F "o" */
    0x3e, 0xff, 0xff, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xff, 0x3e, 0x3e,

    /* U+0070 "p" */
    0xfe, 0xff, 0xff, 0xe7, 0xe7, 0xe7, 0xef, 0xef,
    0xee, 0xe0, 0xe0, 0xe0,

    /* U+0071 "q" */
    0x7f, 0xff, 0xff, 0xfc, 0x7e, 0x3f, 0x1f, 0xcf,
    0xe7, 0x73, 0x81, 0xc0, 0xe0, 0x70,

    /* U+0072 "r" */
    0xef, 0xff, 0xff, 0x8e, 0x1c, 0x38, 0x70, 0xe1,
    0xc3, 0x80,

    /* U+0073 "s" */
    0x3f, 0xff, 0xff, 0xf, 0x87, 0x8f, 0xf, 0xff,
    0xff, 0xf0,

    /* U+0074 "t" */
    0xe7, 0x39, 0xff, 0xff, 0x9c, 0xe7, 0x39, 0xf7,
    0xbc,

    /* U+0075 "u" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xe7,
    0xff, 0x7f, 0x7f,

    /* U+0076 "v" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0x26, 0x3e, 0x3e,
    0x3e, 0x18, 0x18,

    /* U+0077 "w" */
    0xe0, 0xfd, 0xdf, 0xbb, 0xf7, 0x7e, 0xef, 0xdd,
    0xfb, 0xbf, 0x77, 0xff, 0xee, 0x31, 0xc6, 0x0,

    /* U+0078 "x" */
    0xe7, 0xe7, 0xe7, 0xef, 0x1e, 0x7e, 0x78, 0xf7,
    0xe7, 0xe7, 0xe7,

    /* U+0079 "y" */
    0xe7, 0xe7, 0xe7, 0xe7, 0xe7, 0xff, 0xff, 0x3f,
    0x7, 0x3f, 0x3f, 0x3e,

    /* U+007A "z" */
    0xff, 0xff, 0xf8, 0xf3, 0xdf, 0xbe, 0x70, 0xff,
    0xff, 0xf8,

    /* U+007B "{" */
    0x18, 0xdc, 0xe7, 0x3b, 0x9c, 0xf3, 0x9c, 0xe1,
    0x8c,

    /* U+007C "|" */
    0xff, 0xff,

    /* U+007D "}" */
    0xe7, 0x1c, 0xe7, 0x38, 0x63, 0x7b, 0x9c, 0xee,
    0x70,

    /* U+007E "~" */
    0x77, 0xff, 0xff, 0xee
};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 85, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 64, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 7, .adv_w = 85, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 9, .adv_w = 128, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 19, .adv_w = 149, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 32, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 43, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 57, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 59, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 68, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 77, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 85, .adv_w = 149, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 93, .adv_w = 64, .box_w = 3, .box_h = 4, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 95, .adv_w = 107, .box_w = 5, .box_h = 3, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 97, .adv_w = 64, .box_w = 3, .box_h = 3, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 99, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 128, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 137, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 150, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 176, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 189, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 215, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 228, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 241, .adv_w = 64, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 246, .adv_w = 64, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 251, .adv_w = 149, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 261, .adv_w = 128, .box_w = 7, .box_h = 7, .ofs_x = 0, .ofs_y = 3},
    {.bitmap_index = 268, .adv_w = 149, .box_w = 8, .box_h = 10, .ofs_x = 0, .ofs_y = 1},
    {.bitmap_index = 278, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 291, .adv_w = 171, .box_w = 9, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 306, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 320, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 334, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 347, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 361, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 374, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 387, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 401, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 415, .adv_w = 64, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 421, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 444, .adv_w = 128, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 457, .adv_w = 171, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 473, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 487, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 501, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 515, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 529, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 543, .adv_w = 128, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 554, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 568, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 582, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 596, .adv_w = 171, .box_w = 9, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 612, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 640, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 654, .adv_w = 85, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 661, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 675, .adv_w = 85, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 682, .adv_w = 128, .box_w = 7, .box_h = 4, .ofs_x = 0, .ofs_y = 10},
    {.bitmap_index = 686, .adv_w = 149, .box_w = 8, .box_h = 2, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 688, .adv_w = 85, .box_w = 4, .box_h = 4, .ofs_x = 0, .ofs_y = 11},
    {.bitmap_index = 690, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 701, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 715, .adv_w = 128, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 725, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 750, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 759, .adv_w = 149, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 771, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 785, .adv_w = 64, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 791, .adv_w = 85, .box_w = 4, .box_h = 15, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 799, .adv_w = 149, .box_w = 8, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 64, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 819, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 835, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 846, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 857, .adv_w = 149, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 869, .adv_w = 149, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 883, .adv_w = 128, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 893, .adv_w = 128, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 903, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 912, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 923, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 934, .adv_w = 192, .box_w = 11, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 950, .adv_w = 149, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 961, .adv_w = 149, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 973, .adv_w = 128, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 983, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 992, .adv_w = 43, .box_w = 1, .box_h = 16, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 994, .adv_w = 107, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1003, .adv_w = 149, .box_w = 8, .box_h = 4, .ofs_x = 0, .ofs_y = 6}
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
    .line_height = 16,          /*The maximum line height required by the font*/
    .base_line = 1,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};

#endif /*#if UI_FONT_UI_FONT_FONT_HU_DSMALL*/