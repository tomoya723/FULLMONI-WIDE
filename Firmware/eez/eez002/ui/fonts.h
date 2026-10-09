#ifndef EEZ_LVGL_UI_FONTS_H
#define EEZ_LVGL_UI_FONTS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_font_t ui_font_ui_font_font1;
extern const lv_font_t ui_font_ui_font_font_hud36;
extern const lv_font_t ui_font_ui_font_font_hu_dmid;
extern const lv_font_t ui_font_ui_font_font_hu_dsmall;
extern const lv_font_t ui_font_ui_font_font_largr;
extern const lv_font_t ui_font_ui_font_font_hud30;
extern const lv_font_t ui_font_b612_mono;
extern const lv_font_t ui_font_ui_font_612_small;
extern const lv_font_t ui_font_ui_font_612_30;
extern const lv_font_t ui_font_ui_font_612_25;
extern const lv_font_t ui_font_ui_font_612_70;
extern const lv_font_t ui_font_ui_font_612_50;
extern const lv_font_t ui_font_ui_font_612_r_17;

#ifndef EXT_FONT_DESC_T
#define EXT_FONT_DESC_T
typedef struct _ext_font_desc_t {
    const char *name;
    const void *font_ptr;
} ext_font_desc_t;
#endif

extern ext_font_desc_t fonts[];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_FONTS_H*/