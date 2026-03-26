#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* eez003 uses a subset of images (no tachometer backgrounds) */
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

extern const ext_img_desc_t images[12];

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/
