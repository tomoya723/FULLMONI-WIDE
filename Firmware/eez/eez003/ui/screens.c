/* screens.c --- eez003 (Prodrive Dashboard)
 * Hand-crafted LVGL widget tree for Prodrive racing design.
 * Layout: 800x256, 16-bit BGR, black background, cyan accent.
 */
#include <string.h>
#include "screens.h"
#include "images.h"
#include "fonts.h"
#include "actions.h"
#include "vars.h"
#include "styles.h"
#include "ui.h"

objects_t objects;
lv_obj_t *tick_value_change_obj;

/* ------ Color palette ------ */
#define C_BLK    0xff000000
#define C_WHT    0xffffffff
#define C_CYAN   0xff00E0FF
#define C_RED    0xffFF0000
#define C_DIM    0xffC0C0C0
#define C_BARBG  0xff1A2530
#define PM (LV_PART_MAIN | LV_STATE_DEFAULT)
#define PI (LV_PART_INDICATOR | LV_STATE_DEFAULT)

/* Common clear-flag sets */
#define CTN_CLR (LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW)
#define LBL_CLR (LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE)
#define BAR_CLR (LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE)
#define IMG_CLR (LV_OBJ_FLAG_ADV_HITTEST|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW|LV_OBJ_FLAG_SNAPPABLE)

/* --- Helper: bar gauge with Prodrive cyan style --- */
static lv_obj_t *mk_bar(lv_obj_t *p, int16_t x, int16_t y, int16_t w, int16_t h) {
    lv_obj_t *o = lv_bar_create(p);
    lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_bar_set_value(o, 0, LV_ANIM_OFF);
    lv_obj_clear_flag(o, BAR_CLR);
    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
    lv_obj_set_style_radius(o, 0, PM);
    lv_obj_set_style_bg_color(o, lv_color_hex(C_BARBG), PM);
    lv_obj_set_style_bg_opa(o, 255, PM);
    lv_obj_set_style_radius(o, 0, PI);
    lv_obj_set_style_bg_color(o, lv_color_hex(C_CYAN), PI);
    lv_obj_set_style_bg_opa(o, 255, PI);
    return o;
}

/* --- Helper: right-aligned value label --- */
static lv_obj_t *mk_val(lv_obj_t *p, int16_t x, int16_t y, int16_t w,
                         const lv_font_t *f, const char *t) {
    lv_obj_t *o = lv_label_create(p);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_label_set_long_mode(o, LV_LABEL_LONG_CLIP);
    lv_obj_clear_flag(o, LBL_CLR);
    lv_obj_set_style_align(o, LV_ALIGN_RIGHT_MID, PM);
    lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_RIGHT, PM);
    lv_obj_set_style_text_font(o, f, PM);
    lv_obj_set_style_text_color(o, lv_color_hex(C_WHT), PM);
    lv_obj_set_style_width(o, w, PM);
    lv_label_set_text(o, t);
    return o;
}

/* --- Helper: small dim name label --- */
static lv_obj_t *mk_name(lv_obj_t *p, int16_t x, int16_t y, const char *t) {
    lv_obj_t *o = lv_label_create(p);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_clear_flag(o, LBL_CLR);
    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
    lv_obj_set_style_text_font(o, &ui_font_ui_font_font_hu_dsmall, PM);
    lv_obj_set_style_text_color(o, lv_color_hex(C_DIM), PM);
    lv_label_set_text(o, t);
    return o;
}

/* --- Helper: generic label --- */
static lv_obj_t *mk_lbl(lv_obj_t *p, int16_t x, int16_t y,
                         const lv_font_t *f, uint32_t c, const char *t) {
    lv_obj_t *o = lv_label_create(p);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_clear_flag(o, LBL_CLR);
    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
    lv_obj_set_style_text_font(o, f, PM);
    lv_obj_set_style_text_color(o, lv_color_hex(c), PM);
    lv_label_set_text(o, t);
    return o;
}

