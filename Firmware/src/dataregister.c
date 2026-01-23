/*
 * dataregister.c
 *
 *  Created on: 2021/08/25
 *      Author: tomoya723
 *  Modified: 2026/01/15 - Issue #65: CAN設定のカスタム化対応
 */

#include <can.h>
#include "settings.h"			// FULLMONI-WIDE setting parameter
#include "dataregister.h"
#include "lib_table.h"
#include "lib_general.h"
#include "param_storage.h"      /* CAN設定 (Issue #65) */
#include "master_warning.h"    /* Issue #50: マスターワーニング */
#include "speaker.h"           /* 警告音再生 */
#include "GUI.h"               /* emWin GUI関数 */
#include "TEXT.h"              /* TEXT Widget関数 */
#include "WM.h"                /* Window Manager関数 */
#include "../aw002/Source/Generated/Resource.h"  /* AppWizard リソース定義 */
#include "../aw002/Source/Generated/ID_SCREEN_01a.h"  /* ID_TEXT_ACC 定義 */

#define PI 3.1415923

float table_fuel_resistance[9]	= {   0.0,   7.0,  15.0,  33.0,  50.0,  70.0,  95.0, 1000.0};
float table_fuel_calclevel[9]	= { 100.0, 100.0,  82.8,  50.0,  26.6,   8.5,   0.0,    0.0};

const table2D fuel_sender_to_LEVEL_fl_fl = {8, table_fuel_calclevel, table_fuel_resistance};

const float table_tyre_spec[3]			= { 195.0,  50.0,  15.0};
const float table_gear_ratio[6]			= { 3.760, 2.269, 1.645, 1.257, 1.000, 0.843};
unsigned int table_gear_ratio_range[7]	= {   0.0,   0.0,   0.0,   0.0,   0.0,   0.0,   0.0};
const float table_final_gear_ratio		= 4.300;
float fuel_per;
unsigned int gear, gear_pos;

static APPW_PARA_ITEM aPara0[6] = {0};
static APPW_PARA_ITEM aPara1[6] = {0};
static APPW_PARA_ITEM aPara2[6] = {0};
static APPW_PARA_ITEM aPara3[6] = {0};
static APPW_PARA_ITEM aPara4[6] = {0};
static APPW_PARA_ITEM aPara5[6] = {0};
static APPW_PARA_ITEM aPara6[6] = {0};
static APPW_PARA_ITEM aPara7[6] = {0};
static APPW_PARA_ITEM aPara8[6] = {0};

/* ============================================================
 * CAN データ変換 (Issue #65)
 * ============================================================ */

/* CANフレーム配列 (can.cから参照) */
extern can_frame_t rx_dataframe1, rx_dataframe2, rx_dataframe3,
                   rx_dataframe4, rx_dataframe5, rx_dataframe6;

/* 物理値格納配列 (ターゲット変数用) */
static float can_values[16];

/**
 * @brief CANデータを物理値に変換（汎用処理）
 * @param field フィールド定義
 * @return 変換後の物理値
 */
static float can_field_to_physical(const CAN_Field_t *field)
{
    can_frame_t *frames[6] = {
        &rx_dataframe1, &rx_dataframe2, &rx_dataframe3,
        &rx_dataframe4, &rx_dataframe5, &rx_dataframe6
    };

    if (field->channel == 0 || field->channel > 6) {
        return 0.0f;
    }

    can_frame_t *frame = frames[field->channel - 1];
    uint32_t raw = 0;
    uint8_t i;

    /* バイト抽出（エンディアン対応） */
    if (field->endian == 0) {  /* Big Endian */
        for (i = 0; i < field->byte_count; i++) {
            raw = (raw << 8) | frame->data[field->start_byte + i];
        }
    } else {  /* Little Endian */
        for (i = 0; i < field->byte_count; i++) {
            raw |= ((uint32_t)frame->data[field->start_byte + i]) << (i * 8);
        }
    }

    /* 符号付き処理 */
    float value;
    if (field->data_type == 1) {  /* Signed */
        if (field->byte_count == 2) {
            if (raw > 32767) {
                value = (float)((int32_t)raw - 65536);
            } else {
                value = (float)raw;
            }
        } else if (field->byte_count == 1) {
            if (raw > 127) {
                value = (float)((int32_t)raw - 256);
            } else {
                value = (float)raw;
            }
        } else {
            value = (float)raw;
        }
    } else {  /* Unsigned */
        value = (float)raw;
    }

    /* 変換計算: (value + offset) * multiplier / divisor */
    value = value + (float)field->offset;
    if (field->divisor != 0) {
        value = value * (float)field->multiplier / (float)field->divisor;
    }

    return value;
}

/**
 * @brief 全CANフィールドを処理して物理値を更新
 */
