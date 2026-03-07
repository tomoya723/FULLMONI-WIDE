/**
 * @file lv_draw_drw2d.h
 * LVGL Phase 2: DRW2D (D/AVE 2D) hardware accelerated draw context for RX72N
 *
 * Overrides lv_draw_sw_ctx_t::blend with a DRW2D-accelerated solid fill.
 * All other draw operations fall back to the SW renderer.
 *
 * Acceleration coverage:
 *   Accelerated : solid-color fills (opaque + alpha-blended), no mask
 *   SW fallback : masked fills (anti-aliased arcs, rounded rects), image blits
 *
 * Usage in lv_port_disp.c:
 *   disp_drv.draw_ctx_init   = lv_draw_drw2d_ctx_init;
 *   disp_drv.draw_ctx_deinit = lv_draw_drw2d_ctx_deinit;
 *   disp_drv.draw_ctx_size   = sizeof(lv_draw_drw2d_ctx_t);
 */

#ifndef LV_DRAW_DRW2D_H
#define LV_DRAW_DRW2D_H

#include "lvgl/lvgl.h"
#include "lvgl/src/draw/sw/lv_draw_sw.h"  /* lv_draw_sw_ctx_t */

#ifdef __cplusplus
extern "C" {
#endif

/** DRW2D draw context: inherits all SW functions, overrides blend. */
typedef struct {
    lv_draw_sw_ctx_t base;  /* Must be first */
} lv_draw_drw2d_ctx_t;

/**
 * Initialize DRW2D draw context.
 * Opens D/AVE 2D hardware, registers BSP IRQ callback, and overrides
 * blend + wait_for_finish callbacks.
 * Pass to disp_drv.draw_ctx_init.
 */
void lv_draw_drw2d_ctx_init(lv_disp_drv_t *drv, lv_draw_ctx_t *ctx);

/**
 * Deinitialize DRW2D draw context.
 * Closes D/AVE 2D hardware.
 * Pass to disp_drv.draw_ctx_deinit.
 */
void lv_draw_drw2d_ctx_deinit(lv_disp_drv_t *drv, lv_draw_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* LV_DRAW_DRW2D_H */
