/*
 * neopixel.h
 *
 *  Created on: 2024/05/16
 *      Author: tomoya723
 */

#ifndef NEOPIXEL_HEADER_FILE
#define NEOPIXEL_HEADER_FILE
#include "dataregister.h"
#include "platform.h"           // Located in the FIT BSP module

// --------------------------------------------------------------------
// Prototype declaration
// --------------------------------------------------------------------
void Neopixel_SetRGB(unsigned int LED_No, int g, int r, int b);
void Neopixel_InitRGB(void);
void Neopixel_TX(void);

// --------------------------------------------------------------------
// extern宣言
// --------------------------------------------------------------------
extern volatile CAN_data_t			g_CAN_data;
extern volatile CALC_data_t			g_CALC_data;
extern volatile uint8_t I2C1_TX_END_FLG;

#endif /* NEOPIXEL_HEADER_FILE */
