/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU0.h
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_MTU0.
***********************************************************************************************************************/

#ifndef CFG_Config_MTU0_H
#define CFG_Config_MTU0_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_mtu3.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define MTU0_PCLK_COUNTER_DIVISION      (64)
#define _493D_TGRA0_VALUE               (0x493DU) /* TGRA0 value */
#define _00BB_TGRB0_VALUE               (0x00BBU) /* TGRB0 value */
#define _00BB_TGRC0_VALUE               (0x00BBU) /* TGRC0 value */
#define _00BB_TGRD0_VALUE               (0x00BBU) /* TGRD0 value */
#define _00BB_TGRE0_VALUE               (0x00BBU) /* TGRE0 value */
#define _00BB_TGRF0_VALUE               (0x00BBU) /* TGRF0 value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_MTU0_Create(void);
void R_Config_MTU0_Create_UserInit(void);
void R_Config_MTU0_Start(void);
void R_Config_MTU0_Stop(void);
/* Start user code for function. Do not edit comment generated here */
// --------------------------------------------------------------------
// extern declaration
// --------------------------------------------------------------------
extern volatile long			g_int10mscnt;
extern volatile unsigned int	g_fps_cnt, g_fps_max, g_sp_int_flg, g_sw_cnt, g_sw_int_flg;
extern volatile unsigned int	sp_TGRA;
/* End user code. Do not edit comment generated here */
#endif
