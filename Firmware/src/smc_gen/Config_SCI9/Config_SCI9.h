/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_SCI9.h
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_SCI9.
***********************************************************************************************************************/

#ifndef CFG_Config_SCI9_H
#define CFG_Config_SCI9_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_sci.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_SCI9_Create(void);
void R_Config_SCI9_Create_UserInit(void);
void R_Config_SCI9_Start(void);
void R_Config_SCI9_Stop(void);
MD_STATUS R_Config_SCI9_Serial_Send(uint8_t * const tx_buf, uint16_t tx_num);
MD_STATUS R_Config_SCI9_Serial_Receive(uint8_t * const rx_buf, uint16_t rx_num);
void r_Config_SCI9_transmitend_interrupt(void);
static void r_Config_SCI9_callback_transmitend(void);
static void r_Config_SCI9_callback_receiveend(void);
void r_Config_SCI9_receiveerror_interrupt(void);
static void r_Config_SCI9_callback_receiveerror(void);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
