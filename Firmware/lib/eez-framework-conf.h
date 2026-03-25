/*
 * eez-framework-conf.h - RX72N platform configuration for EEZ Framework
 *
 * This file is auto-detected by eez-framework via __has_include().
 * Place it in an include path that rx-elf-g++ can find.
 */

#ifndef EEZ_FRAMEWORK_CONF_H
#define EEZ_FRAMEWORK_CONF_H

/* LVGL mode - disables EEZ-GUI rendering engine */
#define EEZ_FOR_LVGL        1

/* Disable features not available on bare-metal RX72N */
#define EEZ_OPTION_GUI       0   /* No EEZ-GUI, LVGL only */
#define EEZ_OPTION_THREADS   0   /* No RTOS */
#define EEZ_OPTION_FS        0   /* No filesystem */
#define OPTION_KEYBOARD      0
#define OPTION_MOUSE         0
#define OPTION_KEYPAD        0

/* Heap configuration for alloc.cpp */
/* EEZ framework uses its own allocator; size in bytes */
#define EEZ_HEAP_SIZE        (32 * 1024)  /* 32KB - adjust as needed */

/* LVGL header */
#define LV_LVGL_H_INCLUDE_SIMPLE  1

#endif /* EEZ_FRAMEWORK_CONF_H */
