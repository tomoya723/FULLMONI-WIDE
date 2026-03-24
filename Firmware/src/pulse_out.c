/*
 * pulse_out.c
 *
 *  Issue #115: EPS駆動用 タコ・車速パルス出力
 *
 *  Created on: 2026/03/02
 *
 *  タイマー: FIT CMT API (r_cmt_rx) による 10kHz (100µs) 周期割り込み
 *  出力ピン:
 *    PC2 (DO8 / DO2-0) - タコパルス
 *    PC3 (DO9 / DO2-1) - 車速パルス
 */

#include "platform.h"               /* RX72N レジスタ定義 */
#include "r_cmt_rx_if.h"            /* R_CMT_CreatePeriodic */
#include "param_storage.h"          /* PULSE_PER_KM */
#include "pulse_out.h"

/* ---------------------------------------------------------------
 * 内部変数
 * --------------------------------------------------------------- */

/** CMT チャンネル番号（API が自動割り当て） */
static uint32_t s_cmt_handle;

/* --- タコ --- */
/** ハーフ周期カウント値 [100µs 単位]。0 = 停止 */
static volatile uint32_t s_tacho_half = 0u;
/** 現在のカウント */
static volatile uint32_t s_tacho_cnt  = 0u;
/** 現在の出力レベル */
static volatile uint8_t  s_tacho_level = 0u;

/* --- 車速 --- */
/** ハーフ周期カウント値 [100µs 単位]。0 = 停止 */
static volatile uint32_t s_speed_half = 0u;
/** 現在のカウント */
static volatile uint32_t s_speed_cnt  = 0u;
/** 現在の出力レベル */
static volatile uint8_t  s_speed_level = 0u;

/* ---------------------------------------------------------------
 * 出力ポートアクセスマクロ
 *   PC2 = タコ、PC3 = 車速
 * --------------------------------------------------------------- */
#define TACHO_OUT(v)  (PORTC.PODR.BIT.B2 = (v))
#define SPEED_OUT(v)  (PORTC.PODR.BIT.B3 = (v))

/* ---------------------------------------------------------------
 * 10kHz 割り込みハンドラ（100µs 周期）
 * --------------------------------------------------------------- */
static void pulse_out_isr(void * pdata)
{
    (void)pdata;

    /* --- タコ --- */
    if (s_tacho_half == 0u)
    {
        /* 停止中: LOW 固定 */
        TACHO_OUT(0u);
        s_tacho_cnt   = 0u;
        s_tacho_level = 0u;
    }
    else
    {
        s_tacho_cnt++;
        if (s_tacho_cnt >= s_tacho_half)
        {
            s_tacho_cnt   = 0u;
            s_tacho_level ^= 1u;
            TACHO_OUT(s_tacho_level);
        }
    }

    /* --- 車速 --- */
    if (s_speed_half == 0u)
    {
        /* 停止中: LOW 固定 */
        SPEED_OUT(0u);
        s_speed_cnt   = 0u;
        s_speed_level = 0u;
    }
    else
    {
        s_speed_cnt++;
        if (s_speed_cnt >= s_speed_half)
        {
            s_speed_cnt   = 0u;
            s_speed_level ^= 1u;
            SPEED_OUT(s_speed_level);
        }
    }
}

/* ---------------------------------------------------------------
 * 公開 API 実装
 * --------------------------------------------------------------- */

/**
 * @brief パルス出力モジュール初期化
 */
void pulse_out_init(void)
{
    /* 出力 LOW で待機（Config_PORT.c で既に GPIO 出力初期化済み） */
    TACHO_OUT(0u);
    SPEED_OUT(0u);

    s_tacho_half  = 0u;
    s_tacho_cnt   = 0u;
    s_tacho_level = 0u;
    s_speed_half  = 0u;
    s_speed_cnt   = 0u;
    s_speed_level = 0u;

    /* 10kHz CMT タイマー起動（CMT0 は LVGL tick で使用中 → CMT1 が自動選択される） */
    R_CMT_CreatePeriodic(PULSE_OUT_TIMER_HZ, pulse_out_isr, &s_cmt_handle);
}

/**
 * @brief タコパルス周波数を更新する
 *
 * @param rpm  エンジン回転数 [rpm]。0 でパルス停止。
 */
void pulse_out_set_tacho(uint16_t rpm)
{
    uint32_t half;

    if (rpm == 0u)
    {
        s_tacho_half = 0u;
        return;
    }

    /* 上限クリップ */
    if (rpm > TACHO_MAX_RPM)
    {
        rpm = TACHO_MAX_RPM;
    }

    /*
     * half_count = TACHO_HALF_FACTOR / rpm
     *   TACHO_HALF_FACTOR = PULSE_OUT_TIMER_HZ * 60 / (TACHO_PULSE_PER_REV * 2)
     *                     = 10000 * 60 / 4 = 150000
     */
    half = TACHO_HALF_FACTOR / (uint32_t)rpm;

    s_tacho_half = half;
}

/**
 * @brief 車速パルス周波数を更新する
 *
 * @param kmh  車速 [km/h]。0.0f でパルス停止。
 */
void pulse_out_set_speed(float kmh)
{
    uint32_t half;

    if (kmh < 0.5f)
    {
        s_speed_half = 0u;
        return;
    }

    /*
     * f_speed    = kmh * PULSE_PER_KM / 3600       [Hz]
     * half_count = PULSE_OUT_TIMER_HZ / (2 * f_speed)
     *            = PULSE_OUT_TIMER_HZ * 3600 / (2 * kmh * PULSE_PER_KM)
     *            = SPEED_HALF_FACTOR / (kmh * PULSE_PER_KM)
     *   SPEED_HALF_FACTOR = 10000 * 1800 = 18,000,000
     *
     *   200km/h: 18000000 / (200 * 2548) ≈ 35  [100µs]
     *     1km/h: 18000000 / (  1 * 2548) ≈ 7065 [100µs]
     */
    half = (uint32_t)(18000000.0f / (kmh * (float)PULSE_PER_KM));

    /* 最大値ガード（0 除算・異常値による暴走防止） */
    if (half == 0u)
    {
        half = 1u;
    }

    s_speed_half = half;
}
