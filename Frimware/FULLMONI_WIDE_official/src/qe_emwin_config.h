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
* Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : qe_emwin_config.h
* Description  : Configures the emWin module.
***********************************************************************************************************************/
#ifndef EMWIN_RX_CONFIG_HEADER_FILE
#define EMWIN_RX_CONFIG_HEADER_FILE

#define ORIENTATION_0        0
#define ORIENTATION_CW       1
#define ORIENTATION_180      2
#define ORIENTATION_CCW      3

#define LCD_DRV_IC_ST7715    0
#define LCD_DRV_IC_ILI9341   1
#define LCD_DRV_IC_OTHER    99

#define LCD_IF_GLCDC         0
#define LCD_IF_RSPI          1
#define LCD_IF_SCI_SPI       2
#define LCD_IF_OTHER        99

#define TOUCH_IF_SCI_IIC     0
#define TOUCH_IF_RSPI        1
#define TOUCH_IF_SCI_SPI     2
#define TOUCH_IF_OTHER      99
/***********************************************************************************************************************
Configuration Options
***********************************************************************************************************************/

#define EMWIN_GUI_NUM_BYTES           (81920u)
#define EMWIN_XSIZE_PHYS              (800u)
#define EMWIN_YSIZE_PHYS              (256u)
#define EMWIN_BITS_PER_PIXEL          (16)
#define EMWIN_DISPLAY_ORIENTATION     (ORIENTATION_0)

#define EMWIN_LCD_IF                  (LCD_IF_GLCDC)

#define EMWIN_GUI_FRAME_BUFFER1       (0x00800000)
#define EMWIN_GUI_FRAME_BUFFER2       (0x00000040)

#define EMWIN_USE_DRW2D               (1)

#define EMWIN_LCD_IF_NUMBER           (0u)
#define EMWIN_LCD_DRIVER_IC           (LCD_DRV_IC_OTHER)
#define EMWIN_LCD_BAUDRATE            (1000000u)
#define EMWIN_GUI_USE_CACHE           (0)

#define EMWIN_USE_DISP_SIGNAL_PIN     (0)
#define EMWIN_DISP_SIGNAL_PIN         (GPIO_PORT_0_PIN_0)
#define EMWIN_USE_BACKLIGHT_PIN       (1)
#define EMWIN_BACKLIGHT_PIN           (GPIO_PORT_E_PIN_0)
#define EMWIN_USE_DATA_CMD_PIN        (0)
#define EMWIN_DATA_CMD_PIN            (GPIO_PORT_0_PIN_0)
#define EMWIN_USE_LCD_CS_PIN          (0)
#define EMWIN_LCD_CS_PIN              (GPIO_PORT_0_PIN_0)

#define EMWIN_USE_TOUCH               (0)
#define EMWIN_TOUCH_IF                (TOUCH_IF_SCI_IIC)
#define EMWIN_TOUCH_IF_NUMBER         (6u)
#define EMWIN_TOUCH_BAUDRATE          (1000000u)
#define EMWIN_SLAVE_ADDRESS           (0x00000038)
#define EMWIN_USE_MULTITOUCH          (0)
#define EMWIN_MAX_NUM_TOUCHPOINTS     (10u)

#define EMWIN_USE_TOUCH_IC_RESET_PIN  (0)
#define EMWIN_TOUCH_IC_RESET_PIN      (GPIO_PORT_6_PIN_6)
#define EMWIN_USE_TOUCH_CS_PIN        (0)
#define EMWIN_TOUCH_CS_PIN            (GPIO_PORT_0_PIN_0)

#endif /* EMWIN_RX_CONFIG_HEADER_FILE */
