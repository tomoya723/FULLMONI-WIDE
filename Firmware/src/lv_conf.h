/**
 * @file lv_conf.h
 * LVGL v8.3 configuration for FULLMONI-WIDE (RX72N, 800x256, RGB565)
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

/* Swap the 2 bytes of RGB565 color. Useful if the display has a 8-bit interface */
#define LV_COLOR_16_SWAP 0

/* Enable features to draw on transparent background.
 * Colors with opacity won't be mixed with the background color*/
#define LV_COLOR_SCREEN_TRANSP 0

/* Images pixels with this color will not be drawn (chroma key) */
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)

/*=========================
   MEMORY SETTINGS
 *=========================*/

/* Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB) */
/* Placed in RAM2 region (0x00864000) via .bss2 section attribute in lv_port */
#define LV_MEM_CUSTOM 0
#define LV_MEM_SIZE (32U * 1024U)  /* 32KB - LVGL heap */

/* Set an address for the memory pool instead of allocating it as a global array.
 * Can be in external SRAM too. 0: unused */
#define LV_MEM_ADR 0

/* Compiler prefix for a big array declaration in RAM */
#define LV_MEM_ATTR

/* Set the number of intermediate memory buffer used during rendering.
 * Two buffers are needed: buffer1 and buffer2 */
#define LV_MEM_BUF_MAX_NUM 16

/* Use the standard `memcpy` and `memset` instead of LVGL's own functions. */
#define LV_MEMCPY_MEMSET_STD 1

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period in ms */
#define LV_DISP_DEF_REFR_PERIOD 10  /* 10ms = max 100fps target */

/* Input device read period in ms */
#define LV_INDEV_DEF_READ_PERIOD 30

/* Use a custom tick source: ap_10ms calls lv_tick_inc(10) */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE  "lv_port/lv_port_tick.h"
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR lv_port_tick_get()
#endif

/* Default Dot Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 * (Not so important, you can adjust it to modify default sizes) */
#define LV_DPI_DEF 130

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Drawing
 *-----------*/

/* Enable complex draw engine.
 * Required to draw shadow, gradient, rounded corners, circles, arc, skew lines, image transformations */
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0

    /* Allow buffering some shadow calculation.
     * LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer, where shadow size is `shadow_width + radius`
     * Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost */
    #define LV_SHADOW_CACHE_SIZE 0

    /* Set number of maximally cached circle data. */
    #define LV_CIRCLE_CACHE_SIZE 4
#endif

/* Default image cache size. Image caching keeps the images opened.
 * If only the built-in image formats are used there is no real advantage of caching. (I.e. if no new image decoder is added)
 * With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 * However the opened images might consume additional RAM.
 * Set it to 0 to disable caching */
#define LV_IMG_CACHE_DEF_SIZE 0

/* Number of stops allowed per gradient. Increase this to allow more stops.
 * This adds (sizeof(lv_color_t) + 1) bytes per additional stop */
#define LV_GRADIENT_MAX_STOPS 2

/* Default gradient buffer size.
 * When LVGL calculates the gradient "maps" it can save them into a cache to avoid calculating them again.
 * LV_GRAD_CACHE_DEF_SIZE sets the size of this cache in bytes.
 * If the cache is too small the map will be allocated only while it's required for the drawing.
 * 0 mean no caching. */
#define LV_GRAD_CACHE_DEF_SIZE 0

/* Allow dithering the gradients (to achieve visual smooth color gradients on limited color depth display)
 * LV_DITHER_GRADIENT implies allocating one or two more lines of the display's width
 * 1/2 item extra SRAM is consumed */
#define LV_DITHER_GRADIENT 0

/* Add support for error diffusion dithering.
 * Error diffusion dithering gets a much better visual result, but implies more CPU consumption and one more line of color map.  */
#define LV_DITHER_ERROR_DIFFUSION 0

/* Maximum buffer size to allocate for rotation.
 * Only used if software rotation is enabled in the display driver. */
#define LV_DISP_ROT_MAX_BUF (10*1024)

/*-------------
 * GPU
 *-----------*/

/* Use Renesas Dave2D (DRW2D) with RX devices - will be enabled in Phase 2 */
#define LV_USE_GPU_DAVE2D 0

/* Use SDL renderer API. Requires LV_USE_EXTDRV = 1 */
#define LV_USE_GPU_SDL 0

/*-------------
 * Logging
 *-----------*/

/* Enable the log module */
#define LV_USE_LOG 0

/*-------------
 * Asserts
 *-----------*/

/* Enable assertion expressions */
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/* Add a custom handler when assert happens e.g. restart the MCU */
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /* Halt by default */

