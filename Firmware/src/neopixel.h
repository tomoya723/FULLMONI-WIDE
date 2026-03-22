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

/* RIIC1バスリカバリ用 */
void R_Config_RIIC1_Create(void);
void R_Config_RIIC1_Start(void);
void R_Config_RIIC1_Stop(void);

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
extern volatile uint8_t I2C1_RCV_ERR_FLG;
extern volatile unsigned long g_i2c1_err_cnt;

/* I2C1タイムアウト付き送信完了待ち + エラー時バスリセット */
static inline int i2c1_wait_tx(void)
{
	volatile unsigned long timeout = 1000000;
	while (I2C1_TX_END_FLG == 0 && I2C1_RCV_ERR_FLG == 0 && timeout > 0) {
		timeout--;
	}
	if (timeout == 0 || I2C1_RCV_ERR_FLG) {
		R_Config_RIIC1_Stop();
		R_Config_RIIC1_Create();
		R_Config_RIIC1_Start();
		I2C1_TX_END_FLG = 0;
		I2C1_RCV_ERR_FLG = 0;
		g_i2c1_err_cnt++;
		return -1; /* エラー */
	}
	I2C1_TX_END_FLG = 0;
	return 0; /* 正常 */
}

#endif /* NEOPIXEL_HEADER_FILE */