static void process_can_fields(void)
{
    uint8_t i;
    const CAN_Field_t *field;
    float value;

    for (i = 0; i < CAN_FIELD_MAX; i++) {
        field = &g_can_config.fields[i];

        /* 無効なフィールドはスキップ */
        if (field->channel == 0 || field->target_var == CAN_TARGET_NONE) {
            continue;
        }

        /* チャンネルが有効か確認 */
        if (!g_can_config.channels[field->channel - 1].enabled) {
            continue;
        }

        /* 物理値に変換 */
        value = can_field_to_physical(field);

        /* ターゲット変数に代入 */
        switch (field->target_var) {
        case CAN_TARGET_REV:
            g_CALC_data.rev = (unsigned int)value;
            if (g_CALC_data.rev >= 9000) g_CALC_data.rev = 9000;
            g_CALC_data.rev_angle = 3600 - g_CALC_data.rev * 0.3;
            break;
        case CAN_TARGET_AF:
            g_CALC_data.af = value;
            break;
        case CAN_TARGET_NUM1:  /* 水温 */
            g_CALC_data.num1 = value;
            break;
        case CAN_TARGET_NUM2:  /* 吸気温 */
            g_CALC_data.num2 = value;
            break;
        case CAN_TARGET_NUM3:  /* 油温 */
            g_CALC_data.num3 = value;
            break;
        case CAN_TARGET_NUM4:  /* MAP */
            g_CALC_data.num4 = value;
            smooth(g_CALC_data_sm.num4, 0.3, g_CALC_data.num4);
            break;
        case CAN_TARGET_NUM5:  /* 油圧 */
            g_CALC_data.num5 = value;
            smooth(g_CALC_data_sm.num5, 0.1, g_CALC_data.num5);
            break;
        case CAN_TARGET_NUM6:  /* バッテリー電圧 */
            g_CALC_data.num6 = value;
            break;
        case CAN_TARGET_SPEED:
            g_CALC_data.sp = value;
            break;
        default:
            break;
        }
    }
}

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
	unsigned int i, error;
	float fuel1, fuel2, fuel3, a, b, c;
//	APPW_PARA_ITEM aPara0[6] = {0};
//	APPW_PARA_ITEM aPara1[6] = {0};
//	APPW_PARA_ITEM aPara2[6] = {0};
//	APPW_PARA_ITEM aPara3[6] = {0};
//	APPW_PARA_ITEM aPara4[6] = {0};
//	APPW_PARA_ITEM aPara5[6] = {0};
//	APPW_PARA_ITEM aPara6[6] = {0};
//	APPW_PARA_ITEM aPara7[6] = {0};
//	APPW_PARA_ITEM aPara8[6] = {0};

	/* Issue #65: CAN設定に基づく汎用変換処理 */
	process_can_fields();

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

	fuel_per = table2D_getValue(&fuel_sender_to_LEVEL_fl_fl, fuel3);
	guard(fuel_per,100,0);
