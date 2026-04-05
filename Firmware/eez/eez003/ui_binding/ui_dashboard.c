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
 *   Exhaust     : AD3 ≤ 100 (active-low) → show ui_ImgWarnExhaust
 *   Brake       : AD2 ≤ 100 (active-low) → show ui_ImgWarnBrake
 *   Belt        : AD4 ≤ 150 (active-low, #issue8) → show ui_ImageWarnBelt
 *   Master      : any of the above (except water cold) → show ui_ImgWarnMaster
 *   Fuel        : fuel_per < 5% → show / fuel_per > 10% → hide (hysteresis)
 */

#include "platform.h"
#include "lvgl/lvgl.h"
#include "ui/ui.h"
#include "ui/screens.h"
#include "ui/eez_compat.h"
#include "dataregister.h"
#include "ui_dashboard.h"

/* fuel_per is defined in dataregister.c but not declared in dataregister.h */
extern float fuel_per;
/* gear_pos is defined in dataregister.c but not declared in dataregister.h */
extern unsigned int gear_pos;

/* Notification overlay widgets are SLS-generated: ui_NotifyBox / ui_NotifyLabel */

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
#define BAR_FUEL_MIN     0     /* % */
#define BAR_FUEL_MAX     100   /* % */

/* --- Warning thresholds --------------------------------------------------- */
#define WARN_WATER_TEMP_COLD  60    /* °C: これ以下は冷間警告（青表示） */
#define WARN_WATER_TEMP_HOT   100   /* °C: これ以上は過熱警告（通常色表示） */
#define WARN_OIL_PRESS_LO     50
#define WARN_BATT_LO          115   /* 0.1V = 11.5V */
#define WARN_FUEL_SHOW        5     /* %: これ未満で燃料警告点灯 */
#define WARN_FUEL_HIDE        10    /* %: これ超で燃料警告消灯（ヒステリシス） */
#define WARN_BRAKE_ADC_LO     100   /* raw ADC: これ以下でブレーキ液警告 (active-low) */
#define WARN_EXHAUST_ADC_LO   100   /* raw ADC: これ以下で排気温警告 (active-low) */
#define WARN_BELT_ADC_LO      150   /* raw ADC: これ以下でシートベルト警告 (active-low, #issue8) */

/* --- Startup telltale ----------------------------------------------------- */
/* 法規要件: 起動直後に全警告灯を一定時間点灯しインジケータの動作確認を行う */
#define STARTUP_TELLTALE_MS   3000u /* 全灯保持時間 (ms) */
static uint32_t s_startup_ms    = 0;
static bool     s_telltale_done = false;

/* --- Fuel warning hysteresis state ---------------------------------------- */
/* 5%未満で点灯、10%超で消灯、5〜10%の間は前状態を保持 */
static bool s_warn_fuel = false;

/* --- RPM Peak Hold state -------------------------------------------------- */
#define PEAK_HOLD_MS              500u   /* ピーク保持時間 (ms) */
#define PEAK_FALL_RPM_PER_FRAME   150u   /* 落下速度 (RPM/フレーム ≈ 30ms → ~5000 rpm/sec) */

static uint32_t s_rpm_peak          = 0;
static uint32_t s_peak_hold_start   = 0;  /* lv_tick_get() at peak freeze */
static bool     s_peak_falling      = false;

/* --- Needle angle formula ------------------------------------------------- */
/* angle in 0.1° units. RPM 0→9000 maps to 90°→360° (260° sweep). */
static inline int32_t rpm_to_angle(uint32_t rpm)
{
    if (rpm > 9000u) rpm = 9000u;
    return 900 + (int32_t)rpm * 2700 / 9000;
}

/* --- Helper: show/hide widget --------------------------------------------- */
static inline void set_visible(lv_obj_t *obj, bool visible)
{
    if (!obj) {
        return;
    }
    if (visible) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

/* --- Opening → Dashboard フェードアニメーション ---------------------------- */
#define FADE_DURATION_MS  500u  /* フェードアウト時間 (ms) */

static void fade_opa_cb(void *obj, int32_t v)
{
    if (!obj) {
        return;
    }
    lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)v, LV_PART_MAIN);
}

static void opening_fade_done_cb(lv_anim_t *a)
{
    /* フェード完了後: ContainerOpeningを非表示にしopacityをリセット */
    lv_obj_t *obj = (lv_obj_t *)a->var;
    set_visible(obj, false);
    if (!obj) {
        return;
    }
    lv_obj_set_style_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
}

