/*
 * main.c
 *
 *  Created on: 2021/08/25
 *      Author: tomoya723
 */

// --------------------------------------------------------------------
// Include User Header File
// --------------------------------------------------------------------
#include "settings.h"			// FULLMONI-WIDE setting parameter
#include "r_smc_entry.h"
#include "platform.h"           // Located in the FIT BSP module
#include "TFTLCD102.h"
#include "neopixel.h"
#include "can.h"
#include "dataregister.h"
#include "smc_gen/general/r_cg_rtc.h"

// --------------------------------------------------------------------
// グローバル変数宣言
// --------------------------------------------------------------------
volatile long			g_int10mscnt;
volatile unsigned int	g_fps_cnt, g_fps_max, g_sp_int_flg, g_sw_cnt, g_sw_int_flg;
volatile uint8_t I2C_WriteData[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; // 先頭2byteは書き込みアドレス
volatile uint8_t I2C_BUF[] = {0x00,0x00};
volatile uint8_t I2C_BUF2[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
volatile unsigned long sp_int, sp_int_old, tr_int, wr_cnt;
volatile unsigned int sp_TGRA, sch50ms_cnt, sch100ms_cnt;
volatile uint8_t I2C0_TX_END_FLG = 0;
volatile uint8_t I2C0_RX_END_FLG = 0;
volatile uint8_t I2C0_RCV_ERR_FLG = 0;
volatile MD_STATUS I2C0_md_status;
volatile uint8_t I2C1_TX_END_FLG = 0;
volatile uint8_t I2C1_RX_END_FLG = 0;
volatile uint8_t I2C1_RCV_ERR_FLG = 0;
volatile MD_STATUS I2C1_md_status;

// --------------------------------------------------------------------
// グローバル構造体宣言
// --------------------------------------------------------------------
volatile CAN_data_t			g_CAN_data;
volatile CALC_data_t		g_CALC_data;
volatile CALC_data_t		g_CALC_data_sm;

void main(void);
void User_CallBack_transmitend0(void);
void User_CallBack_receiveend0(void);
void User_CallBack_receiveerror0(MD_STATUS status);
void User_CallBack_transmitend1(void);
void User_CallBack_receiveend1(void);
void User_CallBack_receiveerror1(MD_STATUS status);
void Neopixel_SetRGB(unsigned int LED_No, int g, int r, int b);
void Neopixel_InitRGB(void);
void Neopixel_TX(void);
void sch_10ms(void);
void ap_10ms(void);
void ap_50ms(void);
void ap_100ms(void);


extern void LCD_FadeIN(void);

void main(void)
{
	unsigned int shift_rev_cnt;

//	R_Systeminit();
	// MTU0 (10ms Interupt function)
	// MTU1 (Rotary Encorder Input function)
	// MTU3 (LCD Back Light Control)
	// MTU8 (Vehicle speed　Pulse　Interupt function)
	// RIIC ch0 EEPROM 16K 400kbps
	// RIIC ch1 ATtiny85 Neopixel Driver 400kbps

	// RTC Start
/*	R_Config_RTC_Stop();
	RTC.RYRCNT.WORD = 0x0025U;
	RTC.RMONCNT.BYTE = 0x03U;
	RTC.RDAYCNT.BYTE = 0x24U;
	RTC.RHRCNT.BYTE = 0x08U;
	RTC.RMINCNT.BYTE = 0x21U;
	RTC.RSECCNT.BYTE = 0x00U;*/

	R_Config_RTC_Start();

	R_Config_S12AD0_Start();
	S12AD.ADCSR.BIT.ADST = 1;         // ADC start
	while(S12AD.ADCSR.BIT.ADST==1);   // wait for ADC completion
	g_CALC_data.AD0 = S12AD.ADDR0;
	g_CALC_data.AD1 = S12AD.ADDR1;
	g_CALC_data.AD2 = S12AD.ADDR2;
	g_CALC_data.AD3 = S12AD.ADDR3;
	g_CALC_data.AD4 = S12AD.ADDR4;
	g_CALC_data.AD5 = S12AD.ADDR5;
	g_CALC_data.AD6 = S12AD.ADDR6;
	g_CALC_data.AD7 = S12AD.ADDR7;

	R_Config_MTU0_Start();
	R_Config_MTU1_Start();

	R_Config_MTU3_Start();

	// Init CAN
	Init_CAN();

	init_data_store();

	// Init Qe for Display
	APPW_X_Setup();
	APPW_Init(APPW_PROJECT_PATH);
	APPW_CreateRoot(APPW_INITIAL_SCREEN, WM_HBKWIN);

	// First drawing LCD
	GUI_Exec1();
	APPW_Exec();

	LCD_FadeIN();

	R_Config_RIIC0_Start();
	R_Config_RIIC1_Start();

	// Read ODO/Trip pulse log from EEPROM
	R_Config_RIIC0_Master_Send_Without_Stop(0x50,(void *) I2C_BUF,2); //I2C_BUF:Read Adress
	while(I2C0_TX_END_FLG == 0); // Wait for TX done
	I2C0_TX_END_FLG = 0;
	R_Config_RIIC0_Master_Receive(0x50,(void *) I2C_BUF2,12); //I2C_BUF2:Read Data
	while(I2C0_RX_END_FLG == 0); // Wait for RX done
	I2C0_RX_END_FLG = 0;
//	odo = I2C_BUF2[1] << 16 + I2C_BUF2[0];
	wr_cnt = (I2C_BUF2[0] <<  24) + (I2C_BUF2[1] <<  16) + (I2C_BUF2[2] <<  8) + I2C_BUF2[3];
	sp_int = (I2C_BUF2[4] <<  24) + (I2C_BUF2[5] <<  16) + (I2C_BUF2[6] <<  8) + I2C_BUF2[7];
	tr_int = (I2C_BUF2[8] <<  24) + (I2C_BUF2[9] <<  16) + (I2C_BUF2[10] <<  8) + I2C_BUF2[11];
//	wr_cnt = 0;
	// odo trip 初期化時は以下コメントを解除
	/*
	sp_int = 2548000; // データ入力用 1000km
	tr_int = 0; // データ入力用 1000km
	I2C_WriteData[1]  = ((wr_cnt >> 24) & 0x000000FF);
	I2C_WriteData[2]  = ((wr_cnt >> 16) & 0x000000FF);
	I2C_WriteData[3]  = ((wr_cnt >>  8) & 0x000000FF);
	I2C_WriteData[4]  = ((wr_cnt      ) & 0x000000FF);
	I2C_WriteData[5]  = ((sp_int >> 24) & 0x000000FF);
	I2C_WriteData[6]  = ((sp_int >> 16) & 0x000000FF);
	I2C_WriteData[7]  = ((sp_int >>  8) & 0x000000FF);
	I2C_WriteData[8]  = ((sp_int      ) & 0x000000FF);
	I2C_WriteData[9]  = ((tr_int >> 24) & 0x000000FF);
	I2C_WriteData[10] = ((tr_int >> 16) & 0x000000FF);
	I2C_WriteData[11] = ((tr_int >>  8) & 0x000000FF);
	I2C_WriteData[12] = ((tr_int      ) & 0x000000FF);
	R_Config_RIIC0_Master_Send(0x50,(void *) I2C_WriteData,14);		// odo & trip write
	while(I2C0_TX_END_FLG == 0); // I2C送信完了待ち
	I2C0_TX_END_FLG = 0;
	*/

	// Start MTU8 (Vehicle speed　Pulse　Interupt function)
	R_Config_MTU8_Start();

	printf("FULLMONI Init Done.\n");

	Neopixel_InitRGB();
	g_CALC_data.AD7 = S12AD.ADDR7; // issue#4暫定対策：LPF値リセット

	while (1)
	{
		// wait Reflesh Cycletime
		g_int10mscnt =  -3;		// peiod 10ms x  3 = 30ms  33fps
//		g_int10mscnt =  -4;		// peiod 10ms x  4 = 40ms  25fps
//		g_int10mscnt =  -5;		// peiod 10ms x  5 = 50ms  20fps
//		g_int10mscnt =  -10;	// peiod 10ms x 10 = 100ms 10fps

		// fps count
		g_fps_cnt ++;
		APPW_SetVarData(ID_VAR_FPS, g_fps_max);

		data_store();

		GUI_Exec1();
		APPW_Exec();

		main_CAN();

		if((sp_int != sp_int_old) && (g_sp_int_flg == 100))
		{
			I2C_WriteData[1]  = ((wr_cnt >> 24) & 0x000000FF);
			I2C_WriteData[2]  = ((wr_cnt >> 16) & 0x000000FF);
			I2C_WriteData[3]  = ((wr_cnt >>  8) & 0x000000FF);
			I2C_WriteData[4]  = ((wr_cnt      ) & 0x000000FF);
			I2C_WriteData[5]  = ((sp_int >> 24) & 0x000000FF);
			I2C_WriteData[6]  = ((sp_int >> 16) & 0x000000FF);
			I2C_WriteData[7]  = ((sp_int >>  8) & 0x000000FF);
			I2C_WriteData[8]  = ((sp_int      ) & 0x000000FF);
			I2C_WriteData[9]  = ((tr_int >> 24) & 0x000000FF);
			I2C_WriteData[10] = ((tr_int >> 16) & 0x000000FF);
			I2C_WriteData[11] = ((tr_int >>  8) & 0x000000FF);
			I2C_WriteData[12] = ((tr_int      ) & 0x000000FF);
		//	R_Config_RIIC0_Master_Send(0x50,(void *) I2C_WriteData,10);		// odo write
			R_Config_RIIC0_Master_Send(0x50,(void *) I2C_WriteData,14);		// odo & trip write
			while(I2C0_TX_END_FLG == 0); // I2C送信完了待ち
			I2C0_TX_END_FLG = 0;
			sp_int_old = sp_int;
			wr_cnt ++;
		}

		if((g_CALC_data.rev >= 5500) && (g_CALC_data.rev < 6000))
		{
			Neopixel_SetRGB(0, 0, 0, 255);
			Neopixel_SetRGB(1, 0, 0, 0);
			Neopixel_SetRGB(2, 0, 0, 0);
			Neopixel_SetRGB(3, 0, 0, 0);
			Neopixel_SetRGB(4, 0, 0, 0);
			Neopixel_SetRGB(5, 0, 0, 0);
			Neopixel_SetRGB(6, 0, 0, 0);
			Neopixel_SetRGB(7, 0, 0, 255);
		}

		else if((g_CALC_data.rev >= 6000) && (g_CALC_data.rev < 6500))
		{
			Neopixel_SetRGB(0, 0, 0, 255);
			Neopixel_SetRGB(1, 0, 0, 255);
			Neopixel_SetRGB(2, 0, 0, 0);
			Neopixel_SetRGB(3, 0, 0, 0);
			Neopixel_SetRGB(4, 0, 0, 0);
			Neopixel_SetRGB(5, 0, 0, 0);
			Neopixel_SetRGB(6, 0, 0, 255);
			Neopixel_SetRGB(7, 0, 0, 255);
		}
		else if((g_CALC_data.rev >= 6500) && (g_CALC_data.rev < 7000))
		{
			Neopixel_SetRGB(0, 0, 255, 0);
			Neopixel_SetRGB(1, 0, 255, 0);
			Neopixel_SetRGB(2, 0, 255, 0);
			Neopixel_SetRGB(3, 0, 0, 0);
			Neopixel_SetRGB(4, 0, 0, 0);
			Neopixel_SetRGB(5, 0, 255, 0);
			Neopixel_SetRGB(6, 0, 255, 0);
			Neopixel_SetRGB(7, 0, 255, 0);
		}
		else if((g_CALC_data.rev >= 7000) && (g_CALC_data.rev < 7500))
		{
			Neopixel_SetRGB(0, 255, 0, 0);
			Neopixel_SetRGB(1, 255, 0, 0);
			Neopixel_SetRGB(2, 255, 0, 0);
			Neopixel_SetRGB(3, 255, 0, 0);
			Neopixel_SetRGB(4, 255, 0, 0);
			Neopixel_SetRGB(5, 255, 0, 0);
			Neopixel_SetRGB(6, 255, 0, 0);
			Neopixel_SetRGB(7, 255, 0, 0);
		}
		else if((g_CALC_data.rev >= 7500))
		{
			if(shift_rev_cnt == 0)
			{
				shift_rev_cnt = 1;
				Neopixel_SetRGB(0, 255, 255, 255);
				Neopixel_SetRGB(1, 255, 255, 255);
				Neopixel_SetRGB(2, 255, 255, 255);
				Neopixel_SetRGB(3, 255, 255, 255);
				Neopixel_SetRGB(4, 255, 255, 255);
				Neopixel_SetRGB(5, 255, 255, 255);
				Neopixel_SetRGB(6, 255, 255, 255);
				Neopixel_SetRGB(7, 255, 255, 255);
			}
			else
			{
				shift_rev_cnt = 0;
				Neopixel_SetRGB(0, 0, 0, 0);
				Neopixel_SetRGB(1, 0, 0, 0);
				Neopixel_SetRGB(2, 0, 0, 0);
				Neopixel_SetRGB(3, 0, 0, 0);
				Neopixel_SetRGB(4, 0, 0, 0);
				Neopixel_SetRGB(5, 0, 0, 0);
				Neopixel_SetRGB(6, 0, 0, 0);
				Neopixel_SetRGB(7, 0, 0, 0);
			}
		}
		else
		{
			Neopixel_SetRGB(0, 0, 0, 0);
			Neopixel_SetRGB(1, 0, 0, 0);
			Neopixel_SetRGB(2, 0, 0, 0);
			Neopixel_SetRGB(3, 0, 0, 0);
			Neopixel_SetRGB(4, 0, 0, 0);
			Neopixel_SetRGB(5, 0, 0, 0);
			Neopixel_SetRGB(6, 0, 0, 0);
			Neopixel_SetRGB(7, 0, 0, 0);
			shift_rev_cnt = 0;
		}
		Neopixel_TX();

		// Auto dimmer Back Light control
		if(g_CALC_data.AD5 <= 50)
		{
//			MTU3.TGRD = 200;
			MTU3.TGRD = 600;
			g_CALC_data.neobrightness = 0x0F;
		}
		else if(g_CALC_data.AD5 >= 300)
		{
			MTU3.TGRD = 2998;
			g_CALC_data.neobrightness = 0xFF;
		}

//		while(g_int10mscnt < 0);
	}
}

// 10ms scheduled application call
void ap_10ms(void)
{
	data_setLCD10ms();
}

// 50ms scheduled application call
void ap_50ms(void)
{
	data_setLCD50ms();
}

// 100ms scheduled application call
void ap_100ms(void)
{
	data_setLCD100ms();
}


void sch_10ms(void)
{
	//10ms , 50ms , 100ms scheduler here
	ap_10ms();
	sch50ms_cnt += 1;
	sch100ms_cnt += 1;
	if(sch50ms_cnt >= 5)
	{
		ap_50ms();
		sch50ms_cnt =0;
	}
	if(sch100ms_cnt >= 10)
	{
		ap_100ms();
		sch100ms_cnt =0;
	}
}

void User_CallBack_transmitend0(void)
{
	I2C0_TX_END_FLG = 1;
}

void User_CallBack_receiveend0(void)
{
	I2C0_RX_END_FLG = 1;
}

void User_CallBack_receiveerror0(MD_STATUS status)
{
	I2C0_RCV_ERR_FLG = 1;
	I2C0_md_status = status;
}

void User_CallBack_transmitend1(void)
{
	I2C1_TX_END_FLG = 1;
}

void User_CallBack_receiveend1(void)
{
	I2C1_RX_END_FLG = 1;
}

void User_CallBack_receiveerror1(MD_STATUS status)
{
	I2C1_RCV_ERR_FLG = 1;
	I2C1_md_status = status;
}

