/**
 * @file lv_port_disp.h
 * LVGL display port for FULLMONI-WIDE
 * Target: RX72N GLCDC GR2 frame buffer (0x00800000, RGB565, 800x256)
 *
 * Phase 1: Software rendering with memcpy flush
 * Phase 2 (future): DRW2D hardware acceleration via lv_draw_ctx_t override
 */

#ifndef LV_PORT_DISP_H
#define LV_PORT_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

/* Frame buffer base address (GR2, RGB565) */
#define LV_PORT_FB_BASE     ((uint16_t *)0x00800000UL)

/* Display resolution */
#define LV_PORT_HOR_RES     800
#define LV_PORT_VER_RES     256

/* Draw buffer: 16 lines × 800 pixels × 2 buffers = 50KB (fits in RAM2 with heap+GUIConf) */
#define LV_PORT_BUF_LINES   16

/**
 * Initialize LVGL display driver and connect to GLCDC GR2 frame buffer.
 * Must be called after lv_init().
 */
void lv_port_disp_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_DISP_H */
