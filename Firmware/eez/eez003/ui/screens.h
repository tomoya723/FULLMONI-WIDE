#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* --- Screen enum ---------------------------------------------------------- */
enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_SCREEN1 = 1,
    _SCREEN_ID_LAST = 1
};

/* --- Widget object tree --------------------------------------------------- */
typedef struct _objects_t {
    lv_obj_t *screen1;

    /* Main dashboard container */
    lv_obj_t *ui_container_dashboard;

    /* Fan tachometer area (custom drawn) */
    lv_obj_t *ui_container_tacho;
    lv_obj_t *ui_lbl_boost;
    lv_obj_t *ui_lbl_boost_val;
    lv_obj_t *ui_lbl_engine_rpm;

    /* Left column: 3 bar gauges */
    lv_obj_t *ui_bar_water_temp;
    lv_obj_t *ui_lbl_water_temp;
    lv_obj_t *ui_lbl_water_temp_name;

    lv_obj_t *ui_bar_oil_temp;
    lv_obj_t *ui_lbl_oil_temp;
    lv_obj_t *ui_lbl_oil_temp_name;

    lv_obj_t *ui_bar_charge_temp;
    lv_obj_t *ui_lbl_charge_temp;
    lv_obj_t *ui_lbl_charge_temp_name;

    /* Right column: 3 bar gauges */
    lv_obj_t *ui_bar_oil_press;
    lv_obj_t *ui_lbl_oil_press;
    lv_obj_t *ui_lbl_oil_press_name;

    lv_obj_t *ui_bar_fuel_press;
    lv_obj_t *ui_lbl_fuel_press;
    lv_obj_t *ui_lbl_fuel_press_name;

    lv_obj_t *ui_bar_battery;
    lv_obj_t *ui_lbl_battery;
    lv_obj_t *ui_lbl_battery_name;

    /* Center info */
    lv_obj_t *ui_lbl_gear;
    lv_obj_t *ui_lbl_als;
    lv_obj_t *ui_lbl_spd;
    lv_obj_t *ui_lbl_spd_unit;
    lv_obj_t *ui_lbl_road_speed;
    lv_obj_t *ui_lbl_parking;

    /* Left bottom: Fuel + Mode */
    lv_obj_t *ui_bar_fuel;
    lv_obj_t *ui_lbl_fuel_icon;
    lv_obj_t *ui_lbl_mode;
    lv_obj_t *ui_lbl_mode_road;
    lv_obj_t *ui_lbl_mode_sport;
    lv_obj_t *ui_lbl_mode_sport_plus;
    lv_obj_t *ui_mode_border;

    /* Right bottom: ODO/Trip/Range */
    lv_obj_t *ui_lbl_odo_name;
    lv_obj_t *ui_lbl_odo;
    lv_obj_t *ui_lbl_trip_name;
    lv_obj_t *ui_lbl_trip;
    lv_obj_t *ui_lbl_range_name;
    lv_obj_t *ui_lbl_range;

    /* Bottom center: branding */
    lv_obj_t *ui_lbl_prodrive;

    /* Decorative line */
    lv_obj_t *ui_line_deco;

    /* Opening screen */
    lv_obj_t *ui_container_opening;
    lv_obj_t *ui_img_opening;

    /* Warning / telltale icons */
    lv_obj_t *ui_container_telltale;
    lv_obj_t *ui_img_warn_master;
    lv_obj_t *ui_img_warn_oil_press;
    lv_obj_t *ui_img_warn_water_cold;
    lv_obj_t *ui_img_warn_water_hot;
    lv_obj_t *ui_img_warn_exhaust;
    lv_obj_t *ui_img_warn_battery;
    lv_obj_t *ui_img_warn_brake;
    lv_obj_t *ui_image_warn_belt;
    lv_obj_t *ui_img_warn_fuel;

    /* Notification overlay */
    lv_obj_t *ui_notify_box;
    lv_obj_t *ui_notify_label;
} objects_t;

extern objects_t objects;

void create_screen_screen1(void);
void tick_screen_screen1(void);

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens(void);

/* --- Font externals (for direct reference if needed) ---------------------- */
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

#endif /*EEZ_LVGL_UI_SCREENS_H*/
