/**
 * @file lv_port_disp.c
 * LVGL display port for FULLMONI-WIDE
 *
 * Hardware: RX72N GLCDC GR2
 *   - Frame buffer: 0x00800000 (RGB565, 800x256, 400KB)
 *   - GLCDC initialized here: R_GLCDC_Open() fills config from r_lcd_timing.h
 *     (QE_DISPLAY_CONFIGURATION path: 800x256, LINEOFFSET=1600, RGB565)
 *   - R_GLCDC_Control(START_DISPLAY) starts output; _VSYNC_ISR is safe for LVGL
 *
 * Phase 1: Software rendering (CPU memcpy per dirty rectangle)
 * Phase 3: Double buffer + Async DMAC flush (draw buf → GR2 framebuffer via DMAC ch4)
 *   - Two draw buffers (buf1 + buf2, 16 lines each): LVGL renders one while DMAC
 *     copies the other → true render/flush parallelism.
 *   - flush_cb: async — kicks DMAC and returns; TC ISR calls lv_disp_flush_ready().
 *   - Full-width bands (w==800): single contiguous 32-bit DMAC transfer.
 *   - Partial-width bands: CPU memcpy fallback (synchronous, rare).
 *   - DMAC ch4: kept Open permanently (same as USB ch0-3 pattern).
 *     R_DMACA_Close must NOT be called for ch4 — it triggers r_dmaca_module_disable()
 *     (stops DMAC globally) when no other channels are FIT-locked, breaking USB-CDC.
 */

#include "lv_port_disp.h"
#include <string.h>
#include "platform.h"          /* R_BSP_ATTRIB_STRUCT_BIT_ORDER_* macros required by r_glcdc_rx_if.h */
#include "r_glcdc_rx_if.h"
#include "r_glcdc_rx_pinset.h"
#include "r_dmaca_rx_if.h"     /* Phase 3: DMAC flush */

/*---------------------------------------------------------------------------
 * Draw buffers (double buffer)
 *   Placed in .bss2 (RAM2 at 0x00864000, 112KB).
 *   Size per buf: 800 × 16 lines × 2 bytes = 25,600 bytes
 *--------------------------------------------------------------------------*/
static lv_color_t __attribute__((section(".bss2")))
    draw_buf_1[LV_PORT_HOR_RES * LV_PORT_BUF_LINES];
static lv_color_t __attribute__((section(".bss2")))
    draw_buf_2[LV_PORT_HOR_RES * LV_PORT_BUF_LINES];

static lv_disp_draw_buf_t disp_buf;
static lv_disp_drv_t      disp_drv;

/*---------------------------------------------------------------------------
 * DMAC flush state
 *--------------------------------------------------------------------------*/
#define FLUSH_DMAC_CH   (4)   /* DMAC channel 4 — USB uses ch0-3, avoid conflict */
#define FLUSH_DMAC_PRI  (5)   /* Interrupt priority for DMAC74I TC              */

static lv_disp_drv_t *s_flush_drv;  /* saved for TC ISR → lv_disp_flush_ready() */

/*---------------------------------------------------------------------------
 * flush_dmac_isr_cb
 *   Called by FIT DMAC74I ISR when ch4 transfer completes.
 *   Does NOT call R_DMACA_Close — closing ch4 would trigger r_dmaca_module_disable()
 *   (all other channels unlocked from FIT's view), stopping DMAC globally.
 *--------------------------------------------------------------------------*/
static void flush_dmac_isr_cb(void)
{
    /* Clear DMSTS.DTIF (Transfer Interrupt Flag) for ch4.
     * DMIST.DMIS4 is a READ-ONLY summary of DMSTS4.DTIF — writing to DMIST
     * has no effect. Must clear DTIF via CMD_DTIF_STATUS_CLR to deassert
     * the IRQ source and prevent DMAC74I re-entry. */
    dmaca_stat_t stat;
    R_DMACA_Control(FLUSH_DMAC_CH, DMACA_CMD_DTIF_STATUS_CLR, &stat);
    if (s_flush_drv != NULL) {
        lv_disp_flush_ready(s_flush_drv);
    }
}

