/*
 * ui_dashboard.h — eez003 (Prodrive)
 *
 * Data binding layer between g_CALC_data and LVGL widgets + fan tachometer.
 */
#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Initialize dashboard: load screen, set bar ranges, attach fan tachometer */
void ui_dashboard_create(void);

/* Update all widgets from g_CALC_data / g_CALC_data_sm (call every frame) */
void ui_dashboard_update(void);

/* Show notification overlay */
void ui_dashboard_set_notify(const char *msg, uint32_t accent_color);

/* Hide notification overlay */
void ui_dashboard_clear_notify(void);

#ifdef __cplusplus
}
#endif

#endif /* UI_DASHBOARD_H */
