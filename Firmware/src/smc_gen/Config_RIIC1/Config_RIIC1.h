/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_RIIC1.h
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_RIIC1.
***********************************************************************************************************************/

#ifndef CFG_Config_RIIC1_H
#define CFG_Config_RIIC1_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_riic.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _FC_IIC1_SCL_LOW_LEVEL_PERIOD                          (0xFCU) /* SCL clock low-level period setting */
#define _FC_IIC1_SCL_HIGH_LEVEL_PERIOD                         (0xFCU) /* SCL clock high-level period setting */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_RIIC1_Create(void);
void R_Config_RIIC1_Create_UserInit(void);
void R_Config_RIIC1_Start(void);
void R_Config_RIIC1_Stop(void);
MD_STATUS R_Config_RIIC1_Master_Send(uint16_t adr, uint8_t * const tx_buf, uint16_t tx_num);
MD_STATUS R_Config_RIIC1_Master_Send_Without_Stop(uint16_t adr, uint8_t * const tx_buf, uint16_t tx_num);
MD_STATUS R_Config_RIIC1_Master_Receive(uint16_t adr, uint8_t * const rx_buf, uint16_t rx_num);
void R_Config_RIIC1_IIC_StartCondition(void);
void R_Config_RIIC1_IIC_StopCondition(void);
void r_Config_RIIC1_transmitend_interrupt(void);
void r_Config_RIIC1_error_interrupt(void);
static void r_Config_RIIC1_callback_transmitend(void);
static void r_Config_RIIC1_callback_receiveend(void);
static void r_Config_RIIC1_callback_error(MD_STATUS status);
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
