/**
 * @file lv_test_screen.c
 * LVGL FPS benchmark screen for FULLMONI-WIDE (800x256, RGB565)
 *
 * Layout replicates the actual aw002 dashboard rendering load:
 *   Left  : 6 sensor bars (water temp, intake temp, oil temp, MAP, oil pressure, battery)
 *   Center: lv_meter tachometer with needle, tick marks, colored RPM zones
 *   Right : gear indicator, speed label
 *
 * Mock animation: when rpm==0 (engine off), all values auto-sweep for FPS measurement.
 * FPS is shown by LV_USE_PERF_MONITOR (lv_conf.h, bottom-right corner).
 */

#include "lv_test_screen.h"
#include "lvgl/lvgl.h"

#if LV_USE_METER

#include <stdint.h>

/* --- Left column: 6 sensor bars --- */
#define SENSOR_COUNT 6
static lv_obj_t *s_bar[SENSOR_COUNT];
static lv_obj_t *s_lbl_val[SENSOR_COUNT];

/* --- Center: lv_meter tachometer --- */
static lv_obj_t             *s_meter   = NULL;
static lv_meter_indicator_t *s_needle  = NULL;
static lv_obj_t             *s_lbl_rpm = NULL;

/* --- Right column --- */
static lv_obj_t *s_lbl_speed = NULL;

/* --- Mock sweep state (used when rpm==0, engine off) --- */
static int32_t s_mock_rpm = 0;
static int8_t  s_mock_dir = 1;
#define MOCK_RPM_STEP  67
#define MOCK_RPM_MAX   9000

/* Sensor names */
static const char *s_sensor_name[SENSOR_COUNT] = {
    "H2O", "AIR", "OIL", "MAP", "OIL", "BATT"
};

/*---------------------------------------------------------------------------
 * lv_test_screen_create
 *--------------------------------------------------------------------------*/