/* --- Helper: warning icon in telltale strip --- */
static lv_obj_t *mk_warn(lv_obj_t *p, int16_t y, const lv_img_dsc_t *src) {
    lv_obj_t *o = lv_img_create(p);
    lv_obj_set_pos(o, 0, y);
    lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_img_set_src(o, src);
    lv_img_set_pivot(o, 0, 0);
    lv_obj_clear_flag(o, IMG_CLR);
    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
    return o;
}

/* ================================================================= */
void create_screen_screen1() {
    lv_obj_t *obj = lv_obj_create(0);
    objects.screen1 = obj;
    lv_obj_set_pos(obj, 0, 0);
    lv_obj_set_size(obj, 800, 256);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE|LV_OBJ_FLAG_PRESS_LOCK|LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_GESTURE_BUBBLE|LV_OBJ_FLAG_SNAPPABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER);
    lv_obj_set_style_bg_color(obj, lv_color_hex(C_BLK), PM);
    lv_obj_set_style_bg_opa(obj, 255, PM);
    {
        lv_obj_t *scr = obj;

        /* ======== ContainerDashboard ======== */
        {
            lv_obj_t *obj = lv_obj_create(scr);
            objects.ui_container_dashboard = obj;
            lv_obj_set_pos(obj, 18, 0);
            lv_obj_set_size(obj, 765, 256);
            lv_obj_clear_flag(obj, CTN_CLR);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, PM);
            lv_obj_set_style_border_width(obj, 0, PM);
            lv_obj_set_style_radius(obj, 0, PM);
            lv_obj_set_style_pad_top(obj, 0, PM);
            lv_obj_set_style_pad_bottom(obj, 0, PM);
            lv_obj_set_style_pad_left(obj, 0, PM);
            lv_obj_set_style_pad_right(obj, 0, PM);
            lv_obj_set_style_bg_color(obj, lv_color_hex(C_BLK), PM);
            lv_obj_set_style_bg_opa(obj, 255, PM);
            {
                lv_obj_t *d = obj;

                /* -- Fan Tachometer container (custom draw via fan_tacho.c) -- */
                {
                    lv_obj_t *o = lv_obj_create(d);
                    objects.ui_container_tacho = o;
                    lv_obj_set_pos(o, 20, -38);
                    lv_obj_set_size(o, 320, 155);
                    lv_obj_clear_flag(o, CTN_CLR);
                    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
                    lv_obj_set_style_border_width(o, 0, PM);
                    lv_obj_set_style_radius(o, 0, PM);
                    lv_obj_set_style_pad_top(o, 0, PM);
                    lv_obj_set_style_pad_bottom(o, 0, PM);
                    lv_obj_set_style_pad_left(o, 0, PM);
                    lv_obj_set_style_pad_right(o, 0, PM);
                    lv_obj_set_style_bg_opa(o, 0, PM);
                }

                /* -- Boost labels -- */
                objects.ui_lbl_boost     = mk_lbl(d, -130, -115, &ui_font_ui_font_font_hu_dsmall, C_CYAN, "BOOST");
                objects.ui_lbl_boost_val = mk_val(d, -50, -115, 85, &ui_font_ui_font_font_hu_dmid, " 0.00");

                /* -- Engine RPM (below tachometer) -- */
                objects.ui_lbl_engine_rpm = mk_val(d, 20, 48, 100, &ui_font_ui_font_font_hud30, "0");

                /* ===== LEFT COLUMN: 3 bar gauges ===== */
                objects.ui_bar_water_temp      = mk_bar(d, -250, -86, 80, 8);
                objects.ui_lbl_water_temp      = mk_val(d, -180, -86, 55, &ui_font_ui_font_font1, "0");
                objects.ui_lbl_water_temp_name = mk_name(d, -325, -86, "WATER");

                objects.ui_bar_oil_temp       = mk_bar(d, -250, -54, 80, 8);
                objects.ui_lbl_oil_temp       = mk_val(d, -180, -54, 55, &ui_font_ui_font_font1, "0");
                objects.ui_lbl_oil_temp_name  = mk_name(d, -325, -54, "OIL T");

                objects.ui_bar_charge_temp      = mk_bar(d, -250, -22, 80, 8);
                objects.ui_lbl_charge_temp      = mk_val(d, -180, -22, 55, &ui_font_ui_font_font1, "0");
                objects.ui_lbl_charge_temp_name = mk_name(d, -325, -22, "CHG T");

                /* ===== RIGHT COLUMN: 3 bar gauges ===== */
                objects.ui_bar_oil_press      = mk_bar(d, 250, -86, 80, 8);
                objects.ui_lbl_oil_press      = mk_val(d, 310, -86, 55, &ui_font_ui_font_font1, "0.0");
                objects.ui_lbl_oil_press_name = mk_name(d, 175, -86, "OIL P");

                objects.ui_bar_fuel_press      = mk_bar(d, 250, -54, 80, 8);
                objects.ui_lbl_fuel_press      = mk_val(d, 310, -54, 55, &ui_font_ui_font_font1, "0");
                objects.ui_lbl_fuel_press_name = mk_name(d, 175, -54, "FUEL P");

                objects.ui_bar_battery        = mk_bar(d, 250, -22, 80, 8);
                objects.ui_lbl_battery        = mk_val(d, 310, -22, 55, &ui_font_ui_font_font1, "0.0");
                objects.ui_lbl_battery_name   = mk_name(d, 175, -22, "BATT");

                /* ===== CENTER: gear, speed, ALS ===== */
                objects.ui_lbl_gear = mk_lbl(d, 25, -18, &ui_font_ui_font_font_largr, C_WHT, "N");
                objects.ui_lbl_als  = mk_lbl(d, -30, 18, &ui_font_ui_font_font_hu_dsmall, C_CYAN, "ALS");
                lv_obj_add_flag(objects.ui_lbl_als, LV_OBJ_FLAG_HIDDEN);

                objects.ui_lbl_spd      = mk_val(d, -15, 70, 100, &ui_font_ui_font_font_hud36, "0");
                objects.ui_lbl_spd_unit = mk_lbl(d, 65, 82, &ui_font_ui_font_font_hu_dsmall, C_DIM, "km/h");
                objects.ui_lbl_road_speed = mk_val(d, -15, 70, 100, &ui_font_ui_font_font_hud36, "0");
                lv_obj_add_flag(objects.ui_lbl_road_speed, LV_OBJ_FLAG_HIDDEN);

                objects.ui_lbl_parking = mk_lbl(d, 20, 100, &ui_font_ui_font_font_hu_dsmall, C_RED, "P BRAKE");
                lv_obj_add_flag(objects.ui_lbl_parking, LV_OBJ_FLAG_HIDDEN);

                /* ===== LEFT BOTTOM: Fuel bar ===== */
                {
                    lv_obj_t *o = lv_bar_create(d);
                    objects.ui_bar_fuel = o;
                    lv_obj_set_pos(o, -240, 20);
                    lv_obj_set_size(o, 100, 14);
                    lv_bar_set_value(o, 50, LV_ANIM_OFF);
                    lv_obj_clear_flag(o, BAR_CLR);
                    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
                    lv_obj_set_style_radius(o, 0, PM);
                    lv_obj_set_style_bg_color(o, lv_color_hex(C_BARBG), PM);
                    lv_obj_set_style_bg_opa(o, 255, PM);
                    lv_obj_set_style_bg_img_src(o, &img_ui_img_fuel2_png, PM);
                    lv_obj_set_style_radius(o, 0, PI);
                    lv_obj_set_style_bg_color(o, lv_color_hex(C_CYAN), PI);
                    lv_obj_set_style_bg_opa(o, 255, PI);
                    lv_obj_set_style_bg_img_src(o, &img_ui_img_fuel1_png, PI);
                }
                objects.ui_lbl_fuel_icon = mk_lbl(d, -310, 20, &ui_font_ui_font_font_hu_dsmall, C_DIM, "FUEL");

                /* ===== LEFT BOTTOM: Mode selector ===== */
                objects.ui_lbl_mode            = mk_lbl(d, -290, 48, &ui_font_ui_font_font_hu_dsmall, C_DIM, "MODE");
                objects.ui_lbl_mode_road       = mk_lbl(d, -280, 64, &ui_font_ui_font_font_hu_dsmall, C_WHT, "ROAD");
                objects.ui_lbl_mode_sport      = mk_lbl(d, -280, 80, &ui_font_ui_font_font_hu_dsmall, C_DIM, "SPORT");
                objects.ui_lbl_mode_sport_plus = mk_lbl(d, -280, 96, &ui_font_ui_font_font_hu_dsmall, C_DIM, "SPORT+");
                {
                    lv_obj_t *o = lv_obj_create(d);
                    objects.ui_mode_border = o;
                    lv_obj_set_pos(o, -280, 60);
                    lv_obj_set_size(o, 60, 18);
                    lv_obj_clear_flag(o, CTN_CLR);
                    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
                    lv_obj_set_style_bg_opa(o, 0, PM);
                    lv_obj_set_style_border_color(o, lv_color_hex(C_CYAN), PM);
                    lv_obj_set_style_border_width(o, 1, PM);
                    lv_obj_set_style_radius(o, 2, PM);
                    lv_obj_set_style_pad_top(o, 0, PM);
                    lv_obj_set_style_pad_bottom(o, 0, PM);
                    lv_obj_set_style_pad_left(o, 0, PM);
                    lv_obj_set_style_pad_right(o, 0, PM);
                }

                /* ===== RIGHT BOTTOM: ODO / Trip / Range ===== */
                objects.ui_lbl_odo_name   = mk_name(d, 195, 25, "ODO");
                objects.ui_lbl_odo        = mk_val(d, 310, 25, 75, &ui_font_ui_font_font_hu_dmid, "0");
                objects.ui_lbl_trip_name  = mk_name(d, 195, 48, "TRIP");
                objects.ui_lbl_trip       = mk_val(d, 310, 48, 75, &ui_font_ui_font_font_hu_dmid, "0.0");
                objects.ui_lbl_range_name = mk_name(d, 195, 71, "RANGE");
                objects.ui_lbl_range      = mk_val(d, 310, 71, 75, &ui_font_ui_font_font_hu_dmid, "---");

                /* ===== BOTTOM CENTER: branding + deco line ===== */
                objects.ui_lbl_prodrive = mk_lbl(d, 20, 118, &ui_font_ui_font_font_hu_dsmall, C_CYAN, "prodrive");
                {
                    lv_obj_t *o = lv_obj_create(d);
                    objects.ui_line_deco = o;
                    lv_obj_set_pos(o, 20, 58);
                    lv_obj_set_size(o, 220, 1);
                    lv_obj_clear_flag(o, CTN_CLR);
                    lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
                    lv_obj_set_style_bg_color(o, lv_color_hex(C_CYAN), PM);
                    lv_obj_set_style_bg_opa(o, 80, PM);
                    lv_obj_set_style_border_width(o, 0, PM);
                    lv_obj_set_style_radius(o, 0, PM);
                    lv_obj_set_style_pad_top(o, 0, PM);
                    lv_obj_set_style_pad_bottom(o, 0, PM);
                    lv_obj_set_style_pad_left(o, 0, PM);
                    lv_obj_set_style_pad_right(o, 0, PM);
                }
            }
        }

        /* ======== ContainerOpening ======== */
        {
            lv_obj_t *obj = lv_obj_create(scr);
            objects.ui_container_opening = obj;
            lv_obj_set_pos(obj, 18, 0);
            lv_obj_set_size(obj, 765, 256);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, CTN_CLR);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, PM);
            lv_obj_set_style_border_width(obj, 0, PM);
            lv_obj_set_style_radius(obj, 0, PM);
            lv_obj_set_style_pad_top(obj, 0, PM);
            lv_obj_set_style_pad_bottom(obj, 0, PM);
            lv_obj_set_style_pad_left(obj, 0, PM);
            lv_obj_set_style_pad_right(obj, 0, PM);
            lv_obj_set_style_bg_color(obj, lv_color_hex(C_BLK), PM);
            lv_obj_set_style_bg_opa(obj, 255, PM);
            {
                lv_obj_t *o = lv_img_create(obj);
                objects.ui_img_opening = o;
                lv_obj_set_pos(o, 0, 0);
                lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                lv_img_set_src(o, &img_ui_img_fmw_op0_png);
                lv_img_set_pivot(o, 0, 0);
                lv_obj_clear_flag(o, IMG_CLR);
                lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
            }
        }

        /* ======== ContainerTelltale ======== */
        {
            lv_obj_t *obj = lv_obj_create(scr);
            objects.ui_container_telltale = obj;
            lv_obj_set_pos(obj, -382, 0);
            lv_obj_set_size(obj, 35, 256);
            lv_obj_clear_flag(obj, CTN_CLR);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, PM);
            lv_obj_set_style_border_width(obj, 0, PM);
            lv_obj_set_style_radius(obj, 0, PM);
            lv_obj_set_style_pad_top(obj, 0, PM);
            lv_obj_set_style_pad_bottom(obj, 0, PM);
            lv_obj_set_style_pad_left(obj, 0, PM);
            lv_obj_set_style_pad_right(obj, 0, PM);
            lv_obj_set_style_bg_opa(obj, 0, PM);
            {
                lv_obj_t *t = obj;
                objects.ui_img_warn_master     = mk_warn(t, -106, &img_ui_img_ws_masterwarning_png);
                objects.ui_img_warn_oil_press  = mk_warn(t, -76,  &img_ui_img_ws_oilpresswarning_png);
                objects.ui_img_warn_water_cold = mk_warn(t, -46,  &img_ui_img_ws_watarcool_png);
                objects.ui_img_warn_water_hot  = mk_warn(t, -46,  &img_ui_img_ws_waterwarning_png);
                objects.ui_img_warn_exhaust    = mk_warn(t, -16,  &img_ui_img_ws_exhaustwarning_png);
                objects.ui_img_warn_battery    = mk_warn(t,  14,  &img_ui_img_ws_batterywarning_png);
                objects.ui_img_warn_brake      = mk_warn(t,  44,  &img_ui_img_ws_breakwarning_png);
                objects.ui_image_warn_belt     = mk_warn(t,  74,  &img_ui_img_ws_beltwarning_png);
                objects.ui_img_warn_fuel       = mk_warn(t,  104, &img_ui_img_ws_fuelcheck_png);
            }
        }

        /* ======== NotifyBox ======== */
        {
            lv_obj_t *obj = lv_obj_create(scr);
            objects.ui_notify_box = obj;
            lv_obj_set_pos(obj, 0, -100);
            lv_obj_set_size(obj, 696, 50);
            lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
            lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICK_FOCUSABLE|LV_OBJ_FLAG_SCROLLABLE|LV_OBJ_FLAG_SCROLL_CHAIN_HOR|LV_OBJ_FLAG_SCROLL_CHAIN_VER|LV_OBJ_FLAG_SCROLL_ELASTIC|LV_OBJ_FLAG_SCROLL_MOMENTUM|LV_OBJ_FLAG_SCROLL_WITH_ARROW);
            lv_obj_set_style_align(obj, LV_ALIGN_CENTER, PM);
            lv_obj_set_style_border_width(obj, 0, PM);
            lv_obj_set_style_radius(obj, 0, PM);
            lv_obj_set_style_pad_top(obj, 4, PM);
            lv_obj_set_style_pad_bottom(obj, 4, PM);
            lv_obj_set_style_pad_left(obj, 4, PM);
            lv_obj_set_style_pad_right(obj, 4, PM);
            lv_obj_set_style_bg_color(obj, lv_color_hex(C_RED), PM);
            lv_obj_set_style_bg_opa(obj, 255, PM);
            {
                lv_obj_t *o = lv_label_create(obj);
                objects.ui_notify_label = o;
                lv_obj_set_pos(o, 0, 0);
                lv_obj_set_size(o, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                lv_obj_clear_flag(o, LBL_CLR);
                lv_obj_set_style_align(o, LV_ALIGN_CENTER, PM);
                lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, PM);
                lv_obj_set_style_text_font(o, &ui_font_ui_font_font_largr, PM);
                lv_obj_set_style_text_color(o, lv_color_hex(C_WHT), PM);
                lv_obj_set_style_text_opa(o, 255, PM);
                lv_label_set_text(o, "");
            }
        }
    }

    tick_screen_screen1();
}

void tick_screen_screen1() {
}

void create_screens() {
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE),
                                               lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    create_screen_screen1();
}

typedef void (*tick_screen_func_t)();
tick_screen_func_t tick_screen_funcs[] = {
    tick_screen_screen1,
};
void tick_screen(int screen_index) {
    tick_screen_funcs[screen_index]();
}
void tick_screen_by_id(enum ScreensEnum screenId) {
    tick_screen_funcs[screenId - 1]();
}

/* --- Font table --- */
ext_font_desc_t fonts[] = {
    { "ui_font_Font1", &ui_font_ui_font_font1 },
    { "ui_font_FontHUDmid", &ui_font_ui_font_font_hu_dmid },
    { "ui_font_FontHUDsmall", &ui_font_ui_font_font_hu_dsmall },
    { "ui_font_FontLARGR", &ui_font_ui_font_font_largr },
    { "ui_font_FontHUD36", &ui_font_ui_font_font_hud36 },
    { "ui_font_FontHUD30", &ui_font_ui_font_font_hud30 },
#if LV_FONT_MONTSERRAT_8
    { "MONTSERRAT_8", &lv_font_montserrat_8 },
#endif
#if LV_FONT_MONTSERRAT_10
    { "MONTSERRAT_10", &lv_font_montserrat_10 },
#endif
#if LV_FONT_MONTSERRAT_12
    { "MONTSERRAT_12", &lv_font_montserrat_12 },
#endif
#if LV_FONT_MONTSERRAT_14
    { "MONTSERRAT_14", &lv_font_montserrat_14 },
#endif
#if LV_FONT_MONTSERRAT_16
    { "MONTSERRAT_16", &lv_font_montserrat_16 },
#endif
#if LV_FONT_MONTSERRAT_18
    { "MONTSERRAT_18", &lv_font_montserrat_18 },
#endif
#if LV_FONT_MONTSERRAT_20
    { "MONTSERRAT_20", &lv_font_montserrat_20 },
#endif
#if LV_FONT_MONTSERRAT_22
    { "MONTSERRAT_22", &lv_font_montserrat_22 },
#endif
#if LV_FONT_MONTSERRAT_24
    { "MONTSERRAT_24", &lv_font_montserrat_24 },
#endif
#if LV_FONT_MONTSERRAT_26
    { "MONTSERRAT_26", &lv_font_montserrat_26 },
#endif
#if LV_FONT_MONTSERRAT_28
    { "MONTSERRAT_28", &lv_font_montserrat_28 },
#endif
#if LV_FONT_MONTSERRAT_30
    { "MONTSERRAT_30", &lv_font_montserrat_30 },
#endif
#if LV_FONT_MONTSERRAT_32
    { "MONTSERRAT_32", &lv_font_montserrat_32 },
#endif
#if LV_FONT_MONTSERRAT_34
    { "MONTSERRAT_34", &lv_font_montserrat_34 },
#endif
#if LV_FONT_MONTSERRAT_36
    { "MONTSERRAT_36", &lv_font_montserrat_36 },
#endif
#if LV_FONT_MONTSERRAT_38
    { "MONTSERRAT_38", &lv_font_montserrat_38 },
#endif
#if LV_FONT_MONTSERRAT_40
    { "MONTSERRAT_40", &lv_font_montserrat_40 },
#endif
#if LV_FONT_MONTSERRAT_42
    { "MONTSERRAT_42", &lv_font_montserrat_42 },
#endif
#if LV_FONT_MONTSERRAT_44
    { "MONTSERRAT_44", &lv_font_montserrat_44 },
#endif
#if LV_FONT_MONTSERRAT_46
    { "MONTSERRAT_46", &lv_font_montserrat_46 },
#endif
#if LV_FONT_MONTSERRAT_48
    { "MONTSERRAT_48", &lv_font_montserrat_48 },
#endif
};
