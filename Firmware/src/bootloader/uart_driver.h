/*
 * UART Driver Header
 */

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/* 関数プロトタイプ */
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *str);
bool uart_getc(uint8_t *data);
bool uart_data_available(void);

#endif /* UART_DRIVER_H */
