/*
 * eez_compat.h
 *
 * Compatibility macros: maps old SquareLine Studio naming convention
 * (extern lv_obj_t *ui_WidgetName) to EEZ Studio naming convention
 * (objects.ui_widget_name).
 *
 * This allows ui_dashboard.c and other hand-written binding code to
 * keep using the SLS-style names without modification.
 *
 * AUTO-GENERATED MAPPING — update when widgets are added/removed in EEZ Studio.
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

/* --- Tacho / RPM cluster --- */
#define ui_ImgTacho             (objects.ui_img_tacho)
#define ui_ArcRPM               (objects.ui_arc_rpm)
#define ui_LblRPM               (objects.ui_lbl_rpm)
#define ui_LblRPMUnit           (objects.ui_lbl_rpm_unit)
#define ui_imageRPM             (objects.ui_image_rpm)
#define ui_ImagePeakRPM         (objects.ui_image_peak_rpm)
#define ui_Image2               (objects.ui_image2)

/* --- Bar gauges --- */
#define ui_BarWaterTemp         (objects.ui_bar_water_temp)
#define ui_LblWaterTemp         (objects.ui_lbl_water_temp)
#define ui_BarIAT               (objects.ui_bar_iat)
#define ui_LblIAT               (objects.ui_lbl_iat)
#define ui_BarOilTemp           (objects.ui_bar_oil_temp)
#define ui_LblOilTemp           (objects.ui_lbl_oil_temp)
#define ui_BarMAP               (objects.ui_bar_map)
#define ui_LblMAP               (objects.ui_lbl_map)
#define ui_BarOilPress          (objects.ui_bar_oil_press)
#define ui_LblOilPress          (objects.ui_lbl_oil_press)
#define ui_BarBattery           (objects.ui_bar_battery)
#define ui_LblBattery           (objects.ui_lbl_battery)

/* --- Info labels --- */
#define ui_LblAFR               (objects.ui_lbl_afr)
#define ui_LblTrip              (objects.ui_lbl_trip)
#define ui_LblODO               (objects.ui_lbl_odo)
#define ui_LblTIME              (objects.ui_lbl_time)
#define ui_LblGEAR              (objects.ui_lbl_gear)
#define ui_LblSPD               (objects.ui_lbl_spd)
#define ui_LblTripName          (objects.ui_lbl_trip_name)
#define ui_LblTripUnit          (objects.ui_lbl_trip_unit)
#define ui_LblODOName           (objects.ui_lbl_odo_name)
#define ui_LblODOUnit           (objects.ui_lbl_odo_unit)
#define ui_LblSPDUnit           (objects.ui_lbl_spd_unit)

/* --- Fuel gauge --- */
#define ui_BarFUEL              (objects.ui_bar_fuel)
#define ui_LblEmpty             (objects.ui_lbl_empty)
#define ui_LblFull              (objects.ui_lbl_full)

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