/*-------------
 * Others
 *-----------*/

/* 1: Show CPU usage and FPS count */
#define LV_USE_PERF_MONITOR 1
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_TOP_RIGHT
#endif

/* 1: Show the used memory and the memory fragmentation */
#define LV_USE_MEM_MONITOR 0

/* 1: Draw random colored rectangles over the redrawn areas */
#define LV_USE_REFR_DEBUG 0

/* Change the built in (v)snprintf functions */
#define LV_SPRINTF_CUSTOM 0
#define LV_SPRINTF_USE_FLOAT 0

#define LV_USE_USER_DATA 1

/* Garbage Collector settings
 * Used if lvgl is bound to higher level language and the memory is managed by that language */
#define LV_ENABLE_GC 0

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/* For big endian systems set to 1 */
/* RX72N is little-endian */
#define LV_BIG_ENDIAN_SYSTEM 0

/* Define a custom attribute to `lv_tick_inc` function */
#define LV_ATTRIBUTE_TICK_INC

/* Define a custom attribute to `lv_timer_handler` function */
#define LV_ATTRIBUTE_TIMER_HANDLER

/* Define a custom attribute to `lv_disp_flush_ready` function */
#define LV_ATTRIBUTE_FLUSH_READY

/* Required alignment size for buffers */
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 4

/* Will be added where memories needs to be aligned (with -Os data might not be aligned to boundary by default).
 * E.g. __attribute__((aligned(4))) */
#define LV_ATTRIBUTE_MEM_ALIGN __attribute__((aligned(4)))

/* Attribute to mark large constant arrays for example font's bitmaps */
#define LV_ATTRIBUTE_LARGE_CONST

/* Place LVGL heap (work_mem_int in lv_mem.c) in RAM2 (.bss2, 0x00864000).
 * RAM2 layout: draw_buf(50KB) + heap(32KB) + GUIConf(28KB) = 110KB < 112KB. */
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY __attribute__((section(".bss2")))

/* Place performance critical functions into a faster memory (e.g RAM) */
#define LV_ATTRIBUTE_FAST_MEM

/* Prefix variables that are used in GPU accelerated operations, often these need to be placed in RAM sections that are DMA accessible */
#define LV_ATTRIBUTE_DMA

/* Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 * should also appear on LVGL binding API such as Micropython. */
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /* The default value just prevents GCC warning */

/* Extend the default -32k..32k coordinate range to -4M..4M by using int32_t for coordinates instead of int16_t */
#define LV_USE_LARGE_COORD 0

/*==================
 *   FONT USAGE
 *===================*/

/* Montserrat fonts with ASCII range and some symbols using bpp = 4 */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1  /* Default font - enabled */
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Demonstrate special features */
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
#define LV_FONT_SIMSUN_16_CJK            0

/* Pixel perfect monospace font */
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/* Optionally declare custom fonts here.
 * You can use these fonts as default font too and they will be available globally.
 * E.g. #define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2) */
#define LV_FONT_CUSTOM_DECLARE

/* Always set a default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Enable it if you have fonts with a lot of characters.
 * The limit depends on the font size, font face and bpp.
 * Compiler error will be triggered if the limit is exceeded. */
#define LV_FONT_FMT_TXT_LARGE 0

/* Enables/disables support for compressed fonts. */
#define LV_USE_FONT_COMPRESSED 0

/* Enable subpixel rendering */
#define LV_USE_FONT_SUBPX 0

/* Set the pixel order of the display. Physical order of RGB channels.
 * Needed if "subpx fonts" are used.
 * With "normal" font it doesn't matter. */
#define LV_FONT_SUBPX_BGR 0

/*=================
 *  TEXT SETTINGS
 *=================*/

/* Select a character encoding for strings. */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* Can break (wrap) texts on these chars */
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/* If a word is at least this long, will break wherever "prettiest" */
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/* Minimum number of characters in a long word to put on a line before a break. */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/* Minimum number of characters in a long word to put on a line after a break. */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* The control character to use for signalling text recoloring. */
#define LV_TXT_COLOR_CMD "#"

/* Support bidirectional texts. Allows mixing Left-to-Right and Right-to-Left texts. */
#define LV_USE_BIDI 0

/* Enable Arabic/Persian processing
 * In these languages characters should be replaced with an other form based on their position in the text */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 *  WIDGET USAGE
 *================*/