/*---------------------------------------------------------------------------
 * flush_dmac_init
 *   One-time DMAC initialisation: Open ch4 permanently, register TC callback.
 *   Channel is kept locked (never Closed) — same pattern as USB ch0-3.
 *--------------------------------------------------------------------------*/
static void flush_dmac_init(void)
{
    dmaca_stat_t stat;
    R_DMACA_Open(FLUSH_DMAC_CH);                                   /* lock ch4 permanently */
    R_DMACA_Control(FLUSH_DMAC_CH, DMACA_CMD_ALL_ENABLE, NULL);    /* DMST.DMST = 1        */
    R_DMACA_Control(FLUSH_DMAC_CH, DMACA_CMD_DTIF_STATUS_CLR, &stat); /* clear stale DTIF  */
    R_DMACA_Int_Callback(FLUSH_DMAC_CH, flush_dmac_isr_cb);        /* register TC callback */
    R_DMACA_Int_Enable(FLUSH_DMAC_CH, FLUSH_DMAC_PRI);             /* IEN=1, IPR=5         */
    /* NO R_DMACA_Close — ch4 stays open for the lifetime of the program */
}

/*---------------------------------------------------------------------------
 * flush_cb
 *   Called by LVGL when a band is rendered and ready to copy to GR2.
 *
 *   Full-width case (w == LV_PORT_HOR_RES):
 *     Async DMAC: Create/Enable/SoftReq then return immediately.
 *     TC ISR (flush_dmac_isr_cb) calls lv_disp_flush_ready().
 *     No Open/Close per transfer — ch4 is kept open permanently.
 *
 *   Partial-width case (w < LV_PORT_HOR_RES):
 *     Synchronous CPU memcpy fallback (rare).
 *--------------------------------------------------------------------------*/
static void flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    if (w == LV_PORT_HOR_RES) {
        /* Contiguous transfer: (w * h) pixels × 2 bytes, as 32-bit words.
         * Async: TC IRQ fires → flush_dmac_isr_cb → lv_disp_flush_ready. */
        uint32_t count = (uint32_t)(w * h) / 2;   /* 32-bit units */
        uint16_t *dst  = LV_PORT_FB_BASE + (area->y1 * LV_PORT_HOR_RES);

        dmaca_transfer_data_cfg_t cfg;
        cfg.transfer_mode        = DMACA_TRANSFER_MODE_NORMAL;
        cfg.repeat_block_side    = DMACA_REPEAT_BLOCK_DISABLE;
        cfg.data_size            = DMACA_DATA_SIZE_LWORD;
        cfg.act_source           = (dmaca_activation_source_t)0;
        cfg.request_source       = DMACA_TRANSFER_REQUEST_SOFTWARE;
        cfg.dtie_request         = DMACA_TRANSFER_END_INTERRUPT_ENABLE;  /* TC IRQ */
        cfg.esie_request         = DMACA_TRANSFER_ESCAPE_END_INTERRUPT_DISABLE;
        cfg.rptie_request        = DMACA_REPEAT_SIZE_END_INTERRUPT_DISABLE;
        cfg.sarie_request        = DMACA_SRC_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
        cfg.darie_request        = DMACA_DES_ADDR_EXT_REP_AREA_OVER_INTERRUPT_DISABLE;
        cfg.src_addr_mode        = DMACA_SRC_ADDR_INCR;
        cfg.src_addr_repeat_area = DMACA_SRC_ADDR_EXT_REP_AREA_NONE;
        cfg.des_addr_mode        = DMACA_DES_ADDR_INCR;
        cfg.des_addr_repeat_area = DMACA_DES_ADDR_EXT_REP_AREA_NONE;
        cfg.offset_value         = 0;
        cfg.interrupt_sel        = DMACA_ISSUES_INTERRUPT_TO_CPU_END_OF_TRANSFER;  /* DISEL=1: DMAC deasserts IRQ at transfer end, preventing ISR re-entry */
        cfg.p_src_addr           = (void *)color_p;
        cfg.p_des_addr           = (void *)dst;
        cfg.transfer_count       = count;
        cfg.block_size           = 1;

        /* Create/Enable/SoftReq — no Open/Close (ch4 always open) */
        dmaca_stat_t stat;
        s_flush_drv = drv;
        R_DMACA_Create(FLUSH_DMAC_CH, &cfg);
        R_DMACA_Control(FLUSH_DMAC_CH, DMACA_CMD_ENABLE, &stat);
        R_DMACA_Control(FLUSH_DMAC_CH, DMACA_CMD_SOFT_REQ_NOT_CLR_REQ, &stat);
        /* Return without flush_ready — ISR completes the handshake */
        return;

    } else {
        /* Partial-width band: different stride src vs dst → line-by-line CPU copy */
        int32_t y;
        lv_color_t *src = color_p;
        for (y = area->y1; y <= area->y2; y++) {
            uint16_t *dst = LV_PORT_FB_BASE + (y * LV_PORT_HOR_RES) + area->x1;
            memcpy(dst, src, (size_t)w * sizeof(lv_color_t));
            src += w;
        }
    }

    lv_disp_flush_ready(drv);
}

