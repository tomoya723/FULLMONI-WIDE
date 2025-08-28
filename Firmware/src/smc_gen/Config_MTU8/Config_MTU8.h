/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_MTU8.h
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_MTU8.
***********************************************************************************************************************/

#ifndef CFG_Config_MTU8_H
#define CFG_Config_MTU8_H

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
#define MTU8_PCLK_COUNTER_DIVISION      (1024)
#define _0000000B_TGRB8_VALUE               (0x0000000BUL) /* TGRB8 value */
#define _0000000B_TGRC8_VALUE               (0x0000000BUL) /* TGRC8 value */
#define _0000000B_TGRD8_VALUE               (0x0000000BUL) /* TGRD8 value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_MTU8_Create(void);
void R_Config_MTU8_Create_UserInit(void);
void R_Config_MTU8_Start(void);
void R_Config_MTU8_Stop(void);
/* Start user code for function. Do not edit comment generated here */
extern volatile unsigned int	g_sp_int_flg;
extern volatile uint8_t I2C_WriteData[];
extern volatile unsigned long	sp_int;
extern volatile unsigned int	sp_TGRA;
extern volatile uint8_t I2C_TX_END_FLG;
/* End user code. Do not edit comment generated here */
#endif
