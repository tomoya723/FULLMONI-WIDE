/*
 * speaker.c
 *
 * スピーカ出力モジュール（TPU0 + DTC + DA1 + LM4861M）
 * - 正弦波による単音出力（ハードウェア生成、CPU負荷ゼロ）
 * - 警告音パターン再生
 *
 *  Created on: 2026/01/17
 *      Author: FULLMONI-WIDE Project
 */

#include "speaker.h"
#include "platform.h"  /* iodefine.h経由でレジスタ定義を取得 */
#include "Config_TPU0.h"
#include "r_dtc_rx_if.h"

/* ============================================================
 * 内部定数
 * ============================================================ */

/* 正弦波テーブルサイズ */
#define SPEAKER_TABLE_SIZE      (32U)

/* PCLKB = 60MHz (想定値、実際の設定に合わせて調整) */
#define PCLKB_HZ                (60000000UL)

/* パターン定義用構造体 */
typedef struct {
    uint16_t freq_hz;       /* 周波数 */
    uint16_t on_time_ms;    /* ON時間 */
    uint16_t off_time_ms;   /* OFF時間 */
    uint8_t  repeat_count;  /* 繰り返し回数（0=無限） */
} pattern_def_t;

/* ============================================================
 * 正弦波テーブル（32サンプル、12bit、中心2048、振幅2047）
 * ============================================================ */
static const uint16_t s_sine_table[SPEAKER_TABLE_SIZE] = {
    2048, 2447, 2831, 3185, 3496, 3750, 3939, 4056,
    4095, 4056, 3939, 3750, 3496, 3185, 2831, 2447,
    2048, 1649, 1265,  911,  600,  346,  157,   40,
       0,   40,  157,  346,  600,  911, 1265, 1649
};

/* 正弦波テーブルを直接DMA転送（RAM使用量ゼロ） */

/* ============================================================
 * 割り込みハンドラ用変数
 * ============================================================ */
static volatile uint8_t s_sample_index = 0;
static volatile bool s_speaker_active = false;

/* ============================================================
 * 警告音パターンテーブル
 * ============================================================ */
static const pattern_def_t s_patterns[SPEAKER_PATTERN_MAX] = {
    /* SPEAKER_PATTERN_NONE */
    { 0,    0,    0,    0 },
    /* SPEAKER_PATTERN_BEEP_SHORT */
    { 1000, 100,  0,    1 },
    /* SPEAKER_PATTERN_BEEP_LONG */
    { 1000, 500,  0,    1 },
    /* SPEAKER_PATTERN_SHIFT_WARNING */
    { 2000, 50,   50,   0 },    /* 連続ビープ（無限） */
    /* SPEAKER_PATTERN_OVERHEAT */
    { 1500, 200,  200,  0 },    /* 断続音（無限） */
    /* SPEAKER_PATTERN_LOW_FUEL */
    { 800,  150,  100,  2 },    /* 2回ビープ */
    /* SPEAKER_PATTERN_STARTUP */
    { 1200, 150,  0,    1 },    /* 起動音 */
};

/* ============================================================
 * モジュール内部変数
 * ============================================================ */

/* 設定 */
speaker_config_t g_speaker_config = {
    .volume    = SPEAKER_VOLUME_DEFAULT,
    .frequency = SPEAKER_FREQ_DEFAULT,
    .enabled   = true
};

/* 再生状態 */
static struct {
    volatile bool     playing;          /* 再生中フラグ */
    uint16_t frequency;                 /* 現在の周波数 */
    uint16_t duration_ms;               /* 残り再生時間（0=無限） */
    uint16_t elapsed_ms;                /* 経過時間 */

    /* パターン再生用 */
    speaker_pattern_t current_pattern;
    uint8_t  pattern_repeat;            /* 残り繰り返し回数 */
    uint16_t pattern_timer;             /* パターン用タイマ */
    bool     pattern_on_phase;          /* ON/OFFフェーズ */
} s_state = {0};

/* ============================================================
 * 内部関数プロトタイプ
 * ============================================================ */
static void speaker_setup_da(void);
static void speaker_setup_tpu0(uint16_t freq_hz);
static void speaker_pattern_process(void);

/* ============================================================
 * 公開関数
 * ============================================================ */

/**
 * @brief スピーカモジュール初期化
 */
