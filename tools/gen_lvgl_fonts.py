#!/usr/bin/env python3
"""Generate LVGL C font files from TTF using FreeType.

Replaces EEZ Studio's internal lv_font_conv flow with direct FreeType rendering.
Outputs LVGL-compatible C source files for each font definition.
"""

import struct
import sys
import os
from pathlib import Path

import freetype

# ─── Font definitions matching aw002 ────────────────────────────────────
# (eez_name, c_symbol, ttf_file, size_px, bpp, range_start, range_end)
FONT_DEFS = [
    ("Font1",       "ui_font_ui_font_font1",          "x14y24pxHeadUpDaisy.ttf", 27, 1, 32, 127),
    ("FontHUDmid",  "ui_font_ui_font_font_hu_dmid",   "x14y24pxHeadUpDaisy.ttf", 18, 1, 32, 127),
    ("FontHUDsmall","ui_font_ui_font_font_hu_dsmall",  "x8y12pxDenkiChip.ttf",   16, 1, 32, 127),
    ("FontLARGR",   "ui_font_ui_font_font_largr",      "x14y24pxHeadUpDaisy.ttf", 72, 1, 32, 127),
    ("FontHUD36",   "ui_font_ui_font_font_hu_d36",     "x14y24pxHeadUpDaisy.ttf", 36, 1, 32, 127),
    ("FontHUD30",   "ui_font_ui_font_font_hu_d30",     "x14y24pxHeadUpDaisy.ttf", 30, 1, 32, 127),
]


def render_glyphs(ttf_path: str, size_px: int, range_start: int, range_end: int):
    """Render all glyphs in the given range and return bitmap data + metrics."""
    face = freetype.Face(ttf_path)
    face.set_pixel_sizes(0, size_px)

    glyphs = []
    for cp in range(range_start, range_end + 1):
        face.load_char(chr(cp), freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO)
        bm = face.glyph.bitmap
        metrics = face.glyph

        # Extract bitmap as packed bytes (1bpp, MSB first, padded to byte boundary per row)
        width = bm.width
        rows = bm.rows
        pitch = bm.pitch  # bytes per row in FreeType buffer

        # Repack: LVGL uses ceil(width/8) bytes per row, MSB first (same as FreeType mono)
        lvgl_pitch = (width + 7) // 8
        bitmap_bytes = bytearray()
        for row_idx in range(rows):
            for byte_idx in range(lvgl_pitch):
                if byte_idx < pitch:
                    bitmap_bytes.append(bm.buffer[row_idx * pitch + byte_idx])
                else:
                    bitmap_bytes.append(0)

        glyphs.append({
            'codepoint': cp,
            'bitmap': bytes(bitmap_bytes),
            'width': width,
            'height': rows,
            'advance_w': metrics.advance.x >> 6,  # 26.6 fixed point to integer
            'ofs_x': metrics.bitmap_left,
            'ofs_y': metrics.bitmap_top - rows,  # LVGL ofs_y = top - height (from baseline, going down)
        })

    # Compute line_height and base_line from font metrics
    ascender = face.size.ascender >> 6
    descender = face.size.descender >> 6  # negative value
    line_height = ascender - descender
    base_line = -descender

    return glyphs, line_height, base_line


def format_bitmap_bytes(data: bytes, bytes_per_line: int = 16) -> str:
    """Format bitmap bytes as C hex array."""
    lines = []
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i+bytes_per_line]
        hex_vals = ', '.join(f'0x{b:02x}' for b in chunk)
        lines.append(f'    {hex_vals}')
    return ',\n'.join(lines)