/* 1: use the widget */
#define LV_USE_ARC        1   /* Arc - for tachometer/speedometer */
#define LV_USE_BAR        1   /* Bar - for boost/fuel gauge */
#define LV_USE_BTN        0
#define LV_USE_BTNMATRIX  1   /* required by LV_USE_KEYBOARD */
#define LV_USE_CANVAS     0
#define LV_USE_CHECKBOX   0
#define LV_USE_DROPDOWN   1   /* required by ui_helpers.c (SLS-generated) */
#define LV_USE_IMG        1   /* Image - for warning icons */
#define LV_USE_LABEL      1   /* Label - for numeric display */
#define LV_USE_LINE       1   /* Line - for tachometer needle */
#define LV_USE_ROLLER     1   /* required by ui_helpers.c (SLS-generated) */
#define LV_USE_SLIDER     1   /* required by ui_helpers.c (SLS-generated) */
#define LV_USE_SWITCH     0
#define LV_USE_TEXTAREA   1   /* required by ui_helpers.c (SLS-generated) */
#define LV_USE_TABLE      0

/*==================
 * EXTRA COMPONENTS
 *==================*/

/* 1: Use the extra widgets */
#define LV_USE_WIDGETS   1

#define LV_USE_ANIMIMG   0

#define LV_USE_CALENDAR  0
#define LV_USE_CALENDAR_HEADER_ARROW  0
#define LV_USE_CALENDAR_HEADER_DROPDOWN 0

#define LV_USE_CHART     1   /* Chart recorder for AFR/MAP/RPM logging */

#define LV_USE_COLORWHEEL 0

#define LV_USE_IMGBTN    0

#define LV_USE_KEYBOARD  1   /* required by ui_helpers.c (SLS-generated) */

#define LV_USE_LED       0

#define LV_USE_LIST      0

#define LV_USE_MENU      0

#define LV_USE_METER     0  /* Meter widget - replaced by static bg image + lv_line needle */

#define LV_USE_MSGBOX    0

#define LV_USE_SPINBOX   1   /* required by ui_helpers.c (SLS-generated) */

#define LV_USE_SPINNER   0

#define LV_USE_TABVIEW   0

#define LV_USE_TILEVIEW  0

#define LV_USE_WIN       0

#define LV_USE_SPAN      0

/* 1: Enable span in Label widget */
#if LV_USE_SPAN
    /* A line text can contain maximum num of span descriptor */
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

/*==================
 * THEMES
 *==================*/

/* A simple, impressive and very complete theme */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT

    /* 0: Light mode; 1: Dark mode */
    #define LV_THEME_DEFAULT_DARK 1

    /* 1: Enable grow on press */
    #define LV_THEME_DEFAULT_GROW 0

    /* Default transition time in [ms] */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

/* A very simple theme that is a good starting point for a custom theme */
#define LV_USE_THEME_BASIC 1

/* A theme designed for monochrome displays */
#define LV_USE_THEME_MONO 0

/*==================
 * LAYOUTS
 *==================*/

/* A layout similar to Flexbox in CSS. */
#define LV_USE_FLEX 1

/* A layout similar to Grid in CSS. */
#define LV_USE_GRID 0

/*==================
 * 3RD PARTS LIBRARIES
 *==================*/

/* File system interfaces for common APIs */
#define LV_USE_FS_STDIO 0
#define LV_USE_FS_POSIX 0
#define LV_USE_FS_WIN32 0
#define LV_USE_FS_FATFS 0

/* PNG decoder library */
#define LV_USE_PNG 0

/* BMP decoder library */
#define LV_USE_BMP 0

/* JPG decoder library */
#define LV_USE_SJPG 0

/* GIF decoder library */
#define LV_USE_GIF 0

/* QR code library */
#define LV_USE_QRCODE 0

/* FreeType library */
#define LV_USE_FREETYPE 0

/* Rlottie library */
#define LV_USE_RLOTTIE 0

/* FFmpeg library for image decoding and playing videos */
#define LV_USE_FFMPEG 0

/*==================
 * OTHERS
 *==================*/

/* 1: Enable API to take snapshot for object */
#define LV_USE_SNAPSHOT 0

/* 1: Enable Monkey test */
#define LV_USE_MONKEY 0

/* 1: Enable grid navigation */
#define LV_USE_GRIDNAV 0

/* 1: Enable lv_obj fragment */
#define LV_USE_FRAGMENT 0

/* 1: Support using images as font in label or span widgets */
#define LV_USE_IMGFONT 0

/* 1: Enable a published subscriber based messaging system */
#define LV_USE_MSG 0

/* 1: Enable Pinyin input method */
#define LV_USE_IME_PINYIN 0

/*==================
 * EXAMPLES
 *==================*/

/* Enable the examples to be built with the library */
#define LV_BUILD_EXAMPLES 0

/*===================
 * TEST USAGE
 *==================*/
#define LV_USE_TEST 0

#endif /* LV_CONF_H */
