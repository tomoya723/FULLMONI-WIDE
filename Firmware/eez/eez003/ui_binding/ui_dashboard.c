/*
 * ui_dashboard.c — eez003 (Prodrive)
 *
 * Data binding layer: connects g_CALC_data / g_CALC_data_sm to the
 * Prodrive-style LVGL dashboard with fan-shaped tachometer.
 *
 * Sensor mapping (eez003 Prodrive design):
 *   Left column:  WATER TEMP (num1), OIL TEMP (num3), CHARGE TEMP (num2)
 *   Right column: OIL PRESSURE (num5), FUEL PRESSURE (num4), BATTERY (bt)
 *   Center:       RPM (rev) → fan tachometer, BOOST (num4-101.3)/100 bar,
 *                 GEAR (gear_pos), SPEED (sp), ALS, Parking brake (AD2)
 *
 * Warning thresholds (same as eez002):
 *   Water temp  : >= 100°C → hot, <= 60°C → cold
 *   Oil press   : <= 50   → show ui_ImgWarnOilPress
 *   Battery     : <= 11.5V → show ui_ImgWarnBattery
 *   Brake       : AD2 <= 100 (active-low) → show ui_ImgWarnBrake
 *   Exhaust     : AD3 <= 100 (active-low) → show ui_ImgWarnExhaust
 *   Belt        : AD4 <= 150 (active-low) → show ui_ImageWarnBelt
 *   Fuel        : fuel_per < 5% → show / > 10% → hide (hysteresis)
 */

#include "platform.h"
#include "lvgl/lvgl.h"
#include "ui/screens.h"
#include "ui/ui.h"
#include "ui/eez_compat.h"
#include "dataregister.h"
#include "ui_dashboard.h"
#include "fan_tacho.h"

/* fuel_per is defined in dataregister.c but not declared in dataregister.h */
extern float fuel_per;
/* gear_pos is defined in dataregister.c but not declared in dataregister.h */
extern unsigned int gear_pos;

/* --- Bar ranges ----------------------------------------------------------- */
#define BAR_WATER_MIN       0
#define BAR_WATER_MAX       130    /* deg C */
#define BAR_OILTEMP_MIN     0
#define BAR_OILTEMP_MAX     160    /* deg C */
#define BAR_CHARGETEMP_MIN  0
#define BAR_CHARGETEMP_MAX  80     /* deg C (intercooler / charge air) */
#define BAR_OILPRESS_MIN    0
#define BAR_OILPRESS_MAX    500
#define BAR_FUELPRESS_MIN   0
#define BAR_FUELPRESS_MAX   600    /* kPa */
#define BAR_BATT_MIN        100    /* 0.1V -> 10.0V */
#define BAR_BATT_MAX        160    /* 0.1V -> 16.0V */
#define BAR_FUEL_MIN        0      /* % */
#define BAR_FUEL_MAX        100    /* % */

/* --- Warning thresholds --------------------------------------------------- */
#define WARN_WATER_TEMP_COLD  60
#define WARN_WATER_TEMP_HOT   100
#define WARN_OIL_PRESS_LO     50
#define WARN_BATT_LO          115   /* 0.1V = 11.5V */
#define WARN_FUEL_SHOW        5
#define WARN_FUEL_HIDE        10
#define WARN_BRAKE_ADC_LO     100
#define WARN_EXHAUST_ADC_LO   100
#define WARN_BELT_ADC_LO      150

/* --- Startup telltale ----------------------------------------------------- */
#define STARTUP_TELLTALE_MS   3000u
static uint32_t s_startup_ms    = 0;
static bool     s_telltale_done = false;

/* --- Fuel warning hysteresis state ---------------------------------------- */
static bool s_warn_fuel = false;

/* --- Helper: show/hide widget --------------------------------------------- */
static inline void set_visible(lv_obj_t *obj, bool visible)
{
    if (visible) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

/* --- Opening -> Dashboard fade animation ---------------------------------- */
#define FADE_DURATION_MS  500u

static void fade_opa_cb(void *obj, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)v, LV_PART_MAIN);
}

static void opening_fade_done_cb(lv_anim_t *a)
{
    lv_obj_t *obj = (lv_obj_t *)a->var;
    set_visible(obj, false);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
}

