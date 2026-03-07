/*
 * ui_dashboard.c
 *
 * Data binding layer: connects g_CALC_data / g_CALC_data_sm to the
 * SLS-generated LVGL widgets in ui/screens/ui_Screen1.h.
 *
 * Needle angle mapping (0.1° units, clockwise from 3-o'clock):
 *   Arc bg_angles(95, 0) → 265° sweep starting at 95° (≈5-o'clock) going CW to 0° (3-o'clock)
 *   RPM=0    → angle = 950  (95.0°)
 *   RPM=9000 → angle = 3600 (360.0°)  but arc ends at 0°=360° so needle aligns with arc end
 *
 * Warning thresholds:
 *   Water temp  : ≥ 105°C → show ui_ImgWarnWater
 *   IAT         : ≥  60°C → (no dedicated icon, future use)
 *   Oil temp    : ≥ 130°C → (no dedicated icon, future use)
 *   Oil press   : ≤  50   → show ui_ImgWarnOilPress
 *   Battery     : ≤ 11.5V (115 in 0.1V) → show ui_ImgWarnBattery
 *   Exhaust     : ≥ 800°C (AD3 raw) → show ui_ImgWarnExhaust
 *   Brake       : AD2 high → show ui_ImgWarnBrake  (active-low: brake fluid low)
 *   Belt        : AD4 high → show ui_Image11 (seat belt)
 *   Master      : any of the above → show ui_ImgWarnMaster
 *   Fuel        : fuel_per ≤ 10% → show ui_ImgWarnFuel
 */

#include "platform.h"
#include "lvgl/lvgl.h"
#include "ui/ui.h"
#include "ui/screens/ui_Screen1.h"
#include "dataregister.h"
#include "ui_dashboard.h"

/* fuel_per is defined in dataregister.c but not declared in dataregister.h */
extern float fuel_per;

/* Notification overlay widgets are SLS-generated: ui_NotifyBox / ui_NotifyLabel */

/* --- MAP chart series ----------------------------------------------------- */
static lv_chart_series_t *s_ser_map     = NULL;
static uint32_t           s_chart_last_ms = 0;
#define CHART_UPDATE_MS  50    /* push new sample every 50ms */
#define CHART_POINTS     200   /* 200 × 50ms = 10 seconds of history */
#define CHART_MAP_MIN    0
#define CHART_MAP_MAX    300   /* kPa */

/* --- Bar ranges ----------------------------------------------------------- */
#define BAR_WATER_MIN    0
#define BAR_WATER_MAX    130   /* °C */
#define BAR_IAT_MIN      0
#define BAR_IAT_MAX      70    /* °C */
#define BAR_OILTEMP_MIN  0
#define BAR_OILTEMP_MAX  160   /* °C */
#define BAR_MAP_MIN      0
#define BAR_MAP_MAX      300   /* kPa */
#define BAR_OILPRESS_MIN 0
#define BAR_OILPRESS_MAX 500
#define BAR_BATT_MIN     100   /* 0.1V → 10.0V */
#define BAR_BATT_MAX     160   /* 0.1V → 16.0V */

/* --- Warning thresholds --------------------------------------------------- */
#define WARN_WATER_TEMP_HI    105   /* °C */
#define WARN_OIL_PRESS_LO     50
#define WARN_BATT_LO          115   /* 0.1V = 11.5V */
#define WARN_FUEL_LO          10    /* % */
#define WARN_BRAKE_ADC_HI     2000  /* raw ADC value (brake fluid low) */
#define WARN_EXHAUST_ADC_HI   3000  /* raw ADC value for exhaust temp warn */
#define WARN_BELT_ADC_HI      2000  /* raw ADC value for seat belt */

/* --- Needle angle formula ------------------------------------------------- */
/* angle in 0.1° units. RPM 0→9000 maps to 95°→360° (265° sweep). */
static inline int32_t rpm_to_angle(uint32_t rpm)
{
    if (rpm > 9000u) rpm = 9000u;
    return 950 + (int32_t)rpm * 2650 / 9000;
}

