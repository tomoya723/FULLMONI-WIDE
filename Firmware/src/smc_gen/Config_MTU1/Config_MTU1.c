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
* File Name        : Config_MTU1.c
* Component Version: 2.4.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_MTU1.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_MTU1.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Create
* Description  : This function initializes the MTU1 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Create(void)
{
    /* Release MTU channel 1 from stop state */
    MSTP(MTU1) = 0U;

    /* Stop MTU channel 1 counter */
    MTU.TSTRA.BIT.CST1 = 0U;

    /* Set external clock noise filter */
    MTU0.NFCRC.BIT.NFAEN = 1U;
    MTU0.NFCRC.BIT.NFBEN = 1U;

    /* Select noise filter clock PCLK/32 */
    MTU0.NFCRC.BIT.NFCS = 0x03U;

    /* MTU channel 1 is used as phase counting mode */
    MTU1.TMDR1.BYTE = _04_MTU_COT1;
    MTU.TSYRA.BIT.SYNC1 = 0U;
    MTU1.TCR.BYTE = _00_MTU_CKCL_DIS;
    MTU1.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TCIEV_DISABLE | _00_MTU_TCIEU_DISABLE | 
                     _00_MTU_TTGE_DISABLE;
    MTU1.TIOR.BYTE = _00_MTU_IOA_DISABLE | _00_MTU_IOB_DISABLE;
    MTU1.TGRA = _0063_TGRA1_VALUE;
    MTU1.TGRB = _0063_TGRB1_VALUE;

    /* Set MTCLKA pin */
    MPC.P24PFS.BYTE = 0x02U;
    PORT2.PMR.BYTE |= 0x10U;

    /* Set MTCLKB pin */
    MPC.P25PFS.BYTE = 0x02U;
    PORT2.PMR.BYTE |= 0x20U;

    R_Config_MTU1_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Start
* Description  : This function starts the MTU1 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Start(void)
{
    /* Start MTU channel 1 counter */
    MTU.TSTRA.BIT.CST1 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU1_Stop
* Description  : This function stops the MTU1 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU1_Stop(void)
{
    /* Stop MTU channel 1 counter */
    MTU.TSTRA.BIT.CST1 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