void lv_test_screen_create(void)
{
    lv_obj_t *scr = lv_scr_act();

    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    /* --- Left column: 6 sensor bars ---
     * Name label : x=4
     * Bar        : x=44, w=160, h=18
     * Value label: x=210
     * Row height : 40px, start_y=6 → rows at y=6,46,86,126,166,206        */
    static const lv_color_t bar_ind_color[SENSOR_COUNT] = {
        { .full = 0xF880 },  /* Water temp  : red-orange */
        { .full = 0xFC00 },  /* Intake temp : orange     */
        { .full = 0xFFE0 },  /* Oil temp    : yellow     */
        { .full = 0x07FF },  /* MAP         : cyan       */
        { .full = 0x07E0 },  /* Oil pressure: green      */
        { .full = 0x001F },  /* Battery     : blue       */
    };

    int bar_x   = 44;
    int bar_w   = 160;
    int bar_h   = 18;
    int start_y = 6;
    int row_h   = 40;

    for (int i = 0; i < SENSOR_COUNT; i++) {
        int y = start_y + i * row_h;

        lv_obj_t *lbl_name = lv_label_create(scr);
        lv_label_set_text(lbl_name, s_sensor_name[i]);
        lv_obj_set_style_text_color(lbl_name, lv_color_hex(0x888888), LV_PART_MAIN);
        lv_obj_set_pos(lbl_name, 4, y + 2);

        s_bar[i] = lv_bar_create(scr);
        lv_obj_set_size(s_bar[i], bar_w, bar_h);
        lv_obj_set_pos(s_bar[i], bar_x, y);
        lv_bar_set_range(s_bar[i], 0, 100);
        lv_bar_set_value(s_bar[i], 0, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(s_bar[i], lv_color_hex(0x333333), LV_PART_MAIN);
        lv_obj_set_style_bg_color(s_bar[i], bar_ind_color[i], LV_PART_INDICATOR);
        lv_obj_set_style_radius(s_bar[i], 2, LV_PART_MAIN);
        lv_obj_set_style_radius(s_bar[i], 2, LV_PART_INDICATOR);

        s_lbl_val[i] = lv_label_create(scr);
        lv_label_set_text(s_lbl_val[i], "  0");
        lv_obj_set_style_text_color(s_lbl_val[i], lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        lv_obj_set_pos(s_lbl_val[i], bar_x + bar_w + 4, y + 2);
    }

    /* --- Center: lv_meter tachometer (240x240) ---
     * Scale  : 0-9000 RPM, 270° sweep starting at 135° (7 o'clock position)
     * Ticks  : minor every 500 RPM (19 total), major every 1000 RPM with label
     * Zones  : blue 0-6500, yellow 6500-7500, red 7500-9000
     * Needle : red line, slightly inside the tick marks                    */
    s_meter = lv_meter_create(scr);
    lv_obj_set_size(s_meter, 240, 240);
    lv_obj_align(s_meter, LV_ALIGN_CENTER, 0, 0);

    /* Transparent background to blend with black screen */
    lv_obj_set_style_bg_opa(s_meter, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(s_meter, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(s_meter, 6, LV_PART_MAIN);

    /* Scale: 0-9000 RPM, 270° range (135° offset from top → starts at 7 o'clock) */
    lv_meter_scale_t *scale = lv_meter_add_scale(s_meter);
    lv_meter_set_scale_range(s_meter, scale, 0, 9000, 270, 135);

    /* Minor ticks: 19 lines (0, 500, 1000, … 9000) */
    lv_meter_set_scale_ticks(s_meter, scale, 19, 2, 8, lv_color_hex(0xAAAAAA));

    /* Major ticks: every 2nd minor tick (= every 1000 RPM) with label */
    lv_meter_set_scale_major_ticks(s_meter, scale, 2, 3, 16, lv_color_hex(0xFFFFFF), 8);

    /* Colored zone arcs (drawn on the outer rim) */
    lv_meter_indicator_t *arc_ind;

    /* Normal zone: 0-6500 (blue) */
    arc_ind = lv_meter_add_arc(s_meter, scale, 5, lv_color_hex(0x0055AA), 0);
    lv_meter_set_indicator_start_value(s_meter, arc_ind, 0);
    lv_meter_set_indicator_end_value(s_meter, arc_ind, 6500);

    /* Caution zone: 6500-7500 (yellow) */
    arc_ind = lv_meter_add_arc(s_meter, scale, 5, lv_color_hex(0xFFAA00), 0);
    lv_meter_set_indicator_start_value(s_meter, arc_ind, 6500);
    lv_meter_set_indicator_end_value(s_meter, arc_ind, 7500);

    /* Redline zone: 7500-9000 (red) */
    arc_ind = lv_meter_add_arc(s_meter, scale, 5, lv_color_hex(0xFF2200), 0);
    lv_meter_set_indicator_start_value(s_meter, arc_ind, 7500);
    lv_meter_set_indicator_end_value(s_meter, arc_ind, 9000);

    /* Needle: red, r_mod=-20 so it stops just inside the tick ring */
    s_needle = lv_meter_add_needle_line(s_meter, scale, 3, lv_color_hex(0xFF3300), -20);

    /* RPM value label — positioned below center to avoid needle overlap */
    s_lbl_rpm = lv_label_create(scr);
    lv_label_set_text(s_lbl_rpm, "0\nRPM");
    lv_obj_set_style_text_color(s_lbl_rpm, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(s_lbl_rpm, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_set_style_text_align(s_lbl_rpm, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_align_to(s_lbl_rpm, s_meter, LV_ALIGN_CENTER, 0, 30);

    /* --- Right column: gear + speed --- */
    lv_obj_t *lbl_gear = lv_label_create(scr);
    lv_label_set_text(lbl_gear, "N");
    lv_obj_set_style_text_color(lbl_gear, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(lbl_gear, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(lbl_gear, LV_ALIGN_RIGHT_MID, -80, -30);

    s_lbl_speed = lv_label_create(scr);
    lv_label_set_text(s_lbl_speed, "0 km/h");
    lv_obj_set_style_text_color(s_lbl_speed, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(s_lbl_speed, &lv_font_montserrat_16, LV_PART_MAIN);
    lv_obj_align(s_lbl_speed, LV_ALIGN_RIGHT_MID, -50, 10);

    /* Info label */
    lv_obj_t *lbl_info = lv_label_create(scr);
    lv_label_set_text(lbl_info, "FPS Benchmark - Phase 1");
    lv_obj_set_style_text_color(lbl_info, lv_color_hex(0x446688), LV_PART_MAIN);
    lv_obj_align(lbl_info, LV_ALIGN_TOP_MID, 0, 4);
}

/*---------------------------------------------------------------------------
 * lv_test_screen_update
 *
 * Call every main loop cycle. When rpm==0 (engine off / bench test),
 * an internal counter sweeps 0→9000→0 automatically so LVGL renders
 * continuously for meaningful FPS measurement.
 *--------------------------------------------------------------------------*/
void lv_test_screen_update(uint32_t rpm)
{
    uint32_t display_rpm = rpm;

    /* Auto-sweep when engine is off */
    if (display_rpm == 0) {
        s_mock_rpm += (int32_t)s_mock_dir * MOCK_RPM_STEP;
        if (s_mock_rpm >= MOCK_RPM_MAX) { s_mock_rpm = MOCK_RPM_MAX; s_mock_dir = -1; }
        else if (s_mock_rpm <= 0)       { s_mock_rpm = 0;            s_mock_dir =  1; }
        display_rpm = (uint32_t)s_mock_rpm;
    }

    /* Update needle */
    if (s_meter != NULL && s_needle != NULL) {
        lv_meter_set_indicator_value(s_meter, s_needle, (int32_t)display_rpm);
    }

    /* Update RPM label */
    if (s_lbl_rpm != NULL) {
        char buf[16];
        lv_snprintf(buf, sizeof(buf), "%lu\nRPM", (unsigned long)display_rpm);
        lv_label_set_text(s_lbl_rpm, buf);
    }

    /* Update 6 sensor bars — each with a different phase offset (0-100 scale).
     * Derived from RPM sweep to ensure all bars redraw every frame. */
    for (int i = 0; i < SENSOR_COUNT; i++) {
        uint32_t offset = (uint32_t)i * (MOCK_RPM_MAX / SENSOR_COUNT);
        uint32_t raw    = (display_rpm + offset) % (uint32_t)MOCK_RPM_MAX;
        int32_t  val    = (int32_t)(raw * 100u / (uint32_t)MOCK_RPM_MAX);

        if (s_bar[i] != NULL) {
            lv_bar_set_value(s_bar[i], val, LV_ANIM_OFF);
        }
        if (s_lbl_val[i] != NULL) {
            char buf[8];
            lv_snprintf(buf, sizeof(buf), "%3ld", (long)val);
            lv_label_set_text(s_lbl_val[i], buf);
        }
    }

    /* Speed label: proportional to RPM for visual interest */
    if (s_lbl_speed != NULL) {
        uint32_t spd = display_rpm / 90u;
        char buf[16];
        lv_snprintf(buf, sizeof(buf), "%lu km/h", (unsigned long)spd);
        lv_label_set_text(s_lbl_speed, buf);
    }
}

#else /* LV_USE_METER == 0 */

void lv_test_screen_create(void) { /* meter widget disabled */ }
void lv_test_screen_update(uint32_t rpm) { (void)rpm; }

#endif /* LV_USE_METER */
