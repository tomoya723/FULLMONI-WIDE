/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_PORT.c
* Component Version: 2.4.1
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_PORT.
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
#include "Config_PORT.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_PORT_Create
* Description  : This function initializes the PORT
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_PORT_Create(void)
{
    /* Set PORT1 registers */
    PORT1.PODR.BYTE = _00_Pm7_OUTPUT_0;
    PORT1.ODR0.BYTE = _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORT1.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORT1.DSCR.BYTE = _00_Pm2_HIDRV_OFF | _00_Pm3_HIDRV_OFF | _00_Pm4_HIDRV_OFF | _80_Pm7_HIDRV_ON;
    PORT1.DSCR2.BYTE = _00_Pm2_HISPEED_OFF | _00_Pm3_HISPEED_OFF | _00_Pm4_HISPEED_OFF | _00_Pm7_HISPEED_OFF;
    PORT1.PMR.BYTE &= (_7F_Pm7_PIN_GPIO);
    PORT1.PDR.BYTE = _80_Pm7_MODE_OUTPUT | _03_PDR1_DEFAULT;

    /* Set PORT5 registers */
    PORT5.PODR.BYTE = _00_Pm0_OUTPUT_0 | _00_Pm1_OUTPUT_0 | _00_Pm2_OUTPUT_0 | _00_Pm3_OUTPUT_0 | _00_Pm4_OUTPUT_0 | 
                      _00_Pm5_OUTPUT_0;
    PORT5.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORT5.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT;
    PORT5.DSCR.BYTE = _00_Pm0_HIDRV_OFF | _00_Pm1_HIDRV_OFF | _00_Pm2_HIDRV_OFF | _08_Pm3_HIDRV_ON | 
                      _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF;
    PORT5.DSCR2.BYTE = _00_Pm0_HISPEED_OFF | _00_Pm1_HISPEED_OFF | _00_Pm2_HISPEED_OFF | _00_Pm3_HISPEED_OFF | 
                       _00_Pm4_HISPEED_OFF | _00_Pm5_HISPEED_OFF;
    PORT5.PMR.BYTE &= (_FE_Pm0_PIN_GPIO & _FD_Pm1_PIN_GPIO & _FB_Pm2_PIN_GPIO & _F7_Pm3_PIN_GPIO & _EF_Pm4_PIN_GPIO & 
                      _DF_Pm5_PIN_GPIO);
    PORT5.PDR.BYTE = _01_Pm0_MODE_OUTPUT | _02_Pm1_MODE_OUTPUT | _04_Pm2_MODE_OUTPUT | _08_Pm3_MODE_OUTPUT | 
                     _10_Pm4_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT | _C0_PDR5_DEFAULT;

    /* Set PORTC registers */
    PORTC.PODR.BYTE = _00_Pm0_OUTPUT_0 | _00_Pm1_OUTPUT_0 | _00_Pm2_OUTPUT_0 | _00_Pm3_OUTPUT_0 | _00_Pm4_OUTPUT_0 | 
                      _00_Pm5_OUTPUT_0;
    PORTC.ODR0.BYTE = _00_Pm0_CMOS_OUTPUT | _00_Pm1_CMOS_OUTPUT | _00_Pm2_CMOS_OUTPUT | _00_Pm3_CMOS_OUTPUT;
    PORTC.ODR1.BYTE = _00_Pm4_CMOS_OUTPUT | _00_Pm5_CMOS_OUTPUT | _00_Pm6_CMOS_OUTPUT | _00_Pm7_CMOS_OUTPUT;
    PORTC.DSCR.BYTE = _00_Pm0_HIDRV_OFF | _00_Pm1_HIDRV_OFF | _00_Pm2_HIDRV_OFF | _00_Pm3_HIDRV_OFF | 
                      _00_Pm4_HIDRV_OFF | _00_Pm5_HIDRV_OFF | _00_Pm6_HIDRV_OFF | _00_Pm7_HIDRV_OFF;
    PORTC.DSCR2.BYTE = _00_Pm0_HISPEED_OFF | _00_Pm1_HISPEED_OFF | _00_Pm2_HISPEED_OFF | _00_Pm3_HISPEED_OFF | 
                       _00_Pm4_HISPEED_OFF | _00_Pm5_HISPEED_OFF | _00_Pm6_HISPEED_OFF | _00_Pm7_HISPEED_OFF;
    PORTC.PMR.BYTE &= (_FE_Pm0_PIN_GPIO & _FD_Pm1_PIN_GPIO & _FB_Pm2_PIN_GPIO & _F7_Pm3_PIN_GPIO & _EF_Pm4_PIN_GPIO & 
                      _DF_Pm5_PIN_GPIO);
    PORTC.PDR.BYTE = _01_Pm0_MODE_OUTPUT | _02_Pm1_MODE_OUTPUT | _04_Pm2_MODE_OUTPUT | _08_Pm3_MODE_OUTPUT | 
                     _10_Pm4_MODE_OUTPUT | _20_Pm5_MODE_OUTPUT;

    /* Set PORTD registers */
    PORTD.ODR1.BYTE = _00_Pm6_CMOS_OUTPUT;
    PORTD.PCR.BYTE = _00_Pm0_PULLUP_OFF | _00_Pm1_PULLUP_OFF | _00_Pm2_PULLUP_OFF | _00_Pm3_PULLUP_OFF | 
                     _00_Pm4_PULLUP_OFF | _00_Pm5_PULLUP_OFF | _00_Pm7_PULLUP_OFF;
    PORTD.DSCR.BYTE = _00_Pm6_HIDRV_OFF;
    PORTD.DSCR2.BYTE = _00_Pm6_HISPEED_OFF;
    PORTD.PMR.BYTE &= (_FE_Pm0_PIN_GPIO & _FD_Pm1_PIN_GPIO & _FB_Pm2_PIN_GPIO & _F7_Pm3_PIN_GPIO & _EF_Pm4_PIN_GPIO & 
                      _DF_Pm5_PIN_GPIO & _7F_Pm7_PIN_GPIO);
    PORTD.PDR.BYTE = _00_Pm0_MODE_INPUT | _00_Pm1_MODE_INPUT | _00_Pm2_MODE_INPUT | _00_Pm3_MODE_INPUT | 
                     _00_Pm4_MODE_INPUT | _00_Pm5_MODE_INPUT | _00_Pm7_MODE_INPUT;

    R_Config_PORT_Create_UserInit();
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
