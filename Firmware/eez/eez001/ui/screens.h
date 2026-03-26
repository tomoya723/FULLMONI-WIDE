#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_SCREEN1 = 1,
    _SCREEN_ID_LAST = 1
};

typedef struct _objects_t {
    lv_obj_t *screen1;
    lv_obj_t *ui_container_dashboard;
    lv_obj_t *ui_img_back7;
    lv_obj_t *ui_img_tacho;
    lv_obj_t *ui_arc_rpm;
    lv_obj_t *ui_lbl_rpm;
    lv_obj_t *ui_image_rpm;
    lv_obj_t *ui_image_peak_rpm;
    lv_obj_t *ui_image2;
    lv_obj_t *ui_bar_water_temp;
    lv_obj_t *ui_lbl_water_temp;
    lv_obj_t *ui_bar_iat;
    lv_obj_t *ui_lbl_iat;
    lv_obj_t *ui_bar_oil_temp;
    lv_obj_t *ui_lbl_oil_temp;
    lv_obj_t *ui_bar_map;
    lv_obj_t *ui_lbl_map;
    lv_obj_t *ui_bar_oil_press;
    lv_obj_t *ui_lbl_oil_press;
    lv_obj_t *ui_bar_battery;
    lv_obj_t *ui_lbl_battery;
    lv_obj_t *ui_lbl_afr;
    lv_obj_t *ui_lbl_trip;
    lv_obj_t *ui_lbl_odo;
    lv_obj_t *ui_lbl_time;
    lv_obj_t *ui_lbl_gear;
    lv_obj_t *ui_lbl_spd;
    lv_obj_t *ui_bar_fuel;
    lv_obj_t *ui_container_opening;
    lv_obj_t *ui_img_opening;
    lv_obj_t *ui_container_telltale;
    lv_obj_t *ui_img_telltale;
    lv_obj_t *ui_img_warn_master;
    lv_obj_t *ui_img_warn_oil_press;
    lv_obj_t *ui_img_warn_water_cold;
    lv_obj_t *ui_img_warn_water_hot;
    lv_obj_t *ui_img_warn_exhaust;
    lv_obj_t *ui_img_warn_battery;
    lv_obj_t *ui_img_warn_brake;
    lv_obj_t *ui_image_warn_belt;
    lv_obj_t *ui_img_warn_fuel;
    lv_obj_t *ui_notify_box;
    lv_obj_t *ui_notify_label;
} objects_t;

extern objects_t objects;

void create_screen_screen1();
void tick_screen_screen1();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/