/*---------------------------------------------------------------------------
 * _VSYNC_ISR  – GLCDC line-detection callback (set via r_lcd_timing.h)
 *   LVGL uses DMAC for flush, so this just absorbs the interrupt.
 *--------------------------------------------------------------------------*/
void _VSYNC_ISR(void *p)
{
    (void)p;
}

/*---------------------------------------------------------------------------
 * lv_port_disp_init
 *--------------------------------------------------------------------------*/
void lv_port_disp_init(void)
{
    /* Clear frame buffer BEFORE GLCDC is opened.
     * R_GLCDC_Open() may start sync signal output internally, so the
     * framebuffer must be black before any GLCDC operation. */
    memset((void *)LV_PORT_FB_BASE, 0,
           (size_t)LV_PORT_HOR_RES * LV_PORT_VER_RES * sizeof(lv_color_t));

    /* Initialize GLCDC hardware.
     * With QE_DISPLAY_CONFIGURATION defined, R_GLCDC_Open() calls
     * r_glcdc_qe_parameters_setting() which fills cfg from r_lcd_timing.h:
     *   GR2 base=0x00800000, hsize=800, vsize=256, offset=1600, RGB565.
     * _VSYNC_ISR (set as callback by r_lcd_timing.h) only manipulates
     * s_pending_buffer, safe for LVGL. */
    {
        glcdc_cfg_t cfg;
        R_GLCDC_Open(&cfg);          /* QE params auto-fill cfg */
        R_GLCDC_PinSet();            /* Configure LCD multiplex pins */
        R_GLCDC_Control(GLCDC_CMD_START_DISPLAY, NULL);  /* Start output */
    }

    /* Double draw buffer (16 lines each).
     * LVGL alternates between buf1 and buf2: renders into one while DMAC
     * copies the other to GR2 framebuffer — true render/flush parallelism. */
    lv_disp_draw_buf_init(&disp_buf, draw_buf_1, draw_buf_2,
                          LV_PORT_HOR_RES * LV_PORT_BUF_LINES);

    /* Phase 3: DMAC flush initialisation (global enable + TC callback) */
    flush_dmac_init();

    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res      = LV_PORT_HOR_RES;
    disp_drv.ver_res      = LV_PORT_VER_RES;
    disp_drv.flush_cb     = flush_cb;
    disp_drv.draw_buf     = &disp_buf;
    /* full_refresh=1 must NOT be used with a partial (16-line) draw buffer.
     * It calls refr_area_part() once with clip={0,0,799,255}, overflowing the
     * 16-line buffer and passing h=256 to flush_cb (DMAC count 16× too large).
     * Full-screen DMAC is achieved by calling lv_obj_invalidate(lv_scr_act())
     * each frame in main.c, which makes LVGL use the 16-band partial-refresh
     * path correctly (each 800×16 band → flush_cb → DMAC). */

    lv_disp_drv_register(&disp_drv);
}
