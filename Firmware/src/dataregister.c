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
#include "ui_binding/ui_dashboard.h"

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

/* CAN車速受信フラグ（MTU割り込みから参照） */
unsigned char g_speed_from_can = 0;

/* CAN車速→仮想パルス変換用: 端数繰り越し */
static float s_pulse_fraction = 0.0f;

/* 仮想パルス換算定数: PULSE_PER_KM * (sample_period / 3600)
 * = 2548 * (0.05 / 3600) = 0.035389 */
#define CAN_SPEED_PULSE_FACTOR  (PULSE_PER_KM * 0.05f / 3600.0f)

/**
 * @brief CAN車速から仮想パルスを積算する（50ms周期で呼び出し）
 *
 * CAN経由で車速を受信している場合、車速[km/h]を仮想パルス数に変換し
 * 既存のパルス積算変数(sp_int)に加算する。
 * MTU8パルス割り込みの代替処理として機能する。
 * 端数は次回呼び出しに繰り越し、長距離走行での累積誤差を防止する。
 */
void can_speed_to_pulse(void)
{
	float pulse_f;
	unsigned long pulse_int;

	/* CAN車速が無効な場合は何もしない */
	if (!g_speed_from_can) {
		return;
	}

	/* 異常値ガード: 負値や非現実的な車速は積算しない */
	if (g_CALC_data.sp <= 0.0f || g_CALC_data.sp > 400.0f) {
		return;
	}

	/* 車速[km/h] → 50msあたりのパルス数 */
	pulse_f = g_CALC_data.sp * CAN_SPEED_PULSE_FACTOR;

	/* 前回の端数を加算 */
	pulse_f += s_pulse_fraction;

	/* 整数部をパルスカウンタに加算 */
	pulse_int = (unsigned long)pulse_f;
	s_pulse_fraction = pulse_f - (float)pulse_int;

	/* sp_int に加算（既存のODO/TRIP/EEPROM処理はそのまま動作） */
	sp_int += pulse_int;
}



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
	float gear_ratio[6];  /* g_paramから取得したギア比（float変換後） */
	float final_gear;
	int i;

	/* CAN車速受信判定（イニシャル時に1回だけ判定） */
	g_speed_from_can = 0;
	for (i = 0; i < CAN_FIELD_MAX; i++) {
		if (g_can_config.fields[i].target_var == CAN_TARGET_SPEED &&
		    g_can_config.fields[i].channel != 0 &&
		    g_can_config.fields[i].channel <= CAN_CHANNEL_MAX &&
		    g_can_config.channels[g_can_config.fields[i].channel - 1].enabled) {
			g_speed_from_can = 1;
			break;
		}
	}

	// gear ratio table init (g_paramから取得、1000で割ってfloatに変換)
	for (i = 0; i < 6; i++) {
		gear_ratio[i] = g_param.gear_ratio[i] / 1000.0f;
	}
	final_gear = g_param.final_gear_ratio / 1000.0f;

	// タイヤ外周計算
	g_CALC_data.TyreCirc = ((g_param.tyre_width * g_param.tyre_aspect / 100.0f) * 2 + g_param.tyre_rim * 25.4f) * PI / 1000.0f;

	// ギア比判定レンジを計算（隣接ギア間の中間値を閾値とする）
	delta = (gear_ratio[0] - gear_ratio[1]) / 2;
	table_gear_ratio_range[0] = (unsigned int)((gear_ratio[0] + delta) * 1000);
	table_gear_ratio_range[1] = (unsigned int)((gear_ratio[0] - delta) * 1000);

	delta = (gear_ratio[1] - gear_ratio[2]) / 2;
	table_gear_ratio_range[2] = (unsigned int)((gear_ratio[1] - delta) * 1000);

	delta = (gear_ratio[2] - gear_ratio[3]) / 2;
	table_gear_ratio_range[3] = (unsigned int)((gear_ratio[2] - delta) * 1000);

	delta = (gear_ratio[3] - gear_ratio[4]) / 2;
	table_gear_ratio_range[4] = (unsigned int)((gear_ratio[3] - delta) * 1000);

	// 6速が設定されている場合のみ5-6速間の閾値を計算
	if (gear_ratio[5] > 0.0f) {
		delta = (gear_ratio[4] - gear_ratio[5]) / 2;
		table_gear_ratio_range[5] = (unsigned int)((gear_ratio[4] - delta) * 1000);
		table_gear_ratio_range[6] = (unsigned int)((gear_ratio[5] - delta) * 1000);
	} else {
		// 5速設定の場合：5速の下限を適切に設定（6速判定なし）
		delta = (gear_ratio[3] - gear_ratio[4]) / 2;  /* 4-5速間のdeltaを流用 */
		table_gear_ratio_range[5] = (unsigned int)((gear_ratio[4] - delta) * 1000);
		table_gear_ratio_range[6] = 0;  /* 6速判定無効化 */
	}
}