void speaker_init(void)
{
    /* DA1 初期化 */
    speaker_setup_da();

    /* ===== TPU0割り込み方式（軽量） ===== */
    
    /* モジュールストップ解除 */
    SYSTEM.PRCR.WORD = 0xA502U;
    MSTP(TPU0) = 0U;
    SYSTEM.PRCR.WORD = 0xA500U;
    
    /* TPU0停止 */
    TPUA.TSTR.BIT.CST0 = 0U;
    
    /* TPU0設定 */
    TPU0.TCR.BYTE = 0x20U;  /* PCLK/1, TGRAコンペアマッチでクリア */
    TPU0.TIER.BYTE = 0x01U; /* TGIEA有効 */
    TPU0.TGRA = 1562 - 1;   /* 38.4kHz (60MHz/38400) → 1200Hz正弦波 */
    TPU0.TCNT = 0U;
    
    /* 割り込み優先度設定 */
    IPR(PERIB, INTB130) = 14;  /* 高優先度 */
    IR(PERIB, INTB130) = 0U;   /* フラグクリア */
    IEN(PERIB, INTB130) = 1U;  /* 割り込み許可 */
    
    /* スピーカ有効化 */
    s_sample_index = 0;
    s_speaker_active = true;
    
    /* TPU0開始 */
    TPUA.TSTR.BIT.CST0 = 1U;
    
    /* 150ms待ち */
    for (volatile uint32_t d = 0; d < 3600000; d++) { __asm("nop"); }
    
    /* 停止 */
    s_speaker_active = false;
    TPUA.TSTR.BIT.CST0 = 0U;
    IEN(PERIB, INTB130) = 0U;
    DA.DADR1 = SPEAKER_DA_CENTER;

    g_speaker_config.enabled = false;
    s_state.playing = false;
    s_state.current_pattern = SPEAKER_PATTERN_NONE;
}

/**
 * @brief TPU0 TGIA割り込みハンドラ（Config_TPU0_user.cから呼び出し）
 */
void speaker_tpu0_isr(void)
{
    if (s_speaker_active) {
        DA.DADR1 = s_sine_table[s_sample_index];
        s_sample_index = (s_sample_index + 1) & 0x1F;  /* 0-31 循環 */
    }
}

/**
 * @brief スピーカ無効化
 */
void speaker_disable(void)
{
    g_speaker_config.enabled = false;
    speaker_stop();
}

/**
 * @brief 音量設定
 * @param volume 音量（0-100）
 * @note 現在の実装では音量調整は未サポート（固定音量）
 */
void speaker_set_volume(uint8_t volume)
{
    if (volume > SPEAKER_VOLUME_MAX) {
        volume = SPEAKER_VOLUME_MAX;
    }
    g_speaker_config.volume = volume;
    /* 音量調整はハードウェアアンプのボリュームで対応 */
}

/**
 * @brief 周波数設定
 * @param freq_hz 周波数（Hz）
 */
void speaker_set_frequency(uint16_t freq_hz)
{
    if (freq_hz < SPEAKER_FREQ_MIN) {
        freq_hz = SPEAKER_FREQ_MIN;
    } else if (freq_hz > SPEAKER_FREQ_MAX) {
        freq_hz = SPEAKER_FREQ_MAX;
    }
    g_speaker_config.frequency = freq_hz;
}

/* DMA方式を使用する場合は1、ソフトウェア方式は0 */
#define USE_DMA_SPEAKER  0

/**
 * @brief 単音出力開始
 * @param freq_hz 周波数（Hz）
 * @param duration_ms 継続時間（ms）、0=無限
 */
