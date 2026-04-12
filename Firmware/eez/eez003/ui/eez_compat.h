/*
 * eez_compat.h  —  eez003 (Prodrive) 専用版
 *
 * Compatibility macros: maps old SquareLine Studio naming convention
 * (extern lv_obj_t *ui_WidgetName) to EEZ Studio naming convention
 * (objects.ui_widget_name).
 *
 * eez003 は水平バータコメーター等の独自レイアウトのため、eez001/eez002 に存在する
 * 一部ウィジェットが objects_t に含まれない。該当マクロは NULL にマップし、
 * 使用側で NULL チェックにより安全にスキップできるようにする。
 */
#ifndef EEZ_COMPAT_H
#define EEZ_COMPAT_H

#include "screens.h"

/* --- Screen --- */
#define ui_Screen1              (objects.screen1)

/* --- Containers --- */
#define ui_ContainerDashboard   (objects.ui_container_dashboard)
#define ui_ContainerOpening     (objects.ui_container_opening)
#define ui_ContainerTelltale    ((lv_obj_t *)NULL)  /* eez003: not present */

/* --- Tacho / RPM cluster --- */
/* eez003 は水平バー (obj0-24) で表現。針タコ系は存在しない */
#define ui_ImgTacho             ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_ArcRPM               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblRPM               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblRPMUnit           ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_imageRPM             ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_ImagePeakRPM         ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_Image2               ((lv_obj_t *)NULL)  /* eez003: not present */

/* --- Bar gauges --- */
/* eez003 は水平セグメントバー (objNN) で表現。lv_bar 系は存在しない */
#define ui_BarWaterTemp         ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblWaterTemp         (objects.ui_lbl_water_temp)
#define ui_BarIAT               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblIAT               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_BarOilTemp           ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblOilTemp           (objects.ui_lbl_oil_temp)
#define ui_BarMAP               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblMAP               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_BarOilPress          ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblOilPress          (objects.ui_lbl_oil_press)
#define ui_BarBattery           ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblBattery           (objects.ui_lbl_battery)

/* --- Info labels --- */
#define ui_LblAFR               ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblTrip              (objects.ui_lbl_trip)
#define ui_LblODO               (objects.ui_lbl_odo)
#define ui_LblTIME              ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblGEAR              (objects.ui_lbl_gear)
#define ui_LblSPD               (objects.ui_lbl_spd)
#define ui_LblTripName          ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblTripUnit          ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblODOName           ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblODOUnit           ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblSPDUnit           (objects.ui_lbl_spd_unit)

/* --- Fuel gauge --- */
#define ui_BarFUEL              (objects.ui_bar_fuel)
#define ui_LblEmpty             ((lv_obj_t *)NULL)  /* eez003: not present */
#define ui_LblFull              ((lv_obj_t *)NULL)  /* eez003: not present */

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
