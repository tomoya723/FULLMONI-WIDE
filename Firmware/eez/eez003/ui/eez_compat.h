/*
 * eez_compat.h — eez003 (Prodrive)
 *
 * Compatibility macros: maps PascalCase widget names used in
 * ui_dashboard.c to objects.member_name from EEZ-generated screens.h.
 */
#ifndef EEZ_COMPAT_H
#define EEZ_COMPAT_H

#include "screens.h"

/* --- Screen --- */
#define ui_Screen1              (objects.screen1)

/* --- Containers --- */
#define ui_ContainerDashboard   (objects.ui_container_dashboard)
#define ui_ContainerOpening     (objects.ui_container_opening)
#define ui_ContainerTelltale    (objects.ui_container_telltale)
#define ui_ContainerTacho       (objects.ui_container_tacho)

/* --- Fan tachometer labels --- */
#define ui_LblBoost             (objects.ui_lbl_boost)
#define ui_LblBoostVal          (objects.ui_lbl_boost_val)
#define ui_LblEngineRPM         (objects.ui_lbl_engine_rpm)

/* --- Left column bar gauges --- */
#define ui_BarWaterTemp         (objects.ui_bar_water_temp)
#define ui_LblWaterTemp         (objects.ui_lbl_water_temp)
#define ui_BarOilTemp           (objects.ui_bar_oil_temp)
#define ui_LblOilTemp           (objects.ui_lbl_oil_temp)
#define ui_BarChargeTemp        (objects.ui_bar_charge_temp)
#define ui_LblChargeTemp        (objects.ui_lbl_charge_temp)

/* --- Right column bar gauges --- */
#define ui_BarOilPress          (objects.ui_bar_oil_press)
#define ui_LblOilPress          (objects.ui_lbl_oil_press)
#define ui_BarFuelPress         (objects.ui_bar_fuel_press)
#define ui_LblFuelPress         (objects.ui_lbl_fuel_press)
#define ui_BarBattery           (objects.ui_bar_battery)
#define ui_LblBattery           (objects.ui_lbl_battery)

/* --- Center info --- */
#define ui_LblGEAR              (objects.ui_lbl_gear)
#define ui_LblALS               (objects.ui_lbl_als)
#define ui_LblSPD               (objects.ui_lbl_spd)
#define ui_LblSPDUnit           (objects.ui_lbl_spd_unit)
#define ui_LblRoadSpeed         (objects.ui_lbl_road_speed)
#define ui_LblParking           (objects.ui_lbl_parking)

/* --- Fuel gauge + Mode selector --- */
#define ui_BarFUEL              (objects.ui_bar_fuel)
#define ui_LblFuelIcon          (objects.ui_lbl_fuel_icon)
#define ui_LblMode              (objects.ui_lbl_mode)
#define ui_LblModeRoad          (objects.ui_lbl_mode_road)
#define ui_LblModeSport         (objects.ui_lbl_mode_sport)
#define ui_LblModeSportPlus     (objects.ui_lbl_mode_sport_plus)
#define ui_ModeBorder           (objects.ui_mode_border)

/* --- ODO / Trip / Range --- */
#define ui_LblODO               (objects.ui_lbl_odo)
#define ui_LblODOName           (objects.ui_lbl_odo_name)
#define ui_LblTrip              (objects.ui_lbl_trip)
#define ui_LblTripName          (objects.ui_lbl_trip_name)
#define ui_LblRange             (objects.ui_lbl_range)
#define ui_LblRangeName         (objects.ui_lbl_range_name)

/* --- Bottom branding --- */
#define ui_LblProdrive          (objects.ui_lbl_prodrive)

/* --- Opening screen --- */
#define ui_ImgOpening           (objects.ui_img_opening)

/* --- Warning / telltale icons --- */
#define ui_ImgWarnMaster        (objects.ui_img_warn_master)
#define ui_ImgWarnOilPress      (objects.ui_img_warn_oil_press)
#define ui_ImgWarnWaterCold     (objects.ui_img_warn_water_cold)
#define ui_ImgWarnWaterHot      (objects.ui_img_warn_water_hot)
#define ui_ImgWarnExhaust       (objects.ui_img_warn_exhaust)
#define ui_ImgWarnBattery       (objects.ui_img_warn_battery)
#define ui_ImgWarnBrake         (objects.ui_img_warn_brake)
#define ui_ImageWarnBelt        (objects.ui_image_warn_belt)
#define ui_ImgWarnFuel          (objects.ui_img_warn_fuel)

/* --- Notification overlay --- */
#define ui_NotifyBox            (objects.ui_notify_box)
#define ui_NotifyLabel          (objects.ui_notify_label)

#endif /* EEZ_COMPAT_H */
