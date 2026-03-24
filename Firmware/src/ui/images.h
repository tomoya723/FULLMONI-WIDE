#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_ui_img_back5_png;
extern const lv_img_dsc_t img_ui_img_back6_png;
extern const lv_img_dsc_t img_ui_img_marker_red_bar_80x7s_png;
extern const lv_img_dsc_t img_ui_img_marker_red_bar_20x7s_png;
extern const lv_img_dsc_t img_ui_img_fmw_op0_png;
extern const lv_img_dsc_t img_ui_img_fuel1_png;
extern const lv_img_dsc_t img_ui_img_fuel2_png;
extern const lv_img_dsc_t img_ui_img_ws_masterwarning_png;
extern const lv_img_dsc_t img_ui_img_ws_oilpresswarning_png;
extern const lv_img_dsc_t img_ui_img_ws_watarcool_png;
extern const lv_img_dsc_t img_ui_img_ws_waterwarning_png;
extern const lv_img_dsc_t img_ui_img_ws_exhaustwarning_png;
extern const lv_img_dsc_t img_ui_img_ws_batterywarning_png;
extern const lv_img_dsc_t img_ui_img_ws_breakwarning_png;
extern const lv_img_dsc_t img_ui_img_ws_beltwarning_png;
extern const lv_img_dsc_t img_ui_img_ws_fuelcheck_png;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[16];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/