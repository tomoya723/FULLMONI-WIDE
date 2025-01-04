/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No 
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all 
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM 
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES 
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS 
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of 
* this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.    
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : aprx72n0a.h
* H/W Platform : APRX72N0A_H
* Description  : 
***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 07.08.2013 1.00     First Release
*         : 27.05.2014 1.01     Corrected wrong pin definition for SW2
***********************************************************************************************************************/

#ifndef APRX72N0A_H
#define APRX72N0A_H

/* Local defines */
#define LED_ON              (1)
#define LED_OFF             (0)

/* LEDs */
#define LED0                PORT9.PODR.BIT.B5
#define LED1                PORT9.PODR.BIT.B6
#define LED0_PDR            PORT9.PDR.BIT.B5
#define LED1_PDR            PORT9.PDR.BIT.B6

#endif /* APRX72N0A_H */

extern can_frame_t	tx_dataframe,
//							rx_dataframe,
							rx_dataframe1,
							rx_dataframe2,
							rx_dataframe3,
							rx_dataframe4,
							rx_dataframe5,
							rx_dataframe6,
							remote_frame;
