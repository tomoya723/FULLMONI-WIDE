/**
 * @file lv_port_tick.h
 * LVGL tick port for FULLMONI-WIDE
 *
 * Uses the existing 10ms scheduler (ap_10ms) as the tick source.
 * lv_port_tick_inc() is called from ap_10ms() every 10ms.
 * lv_port_tick_get() is used by LVGL via LV_TICK_CUSTOM_SYS_TIME_EXPR.
 */

#ifndef LV_PORT_TICK_H
#define LV_PORT_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Increment the LVGL tick counter by 10ms.
 * Call this from ap_10ms() in main.c.
 */
void lv_port_tick_inc(void);

/**
 * Return current LVGL tick time in milliseconds.
 * Used by LV_TICK_CUSTOM_SYS_TIME_EXPR in lv_conf.h.
 */
uint32_t lv_port_tick_get(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_PORT_TICK_H */
