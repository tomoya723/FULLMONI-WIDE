/*
* Copyright (c) 2016 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
* File Name        : Config_SCI9_user.c
* Component Version: 1.12.0
* Device(s)        : R5F572NNDxFP
* Description      : This file implements device driver for Config_SCI9.
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
#include "Config_SCI9.h"
/* Start user code for include. Do not edit comment generated here */
#include "../../param_console.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_sci9_tx_address;                /* SCI9 transmit buffer address */
extern volatile uint16_t  g_sci9_tx_count;                   /* SCI9 transmit data number */
extern volatile uint8_t * gp_sci9_rx_address;                /* SCI9 receive buffer address */
extern volatile uint16_t  g_sci9_rx_count;                   /* SCI9 receive data number */
extern volatile uint16_t  g_sci9_rx_length;                  /* SCI9 receive data length */
/* Start user code for global. Do not edit comment generated here */
/* パラメータ変更モード用フラグ */
volatile uint8_t g_uart_rx_trigger = 0;     /* 受信トリガーフラグ */
volatile uint8_t g_param_mode_active = 0;   /* パラメータモードアクティブ */
static uint8_t g_sci9_rx_single_buf[1];     /* 1バイト受信用バッファ */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_SCI9_Create_UserInit
* Description  : This function adds user code after initializing the SCI9 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_SCI9_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_transmit_interrupt
* Description  : This function is TXI9 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI9_transmit_interrupt(void)
{
    if (0U < g_sci9_tx_count)
    {
        SCI9.TDR = *gp_sci9_tx_address;
        gp_sci9_tx_address++;
        g_sci9_tx_count--;
    }
    else
    {
        SCI9.SCR.BIT.TIE = 0U;
        SCI9.SCR.BIT.TEIE = 1U;
    }
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_transmitend_interrupt
* Description  : This function is TEI9 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI9_transmitend_interrupt(void)
{
    /* Set TXD9 pin */
    PORTB.PMR.BYTE &= 0x7FU;

    SCI9.SCR.BIT.TIE = 0U;
    SCI9.SCR.BIT.TE = 0U;
    SCI9.SCR.BIT.TEIE = 0U;
    
    r_Config_SCI9_callback_transmitend();
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_receive_interrupt
* Description  : This function is RXI9 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI9_receive_interrupt(void)
{
    /* Start user code for r_Config_SCI9_receive_interrupt. Do not edit comment generated here */
    uint8_t rx_data = SCI9.RDR;
    
    if (g_param_mode_active) {
        /* パラメータモード中：リングバッファに追加 */
        param_console_rx_push(rx_data);
        /* 継続受信（RIE, REは有効のまま） */
    } else {
        /* 通常モード：トリガーフラグをセット */
        g_uart_rx_trigger = 1;
        /* 継続受信のため再設定 */
        R_Config_SCI9_Serial_Receive(g_sci9_rx_single_buf, 1);
    }
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_receiveerror_interrupt
* Description  : This function is ERI9 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void r_Config_SCI9_receiveerror_interrupt(void)
{
    uint8_t err_type;
    
    r_Config_SCI9_callback_receiveerror();
    
    /* Clear overrun, framing and parity error flags */
    err_type = SCI9.SSR.BYTE;
    err_type &= 0xC7U;
    err_type |= 0xC0U;
    SCI9.SSR.BYTE = err_type;
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_callback_transmitend
* Description  : This function is a callback function when SCI9 finishes transmission
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI9_callback_transmitend(void)
{
    /* Start user code for r_Config_SCI9_callback_transmitend. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_callback_receiveend
* Description  : This function is a callback function when SCI9 finishes reception
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI9_callback_receiveend(void)
{
    /* Start user code for r_Config_SCI9_callback_receiveend. Do not edit comment generated here */
    g_uart_rx_trigger = 1;  /* 受信トリガーフラグをセット */
    /* 次の1バイト受信を開始 */
    R_Config_SCI9_Serial_Receive(g_sci9_rx_single_buf, 1);
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_SCI9_callback_receiveerror
* Description  : This function is a callback function when SCI9 reception encounters error
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

static void r_Config_SCI9_callback_receiveerror(void)
{
    /* Start user code for r_Config_SCI9_callback_receiveerror. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