static void start_opening_fade(void)
{
    /* ContainerDashboard をフェード前に表示 (ContainerOpening の背後でレンダリング) */
    set_visible(ui_ContainerDashboard, true);

    /* ContainerOpening をフェードアウト */
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
    /* Initialize SLS-generated screen */
    ui_init();

    if (!ui_Screen1) {
        return;
    }

    /* SLSのデフォルト背景色（白）を黒に上書きして白画面明滅を防止 */
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_black(), LV_PART_MAIN);

    /* Set bar ranges (SLS sets initial value=25 with default range 0-100) */
    /* Legacy bar widgets were removed in current EEZ layout. */
    if (ui_BarFUEL) {
        lv_bar_set_range(ui_BarFUEL, BAR_FUEL_MIN, BAR_FUEL_MAX);
    }

    /* 法規要件: 起動時に全警告灯を点灯（テルテール動作確認）*/
    set_visible(ui_ImgWarnMaster,    true);
    set_visible(ui_ImgWarnOilPress,  true);
    set_visible(ui_ImgWarnWaterCold, true);
    set_visible(ui_ImgWarnWaterHot,  true);
    set_visible(ui_ImgWarnExhaust,   true);
    set_visible(ui_ImgWarnBattery,   true);
    set_visible(ui_ImgWarnBrake,     true);
    set_visible(ui_ImageWarnBelt,    true);
    set_visible(ui_ImgWarnFuel,      true);
    set_visible(ui_ContainerDashboard, false); /* オープニング期間中はダッシュボードを非表示 */
    set_visible(ui_ContainerOpening,  true);  /* テルテール期間中はオープニング画面を表示 */
    s_startup_ms    = lv_tick_get();
    s_telltale_done = false;

    /* RPM needle widgets were removed from the current EEZ layout. */
    s_rpm_peak      = 0;
    s_peak_falling  = false;
    s_peak_hold_start = lv_tick_get();
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_set_notify(const char *msg, uint32_t accent_color)
{
    if (msg == NULL || msg[0] == '\0') {
        ui_dashboard_clear_notify();
        return;
    }
    if (!ui_NotifyBox || !ui_NotifyLabel) {
        return;
    }
    /* HUD style: 黒背景 + アクセントカラー（枠・文字） */
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
    if (!ui_NotifyBox) {
        return;
    }
    lv_obj_add_flag(ui_NotifyBox, LV_OBJ_FLAG_HIDDEN);
}

