/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : r_cg_r12da.h
* Version          : 1.0.200
* Device(s)        : R5F572NNDxFP
* Description      : General header file for R12DA peripheral.
***********************************************************************************************************************/

#ifndef DA_H
#define DA_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/* 
    D/A Control Register (DACR)
*/
/* D/A Enable (DAE) */
#define _00_DA_DISABLE                    (0x00U) /* D/A conversion of channels 0 and 1 is controlled individually */
#define _20_DA_ENABLE                     (0x20U) /* D/A conversion of channels 0 and 1 is enabled collectively */
/* D/A Output Enable 0 (DAOE0) */
#define _00_DA0_DISABLE                   (0x00U) /* Analog output of channel 0 (DA0) is disabled */
#define _40_DA0_ENABLE                    (0x40U) /* D/A conversion of channel 0 is enabled */
/* D/A Output Enable 1 (DAOE1) */
#define _00_DA1_DISABLE                   (0x00U) /* Analog output of channel 1 (DA1) is disabled */
#define _80_DA1_ENABLE                    (0x80U) /* D/A conversion of channel 1 is enabled */
/* DACR default value */
#define _1F_DA_DACR_DEFAULT               (0x1FU) /* Write default value of DACR */

/*
    DADRm Format Select Register (DADPR)
*/
/* DADRm Format Select (DPSEL) */
#define _00_DA_DPSEL_R                    (0x00U) /* Data is flush with the right end of the D/A data register */
#define _80_DA_DPSEL_L                    (0x80U) /* Data is flush with the left end of the D/A data register */

/*
    D/A-A/D Synchronous Start Control Register (DAADSCR)
*/
/* D/A-A/D Synchronous Conversion (DAADST) */
#define _00_DA_DAADSYNC_DISABLE           (0x00U) /* D/A converter does not synchronize with A/D converter */
#define _80_DA_DAADSYNC_ENABLE            (0x80U) /* D/A converter synchronizes with A/D converter operation */

/*
    D/A A/D Synchronous Unit Select Register (DAADUSR)
*/
/* A/D Unit 1 Select (AMADSEL1) */
#define _00_DA_DAADSYNC_UNIT1_DISABLE     (0x00U) /* S12AD unit 1 is not selected for D/A A/D Synchronous */
#define _02_DA_DAADSYNC_UNIT1_ENABLE      (0x02U) /* S12AD unit 1 is selected for D/A A/D Synchronous */

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif

