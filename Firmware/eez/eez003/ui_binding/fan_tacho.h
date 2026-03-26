/*
 * fan_tacho.h — Prodrive-style fan/sweep tachometer
 *
 * Custom LVGL drawing for a fan-shaped RPM gauge with:
 *   - Segmented blocks arranged in a radial sweep
 *   - Cyan fill for normal range, red fill for redline (≥7000 RPM)
 *   - RPM number labels along the sweep curve
 */
#ifndef FAN_TACHO_H
#define FAN_TACHO_H

#include <lvgl/lvgl.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Attach the custom draw handler to the tachometer container.
 * Call once after screen creation. */
void fan_tacho_init(lv_obj_t *container);

/* Set the current RPM value (0–9000).
 * The fan tachometer redraws on the next LVGL refresh cycle. */
void fan_tacho_set_rpm(uint32_t rpm);

/* Set redline start RPM (default: 7000) */
void fan_tacho_set_redline(uint32_t rpm);

#ifdef __cplusplus
}
#endif

#endif /* FAN_TACHO_H */
