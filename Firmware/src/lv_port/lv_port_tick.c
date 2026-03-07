/**
 * @file lv_port_tick.c
 * LVGL tick port for FULLMONI-WIDE
 *
 * The RX72N 10ms timer interrupt drives ap_10ms().
 * We hook into that to provide LVGL with a monotonic millisecond counter.
 *
 * Integration in main.c:
 *   #include "lv_port/lv_port_tick.h"
 *
 *   void ap_10ms(void) {
 *       data_setLCD10ms();
 *       lv_port_tick_inc();   // <- add this line
 *   }
 */

#include "lv_port_tick.h"
#include <stdint.h>

static volatile uint32_t s_tick_ms = 0;

void lv_port_tick_inc(void)
{
    s_tick_ms += 10;
}

uint32_t lv_port_tick_get(void)
{
    return s_tick_ms;
}