static void start_opening_fade(void)
{
    set_visible(ui_ContainerDashboard, true);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui_ContainerOpening);
    lv_anim_set_exec_cb(&a, fade_opa_cb);
    lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
    lv_anim_set_time(&a, FADE_DURATION_MS);
    lv_anim_set_ready_cb(&a, opening_fade_done_cb);
    lv_anim_start(&a);
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_create(void)
{
    ui_init();

    lv_obj_set_style_bg_color(ui_Screen1, lv_color_black(), LV_PART_MAIN);

    /* Set bar ranges */
    lv_bar_set_range(ui_BarWaterTemp,  BAR_WATER_MIN,      BAR_WATER_MAX);
    lv_bar_set_range(ui_BarOilTemp,    BAR_OILTEMP_MIN,    BAR_OILTEMP_MAX);
    lv_bar_set_range(ui_BarChargeTemp, BAR_CHARGETEMP_MIN, BAR_CHARGETEMP_MAX);
    lv_bar_set_range(ui_BarOilPress,   BAR_OILPRESS_MIN,   BAR_OILPRESS_MAX);
    lv_bar_set_range(ui_BarFuelPress,  BAR_FUELPRESS_MIN,  BAR_FUELPRESS_MAX);
    lv_bar_set_range(ui_BarBattery,    BAR_BATT_MIN,       BAR_BATT_MAX);
    lv_bar_set_range(ui_BarFUEL,       BAR_FUEL_MIN,       BAR_FUEL_MAX);

    /* Initialize fan tachometer on the tacho container */
    fan_tacho_init(ui_ContainerTacho);
    fan_tacho_set_redline(7000);
    fan_tacho_set_rpm(0);

    /* Startup telltale: show all warning icons */
    set_visible(ui_ImgWarnMaster,    true);
    set_visible(ui_ImgWarnOilPress,  true);
    set_visible(ui_ImgWarnWaterCold, true);
    set_visible(ui_ImgWarnWaterHot,  true);
    set_visible(ui_ImgWarnExhaust,   true);
    set_visible(ui_ImgWarnBattery,   true);
    set_visible(ui_ImgWarnBrake,     true);
    set_visible(ui_ImageWarnBelt,    true);
    set_visible(ui_ImgWarnFuel,      true);
    set_visible(ui_ContainerDashboard, false);
    set_visible(ui_ContainerOpening,  true);
    s_startup_ms    = lv_tick_get();
    s_telltale_done = false;

    /* ALS off by default */
    set_visible(ui_LblALS, false);
    /* Parking brake hidden by default */
    set_visible(ui_LblParking, false);
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_set_notify(const char *msg, uint32_t accent_color)
{
    if (msg == NULL || msg[0] == '\0') {
        ui_dashboard_clear_notify();
        return;
    }
    lv_obj_set_style_bg_color(ui_NotifyBox, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(ui_NotifyBox, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_color(ui_NotifyBox, lv_color_hex(accent_color), LV_PART_MAIN);
    lv_obj_set_style_border_width(ui_NotifyBox, 2, LV_PART_MAIN);
    lv_obj_set_style_text_color(ui_NotifyLabel, lv_color_hex(accent_color), LV_PART_MAIN);
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
    /* --- Startup telltale / Opening period -------------------------------- */
    if (!s_telltale_done) {
        if ((uint32_t)(lv_tick_get() - s_startup_ms) >= STARTUP_TELLTALE_MS) {
            s_telltale_done = true;
            start_opening_fade();
        } else {
            return;
        }
    }

    /* Snapshot volatile data */
    float  rev     = g_CALC_data.rev;
    float  num1    = g_CALC_data.num1;    /* water temp deg C */
    float  num2    = g_CALC_data.num2;    /* charge temp (IAT) deg C */
    float  num3    = g_CALC_data.num3;    /* oil temp deg C */
    float  num4    = g_CALC_data_sm.num4; /* fuel press / MAP kPa (smoothed) */
    float  num5    = g_CALC_data_sm.num5; /* oil press (smoothed) */
    float  bt      = g_CALC_data.bt;      /* battery V */
    float  ad2     = g_CALC_data.AD2;     /* brake ADC */
    float  ad3     = g_CALC_data.AD3;     /* exhaust ADC */
    float  ad4     = g_CALC_data.AD4;     /* belt ADC */
    float  sp      = g_CALC_data.sp;      /* speed km/h */
    double odo     = g_CALC_data.odo;
    double trip    = g_CALC_data.trip;

    /* --- Fan tachometer --------------------------------------------------- */
    uint32_t rpm = (rev > 0.0f) ? (uint32_t)rev : 0u;
    fan_tacho_set_rpm(rpm);

    /* --- RPM label -------------------------------------------------------- */
    lv_label_set_text_fmt(ui_LblEngineRPM, "%5u", rpm);

    /* --- Boost value (MAP - atmospheric, in bar with 1 decimal) ----------- */
    {
        /* num4 = MAP in kPa; boost = (MAP - 101.3) / 100.0 bar */
        int32_t boost_mbar = (int32_t)(num4 * 10.0f) - 1013; /* mbar * 10 */
        int32_t boost_abs = boost_mbar < 0 ? -boost_mbar : boost_mbar;
        if (boost_mbar < 0) {
            lv_label_set_text_fmt(ui_LblBoostVal, "-%d.%02d", (int)(boost_abs / 1000), (int)((boost_abs % 1000) / 10));
        } else {
            lv_label_set_text_fmt(ui_LblBoostVal, " %d.%02d", (int)(boost_abs / 1000), (int)((boost_abs % 1000) / 10));
        }
    }

    /* --- Sensor labels ---------------------------------------------------- */
    lv_label_set_text_fmt(ui_LblWaterTemp,  "%3d", (int)num1);
    lv_label_set_text_fmt(ui_LblOilTemp,    "%3d", (int)num3);
    lv_label_set_text_fmt(ui_LblChargeTemp, "%3d", (int)num2);
    /* Oil pressure: decimal_shift=1, value is 10x (39 -> 3.9) */
    {
        int32_t op = (int32_t)(num5 + 0.5f);
        lv_label_set_text_fmt(ui_LblOilPress, "%2d.%1d", op / 10, op % 10);
    }
    /* Fuel pressure: kPa integer */
    lv_label_set_text_fmt(ui_LblFuelPress, "%3d", (int)num4);
    /* Battery: integer arithmetic for 1 decimal */
    {
        int32_t bv = (int32_t)(bt * 10.0f + 0.5f);
        lv_label_set_text_fmt(ui_LblBattery, "%2d.%1d", bv / 10, bv % 10);
    }
    /* Speed */
    lv_label_set_text_fmt(ui_LblSPD, "%3d", (int)sp);
    /* ODO */
    lv_label_set_text_fmt(ui_LblODO, "%6d", (int32_t)odo);
    /* Trip: 1 decimal */
    {
        int32_t tv = (int32_t)(trip * 10.0 + 0.5);
        lv_label_set_text_fmt(ui_LblTrip, "%4d.%1d", tv / 10, tv % 10);
    }
    /* Gear: 0 = "N", 1-6 = number */
    if (gear_pos == 0u) {
        lv_label_set_text(ui_LblGEAR, "N");
    } else {
        lv_label_set_text_fmt(ui_LblGEAR, "%u", gear_pos);
    }

    /* --- Parking brake (AD2 active-low) ---------------------------------- */
    set_visible(ui_LblParking, (ad2 <= (float)WARN_BRAKE_ADC_LO));

    /* --- Bar indicators --------------------------------------------------- */
    lv_bar_set_value(ui_BarWaterTemp,  (int32_t)num1, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarOilTemp,    (int32_t)num3, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarChargeTemp, (int32_t)num2, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarOilPress,   (int32_t)num5, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarFuelPress,  (int32_t)num4, LV_ANIM_OFF);
    lv_bar_set_value(ui_BarBattery,    (int32_t)(bt * 10.0f), LV_ANIM_OFF);
    lv_bar_set_value(ui_BarFUEL,       (int32_t)fuel_per, LV_ANIM_OFF);

    /* --- Warning icons ---------------------------------------------------- */
    {
        bool warn_water_cold = (num1 <= (float)WARN_WATER_TEMP_COLD);
        bool warn_water_hot  = (num1 >= (float)WARN_WATER_TEMP_HOT);
        bool warn_oilpress   = (rpm > 0u) && ((int32_t)num5 <= WARN_OIL_PRESS_LO);
        bool warn_batt       = ((int32_t)(bt * 10.0f) <= WARN_BATT_LO);
        bool warn_exhaust    = (ad3 <= (float)WARN_EXHAUST_ADC_LO);
        bool warn_brake      = (ad2 <= (float)WARN_BRAKE_ADC_LO);
        bool warn_belt       = (ad4 <= (float)WARN_BELT_ADC_LO);

        if (fuel_per > (float)WARN_FUEL_HIDE)      { s_warn_fuel = false; }
        else if (fuel_per < (float)WARN_FUEL_SHOW)  { s_warn_fuel = true;  }
        bool warn_fuel   = s_warn_fuel;
        bool warn_master = warn_water_hot || warn_oilpress || warn_batt ||
                           warn_exhaust || warn_brake || warn_fuel;

        set_visible(ui_ImgWarnMaster,    warn_master);
        set_visible(ui_ImgWarnWaterCold, warn_water_cold);
        set_visible(ui_ImgWarnWaterHot,  warn_water_hot);
        set_visible(ui_ImgWarnOilPress,  warn_oilpress);
        set_visible(ui_ImgWarnBattery,   warn_batt);
        set_visible(ui_ImgWarnExhaust,   warn_exhaust);
        set_visible(ui_ImgWarnBrake,     warn_brake);
        set_visible(ui_ImageWarnBelt,    warn_belt);
        set_visible(ui_ImgWarnFuel,      warn_fuel);
    }
}
