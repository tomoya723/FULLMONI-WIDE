/*
 * pulse_out.h
 *
 *  Issue #115: EPS駆動用 タコ・車速パルス出力
 *
 *  Created on: 2026/03/02
 *
 *  出力ピン:
 *    PC2 (DO8 / DO2-0) - タコパルス  (JIS D 5601: 2パルス/rev)
 *    PC3 (DO9 / DO2-1) - 車速パルス  (PULSE_PER_KM = 2548パルス/km)
 *
 *  トランジスタバッファ Q5/Q6 経由でオープンコレクタ 12V 出力
 */

#ifndef PULSE_OUT_H_
#define PULSE_OUT_H_

#include <stdint.h>

/* ---------------------------------------------------------------
 * パルス仕様定数  (JIS D 5601:1992)
 * --------------------------------------------------------------- */

/** タコ: パルス数 / rev */
#define TACHO_PULSE_PER_REV     (2u)

/** タコ: 上限クリップ [rpm] */
#define TACHO_MAX_RPM           (5000u)

/** ベースタイマー周波数 [Hz]  (10kHz = 100µs 周期) */
#define PULSE_OUT_TIMER_HZ      (10000u)

/**
 * タコ ハーフ周期係数
 *   half_count = TACHO_HALF_FACTOR / RPM
 *   = (PULSE_OUT_TIMER_HZ * 60) / (TACHO_PULSE_PER_REV * 2)
 *   = 10000 * 60 / (2 * 2) = 150000
 */
#define TACHO_HALF_FACTOR       (150000UL)

/**
 * 車速 ハーフ周期係数
 *   half_count = SPEED_HALF_FACTOR / (speed_kmh * PULSE_PER_KM)
 *   = (PULSE_OUT_TIMER_HZ * 3600) / (PULSE_PER_KM * 2)  ... 整数化のため分子をまとめる
 *   SPEED_HALF_FACTOR = PULSE_OUT_TIMER_HZ * 3600 / 2 = 18000000
 *   → half_count = 18000000 / (speed_kmh * PULSE_PER_KM / 3600 * 3600 / 1)
 *                = 18000000 * 3600 / (speed_kmh * PULSE_PER_KM)  ... 浮動小数演算で計算
 */
#define SPEED_HALF_FACTOR       (18000000UL)

/* ---------------------------------------------------------------
 * 公開 API
 * --------------------------------------------------------------- */

/**
 * @brief パルス出力モジュール初期化
 *
 * CMT タイマー（FIT r_cmt_rx）を 10kHz で起動し、
 * PC2/PC3 を LOW 出力として待機状態にする。
 * main() の param_storage_load() 完了後に呼び出すこと。
 */
void pulse_out_init(void);

/**
 * @brief タコパルス周波数を更新する
 *
 * @param rpm  エンジン回転数 [rpm]
 *             0 を渡すとパルス停止（出力 LOW）。
 *             TACHO_MAX_RPM 超は TACHO_MAX_RPM にクリップ。
 */
void pulse_out_set_tacho(uint16_t rpm);

/**
 * @brief 車速パルス周波数を更新する
 *
 * @param kmh  車速 [km/h]（0.0f でパルス停止）
 */
void pulse_out_set_speed(float kmh);

#endif /* PULSE_OUT_H_ */
