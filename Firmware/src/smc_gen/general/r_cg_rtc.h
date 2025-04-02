/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : r_cg_rtc.h
* Version          : 1.0.140
* Device(s)        : R5F572NNDxFP
* Description      : General header file for RTC peripheral.
***********************************************************************************************************************/

#ifndef RTC_H
#define RTC_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/*
    RTC Control Register 1 (RCR1)
*/
/* Alarm Interrupt Enable (AIE) */
#define _00_RTC_ALARM_INT_DISABLE                   (0x00U) /* Disable alarm interrupt */
#define _01_RTC_ALARM_INT_ENABLE                    (0x01U) /* Enable alarm interrupt */
/* Carry Interrupt Enable (CIE) */
#define _00_RTC_CARRY_INT_DISABLE                   (0x00U) /* Disable carry interrupt */
#define _02_RTC_CARRY_INT_ENABLE                    (0x02U) /* Enable carry interrupt */
/* Periodic Interrupt Enable (PIE) */
#define _00_RTC_PERIOD_INT_DISABLE                  (0x00U) /* Disable periodic interrupt */
#define _04_RTC_PERIOD_INT_ENABLE                   (0x04U) /* Enable periodic interrupt */
/* RTCOUT Output Select (RTCOS) */
#define _00_RTC_RTCOUT_FREQ_1HZ                     (0x00U) /* RTCOUT outputs 1 Hz */
#define _08_RTC_RTCOUT_FREQ_64HZ                    (0x08U) /* RTCOUT outputs 64 Hz */
/* Periodic Interrupt Select (PES[3:0]) */
#define _00_RTC_PERIODIC_INT_PERIOD_DISABLE         (0x00U) /* No periodic interrupts are generated */
#define _60_RTC_PERIODIC_INT_PERIOD_1_256           (0x60U) /* 1/256 second */
#define _70_RTC_PERIODIC_INT_PERIOD_1_128           (0x70U) /* 1/128 second */
#define _80_RTC_PERIODIC_INT_PERIOD_1_64            (0x80U) /* 1/64 second. */
#define _90_RTC_PERIODIC_INT_PERIOD_1_32            (0x90U) /* 1/32 second */
#define _A0_RTC_PERIODIC_INT_PERIOD_1_16            (0xA0U) /* 1/16 second */
#define _B0_RTC_PERIODIC_INT_PERIOD_1_8             (0xB0U) /* 1/8 second */
#define _C0_RTC_PERIODIC_INT_PERIOD_1_4             (0xC0U) /* 1/4 second */
#define _D0_RTC_PERIODIC_INT_PERIOD_1_2             (0xD0U) /* 1/2 second */
#define _E0_RTC_PERIODIC_INT_PERIOD_1               (0xE0U) /* 1 second */
#define _F0_RTC_PERIODIC_INT_PERIOD_2               (0xF0U) /* 2 seconds */

/*
    RTC Control Register 2 (RCR2)
*/
/* Start and Stop */
#define _00_RTC_RCR2_START_STOP                     (0x00U) /* Stop RTC counter */
#define _01_RTC_RCR2_START_START                    (0x01U) /* Start RTC counter */
/* RTC Software Reset (RESET) */
#define _00_RTC_RCR2_RESET_READY_READONLY           (0x00U) /* Normal operation or RESET completed */
#define _02_RTC_RCR2_RESET                          (0x02U) /* During RTC software reset */
/* 30-Second Adjustment (ADJ30) */
#define _00_RTC_30SEC_ADJUSTMENT_READY_READONLY     (0x00U) /* Normal operation or 30-second adjustment completed */
#define _04_RTC_30SEC_ADJUSTMENT                    (0x04U) /* During 30-second adjustment */
/* RTCOUT Output Enable (RTCOE) */
#define _00_RTC_RTCOUT_OUTPUT_DISABLE               (0x00U) /* Disable RTCOUT output */
#define _08_RTC_RTCOUT_OUTPUT_ENABLE                (0x08U) /* Enable RTCOUT output */
/* Automatic Adjustment Enable (AADJE) */
#define _00_RTC_AUTO_ADJUSTMENT_DISABLE             (0x00U) /* Disable automatic adjustment */
#define _10_RTC_AUTO_ADJUSTMENT_ENABLE              (0x10U) /* Enable automatic adjustment */
/* Automatic Adjustment Period Select (AADJP) */
#define _00_RTC_AUTO_ADJUSTMENT_PERIOD_1MIN         (0x00U) /* Adjustment period every minute */
#define _20_RTC_AUTO_ADJUSTMENT_PERIOD_10SEC        (0x20U) /* Adjustment period every 10 seconds */
#define _00_RTC_AUTO_ADJUSTMENT_PERIOD_32SEC        (0x00U) /* Adjustment period every 32 seconds */
#define _20_RTC_AUTO_ADJUSTMENT_PERIOD_8SEC         (0x20U) /* Adjustment period every 8 seconds */
/* Hours Mode (HR24) */
#define _00_RTC_HOUR_MODE_12                        (0x00U) /* 12-hour mode */
#define _40_RTC_HOUR_MODE_24                        (0x40U) /* 24-hour mode */
/* Count Mode Select (CNTMD) */
#define _00_RTC_COUNT_MODE_CAL                      (0x00U) /* Calendar mode */
#define _80_RTC_COUNT_MODE_BIN                      (0x80U) /* Binary mode */