void speaker_beep(uint16_t freq_hz, uint16_t duration_ms)
{
    if (!g_speaker_config.enabled) {
        return;
    }

    /* 周波数制限 */
    if (freq_hz < SPEAKER_FREQ_MIN) {
        freq_hz = SPEAKER_FREQ_MIN;
    } else if (freq_hz > SPEAKER_FREQ_MAX) {
        freq_hz = SPEAKER_FREQ_MAX;
    }

    /* 状態設定 */
    s_state.frequency = freq_hz;
    s_state.duration_ms = duration_ms;
    s_state.elapsed_ms = 0;

#if USE_DMA_SPEAKER
    /* DTC方式: TPU0 + DTC */
    dtc_cmd_arg_t dtc_arg;
    dtc_transfer_data_cfg_t dtc_config;  /* ローカル変数 */

    /* TPU0停止 */
    TPUA.TSTR.BIT.CST0 = 0U;

    /* DTC再設定 */
    dtc_config.transfer_mode         = DTC_TRANSFER_MODE_REPEAT;
    dtc_config.data_size             = DTC_DATA_SIZE_WORD;
    dtc_config.src_addr_mode         = DTC_SRC_ADDR_INCR;
    dtc_config.dest_addr_mode        = DTC_DES_ADDR_FIXED;
    dtc_config.repeat_block_side     = DTC_REPEAT_BLOCK_SOURCE;
    dtc_config.chain_transfer_enable = DTC_CHAIN_TRANSFER_DISABLE;
    dtc_config.chain_transfer_mode   = DTC_CHAIN_TRANSFER_CONTINUOUSLY;
    dtc_config.response_interrupt    = DTC_INTERRUPT_AFTER_ALL_COMPLETE;
    dtc_config.source_addr           = (uint32_t)s_sine_table;
    dtc_config.dest_addr             = 0x00088042UL;
    dtc_config.transfer_count        = SPEAKER_TABLE_SIZE;
    dtc_config.block_size            = 0;
    dtc_config.writeback_disable     = DTC_WRITEBACK_ENABLE;
    dtc_config.sequence_end          = DTC_SEQUENCE_TRANSFER_CONTINUE;
    dtc_config.refer_index_table_enable = DTC_REFER_INDEX_TABLE_DISABLE;
    dtc_config.disp_add_enable       = DTC_SRC_ADDR_DISP_ADD_DISABLE;
    R_DTC_Create(DTCE_TPU0_TGI0A, &s_dtc_transfer_data, &dtc_config, 0);

    /* TPU0周期設定 */
    /* サンプリングレート = freq_hz * TABLE_SIZE */
    uint32_t tpu_period = PCLKB_HZ / ((uint32_t)freq_hz * SPEAKER_TABLE_SIZE);
    if (tpu_period < 2) tpu_period = 2;
    if (tpu_period > 65535) tpu_period = 65535;
    TPU0.TGRA = (uint16_t)(tpu_period - 1U);
    TPU0.TCNT = 0U;

    /* DTC起動要因有効化 */
    dtc_arg.act_src = DTCE_TPU0_TGI0A;
    R_DTC_Control(DTC_CMD_ACT_SRC_ENABLE, NULL, &dtc_arg);

    /* DTC開始 */
    R_DTC_Control(DTC_CMD_DTC_START, NULL, NULL);

    /* TPU0開始 */
    TPUA.TSTR.BIT.CST0 = 1U;

    s_state.playing = true;
#else
    /* ソフトウェア方式 */
    uint32_t sample_delay = (PCLKB_HZ * 10UL) / ((uint32_t)freq_hz * SPEAKER_TABLE_SIZE * 32UL);
    uint32_t total_samples = ((uint32_t)duration_ms * freq_hz * SPEAKER_TABLE_SIZE) / 1000;

    for (uint32_t i = 0; i < total_samples; i++) {
        DA.DADR1 = s_sine_table[i % SPEAKER_TABLE_SIZE];
        for (volatile uint32_t d = 0; d < sample_delay; d++) { __asm("nop"); }
    }

    /* 無音に戻す */
    DA.DADR1 = SPEAKER_DA_CENTER;
    s_state.playing = false;
#endif
}

/**
 * @brief 音出力停止
 */
void speaker_stop(void)
{
    dtc_cmd_arg_t dtc_arg;

    s_state.playing = false;
    s_state.current_pattern = SPEAKER_PATTERN_NONE;

    /* TPU0停止 */
    TPUA.TSTR.BIT.CST0 = 0U;

    /* DTC停止 */
    dtc_arg.act_src = DTCE_TPU0_TGI0A;
    R_DTC_Control(DTC_CMD_ACT_SRC_DISABLE, NULL, &dtc_arg);

    /* 中点電圧を出力（無音状態） */
    DA.DADR1 = SPEAKER_DA_CENTER;
}

/**
 * @brief 警告音パターン再生
 * @param pattern パターン種別
 */
void speaker_play_pattern(speaker_pattern_t pattern)
{
    if (!g_speaker_config.enabled) {
        return;
    }

    if (pattern >= SPEAKER_PATTERN_MAX) {
        return;
    }

    if (pattern == SPEAKER_PATTERN_NONE) {
        speaker_stop();
        return;
    }

    const pattern_def_t *p = &s_patterns[pattern];

    s_state.current_pattern = pattern;
    s_state.pattern_repeat = p->repeat_count;
    s_state.pattern_timer = 0;
    s_state.pattern_on_phase = true;

    /* 最初のビープ開始 */
    speaker_beep(p->freq_hz, p->on_time_ms);
}

/**
 * @brief 周期処理（10msスケジューラから呼び出し）
 * @note パターン管理と時間管理のみ。波形生成はハードウェア。
 */
