/*
 * dataregister.c
 *
 *  Created on: 2021/08/25
 *      Author: tomoya723
 */

#include <can.h>
#include "../aw/Source/Generated/Resource.h"
#include "../aw/Source/Generated/ID_SCREEN_01.h"
#include "../aw/Source/Generated/ID_SCREEN_02.h"

#include "dataregister.h"
#include "lib_table.h"

#define PI 3.1415923

//float table_fuel_resistance[9]	= {   0.0,   7.0,  15.0,  33.0,  50.0,  70.0,  95.0, 1000.0};
//float table_fuel_calclevel[9]		= { 100.0, 100.0,  82.8,  50.0,  26.6,   8.5,   0.0,    0.0};

float table_fuel_resistance[9]		= { 1000.0,  95.0,  70.0,  50.0,  33.0,  15.0,   7.0,   0.0};
float table_fuel_calclevel[9]		= {    0.0,   0.0,   8.5,  26.6,  50.0,  82.8, 100.0, 100.0};

//const table2D fuel_sender_to_LEVEL_fl_fl = {9, table_fuel_calclevel, table_fuel_resistance};
const table2D fuel_sender_to_LEVEL_fl_fl = {8, table_fuel_resistance, table_fuel_calclevel};

const float table_tyre_spec[3]			= { 195.0,  50.0,  15.0};
const float table_gear_ratio[6]			= { 3.760, 2.269, 1.645, 1.257, 1.000, 0.843};
unsigned int table_gear_ratio_range[7]	= {   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0};
const float table_final_gear_ratio		= 4.300;

static APPW_PARA_ITEM aPara0[6] = {0};
static APPW_PARA_ITEM aPara1[6] = {0};
static APPW_PARA_ITEM aPara2[6] = {0};
static APPW_PARA_ITEM aPara3[6] = {0};
static APPW_PARA_ITEM aPara4[6] = {0};
static APPW_PARA_ITEM aPara5[6] = {0};
static APPW_PARA_ITEM aPara6[6] = {0};
static APPW_PARA_ITEM aPara7[6] = {0};
static APPW_PARA_ITEM aPara8[6] = {0};

void init_data_store(void)
{
	float delta;

	// gear ratio table init
	g_CALC_data.TyreCirc = ((table_tyre_spec[0] * table_tyre_spec[1] / 100) * 2 + table_tyre_spec[2] * 25.4) * PI / 1000;

	delta = (table_gear_ratio[0] - table_gear_ratio[1]) / 2;
	table_gear_ratio_range[0] = (table_gear_ratio[0] + delta) * 1000;
	table_gear_ratio_range[1] = (table_gear_ratio[0] - delta) * 1000;

	delta = (table_gear_ratio[1] - table_gear_ratio[2]) / 2;
	table_gear_ratio_range[2] = (table_gear_ratio[1] - delta) * 1000;

	delta = (table_gear_ratio[2] - table_gear_ratio[3]) / 2;
	table_gear_ratio_range[3] = (table_gear_ratio[2] - delta) * 1000;

	delta = (table_gear_ratio[3] - table_gear_ratio[4]) / 2;
	table_gear_ratio_range[4] = (table_gear_ratio[3] - delta) * 1000;

	delta = (table_gear_ratio[4] - table_gear_ratio[5]) / 2;
	table_gear_ratio_range[5] = (table_gear_ratio[4] - delta) * 1000;
	table_gear_ratio_range[6] = (table_gear_ratio[5] - delta) * 1000;
}