void data_store(void)
{
	unsigned int i, error;
	float fuel1, fuel2, fuel3, a, b, c;

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
	g_CALC_data.odo = sp_int / PULSE_PER_KM;
	g_CALC_data.trip = ((float)(sp_int - tr_int)) / PULSE_PER_KM;

	// gear ratio & shift position
	gear = (unsigned int)(g_CALC_data.rev * g_CALC_data.TyreCirc * 60 / g_CALC_data.sp / (g_param.final_gear_ratio / 1000.0f));
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
	else if	(( g_param.gear_ratio[5] > 0 ) &&  /* 6速ギア比が設定されている場合のみ判定 */
			 ( table_gear_ratio_range[5] >= gear ) && ( table_gear_ratio_range[6] <= gear ))
	{
		gear_pos = 6;
	}
	else
	{
		gear_pos = 0;
	}

//	sprintf((void *) g_CALC_data.str_time,"%2x:%02x:%02x", (RTC.RHRCNT.BYTE & 0x3F), RTC.RMINCNT.BYTE, RTC.RSECCNT.BYTE);
	sprintf((void *) g_CALC_data.str_time,"%2x:%02x", (RTC.RHRCNT.BYTE & 0x3F), RTC.RMINCNT.BYTE);
	// SW Input
	if(g_sw_int_flg == 1)
	{
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
}

void data_setLCD50ms(void)
{
}

/* Issue #50: マスターワーニング表示状態 */
static uint8_t s_warning_displayed = 0;
static uint8_t s_warning_gui_update_needed = 0;  /* GUI更新要求フラグ */
static uint8_t s_warning_sound_cooldown = 0;     /* 警告音クールダウン（100ms単位）*/

/* 警告音の最小間隔（100ms単位、30 = 3秒）*/
#define WARNING_SOUND_COOLDOWN  30

/**
 * @brief マスターワーニングGUI更新（メインループから呼び出し）
 */
void master_warning_gui_update(void)
{
	if (!s_warning_gui_update_needed) {
		return;
	}
	s_warning_gui_update_needed = 0;

	if (master_warning_is_active()) {
		const char *msg = master_warning_get_message();
		ui_dashboard_set_notify(msg, 0xFF0000u);  /* 赤背景 */
		s_warning_displayed = 1;
	} else {
		if (s_warning_displayed) {
			ui_dashboard_clear_notify();
			s_warning_displayed = 0;
		}
	}
}

void data_setLCD100ms(void)
{
	/* 警告音クールダウン処理 */
	if (s_warning_sound_cooldown > 0) {
		s_warning_sound_cooldown--;
	}

	/* パラメータモード中は警告チェックをスキップ */
	if (g_system_mode == MODE_PARAM) {
		return;
	}

	/* Issue #50: マスターワーニング処理（タイマー割り込みコンテキスト）*/
	master_warning_check();

	if (master_warning_is_active()) {
		/* 警告音をクールダウン間隔で繰り返し再生 */
		if (s_warning_sound_cooldown == 0) {
			if (g_can_config.sound_enabled) {
				speaker_play_warning();
			}
			s_warning_sound_cooldown = WARNING_SOUND_COOLDOWN;
		}
		/* GUI更新は常に行う（値がリアルタイムで変わる場合に対応）*/
		s_warning_gui_update_needed = 1;
	} else {
		/* 警告解除 */
		if (s_warning_displayed) {
			s_warning_gui_update_needed = 1;
		}
	}

}

