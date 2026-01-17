/* Generated configuration header file - do not edit */
/***********************************************************************************************************************
* Copyright (c) 2014 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*******************************************************************************
* File Name     : r_dtc_rx_config.h
* Description   : Configures the DTC drivers
********************************************************************************
* History : DD.MM.YYYY Version Description
*         : 15.01.2014 1.00    Initial revision
*         : 17.07.2014 2.00    Second  revision
*         : 12.11.2014 2.01    Added RX113.
*         : 30.01.2015 2.02    Added RX71M.
*         : 13.04.2015 2.03    Added RX231 and RX230.
*         : 24.12.2015 2.04    Added RX130, RX23T and RX24T.
*         :                    Modified #define name from "DTC_CFG_SHORT_ADDRRESS_MODE"
*         :                    to "DTC_CFG_SHORT_ADDRESS_MODE".
*         : 30.09.2016 2.05    Added RX65N.
*         :                    Added #define DTC_CFG_USE_SEQUENCE_TRANSFER.
*         : 31.03.2017 2.07    Added RX24U and RX24T-512KB.
*         : 31.07.2017 2.08    Supported RX65N-2MB and RX130-512KB.
*                              Fixed to correspond to Renesas coding rule.
*         : 28.09.2018 2.10    Supported RX66T.
*         : 01.02.2019 2.20    Supported RX72T, RX65N-64pin.
*         : 15.03.2025 4.51    Updated disclaimer.
*******************************************************************************/
#ifndef DTC_RX_CONFIG_H
#define DTC_RX_CONFIG_H

#define DTC_DISABLE        (0)
#define DTC_ENABLE         (1)
/*
 * SPECIFY WHETHER TO INCLUDE CODE FOR API PARAMETER CHECKING
 *  0 : Compiles out parameter checking.
 *  1 : Includes parameter checking.
 * Default value is set to BSP_CFG_PARAM_CHECKING_ENABLE to 
 * re-use the system default setting.
*/
#define DTC_CFG_PARAM_CHECKING_ENABLE       (BSP_CFG_PARAM_CHECKING_ENABLE)

/*
 * SPECIFY WHETHER THE DTCER REGISTERS WILL BE CLEARED IN R_DTC_OPEN()
 * DTC_DISABLE : Do nothing.
 * DTC_ENABLE  : Clear all DTCER registers in R_DTC_Open().
*/
#define DTC_CFG_DISABLE_ALL_ACT_SOURCE      (DTC_ENABLE)

/*
 * SPECIFY WHICH ADDRESS MODE IS SUPPORTED BY DTC
 * DTC_DISABLE : Select the Full address mode.
 * DTC_ENABLE  : Select the Short address mode.
*/
#define DTC_CFG_SHORT_ADDRESS_MODE          (DTC_DISABLE)

/*
 * SPECIFY WHETHER THE TRANSFER DATA READ SKIP IS ENABLED
 * DTC_DISABLE : Disable Transfer Data Read Skip.
 * DTC_ENABLE  : Enable Transfer Data Read Skip. 
*/
#define DTC_CFG_TRANSFER_DATA_READ_SKIP_EN  (DTC_ENABLE)

/*
 * SPECIFY WHETHER THE DMAC FIT MODULE IS USED WITH DTC FIT MODULE
 * DTC_DISABLE : DMAC FIT module is not used with DTC FIT module.
 * DTC_ENABLE  : DMAC FIT module is used with DTC FIT module.
*/
#define DTC_CFG_USE_DMAC_FIT_MODULE         (DTC_ENABLE)

/* 
 * SPECIFY WHETHER THE SEQUENCE TRANSFER IS USED
 * Also, set DTC_DISABLE to DTC_CFG_SHORT_ADDRESS_MODE.
 * DTC_DISABLE : Not use sequence transfer.
 * DTC_ENABLE  : Use sequence transfer.
*/
#define DTC_CFG_USE_SEQUENCE_TRANSFER       (DTC_DISABLE)


#endif /* DTC_RX_CONFIG_H */