//	if(fuel4 <   0) fuel4 =   0;
//	if(fuel4 > 100) fuel4 = 100;

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

	APPW_SetVarData(ID_VAR_AD1, g_CALC_data.AD1 * 10);
	APPW_SetVarData(ID_VAR_AD2, g_CALC_data.AD2 * 10);
	APPW_SetVarData(ID_VAR_AD3, g_CALC_data.AD3 * 10);
	APPW_SetVarData(ID_VAR_AD4, g_CALC_data.AD4 * 10);
	aPara0[0].v = 0;																			APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_00 , APPW_JOB_SETVIS, aPara0);	// Master Warning
	aPara1[0].v = 0;																			APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_01 , APPW_JOB_SETVIS, aPara1);	// Oil Warning
	if(g_CALC_data.num1 >  60)	{	aPara2[0].v = 0;	} else {		aPara2[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_02L, APPW_JOB_SETVIS, aPara2);	// Water Temp Warning Low
	if(g_CALC_data.num1 < 100)	{	aPara3[0].v = 0;	} else {		aPara3[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_02H, APPW_JOB_SETVIS, aPara3);	// Water Temp Warning High
	if(g_CALC_data.AD3 >  100)	{	aPara4[0].v = 0;	} else {		aPara4[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_03 , APPW_JOB_SETVIS, aPara4);	// EXtemp Warning
	if(g_CALC_data.AD1 > 1000)	{	aPara5[0].v = 0;	} else {		aPara5[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_04 , APPW_JOB_SETVIS, aPara5);	// Battery Warning
	if(g_CALC_data.AD2 >  100)	{	aPara6[0].v = 0;	} else {		aPara6[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_05 , APPW_JOB_SETVIS, aPara6);	// Break Warning
//	if(g_CALC_data.AD4 >  100)	{	aPara7[0].v = 0;	} else {		aPara7[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_06 , APPW_JOB_SETVIS, aPara7);	// Belts Warning
	if(g_CALC_data.AD4 >  150)	{	aPara7[0].v = 0;	} else {		aPara7[0].v = 1;	}	APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_06 , APPW_JOB_SETVIS, aPara7);	// Belts Warning #issue8暫定
	if(fuel_per > 10)	{	aPara8[0].v = 0;	} else if (fuel_per < 5)	{ aPara8[0].v = 1;	}		APPW_DoJob(ID_SCREEN_Telltale, ID_ICON_07 , APPW_JOB_SETVIS, aPara8);	// Fuel Empty


//	sprintf((void *) g_CALC_data.str_time,"%2x:%02x:%02x", (RTC.RHRCNT.BYTE & 0x3F), RTC.RMINCNT.BYTE, RTC.RSECCNT.BYTE);
	sprintf((void *) g_CALC_data.str_time,"%2x:%02x", (RTC.RHRCNT.BYTE & 0x3F), RTC.RMINCNT.BYTE);
	APPW_SetText(ID_SCREEN_01a,ID_NUM_TIME,	(void *) g_CALC_data.str_time	);
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

void data_setLCD10ms(void)
{
	// Set value to appWizard variables.

	APPW_SetVarData(ID_VAR_AF, g_CALC_data.af);
	APPW_SetVarData(ID_VAR_REV, g_CALC_data.rev);
}

void data_setLCD50ms(void)
{
	APPW_SetVarData(ID_VAR_04, g_CALC_data_sm.num4); // MAP
}

/* Issue #50: マスターワーニング表示状態 */
static uint8_t s_warning_displayed = 0;

void data_setLCD100ms(void)
{
	/* Issue #50: マスターワーニング処理 */
	master_warning_check();
	master_warning_update_display();  /* 警告音再生（立ち上がり検出）*/
	
	if (master_warning_is_active()) {
		/* 警告発報中は表示ON */
		if (!s_warning_displayed) {
			/* 警告音を再生（最初の1回だけ）*/
			speaker_play_warning();
			
			/* テキストと背景色を先に設定してから表示ONにする */
			WM_HWIN hWin = WM_GetDialogItem(ID_SCREEN_01a_RootInfo.hWin, ID_TEXT_ACC);
			if (hWin) {
				const char *msg = master_warning_get_message();
				if (msg != NULL && msg[0] != '\0') {
					TEXT_SetText(hWin, msg);
				}
				TEXT_SetBkColor(hWin, 0xFFFF0000);  /* 赤背景 (ARGB) */
			}
			APPW_SetVarData(ID_VAR_PRM, 1);
			s_warning_displayed = 1;
		}
		/* 
		 * Note: 複数警告の表示切り替えは廃止
		 * emWinはリエントラントでないため、タイマー割り込みから
		 * TEXT_SetTextを繰り返し呼ぶとGUIが破壊される
		 * 将来的にはメインループでGUI更新を行う設計に変更が必要
		 */
	} else {
		/* 警告解除 */
		if (s_warning_displayed) {
			/* 背景色を緑に戻す（パラメータモード用）*/
			WM_HWIN hWin = WM_GetDialogItem(ID_SCREEN_01a_RootInfo.hWin, ID_TEXT_ACC);
			if (hWin) {
				TEXT_SetBkColor(hWin, 0xFF00AA00);  /* 緑背景 (ARGB) */
			}
			s_warning_displayed = 0;
		}
		if (g_system_mode == MODE_NORMAL) {
			APPW_SetVarData(ID_VAR_PRM, 0);
		}
	}

	APPW_SetVarData(ID_VAR_01, g_CALC_data.num1); //WaterTemp
	APPW_SetVarData(ID_VAR_02, g_CALC_data.num2); //IAT
	APPW_SetVarData(ID_VAR_03, g_CALC_data.num3); //OIL temp
	APPW_SetVarData(ID_VAR_05, g_CALC_data_sm.num5); //Oilpressure
	APPW_SetVarData(ID_VAR_06, g_CALC_data.num6); //Battv// Set value to appWizard variables.

	APPW_SetVarData(ID_VAR_GEAR, gear_pos);
	APPW_SetVarData(ID_VAR_BATT, g_CALC_data.bt * 10);
	APPW_SetVarData(ID_VAR_FL1, gear * 10);
	APPW_SetVarData(ID_VAR_BL1, g_CALC_data.AD5 * 10);
	APPW_SetVarData(ID_VAR_SPEED, g_CALC_data.sp * 1.06);
	APPW_SetVarData(ID_VAR_ODO, g_CALC_data.odo);
	APPW_SetVarData(ID_VAR_TRIP, g_CALC_data.trip * 10);
	APPW_SetVarData(ID_VAR_FUEL, fuel_per);

}

