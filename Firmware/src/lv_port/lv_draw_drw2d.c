/**
 * @file lv_draw_drw2d.c
 * LVGL Phase 2: DRW2D (D/AVE 2D) hardware accelerated rendering for RX72N
 *
 * Architecture:
 *   Overrides lv_draw_sw_ctx_t::blend with DRW2D d2_renderbox.
 *   SW fallback for: masked fills (anti-aliased arcs, rounded corners),
 *   image blits, gradient fills.
 *
 * DRW2D IRQ:
 *   Registered via BSP R_BSP_InterruptWrite(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ).
 *   d2_flushframe() waits synchronously after each render buffer execute.
 *   ICU_GROUPAL1_ENABLE must remain 0 in dave_irq_rx.c — BSP manages GROUPAL1.
 *
 * Reference: lv_gpu_d2_ra6m3.c (LVGL v8.3 Renesas RA port) adapted for RX72N.
 *   Differences: no d1_maptovidmem, no clean_dcache_cb (RX72N SRAM not cached).
 */

#include "lv_draw_drw2d.h"
#include "platform.h"       /* BSP includes: mcu_interrupts.h, r_bsp_interrupts.h */
#include "dave_driver.h"

/* drw_int_isr() is defined in dave_irq_rx.c (no public header for it) */
extern void drw_int_isr(void);

/*---------------------------------------------------------------------------
 * Module-level D/AVE 2D state (singleton — one HW device per MCU)
 *--------------------------------------------------------------------------*/
static d2_device       *s_d2 = NULL;
static d2_renderbuffer *s_rb = NULL;

/*---------------------------------------------------------------------------
 * BSP group-interrupt callback wrapper.
 * bsp_int_cb_t is  void (*)(void *); drw_int_isr() takes no argument.
 *--------------------------------------------------------------------------*/
static void drw2d_bsp_irq(void *p)
{
    (void)p;
    drw_int_isr();
}

/*---------------------------------------------------------------------------
 * color_lv_to_d2
 *   LVGL RGB565 lv_color_t → D/AVE 2D ARGB8888 d2_color.
 *   D/AVE 2D ignores the alpha byte for solid fills but it must be non-zero
 *   when using alpha-blend mode, so set to 0xFF.
 *--------------------------------------------------------------------------*/
static d2_color color_lv_to_d2(lv_color_t c)
{
    uint32_t r = ((uint32_t)c.ch.red   << 3) | ((uint32_t)c.ch.red   >> 2);
    uint32_t g = ((uint32_t)c.ch.green << 2) | ((uint32_t)c.ch.green >> 4);
    uint32_t b = ((uint32_t)c.ch.blue  << 3) | ((uint32_t)c.ch.blue  >> 2);
    return (d2_color)(0xFF000000UL | (r << 16) | (g << 8) | b);
}

/*---------------------------------------------------------------------------
 * drw2d_blend
 *
 * Overrides lv_draw_sw_ctx_t::blend.
 * Accelerates solid fills (no mask, no src image) with d2_renderbox.
 * Falls back to lv_draw_sw_blend_basic for everything else.
 *
 * Coordinate convention:
 *   dsc->blend_area and draw_ctx->clip_area are screen-relative.
 *   D/AVE 2D render target is LVGL's draw buffer, whose origin is
 *   draw_ctx->buf_area.x1 / .y1 on screen.
 *   → subtract buf_area origin to get buffer-relative coords.
 *--------------------------------------------------------------------------*/
