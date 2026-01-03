/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Pin.c
* Version          : 1.0.2
* Device(s)        : R5F572NNDxFP
* Description      : This file implements SMC pin code generation.
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
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Pins_Create
* Description  : This function initializes Smart Configurator pins
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Pins_Create(void)
{
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);

    /* Set AN000 pin */
    PORT4.PMR.BYTE &= 0xFEU;
    PORT4.PDR.BYTE &= 0xFEU;
    MPC.P40PFS.BYTE = 0x80U;

    /* Set AN001 pin */
    PORT4.PMR.BYTE &= 0xFDU;
    PORT4.PDR.BYTE &= 0xFDU;
    MPC.P41PFS.BYTE = 0x80U;

    /* Set AN002 pin */
    PORT4.PMR.BYTE &= 0xFBU;
    PORT4.PDR.BYTE &= 0xFBU;
    MPC.P42PFS.BYTE = 0x80U;

    /* Set AN003 pin */
    PORT4.PMR.BYTE &= 0xF7U;
    PORT4.PDR.BYTE &= 0xF7U;
    MPC.P43PFS.BYTE = 0x80U;

    /* Set AN004 pin */
    PORT4.PMR.BYTE &= 0xEFU;
    PORT4.PDR.BYTE &= 0xEFU;
    MPC.P44PFS.BYTE = 0x80U;

    /* Set AN005 pin */
    PORT4.PMR.BYTE &= 0xDFU;
    PORT4.PDR.BYTE &= 0xDFU;
    MPC.P45PFS.BYTE = 0x80U;

    /* Set AN006 pin */
    PORT4.PMR.BYTE &= 0xBFU;
    PORT4.PDR.BYTE &= 0xBFU;
    MPC.P46PFS.BYTE = 0x80U;

    /* Set AN007 pin */
    PORT4.PMR.BYTE &= 0x7FU;
    PORT4.PDR.BYTE &= 0x7FU;
    MPC.P47PFS.BYTE = 0x80U;

    /* Set CRX0 pin */
    MPC.P33PFS.BYTE = 0x10U;
    PORT3.PMR.BYTE |= 0x08U;

    /* Set CTX0 pin */
    MPC.P32PFS.BYTE = 0x10U;
    PORT3.PMR.BYTE |= 0x04U;

    /* Set LCD_CLK pin */
    MPC.PB5PFS.BYTE = 0x25U;
    PORTB.PMR.BYTE |= 0x20U;

    /* Set LCD_DATA0 pin */
    MPC.PB0PFS.BYTE = 0x25U;
    PORTB.PMR.BYTE |= 0x01U;

    /* Set LCD_DATA1 pin */
    MPC.PA7PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x80U;

    /* Set LCD_DATA2 pin */
    MPC.PA6PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x40U;

    /* Set LCD_DATA3 pin */
    MPC.PA5PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x20U;

    /* Set LCD_DATA4 pin */
    MPC.PA4PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x10U;

    /* Set LCD_DATA5 pin */
    MPC.PA3PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x08U;

    /* Set LCD_DATA6 pin */
    MPC.PA2PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x04U;

    /* Set LCD_DATA7 pin */
    MPC.PA1PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x02U;

    /* Set LCD_DATA8 pin */
    MPC.PA0PFS.BYTE = 0x25U;
    PORTA.PMR.BYTE |= 0x01U;

    /* Set LCD_DATA9 pin */
    MPC.PE7PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x80U;

    /* Set LCD_DATA10 pin */
    MPC.PE6PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x40U;

    /* Set LCD_DATA11 pin */
    MPC.PE5PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x20U;

    /* Set LCD_DATA12 pin */
    MPC.PE4PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x10U;

    /* Set LCD_DATA13 pin */
    MPC.PE3PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x08U;

    /* Set LCD_DATA14 pin */
    MPC.PE2PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x04U;

    /* Set LCD_DATA15 pin */
    MPC.PE1PFS.BYTE = 0x25U;
    PORTE.PMR.BYTE |= 0x02U;

    /* Set LCD_TCON2 pin */
    MPC.PB2PFS.BYTE = 0x25U;
    PORTB.PMR.BYTE |= 0x04U;

    /* Set MTCLKA pin */
    MPC.P24PFS.BYTE = 0x02U;
    PORT2.PMR.BYTE |= 0x10U;

    /* Set MTCLKB pin */
    MPC.P25PFS.BYTE = 0x02U;
    PORT2.PMR.BYTE |= 0x20U;

    /* Set MTIOC3C pin */
    MPC.PJ3PFS.BYTE = 0x01U;
    PORTJ.PMR.BYTE |= 0x08U;

    /* Set MTIOC8A pin */
    MPC.PD6PFS.BYTE = 0x08U;
    PORTD.PMR.BYTE |= 0x40U;

    /* Set RXD9 pin */
    MPC.PB6PFS.BYTE = 0x0AU;
    PORTB.PMR.BYTE |= 0x40U;

    /* Set SCL0 pin */
    MPC.P12PFS.BYTE = 0x0FU;
    PORT1.PMR.BYTE |= 0x04U;

    /* Set SCL1 pin */
    MPC.P21PFS.BYTE = 0x0FU;
    PORT2.PMR.BYTE |= 0x02U;

    /* Set SDA0 pin */
    MPC.P13PFS.BYTE = 0x0FU;
    PORT1.PMR.BYTE |= 0x08U;

    /* Set SDA1 pin */
    MPC.P20PFS.BYTE = 0x0FU;
    PORT2.PMR.BYTE |= 0x01U;

    /* Set TXD9 pin */
    PORTB.PODR.BYTE |= 0x80U;
    MPC.PB7PFS.BYTE = 0x0AU;
    PORTB.PDR.BYTE |= 0x80U;
    // PORTB.PMR.BIT.B7 = 1U; // Please set the PMR bit after TE bit is set to 1.

    R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
}