/*
    RTC Control Register (RCR3)
*/
/* Sub-clock Control (RTCEN) */
#define _00_RTC_SUBCLOCK_DISABLE                    (0x00U) /* Disable Sub-clock oscillator */
#define _01_RTC_SUBCLOCK_ENABLE                     (0x01U) /* Enable Sub-clock oscillator */
/* Sub-clock Oscillator Drive Capacity Control (RTCDV[1:0]) */
#define _02_RTC_SUBCLOCK_DRIVE_LOW                  (0x02U) /* Drive capacity for low CL */
#define _0C_RTC_SUBCLOCK_DRIVE_STANDARD             (0x0CU) /* Drive capacity for standard CL */

/*
    RTC Control Register 4 (RCR4)
*/
/* Count source select (RCKSEL) */
#define _00_RTC_SOURCE_SELECT_SUB                   (0x00U) /* Select sub-clock oscillator */
#define _01_RTC_SOURCE_SELECT_MAIN_FORCED           (0x01U) /* Select main clock oscillator */

/*
    Timer Error Adjustment Register (RADJ)
*/
/* Plus-Minus (PMADJ[1:0]) */
#define _40_RTC_TIMER_ERROR_ADJUST_PLUS             (0x40U) /* Addition to the prescaler */
#define _80_RTC_TIMER_ERROR_ADJUST_MINUS            (0x80U) /* Subtraction from the prescaler */

/*
    Time Capture Control Register y (RTCCRy) (y = 0 to 2)
*/
/* Time Capture Control (TCCT[1:0]) */
#define _00_RTC_TIME_CAPTURE_NO_EVENT               (0x00U) /* No event is detected */
#define _01_RTC_TIME_CAPTURE_RISE                   (0x01U) /* Rising edge is detected */
#define _02_RTC_TIME_CAPTURE_FALL                   (0x02U) /* Falling edge is detected */
#define _03_RTC_TIME_CAPTURE_BOTH                   (0x03U) /* Both edges are detected */
/* Time Capture Status (TCST) */
#define _00_RTC_TIME_CAPTURE_STATUS                 (0x00U) /* Time capture status */
/* Time Capture Noise Filter Control (TCNF[1:0]) */
#define _00_RTC_TIME_CAPTURE_NOISE_FILTER_OFF       (0x00U) /* Noise filter OFF */
#define _20_RTC_TIME_CAPTURE_NOISE_FILTER_ON        (0x20U) /* Noise filter ON (count source) */
#define _30_RTC_TIME_NOISE_FILTER_DIVIDED32_ON      (0x30U) /* Noise filter ON (count source divided by 32) */
/* Time Capture Event Input Pin Enable (TCEN) */
#define _00_RTC_TIME_CAPTURE_EVENT_DISABLE          (0x00U) /* Disable time capture event input pin */
#define _80_RTC_TIME_CAPTURE_EVENT_ENABLE           (0x80U) /* Enable time capture event input pin */

/*
    Hour Counter (RHRCNT)
*/
/* PM */
#define _00_RTC_HOUR_ATTRIB_AM                      (0x00U) /* AM */
#define _40_RTC_HOUR_ATTRIB_PM                      (0x40U) /* PM */

/*
    Second Alarm Register (RSECAR)
*/
/* ENB */
#define _00_RTC_ALARM_SECOND_DISABLE                (0x00U) /* Disable alarm second */
#define _80_RTC_ALARM_SECOND_ENABLE                 (0x80U) /* Enable alarm second */

/*
    Minute Alarm Register (RMINAR)
*/
/* ENB */
#define _00_RTC_ALARM_MINUTE_DISABLE                (0x00U) /* Disable alarm minute */
#define _80_RTC_ALARM_MINUTE_ENABLE                 (0x80U) /* Enable alarm minute */

