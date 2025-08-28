/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_S12AD0.h
* Component Version: 2.5.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_S12AD0.
***********************************************************************************************************************/

#ifndef CFG_Config_S12AD0_H
#define CFG_Config_S12AD0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_s12ad.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _0B_AD0_SAMPLING_STATE_0           (0x0BU)   /* ANx00 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_1           (0x0BU)   /* ANx01 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_2           (0x0BU)   /* ANx02 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_3           (0x0BU)   /* ANx03 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_4           (0x0BU)   /* ANx04 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_5           (0x0BU)   /* ANx05 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_6           (0x0BU)   /* ANx06 sampling time setting */
#define _0B_AD0_SAMPLING_STATE_7           (0x0BU)   /* ANx07 sampling time setting */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_S12AD0_Create(void);
void R_Config_S12AD0_Create_UserInit(void);
void R_Config_S12AD0_Start(void);
void R_Config_S12AD0_Stop(void);
void R_Config_S12AD0_Get_ValueResult(ad_channel_t channel, uint16_t * const buffer);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
