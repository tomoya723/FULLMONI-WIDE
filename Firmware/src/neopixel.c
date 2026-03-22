/*
 * neopixel.c
 *
 *  Created on: 2024/05/16
 *      Author: tomoyasato
 */

#include "neopixel.h"
#include "dataregister.h"
#include "platform.h"           // Located in the FIT BSP module
#include "r_smc_entry.h"

void Neopixel_SetRGB(unsigned int LED_No, int g, int r, int b)
{
	unsigned int i;

	if(g < 0) g = 0;
	if(r < 0) r = 0;
	if(b < 0) b = 0;
	if(g > 255) g = 255;
	if(r > 255) r = 255;
	if(b > 255) b = 255;
	g_CALC_data.neopixel[LED_No * 24    ] = g;
	g_CALC_data.neopixel[LED_No * 24 + 1] = r;
	g_CALC_data.neopixel[LED_No * 24 + 2] = b;

}

void Neopixel_InitRGB(void)
{
	uint8_t NPcom0[]  = {0x00};
	uint8_t NPcom1[]  = {0x01, 0xFF};
//	uint8_t NPcom2[]  = {0x02, 0x00, 0xFF, 0x00, 0x00};
//	uint8_t NPcom3[]  = {0x02, 0x01, 0x00, 0xFF, 0x00};
//	uint8_t NPcom4[]  = {0x02, 0x02, 0xFF, 0xFF, 0x00};
//	uint8_t NPcom5[]  = {0x02, 0x03, 0x00, 0x00, 0xFF};
//	uint8_t NPcom6[]  = {0x02, 0x04, 0xFF, 0x00, 0xFF};
//	uint8_t NPcom7[]  = {0x02, 0x05, 0x00, 0xFF, 0xFF};
//	uint8_t NPcom8[]  = {0x02, 0x06, 0xFF, 0xFF, 0xFF};
//	uint8_t NPcom9[]  = {0x02, 0x07, 0xFF, 0x00, 0x00};
	unsigned int i, j;
	g_CALC_data.neobrightness = 0xFF;

	for( i = 0; i < 8; i ++ )
	{
		Neopixel_SetRGB(i, 0, 0, 0);
	}
	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom0,  1);	i2c1_wait_tx();
	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom1,  2);	i2c1_wait_tx();
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom2,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom3,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom4,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom5,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom6,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom7,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom8,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
//	R_Config_RIIC1_Master_Send(0x06, (void *) NPcom9,  5);	while(I2C1_TX_END_FLG == 0){}	I2C1_TX_END_FLG = 0;
	Neopixel_TX();

	for( j = 0; j < 50; j = j + 10 )
	{
		for( i = 0; i < 8; i ++ )
		{
			Neopixel_SetRGB(i, j - 5 - i * 10, j - 5 - i * 10, j - i * 10);
			Neopixel_TX();
		}
	}
	for( j = 50; j > 0; j = j - 2 )
	{
		for( i = 0; i < 8; i ++ )
		{
			Neopixel_SetRGB(i, j - 5, j - 5, j);
			Neopixel_TX();
		}
	}
}

void Neopixel_TX(void)
{
	uint8_t NPcom[8][5];
	uint8_t NPcom1[1];
//	unsigned int i;
//
//	// 8灯分のコマンドを送信
//	for( i = 0; i < 8; i ++ )
//	{
//		NPcom[i][0] = 0x02;
//		NPcom[i][1] = i;
//		NPcom[i][2] = g_CALC_data.neopixel[i * 24    ];
//		NPcom[i][3] = g_CALC_data.neopixel[i * 24 + 1];
//		NPcom[i][4] = g_CALC_data.neopixel[i * 24 + 2];
//		R_Config_RIIC1_Master_Send(0x06, NPcom[i],  5);
//		while(I2C1_TX_END_FLG == 0);
//		I2C1_TX_END_FLG = 0;
//	}
	NPcom1[0] = 0x01;
	NPcom1[1] = g_CALC_data.neobrightness;
	R_Config_RIIC1_Master_Send(0x06, NPcom1,  2);
	if (i2c1_wait_tx() < 0) return;
	for (int i = 0; i < 8; i++) {
		NPcom[i][0] = 0x02;
		NPcom[i][1] = i;
		NPcom[i][2] = g_CALC_data.neopixel[i * 24    ];
		NPcom[i][3] = g_CALC_data.neopixel[i * 24 + 1];
		NPcom[i][4] = g_CALC_data.neopixel[i * 24 + 2];
		R_Config_RIIC1_Master_Send(0x06, NPcom[i],  5);
		if (i2c1_wait_tx() < 0) return;
	}
}