/*
    Hour Alarm Register (RHRAR)
*/
/* ENB */
#define _00_RTC_ALARM_HOUR_DISABLE                  (0x00U) /* Disable alarm hour */
#define _80_RTC_ALARM_HOUR_ENABLE                   (0x80U) /* Enable alarm hour */
/* PM */
#define _00_RTC_ALARM_HOUR_ATTRIB_AM                (0x00U) /* AM */
#define _40_RTC_ALARM_HOUR_ATTRIB_PM                (0x40U) /* PM */

/*
    Day-of-Week Alarm Register (RWKAR)
*/
/* ENB */
#define _00_RTC_ALARM_WEEK_DISABLE                  (0x00U) /* Disable alarm week */
#define _80_RTC_ALARM_WEEK_ENABLE                   (0x80U) /* Enable alarm week */

/*
    Date Alarm Register (RDAYAR)
*/
/* ENB */
#define _00_RTC_ALARM_DAY_DISABLE                   (0x00U) /* Disable alarm day */
#define _80_RTC_ALARM_DAY_ENABLE                    (0x80U) /* Enable alarm day */

/*
    Month Alarm Register (RMONAR)
*/
/* ENB */
#define _00_RTC_ALARM_MONTH_DISABLE                 (0x00U) /* Disable alarm month */
#define _80_RTC_ALARM_MONTH_ENABLE                  (0x80U) /* Enable alarm month */

/*
    Year Alarm Enable Register (RYRAREN)
*/
/* ENB */
#define _00_RTC_ALARM_YEAR_DISABLE                  (0x00U) /* Disable alarm year */
#define _80_RTC_ALARM_YEAR_ENABLE                   (0x80U) /* Enable alarm year */
/*
    Interrupt Source Priority Register n (IPRn)
*/
#define _00_RTC_PRIORITY_LEVEL0                     (0x00U) /* Level 0 (interrupt disabled) */
#define _01_RTC_PRIORITY_LEVEL1                     (0x01U) /* Level 1 */
#define _02_RTC_PRIORITY_LEVEL2                     (0x02U) /* Level 2 */
#define _03_RTC_PRIORITY_LEVEL3                     (0x03U) /* Level 3 */
#define _04_RTC_PRIORITY_LEVEL4                     (0x04U) /* Level 4 */
#define _05_RTC_PRIORITY_LEVEL5                     (0x05U) /* Level 5 */
#define _06_RTC_PRIORITY_LEVEL6                     (0x06U) /* Level 6 */
#define _07_RTC_PRIORITY_LEVEL7                     (0x07U) /* Level 7 */
#define _08_RTC_PRIORITY_LEVEL8                     (0x08U) /* Level 8 */
#define _09_RTC_PRIORITY_LEVEL9                     (0x09U) /* Level 9 */
#define _0A_RTC_PRIORITY_LEVEL10                    (0x0AU) /* Level 10 */
#define _0B_RTC_PRIORITY_LEVEL11                    (0x0BU) /* Level 11 */
#define _0C_RTC_PRIORITY_LEVEL12                    (0x0CU) /* Level 12 */
#define _0D_RTC_PRIORITY_LEVEL13                    (0x0DU) /* Level 13 */
#define _0E_RTC_PRIORITY_LEVEL14                    (0x0EU) /* Level 14 */
#define _0F_RTC_PRIORITY_LEVEL15                    (0x0FU) /* Level 15 (highest) */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef struct
{
    uint8_t rseccnt;
    uint8_t rmincnt;
    uint8_t rhrcnt;
    uint8_t rdaycnt;
    uint8_t rwkcnt;
    uint8_t rmoncnt;
    uint16_t ryrcnt;
} rtc_calendarcounter_value_t;

typedef struct
{
    uint8_t sec_enb;
    uint8_t min_enb;
    uint8_t hr_enb;
    uint8_t day_enb;
    uint8_t wk_enb;
    uint8_t mon_enb;
    uint8_t yr_enb;
} rtc_calendar_alarm_enable_t;

typedef struct
{
    uint8_t rsecar;
    uint8_t rminar;
    uint8_t rhrar;
    uint8_t rdayar;
    uint8_t rwkar;
    uint8_t rmonar;
    uint16_t ryrar;
} rtc_calendar_alarm_value_t;

typedef enum
{
    PES_1_256_SEC = 0x60U,
    PES_1_128_SEC = 0x70U,
    PES_1_64_SEC = 0x80U,
    PES_1_32_SEC = 0x90U,
    PES_1_16_SEC = 0xA0U,
    PES_1_8_SEC = 0xB0U,
    PES_1_4_SEC = 0xC0U,
    PES_1_2_SEC = 0xD0U,
    PES_1_SEC = 0xE0U,
    PES_2_SEC = 0xF0U
} rtc_int_period_t;

typedef enum
{
    RTCOUT_1HZ,
    RTCOUT_64HZ
} rtc_rtcout_period_t;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

