/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_RTC.h
* Component Version: 1.8.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_RTC.
***********************************************************************************************************************/

#ifndef CFG_Config_RTC_H
#define CFG_Config_RTC_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_rtc.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _04_FOUR_READ_COUNT                         (0x04U)/* Perform 4 read operations */
#define _2256_RTC_SUB_6_CYCLE_WAIT                  (0x2256U)
#define _0005B8D9_RTC_2_PERIODIC_INTERRUPTS_CYCLE   (0x0005B8D9UL)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_RTC_Create(void);
void R_Config_RTC_Create_UserInit(void);
void R_Config_RTC_Start(void);
void R_Config_RTC_Stop(void);
void R_Config_RTC_Restart(rtc_calendarcounter_value_t counter_write_val);
void R_Config_RTC_Get_CalendarCounterValue(rtc_calendarcounter_value_t * const counter_read_val);
void R_Config_RTC_Set_CalendarCounterValue(rtc_calendarcounter_value_t counter_write_val);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
