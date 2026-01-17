/*
 * speaker.h
 *
 * スピーカ出力モジュール（DA1 + LM4861M）
 * - 単音（ビープ）出力
 * - 警告音パターン
 *
 *  Created on: 2026/01/17
 *      Author: FULLMONI-WIDE Project
 */

#ifndef SPEAKER_H_
#define SPEAKER_H_

#include <stdint.h>
#include <stdbool.h>

/* ============================================================
 * 定数定義
 * ============================================================ */

/* DA出力範囲（12bit: 0-4095） */
#define SPEAKER_DA_MAX          (4095U)
#define SPEAKER_DA_CENTER       (2048U)  /* 無音時の中点電圧 */
#define SPEAKER_DA_MIN          (0U)

/* 音量レベル（0-100%） */
#define SPEAKER_VOLUME_MAX      (100U)
#define SPEAKER_VOLUME_DEFAULT  (50U)
#define SPEAKER_VOLUME_MIN      (0U)

/* 周波数範囲（Hz） */
#define SPEAKER_FREQ_MIN        (100U)
#define SPEAKER_FREQ_MAX        (4000U)
#define SPEAKER_FREQ_DEFAULT    (1000U)

/* サンプリングレート（Hz）- 10msスケジューラから呼び出し */
#define SPEAKER_SAMPLE_RATE     (100U)   /* 100Hz = 10ms周期 */

/* ============================================================
 * 警告音パターン定義
 * ============================================================ */
typedef enum {
    SPEAKER_PATTERN_NONE = 0,       /* 無音 */
    SPEAKER_PATTERN_BEEP_SHORT,     /* 短いビープ（100ms） */
    SPEAKER_PATTERN_BEEP_LONG,      /* 長いビープ（500ms） */
    SPEAKER_PATTERN_SHIFT_WARNING,  /* シフト警告（連続ビープ） */
    SPEAKER_PATTERN_OVERHEAT,       /* オーバーヒート警告（断続音） */
    SPEAKER_PATTERN_LOW_FUEL,       /* 燃料警告（2回ビープ） */
    SPEAKER_PATTERN_STARTUP,        /* 起動音 */
    SPEAKER_PATTERN_MAX
} speaker_pattern_t;

/* ============================================================
 * 構造体定義
 * ============================================================ */
typedef struct {
    uint8_t  volume;        /* 音量 0-100 */
    uint16_t frequency;     /* 周波数 Hz */
    bool     enabled;       /* スピーカ有効/無効 */
} speaker_config_t;

/* ============================================================
 * グローバル変数（外部参照用）
 * ============================================================ */
extern speaker_config_t g_speaker_config;

/* ============================================================
 * 関数プロトタイプ
 * ============================================================ */

/**
 * @brief スピーカモジュール初期化
 */
void speaker_init(void);

/**
 * @brief スピーカ有効化
 */
void speaker_enable(void);

/**
 * @brief スピーカ無効化
 */
void speaker_disable(void);

/**
 * @brief 音量設定
 * @param volume 音量（0-100）
 */
void speaker_set_volume(uint8_t volume);

/**
 * @brief 周波数設定
 * @param freq_hz 周波数（Hz）
 */
void speaker_set_frequency(uint16_t freq_hz);

/**
 * @brief 単音出力開始
 * @param freq_hz 周波数（Hz）
 * @param duration_ms 継続時間（ms）、0=無限
 */
void speaker_beep(uint16_t freq_hz, uint16_t duration_ms);

/**
 * @brief 音出力停止
 */
void speaker_stop(void);

/**
 * @brief 警告音パターン再生
 * @param pattern パターン種別
 */
void speaker_play_pattern(speaker_pattern_t pattern);

/**
 * @brief 周期処理（10msスケジューラから呼び出し）
 *        波形生成とDA出力を行う
 */
void speaker_process(void);

/**
 * @brief 再生中かどうか
 * @return true: 再生中、false: 停止中
 */
bool speaker_is_playing(void);

/**
 * @brief TPU0 TGIA割り込みハンドラ（Config_TPU0_user.cから呼び出し）
 */
void speaker_tpu0_isr(void);

#endif /* SPEAKER_H_ */