/* --- Helper: show/hide widget --------------------------------------------- */
static inline void set_visible(lv_obj_t *obj, bool visible)
{
    if (visible) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_create(void)
{
    /* Initialize SLS-generated screen */
    ui_init();

    /* Set bar ranges (SLS sets initial value=25 with default range 0-100) */
    lv_bar_set_range(ui_BarWaterTemp, BAR_WATER_MIN,    BAR_WATER_MAX);
    lv_bar_set_range(ui_BarIAT,       BAR_IAT_MIN,      BAR_IAT_MAX);
    lv_bar_set_range(ui_BarOilTemp,   BAR_OILTEMP_MIN,  BAR_OILTEMP_MAX);
    lv_bar_set_range(ui_BarMAP,       BAR_MAP_MIN,      BAR_MAP_MAX);
    lv_bar_set_range(ui_BarOilPress,  BAR_OILPRESS_MIN, BAR_OILPRESS_MAX);
    lv_bar_set_range(ui_BarBattery,   BAR_BATT_MIN,     BAR_BATT_MAX);

    /* Hide all warning icons at startup */
    set_visible(ui_ImgWarnMaster,   false);
    set_visible(ui_ImgWarnOilPress, false);
    set_visible(ui_ImgWarnWater,    false);
    set_visible(ui_ImgWarnExhaust,  false);
    set_visible(ui_ImgWarnBattery,  false);
    set_visible(ui_ImgWarnBrake,    false);
    set_visible(ui_Image11,         false);  /* belt warning */
    set_visible(ui_ImgWarnFuel,     false);
    set_visible(ui_Image9,          false);  /* belt warning (off-screen in SLS) */

    /* Initial needle position at 0 rpm */
    lv_img_set_angle(ui_imageRPM, (int16_t)rpm_to_angle(0));

    /* --- MAP chart recorder ------------------------------------------------ */
    lv_chart_set_update_mode(ui_MAPChart, LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_set_point_count(ui_MAPChart, CHART_POINTS);
    lv_chart_set_range(ui_MAPChart, LV_CHART_AXIS_PRIMARY_Y, CHART_MAP_MIN, CHART_MAP_MAX);
    s_ser_map = lv_chart_add_series(ui_MAPChart, lv_color_hex(0x00CFFF),
                                    LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_all_value(ui_MAPChart, s_ser_map, 0);
    s_chart_last_ms = lv_tick_get();
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_set_notify(const char *msg, uint32_t bg_color)
{
    if (msg == NULL || msg[0] == '\0') {
        ui_dashboard_clear_notify();
        return;
    }
    lv_obj_set_style_bg_color(ui_NotifyBox, lv_color_hex(bg_color), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NotifyBox, LV_OPA_COVER, LV_PART_MAIN);
    lv_label_set_text(ui_NotifyLabel, msg);
    lv_obj_clear_flag(ui_NotifyBox, LV_OBJ_FLAG_HIDDEN);
}

void ui_dashboard_clear_notify(void)
{
    lv_obj_add_flag(ui_NotifyBox, LV_OBJ_FLAG_HIDDEN);
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_update(void)
{
    /* Snapshot volatile data to locals for consistent frame */
    float rev      = g_CALC_data.rev;
    float num1     = g_CALC_data.num1;   /* water temp °C */
    float num2     = g_CALC_data.num2;   /* IAT °C */
    float num3     = g_CALC_data.num3;   /* oil temp °C */
    float num4     = g_CALC_data_sm.num4; /* MAP kPa (50ms smoothed) */
    float num5     = g_CALC_data_sm.num5; /* oil press (50ms smoothed) */
    float bt       = g_CALC_data.bt;     /* battery V (float, e.g. 12.5) */
    float ad2      = g_CALC_data.AD2;    /* brake ADC */
    float ad3      = g_CALC_data.AD3;    /* exhaust ADC */
    float ad4      = g_CALC_data.AD4;    /* belt ADC */

    /* --- Tachometer needle + arc ----------------------------------------- */
    uint32_t rpm = (rev > 0.0f) ? (uint32_t)rev : 0u;
    lv_img_set_angle(ui_imageRPM, (int16_t)rpm_to_angle(rpm));
    lv_arc_set_value(ui_ArcRPM, (int32_t)rpm);

    /* --- RPM label --------------------------------------------------------- */
    lv_label_set_text_fmt(ui_LblRPM, "%5u", rpm);

    /* --- Sensor labels ----------------------------------------------------- */
    lv_label_set_text_fmt(ui_LblWaterTemp, "%3d", (int)num1);
    lv_label_set_text_fmt(ui_LblIAT,       "%3d", (int)num2);
    lv_label_set_text_fmt(ui_LblOilTemp,   "%3d", (int)num3);
    lv_label_set_text_fmt(ui_LblMAP,       "%3d", (int)num4);
    lv_label_set_text_fmt(ui_LblOilPress,  "%3d", (int)num5);
    /* Battery: convert float V to 0.1V integer for display (e.g. 12.5 → "12.5") */
    lv_label_set_text_fmt(ui_LblBattery,   "%4.1f", (double)bt);

    /* --- Bar indicators ---------------------------------------------------- */
    lv_bar_set_value(ui_BarWaterTemp, (int32_t)num1, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarIAT,       (int32_t)num2, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarOilTemp,   (int32_t)num3, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarMAP,       (int32_t)num4, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarOilPress,  (int32_t)num5, LV_ANIM_OFF);
    /* battery bar: use 0.1V integer (bt * 10) */
    lv_bar_set_value(ui_BarBattery,   (int32_t)(bt * 10.0f), LV_ANIM_OFF);

    /* --- Warning icons ----------------------------------------------------- */
    bool warn_water    = (num1 >= WARN_WATER_TEMP_HI);
    bool warn_oilpress = (num5 <= WARN_OIL_PRESS_LO);
    bool warn_batt     = ((int32_t)(bt * 10.0f) <= WARN_BATT_LO);
    bool warn_exhaust  = (ad3 >= WARN_EXHAUST_ADC_HI);
    bool warn_brake    = (ad2 >= WARN_BRAKE_ADC_HI);
    bool warn_belt     = (ad4 >= WARN_BELT_ADC_HI);
    bool warn_fuel     = (fuel_per <= (float)WARN_FUEL_LO);
    bool warn_master   = warn_water || warn_oilpress || warn_batt ||
                         warn_exhaust || warn_brake || warn_fuel;

    set_visible(ui_ImgWarnMaster,   warn_master);
    set_visible(ui_ImgWarnWater,    warn_water);
    set_visible(ui_ImgWarnOilPress, warn_oilpress);
    set_visible(ui_ImgWarnBattery,  warn_batt);
    set_visible(ui_ImgWarnExhaust,  warn_exhaust);
    set_visible(ui_ImgWarnBrake,    warn_brake);
    set_visible(ui_Image11,         warn_belt);
    set_visible(ui_ImgWarnFuel,     warn_fuel);

    /* --- MAP chart (50ms push) --------------------------------------------- */
    if (s_ser_map != NULL) {
        uint32_t now = lv_tick_get();
        if ((uint32_t)(now - s_chart_last_ms) >= CHART_UPDATE_MS) {
            s_chart_last_ms = now;
            lv_chart_set_next_value(ui_MAPChart, s_ser_map, (lv_coord_t)num4);
        }
    }
}