void speaker_process(void)
{
    if (!g_speaker_config.enabled) {
        return;
    }

    /* パターン再生中の処理 */
    if (s_state.current_pattern != SPEAKER_PATTERN_NONE) {
        speaker_pattern_process();
    }

    /* 単音再生の時間管理 */
    if (!s_state.playing) {
        return;
    }

    /* 継続時間チェック */
    if (s_state.duration_ms > 0) {
        s_state.elapsed_ms += 10U;  /* 10ms周期 */
        if (s_state.elapsed_ms >= s_state.duration_ms) {
            /* パターン再生中でなければ停止 */
            if (s_state.current_pattern == SPEAKER_PATTERN_NONE) {
                speaker_stop();
            } else {
                s_state.playing = false;
                R_Config_TPU0_Stop();
                DA.DADR1 = SPEAKER_DA_CENTER;
            }
        }
    }
}

/**
 * @brief 再生中かどうか
 * @return true: 再生中、false: 停止中
 */
bool speaker_is_playing(void)
{
    return s_state.playing || (s_state.current_pattern != SPEAKER_PATTERN_NONE);
}

/* ============================================================
 * 内部関数
 * ============================================================ */

/**
 * @brief DA1 初期化
 */
static void speaker_setup_da(void)
{
    /* DAモジュールのスタンバイ解除 */
    MSTP(DA) = 0U;

    /* ピン設定（P05をDA1として使用） */
    /* MPC書き込み許可 */
    MPC.PWPR.BIT.B0WI = 0U;
    MPC.PWPR.BIT.PFSWE = 1U;

    /* P05をアナログ出力に設定 */
    PORT0.PMR.BIT.B5 = 0U;
    PORT0.PDR.BIT.B5 = 0U;
    MPC.P05PFS.BYTE = 0x80U;  /* ASEL=1 */

    /* MPC書き込み禁止 */
    MPC.PWPR.BIT.PFSWE = 0U;
    MPC.PWPR.BIT.B0WI = 1U;

    /* DA設定 */
    DA.DADPR.BYTE = 0x00U;      /* 右詰め */
    DA.DAADSCR.BYTE = 0x00U;    /* A/D同期なし */
    DA.DACR.BIT.DAE = 0U;       /* 個別制御 */
    DA.DACR.BIT.DAOE1 = 1U;     /* DA1出力有効 */

    /* 中点電圧出力（無音状態） */
    DA.DADR1 = SPEAKER_DA_CENTER;
}

/**
 * @brief TPU0 周期設定（サンプリングタイマ）
 * @param freq_hz 出力周波数（Hz）
 */
static void speaker_setup_tpu0(uint16_t freq_hz)
{
    /* TPU0停止 */
    R_Config_TPU0_Stop();

    /*
     * サンプリング周期計算:
     * 出力周波数 = freq_hz
     * テーブルサイズ = 32
     * サンプリングレート = freq_hz * 32
     * TPU周期 = PCLKB / サンプリングレート
     */
    uint32_t sample_rate = (uint32_t)freq_hz * SPEAKER_TABLE_SIZE;
    uint32_t tpu_period = PCLKB_HZ / sample_rate;

    if (tpu_period < 2) tpu_period = 2;
    if (tpu_period > 65535) tpu_period = 65535;

    TPU0.TGRA = (uint16_t)(tpu_period - 1U);
    TPU0.TCNT = 0U;
}

/* 音量調整は削除（固定音量、ハードウェアアンプで調整可能） */

/**
 * @brief パターン再生処理
 */
static void speaker_pattern_process(void)
{
    const pattern_def_t *p = &s_patterns[s_state.current_pattern];

    s_state.pattern_timer += 10U;  /* 10ms周期 */

    if (s_state.pattern_on_phase) {
        /* ON フェーズ */
        if (s_state.pattern_timer >= p->on_time_ms) {
            s_state.pattern_timer = 0;

            if (p->off_time_ms > 0) {
                /* OFFフェーズへ移行 */
                s_state.pattern_on_phase = false;
                s_state.playing = false;
                R_Config_TPU0_Stop();
                DA.DADR1 = SPEAKER_DA_CENTER;
            } else {
                /* OFFなし → 繰り返しカウント処理 */
                if (s_state.pattern_repeat > 0) {
                    s_state.pattern_repeat--;
                    if (s_state.pattern_repeat == 0) {
                        /* パターン終了 */
                        speaker_stop();
                    }
                }
            }
        }
    } else {
        /* OFF フェーズ */
        if (s_state.pattern_timer >= p->off_time_ms) {
            s_state.pattern_timer = 0;

            /* 繰り返しカウント処理 */
            if (s_state.pattern_repeat > 0) {
                s_state.pattern_repeat--;
                if (s_state.pattern_repeat == 0) {
                    /* パターン終了 */
                    speaker_stop();
                    return;
                }
            }

            /* 次のONフェーズ開始 */
            s_state.pattern_on_phase = true;
            speaker_beep(p->freq_hz, p->on_time_ms);
        }
    }
}