void data_store(void)
{
	unsigned int i, gear, gear_pos, error;
	float fuel1, fuel2, fuel3, fuel4, a, b, c;
//	APPW_PARA_ITEM aPara0[6] = {0};
//	APPW_PARA_ITEM aPara1[6] = {0};
//	APPW_PARA_ITEM aPara2[6] = {0};
//	APPW_PARA_ITEM aPara3[6] = {0};
//	APPW_PARA_ITEM aPara4[6] = {0};
//	APPW_PARA_ITEM aPara5[6] = {0};
//	APPW_PARA_ITEM aPara6[6] = {0};
//	APPW_PARA_ITEM aPara7[6] = {0};
//	APPW_PARA_ITEM aPara8[6] = {0};

	g_CALC_data.num1 = (float)((((unsigned int)rx_dataframe2.data[0]) << 8) + rx_dataframe2.data[1]) / 10.0  ; // water Temp
	if(g_CALC_data.num1 > 32767) g_CALC_data.num1 = g_CALC_data.num1 -65534;
	g_CALC_data.num2 = (float)((((unsigned int)rx_dataframe1.data[6]) << 8) + rx_dataframe1.data[7]) / 10.0  ; // Inlet Air Temp
	if(g_CALC_data.num2 > 32767) g_CALC_data.num2 = g_CALC_data.num2 -65534;
	g_CALC_data.num3 = (float)((((unsigned int)rx_dataframe3.data[6]) << 8) + rx_dataframe3.data[7]) / 10.0  ; // OIL Temp
	if(g_CALC_data.num3 > 32767) g_CALC_data.num3 = g_CALC_data.num3 -65534;
	g_CALC_data.num4 = (float)((((unsigned int)rx_dataframe1.data[4]) << 8) + rx_dataframe1.data[5]) * 1.0   ; // MAP
	if(g_CALC_data.num4 > 32767) g_CALC_data.num4 = g_CALC_data.num4 -65534;
	g_CALC_data.num5 = (float)((((unsigned int)rx_dataframe4.data[0]) << 8) + rx_dataframe4.data[1]) * 0.01  ; // OIL Pressure
	if(g_CALC_data.num5 > 32767) g_CALC_data.num5 = g_CALC_data.num5 -65534;
	g_CALC_data.num6 = (float)((((unsigned int)rx_dataframe4.data[6]) << 8) + rx_dataframe4.data[7]) * 0.1   ; // Battery Voltage
	if(g_CALC_data.num6 > 32767) g_CALC_data.num6 = g_CALC_data.num6 -65534;

	// air fuel ratio
	g_CALC_data.af   = (float)((((unsigned int)rx_dataframe2.data[2]) << 8) + rx_dataframe2.data[3]) * 0.147 ; // A/F

	// engine rpm
	g_CALC_data.rev  =        ((((unsigned int)rx_dataframe1.data[0]) << 8) + rx_dataframe1.data[1])         ; // REV
//	g_CALC_data.rev  =        ((((unsigned int)rx_dataframe1.data[2]) << 8) + rx_dataframe1.data[3]) * 10    ; // REV test from TPS
	if(g_CALC_data.rev >= 9000) g_CALC_data.rev = 9000;
	g_CALC_data.rev_angle = 3600 - g_CALC_data.rev * 0.3;

	// battery voltage
//	g_CALC_data.bt = ((g_CALC_data.AD6 * 198) / 4096) / 10; // Vref = 3.3V
	g_CALC_data.bt = ((g_CALC_data.AD6 * 150) / 4096) / 10; // Vref = 3.0V

	// fuel lebel
	// Vref = 3.3V pullup 300��
//	fuel1 = g_CALC_data.AD7 * 31680;
//	fuel2 = g_CALC_data.bt * 90090 - g_CALC_data.AD7 * 105.6;
	// Vref = 3.0V pullup 560��
	fuel1 = g_CALC_data.AD7 * 1680;
	fuel2 = g_CALC_data.bt * 4095 - g_CALC_data.AD7 * 3;
	fuel3 = fuel1 / fuel2;	// �Z���_�[��R�l�Z�o
//	for(i = 1; i <= 7; i ++)
//	{
// refactor to look up function
//	i = 1;
//	while(i <= 7)
//	{
//		if(fuel3 <= table_fuel_resistance[i])
//		{
//			a = (table_fuel_resistance[i] - table_fuel_resistance[i-1]);
//			b = (fuel3 - table_fuel_resistance[i-1]);
//			c = b / a;
//			fuel4 = table_fuel_calclevel[i-1] - ((table_fuel_calclevel[i-1] - table_fuel_calclevel[i]) * c);
//			fuel4 = (fuel4 );
//			break;
//		}
//		else
//		{
//			i++;
//		}
//	}

	fuel4 = table2D_getValue(&fuel_sender_to_LEVEL_fl_fl, fuel3);
	if(fuel4 <   0) fuel4 =   0;
	if(fuel4 > 100) fuel4 = 100;

	// vehicle speed
	g_CALC_data.odo = sp_int / 2548;
	g_CALC_data.trip = ((float)(sp_int - tr_int)) / 2548;

	// gear ratio & shift position
	gear = (unsigned int)(g_CALC_data.rev * g_CALC_data.TyreCirc * 60 / g_CALC_data.sp / table_final_gear_ratio);
	if		(( table_gear_ratio_range[0] >= gear ) && ( table_gear_ratio_range[1] <= gear ))
	{
		gear_pos = 1;
	}
	else if	(( table_gear_ratio_range[1] >= gear ) && ( table_gear_ratio_range[2] <= gear ))
	{
		gear_pos = 2;
	}
	else if	(( table_gear_ratio_range[2] >= gear ) && ( table_gear_ratio_range[3] <= gear ))
	{
		gear_pos = 3;
	}
	else if	(( table_gear_ratio_range[3] >= gear ) && ( table_gear_ratio_range[4] <= gear ))
	{
		gear_pos = 4;
	}
	else if	(( table_gear_ratio_range[4] >= gear ) && ( table_gear_ratio_range[5] <= gear ))
	{
		gear_pos = 5;
	}
	else if	(( table_gear_ratio_range[5] >= gear ) && ( table_gear_ratio_range[6] <= gear ))
	{
		gear_pos = 6;
	}
	else
	{
		gear_pos = 0;
	}

	// Set value to appWizard variables.
	APPW_SetVarData(ID_VAR_REV_A, g_CALC_data.rev_angle);
	APPW_SetVarData(ID_VAR_01, g_CALC_data.num1);
	APPW_SetVarData(ID_VAR_02, g_CALC_data.num2);
	APPW_SetVarData(ID_VAR_03, g_CALC_data.num3);
	APPW_SetVarData(ID_VAR_04, g_CALC_data.num4);
	APPW_SetVarData(ID_VAR_05, g_CALC_data.num5);
	APPW_SetVarData(ID_VAR_06, g_CALC_data.num6);
	APPW_SetVarData(ID_VAR_AF, g_CALC_data.af);
	APPW_SetVarData(ID_VAR_REV, g_CALC_data.rev);
	APPW_SetVarData(ID_VAR_GEAR, gear_pos);
	APPW_SetVarData(ID_VAR_BATT, g_CALC_data.bt * 10);
	APPW_SetVarData(ID_VAR_FL1, gear * 10);
	APPW_SetVarData(ID_VAR_BL1, g_CALC_data.AD5 * 10);
	APPW_SetVarData(ID_VAR_SPEED, g_CALC_data.sp * 1.06);
	APPW_SetVarData(ID_VAR_ODO, g_CALC_data.odo);
	APPW_SetVarData(ID_VAR_TRIP, g_CALC_data.trip * 10);
	APPW_SetVarData(ID_VAR_FUEL, fuel4);
	APPW_SetVarData(ID_VAR_AD1, g_CALC_data.AD1 * 10);
	APPW_SetVarData(ID_VAR_AD2, g_CALC_data.AD2 * 10);
	APPW_SetVarData(ID_VAR_AD3, g_CALC_data.AD3 * 10);
	APPW_SetVarData(ID_VAR_AD4, g_CALC_data.AD4 * 10);
	aPara0[0].v = 0;																			APPW_DoJob(ID_SCREEN_02, ID_ICON_00 , APPW_JOB_SETVIS, aPara0);	// Master Warning
	aPara1[0].v = 0;																			APPW_DoJob(ID_SCREEN_02, ID_ICON_01 , APPW_JOB_SETVIS, aPara1);	// Oil Warning
	if(g_CALC_data.num1 >  60)	{	aPara2[0].v = 0;	} else {		aPara2[0].v = 1;	}	APPW_DoJob(ID_SCREEN_02, ID_ICON_02L, APPW_JOB_SETVIS, aPara2);	// Water Temp Warning Low
	if(g_CALC_data.num1 < 100)	{	aPara3[0].v = 0;	} else {		aPara3[0].v = 1;	}	APPW_DoJob(ID_SCREEN_02, ID_ICON_02H, APPW_JOB_SETVIS, aPara3);	// Water Temp Warning High
	if(g_CALC_data.AD3 >  100)	{	aPara4[0].v = 0;	} else {		aPara4[0].v = 1;	}	APPW_DoJob(ID_SCREEN_02, ID_ICON_03 , APPW_JOB_SETVIS, aPara4);	// Battery Warning
	if(g_CALC_data.AD1 > 1000)	{	aPara5[0].v = 0;	} else {		aPara5[0].v = 1;	}	APPW_DoJob(ID_SCREEN_02, ID_ICON_04 , APPW_JOB_SETVIS, aPara5);	// Battery Warning
	if(g_CALC_data.AD2 >  100)	{	aPara6[0].v = 0;	} else {		aPara6[0].v = 1;	}	APPW_DoJob(ID_SCREEN_02, ID_ICON_05 , APPW_JOB_SETVIS, aPara6);	// Break Warning
	if(g_CALC_data.AD4 >  100)	{	aPara7[0].v = 0;	} else {		aPara7[0].v = 1;	}	APPW_DoJob(ID_SCREEN_02, ID_ICON_06 , APPW_JOB_SETVIS, aPara7);	// Belts Warning
	if(fuel4 > 10)	{	aPara8[0].v = 0;	} else if (fuel4 < 5)	{ aPara8[0].v = 1;	}		APPW_DoJob(ID_SCREEN_02, ID_ICON_07 , APPW_JOB_SETVIS, aPara8);	// Fuel Empty


//	sprintf((void *) g_CALC_data.str_time,"%2x:%02x:%02x", (RTC.RHRCNT.BYTE & 0x3F), RTC.RMINCNT.BYTE, RTC.RSECCNT.BYTE);
	sprintf((void *) g_CALC_data.str_time,"%2x:%02x", (RTC.RHRCNT.BYTE & 0x3F), RTC.RMINCNT.BYTE);
	APPW_SetText(ID_SCREEN_01,ID_NUM_TIME,	(void *) g_CALC_data.str_time	);

	// SW Input
	if(g_sw_int_flg == 1)
	{
//		APPW_SetVarData(ID_VAR_SW, 1);	// VAR trig
		PORT5.PODR.BIT.B0 = ~PORT5.PODR.BIT.B0;
		PORT5.PODR.BIT.B1 = ~PORT5.PODR.BIT.B1;
		PORT5.PODR.BIT.B2 = ~PORT5.PODR.BIT.B2;
		PORT5.PODR.BIT.B3 = ~PORT5.PODR.BIT.B3;
		PORT5.PODR.BIT.B4 = ~PORT5.PODR.BIT.B4;
		PORT5.PODR.BIT.B5 = ~PORT5.PODR.BIT.B5;
		PORTC.PODR.BIT.B0 = ~PORTC.PODR.BIT.B0;
		PORTC.PODR.BIT.B1 = ~PORTC.PODR.BIT.B1;
		PORTC.PODR.BIT.B2 = ~PORTC.PODR.BIT.B2;
		PORTC.PODR.BIT.B3 = ~PORTC.PODR.BIT.B3;
		PORTC.PODR.BIT.B4 = ~PORTC.PODR.BIT.B4;
		PORTC.PODR.BIT.B5 = ~PORTC.PODR.BIT.B5;
		PORT9.PODR.BIT.B2 = ~PORT9.PODR.BIT.B2;
		g_sw_int_flg = 0;
	}
	else
	{
	}

}