static void drw2d_blend(lv_draw_ctx_t *draw_ctx, const lv_draw_sw_blend_dsc_t *dsc)
{
    lv_area_t blend_area;
    if (!_lv_area_intersect(&blend_area, dsc->blend_area, draw_ctx->clip_area)) return;

    bool done = false;

    if (s_d2 != NULL
        && dsc->src_buf    == NULL              /* solid fill only */
        && dsc->mask_buf   == NULL              /* no per-pixel mask (anti-alias etc.) */
        && dsc->blend_mode == LV_BLEND_MODE_NORMAL
        && lv_area_get_size(&blend_area) > 50)  /* skip HW overhead for tiny areas */
    {
        lv_coord_t stride      = lv_area_get_width(draw_ctx->buf_area);
        lv_coord_t band_height = lv_area_get_height(draw_ctx->buf_area);
        lv_color_t *dbuf       = draw_ctx->buf;

        /* Buffer-relative coordinates */
        lv_coord_t bx1 = blend_area.x1 - draw_ctx->buf_area->x1;
        lv_coord_t by1 = blend_area.y1 - draw_ctx->buf_area->y1;
        lv_coord_t bx2 = blend_area.x2 - draw_ctx->buf_area->x1;
        lv_coord_t by2 = blend_area.y2 - draw_ctx->buf_area->y1;
        lv_coord_t bw  = bx2 - bx1 + 1;
        lv_coord_t bh  = by2 - by1 + 1;

        if (bw <= 0 || bh <= 0) goto sw_fallback;

        d2_selectrenderbuffer(s_d2, s_rb);

        /* Set render target: LVGL draw buffer (RGB565, stride×band_height) */
        d2_framebuffer(s_d2, dbuf,
                       (d2_s32)stride,
                       (d2_u32)stride,
                       (d2_u32)(band_height < 2 ? 2 : band_height),
                       d2_mode_rgb565);

        /* Clip rect (buffer-relative) */
        d2_cliprect(s_d2, (d2_border)bx1, (d2_border)by1,
                          (d2_border)bx2, (d2_border)by2);

        /* Color and alpha */
        d2_setcolor(s_d2, 0, color_lv_to_d2(dsc->color));
        d2_setalpha(s_d2, (d2_alpha)(dsc->opa > LV_OPA_MAX ? 255 : (uint8_t)dsc->opa));

        if (dsc->opa >= LV_OPA_MAX) {
            /* Opaque: src overwrites dst (faster, no read-modify-write) */
            d2_setblendmode(s_d2, d2_bm_one, d2_bm_zero);
        } else {
            /* Translucent: standard over-compositing */
            d2_setblendmode(s_d2, d2_bm_alpha, d2_bm_one_minus_alpha);
        }

        /* Render filled rectangle (4-bit fixed-point coordinates) */
        d2_renderbox(s_d2,
                     D2_FIX4(bx1), D2_FIX4(by1),
                     D2_FIX4(bw),  D2_FIX4(bh));

        /* Flush synchronously (d2_flushframe waits for dlist IRQ) */
        d2_executerenderbuffer(s_d2, s_rb, 0);
        d2_flushframe(s_d2);

        done = true;
    }

sw_fallback:
    if (!done) lv_draw_sw_blend_basic(draw_ctx, dsc);
}

/*---------------------------------------------------------------------------
 * drw2d_wait
 *   Ensures all DRW2D operations are complete before LVGL reads back the
 *   draw buffer (e.g. before flush_cb copies to the GLCDC frame buffer).
 *--------------------------------------------------------------------------*/
static void drw2d_wait(lv_draw_ctx_t *draw_ctx)
{
    if (s_d2 != NULL) d2_flushframe(s_d2);
    lv_draw_sw_wait_for_finish(draw_ctx);
}

/*---------------------------------------------------------------------------
 * lv_draw_drw2d_ctx_init
 *--------------------------------------------------------------------------*/
void lv_draw_drw2d_ctx_init(lv_disp_drv_t *drv, lv_draw_ctx_t *ctx)
{
    LV_UNUSED(drv);

    /* Initialize SW context first — sets all callbacks to SW defaults */
    lv_draw_sw_init_ctx(drv, ctx);

    /* Open D/AVE 2D device (singleton — only once even if called multiple times) */
    if (s_d2 == NULL) {
        s_d2 = d2_opendevice(0);
        if (s_d2 == NULL) return;  /* HW not available — SW-only mode */

        d2_setdlistblocksize(s_d2, 25);

        if (d2_inithw(s_d2, 0) != D2_OK) {
            d2_closedevice(s_d2);
            s_d2 = NULL;
            return;
        }

        s_rb = d2_newrenderbuffer(s_d2, 20, 20);
        if (s_rb == NULL) {
            d2_closedevice(s_d2);
            s_d2 = NULL;
            return;
        }

        /* Register DRW2D IRQ through BSP GROUPAL1 dispatch framework.
         * BSP handles the GROUPAL1 ISR; drw_int_isr() is called on bit 11. */
        R_BSP_InterruptWrite(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ,
                             (bsp_int_cb_t)drw2d_bsp_irq);
    }

    /* Override blend + wait with DRW2D versions */
    lv_draw_sw_ctx_t *sw = (lv_draw_sw_ctx_t *)ctx;
    sw->blend            = drw2d_blend;
    ctx->wait_for_finish = drw2d_wait;
}

/*---------------------------------------------------------------------------
 * lv_draw_drw2d_ctx_deinit
 *--------------------------------------------------------------------------*/
void lv_draw_drw2d_ctx_deinit(lv_disp_drv_t *drv, lv_draw_ctx_t *ctx)
{
    LV_UNUSED(drv);
    LV_UNUSED(ctx);

    if (s_d2 != NULL) {
        if (s_rb != NULL) {
            d2_freerenderbuffer(s_d2, s_rb);
            s_rb = NULL;
        }
        R_BSP_InterruptWrite(BSP_INT_SRC_AL1_DRW2D_DRW_IRQ, NULL);
        d2_closedevice(s_d2);
        s_d2 = NULL;
    }
}
