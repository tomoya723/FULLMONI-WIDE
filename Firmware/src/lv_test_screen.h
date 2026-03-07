/**
 * @file lv_test_screen.h
 * LVGL Phase 1 test screen for FULLMONI-WIDE
 *
 * Displays:
 *  - Dark background (navy)
 *  - "LVGL OK" label (center)
 *  - Build info label (top-left)
 *  - FPS counter (top-right, via LV_USE_PERF_MONITOR)
 *  - Arc widget (simulates tachometer) to verify arc rendering
 *  - Color bar (verifies RGB565 color accuracy)
 */

#ifndef LV_TEST_SCREEN_H
#define LV_TEST_SCREEN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create and load the LVGL test screen.
 * Call once after lv_port_disp_init().
 */
void lv_test_screen_create(void);

/**
 * Update dynamic elements on the test screen (call from main loop).
 * @param rpm  Simulated RPM value (0-8000) for the arc widget
 */
void lv_test_screen_update(uint32_t rpm);

#ifdef __cplusplus
}
#endif

#endif /* LV_TEST_SCREEN_H */
