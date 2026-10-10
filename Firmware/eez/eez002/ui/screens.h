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
    SCREEN_ID_SCREEN1_1 = 2,
    _SCREEN_ID_LAST = 2
};

typedef struct _objects_t {
    lv_obj_t *screen1;
    lv_obj_t *screen1_1;
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
    lv_obj_t *ui_lbl_diff_b1;
    lv_obj_t *ui_lbl_lambda_b1;
    lv_obj_t *ui_lbl_afr_3;
    lv_obj_t *ui_lbl_afr_4;
    lv_obj_t *ui_lbl_afr_7;
    lv_obj_t *ui_lbl_afr_8;
    lv_obj_t *ui_lbl_diff_b2;
    lv_obj_t *ui_lbl_lambda_b2;
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
    lv_obj_t *ui_container_dashboard_1;
    lv_obj_t *ui_panel_oil_press_warn_1;
    lv_obj_t *ui_panel_oil_temp_warn_1;
    lv_obj_t *ui_panel_water_temp_warn_1;
    lv_obj_t *ui_lbl_rpm_1;
    lv_obj_t *ui_lbl_water_temp_1;
    lv_obj_t *ui_lbl_oil_temp_1;
    lv_obj_t *ui_lbl_oil_press_1;
    lv_obj_t *ui_lbl_battery_1;
    lv_obj_t *ui_lbl_afr_1;
    lv_obj_t *ui_lbl_time_1;
    lv_obj_t *ui_lbl_spd_1;
    lv_obj_t *ui_bar_fuel_1;
    lv_obj_t *ui_lbl_track_mode_1;
    lv_obj_t *ui_lbl_tach_unit_1;
    lv_obj_t *ui_lbl_oil_press_cap_1;
    lv_obj_t *ui_lbl_oil_temp_cap_1;
    lv_obj_t *ui_lbl_water_temp_cap_1;
    lv_obj_t *ui_lbl_fuel_cap_1;
    lv_obj_t *ui_lbl_fuel_e_1;
    lv_obj_t *ui_lbl_fuel_f_1;
    lv_obj_t *ui_lbl_afr_cap_1;
    lv_obj_t *ui_lbl_volt_cap_1;
    lv_obj_t *ui_rpm_8;
    lv_obj_t *ui_rpm_7;
    lv_obj_t *ui_rpm_6;
    lv_obj_t *ui_rpm_5;
    lv_obj_t *ui_rpm_4;
    lv_obj_t *ui_rpm_3;
    lv_obj_t *ui_rpm_2;
    lv_obj_t *ui_rpm_1;
    lv_obj_t *ui_rpm_0;
    lv_obj_t *ui_bar_rpm_1;
    lv_obj_t *ui_lbl_spd_cap;
    lv_obj_t *ui_lbl_rpm_cap_1;
    lv_obj_t *ui_line_upper;
    lv_obj_t *ui_line_lower;
    lv_obj_t *ui_lbl_oil_press_cap_2;
    lv_obj_t *ui_lbl_oil_temp_cap_2;
    lv_obj_t *ui_lbl_water_temp_cap_2;
    lv_obj_t *ui_lbl_fuel_low_1;
    lv_obj_t *ui_container_opening_1;
    lv_obj_t *ui_img_opening_1;
    lv_obj_t *ui_container_telltale_1;
    lv_obj_t *ui_img_telltale_1;
    lv_obj_t *ui_img_warn_master_1;
    lv_obj_t *ui_img_warn_oil_press_1;
    lv_obj_t *ui_img_warn_water_cold_1;
    lv_obj_t *ui_img_warn_water_hot_1;
    lv_obj_t *ui_img_warn_exhaust_1;
    lv_obj_t *ui_img_warn_battery_1;
    lv_obj_t *ui_img_warn_brake_1;
    lv_obj_t *ui_image_warn_belt_1;
    lv_obj_t *ui_img_warn_fuel_1;
    lv_obj_t *ui_notify_box_1;
    lv_obj_t *ui_notify_label_1;
} objects_t;

extern objects_t objects;

void create_screen_screen1();
void tick_screen_screen1();

void create_screen_screen1_1();
void tick_screen_screen1_1();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/