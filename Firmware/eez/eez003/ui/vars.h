#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_NONE
};

extern int32_t get_var_rpm(void);
extern void set_var_rpm(int32_t value);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/