/* -------------------------------------------------------------------------- */
void ui_dashboard_update(void)
{
    /* --- Startup telltale / Opening period check ----------------------- */
    /* テルテール中は全ウィジェット更新をスキップ（全灯状態を維持）*/
    if (!s_telltale_done) {
        if ((uint32_t)(lv_tick_get() - s_startup_ms) >= STARTUP_TELLTALE_MS) {
            s_telltale_done = true;
            start_opening_fade(); /* フェードアウト開始 (完了後に ContainerOpening を隠す) */
        } else {
            return;
        }
    }

    /* Snapshot volatile data to locals for consistent frame */
    float  rev     = g_CALC_data.rev;
    float  num1    = g_CALC_data.num1;    /* water temp °C */
    float  num2    = g_CALC_data.num2;    /* IAT °C */
    float  num3    = g_CALC_data.num3;    /* oil temp °C */
    float  num4    = g_CALC_data_sm.num4; /* MAP kPa (50ms smoothed) */
    float  num5    = g_CALC_data_sm.num5; /* oil press (50ms smoothed) */
    float  bt      = g_CALC_data.bt;      /* battery V (float, e.g. 12.5) */
    float  ad2     = g_CALC_data.AD2;     /* brake ADC */
    float  ad3     = g_CALC_data.AD3;     /* exhaust ADC */
    float  ad4     = g_CALC_data.AD4;     /* belt ADC */
    float  af      = g_CALC_data.af;      /* air-fuel ratio */
    float  sp      = g_CALC_data.sp;      /* vehicle speed km/h */
    double odo     = g_CALC_data.odo;     /* odometer km */
    double trip    = g_CALC_data.trip;    /* trip meter km */

    /* --- Tachometer needle + arc ----------------------------------------- */
    uint32_t rpm = (rev > 0.0f) ? (uint32_t)rev : 0u;

    /* --- RPM Peak Hold ---------------------------------------------------- */
    /* RPMが上昇中: ピーク更新・保持タイマーリセット */
    /* RPMが下降中: 500ms保持後、現在値へ向けて落下アニメーション */
    {
        uint32_t now = lv_tick_get();
        if (rpm >= s_rpm_peak) {
            s_rpm_peak        = rpm;
            s_peak_hold_start = now;
            s_peak_falling    = false;
        } else {
            if (!s_peak_falling) {
                if ((uint32_t)(now - s_peak_hold_start) >= PEAK_HOLD_MS) {
                    s_peak_falling = true;
                }
            }
            if (s_peak_falling) {
                uint32_t diff = s_rpm_peak - rpm;
                if (diff > PEAK_FALL_RPM_PER_FRAME) {
                    s_rpm_peak -= PEAK_FALL_RPM_PER_FRAME;
                } else {
                    s_rpm_peak     = rpm;
                    s_peak_falling = false;
                }
            }
        }
    }

    /* --- Sensor labels ----------------------------------------------------- */
    if (ui_LblWaterTemp) {
        lv_label_set_text_fmt(ui_LblWaterTemp, "%3d", (int)num1);
    }
    if (objects.ui_lbl_charge_temp) {
        lv_label_set_text_fmt(objects.ui_lbl_charge_temp, "%3d", (int)num2);
    }
    if (ui_LblOilTemp) {
        lv_label_set_text_fmt(ui_LblOilTemp, "%3d", (int)num3);
    }
    if (objects.ui_lbl_boost_val) {
        lv_label_set_text_fmt(objects.ui_lbl_boost_val, "%3d", (int)num4);
    }
    /* 油圧: CAN変換後の内部値は decimal_shift=1 で10倍 (39→3.9) */
    {
        int32_t op = (int32_t)(num5 + 0.5f);
        if (ui_LblOilPress) {
            lv_label_set_text_fmt(ui_LblOilPress, "%2d.%1d", op / 10, op % 10);
        }
    }
    /* Battery: %f not supported by LVGL tiny_printf; use integer arithmetic */
    {
        int32_t bv = (int32_t)(bt * 10.0f + 0.5f);   /* e.g. 12.5V → 125 */
        if (ui_LblBattery) {
            lv_label_set_text_fmt(ui_LblBattery, "%2d.%1d", bv / 10, bv % 10);
        }
    }
    /* AFR: show on available range label in current layout. */
    {
        int32_t av = (int32_t)(af + 0.5f);
        if (objects.ui_lbl_range) {
            lv_label_set_text_fmt(objects.ui_lbl_range, "%2d.%1d", av / 10, av % 10);
        }
    }
    /* Speed: integer km/h */
    if (ui_LblSPD) {
        lv_label_set_text_fmt(ui_LblSPD, "%3d", (int)sp);
    }
    /* TIME label does not exist in current layout. */
    /* ODO: integer km (double → int32) */
    if (ui_LblODO) {
        lv_label_set_text_fmt(ui_LblODO, "%6d", (int32_t)odo);
    }
    /* Trip: 1 decimal km (e.g. 123.4) */
    {
        int32_t tv = (int32_t)(trip * 10.0 + 0.5);
        if (ui_LblTrip) {
            lv_label_set_text_fmt(ui_LblTrip, "%4d.%1d", tv / 10, tv % 10);
        }
    }
    /* Gear: 0 = Neutral → "N", 1-6 = gear number */
    if (ui_LblGEAR) {
        if (gear_pos == 0u) {
            lv_label_set_text(ui_LblGEAR, "N");
        } else {
            lv_label_set_text_fmt(ui_LblGEAR, "%u", gear_pos);
        }
    }

    /* --- Bar indicators ---------------------------------------------------- */
    if (ui_BarFUEL) {
        lv_bar_set_value(ui_BarFUEL, (int32_t)fuel_per, LV_ANIM_OFF);
    }

    /* --- Warning icons ----------------------------------------------------- */
    {
        bool warn_water_cold = (num1 <= (float)WARN_WATER_TEMP_COLD);
        bool warn_water_hot  = (num1 >= (float)WARN_WATER_TEMP_HOT);
        bool warn_oilpress   = (rpm > 0u) && ((int32_t)num5 <= WARN_OIL_PRESS_LO);
        bool warn_batt       = ((int32_t)(bt * 10.0f) <= WARN_BATT_LO);
        bool warn_exhaust    = (ad3 <= (float)WARN_EXHAUST_ADC_LO);   /* active-low */
        bool warn_brake      = (ad2 <= (float)WARN_BRAKE_ADC_LO);    /* active-low */
        bool warn_belt       = (ad4 <= (float)WARN_BELT_ADC_LO);     /* active-low (#issue8) */
        /* 燃料ヒステリシス: 5%未満で点灯、10%超で消灯、5〜10%は前状態保持 */
        if (fuel_per > (float)WARN_FUEL_HIDE)      { s_warn_fuel = false; }
        else if (fuel_per < (float)WARN_FUEL_SHOW) { s_warn_fuel = true;  }
        bool warn_fuel       = s_warn_fuel;
        /* 冷間警告はマスター警告に含めない（過熱のみ） */
        bool warn_master     = warn_water_hot || warn_oilpress || warn_batt ||
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

/* EEZ native variable accessors (declared in ui/vars.h) */
int32_t get_var_rpm(void)
{
    float rev = g_CALC_data.rev;
    if (rev <= 0.0f) {
        return 0;
    }
    return (int32_t)(rev + 0.5f);
}

void set_var_rpm(int32_t value)
{
    (void)value;
    /* Read-only binding in current design. */
}
