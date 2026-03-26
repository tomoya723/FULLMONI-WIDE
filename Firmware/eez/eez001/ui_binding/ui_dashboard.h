/*
 * ui_dashboard.h
 *
 * Data binding layer between g_CALC_data and SLS-generated UI widgets.
 * This file is hand-written; the SLS-generated ui/ directory is kept separate.
 */

#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Initialize dashboard: load screen, set bar ranges, hide all warnings */
void ui_dashboard_create(void);

/* Update all widgets from g_CALC_data / g_CALC_data_sm (call every frame) */
void ui_dashboard_update(void);

/* Show notification overlay (warning message, HOST ACCESS, etc.)
 *   msg          : text to display (NULL or "" to hide)
 *   accent_color : RGB888 (枠・文字色, e.g. 0xFF0000=赤警告, 0x00AA00=緑HOST ACCESS)
 *                  背景は常に黒 (HUDスタイル)
 */
void ui_dashboard_set_notify(const char *msg, uint32_t accent_color);

/* Hide notification overlay */
void ui_dashboard_clear_notify(void);

#ifdef __cplusplus
}
#endif

#endif /* UI_DASHBOARD_H */