def generate_c_file(eez_name: str, c_symbol: str, ttf_name: str,
                     size_px: int, bpp: int, range_start: int, range_end: int,
                     glyphs: list, line_height: int, base_line: int) -> str:
    """Generate LVGL C font source file content."""

    guard = c_symbol.upper()

    # Build bitmap array
    all_bitmaps = bytearray()
    glyph_entries = []

    # Entry 0: reserved
    glyph_entries.append(
        '    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */'
    )

    for g in glyphs:
        bitmap_index = len(all_bitmaps)
        all_bitmaps.extend(g['bitmap'])
        adv_w = g['advance_w'] * 16  # LVGL stores advance in 1/16 px
        glyph_entries.append(
            f'    {{.bitmap_index = {bitmap_index}, .adv_w = {adv_w}, '
            f'.box_w = {g["width"]}, .box_h = {g["height"]}, '
            f'.ofs_x = {g["ofs_x"]}, .ofs_y = {g["ofs_y"]}}}'
        )

    bitmap_hex = format_bitmap_bytes(all_bitmaps)
    glyph_dsc_str = ',\n'.join(glyph_entries)

    range_length = range_end - range_start + 1

    # Compute underline metrics
    underline_pos = -(size_px // 5)
    underline_thick = max(1, size_px // 12)

    c_code = f"""\
/*******************************************************************************
 * Size: {size_px} px
 * Bpp: {bpp}
 * Opts: --bpp {bpp} --size {size_px} --no-compress --font {ttf_name} --range {range_start}-{range_end} --format lvgl
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

#ifndef {guard}
#define {guard} 1
#endif

#if {guard}

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {{
{bitmap_hex}
}};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {{
{glyph_dsc_str}
}};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{{
    {{
        .range_start = {range_start}, .range_length = {range_length}, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }}
}};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {{
#else
static lv_font_fmt_txt_dsc_t font_dsc = {{
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = {bpp},
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif
}};

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t {c_symbol} = {{
#else
lv_font_t {c_symbol} = {{
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = {line_height},          /*The maximum line height required by the font*/
    .base_line = {base_line},             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = {underline_pos},
    .underline_thickness = {underline_thick},
#endif
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
}};

#endif /*#if {guard}*/
"""
    return c_code


def main():
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    eez_dir = project_root / "Firmware" / "eez"
    fonts_dir = project_root / "Firmware" / "src" / "ui" / "fonts"

    fonts_dir.mkdir(parents=True, exist_ok=True)

    print("=== LVGL Font Generator ===")
    print(f"TTF source dir: {eez_dir}")
    print(f"Output dir:     {fonts_dir}")
    print()

    for eez_name, c_symbol, ttf_name, size_px, bpp, r_start, r_end in FONT_DEFS:
        ttf_path = str(eez_dir / ttf_name)
        if not os.path.exists(ttf_path):
            print(f"ERROR: TTF not found: {ttf_path}")
            continue

        print(f"Generating {eez_name}: {ttf_name} @ {size_px}px ...", end=" ", flush=True)

        glyphs, line_height, base_line = render_glyphs(ttf_path, size_px, r_start, r_end)
        c_code = generate_c_file(eez_name, c_symbol, ttf_name,
                                  size_px, bpp, r_start, r_end,
                                  glyphs, line_height, base_line)

        out_path = fonts_dir / f"{c_symbol}.c"
        with open(out_path, 'w', encoding='utf-8', newline='\n') as f:
            f.write(c_code)

        total_bitmap = sum(len(g['bitmap']) for g in glyphs)
        print(f"OK ({len(glyphs)} glyphs, {total_bitmap} bytes bitmap, line_h={line_height})")

    # Update fonts.h
    print("\nUpdating fonts.h ...", end=" ", flush=True)
    fonts_h = fonts_dir.parent / "fonts.h"
    h_content = '#ifndef EEZ_LVGL_UI_FONTS_H\n#define EEZ_LVGL_UI_FONTS_H\n\n#include <lvgl/lvgl.h>\n\n'
    for _, c_symbol, _, _, _, _, _ in FONT_DEFS:
        h_content += f'extern const lv_font_t {c_symbol};\n'
    h_content += '\ntypedef struct _ext_font_desc_t {\n    const char *name;\n    const void *font_ptr;\n} ext_font_desc_t;\n\nextern ext_font_desc_t fonts[];\n#endif\n'
    with open(fonts_h, 'w', encoding='utf-8', newline='\n') as f:
        f.write(h_content)
    print("OK")

    print("\n=== Done! ===")
    print("Remember to update ext_font_desc_t fonts[] in screens.c if new fonts were added.")
    print("New fonts to add to screens.c:")
    existing = {"ui_font_Font1", "ui_font_FontHUDmid", "ui_font_FontHUDsmall", "ui_font_FontLARGR"}
    for eez_name, c_symbol, _, _, _, _, _ in FONT_DEFS:
        ext_name = f"ui_font_{eez_name}"
        if ext_name not in existing:
            print(f'  {{ "{ext_name}", &{c_symbol} }},')


if __name__ == '__main__':
    main()
