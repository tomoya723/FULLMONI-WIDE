/*
 * usb_cdc_minimal.h
 *
 * Minimal USB CDC Header for Bootloader
 * Uses direct register access without FIT module
 */

#ifndef USB_CDC_MINIMAL_H_
#define USB_CDC_MINIMAL_H_

#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef enum {
    USB_CDC_SUCCESS = 0,
    USB_CDC_ERR_FAILED,
    USB_CDC_ERR_NOT_OPEN,
    USB_CDC_ERR_BUSY
} usb_cdc_err_t;

typedef enum {
    USB_CDC_STATE_DETACHED = 0,
    USB_CDC_STATE_ATTACHED,
    USB_CDC_STATE_CONFIGURED,
    USB_CDC_STATE_READY
} usb_cdc_state_t;

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

/**
 * @brief Initialize USB CDC (minimal implementation)
 * @return USB_CDC_SUCCESS on success
 */
usb_cdc_err_t usb_cdc_minimal_init(void);

/**
 * @brief USB CDC task (call from main loop)
 *        Handles USB events in polling mode
 */
void usb_cdc_minimal_task(void);

/**
 * @brief Get current CDC state
 * @return Current state
 */
usb_cdc_state_t usb_cdc_minimal_get_state(void);

/**
 * @brief Check if CDC is ready for communication
 * @return true if ready
 */
bool usb_cdc_minimal_is_ready(void);

/**
 * @brief Send data via USB CDC
 * @param data Data to send
 * @param len Data length
 * @return USB_CDC_SUCCESS on success
 */
usb_cdc_err_t usb_cdc_minimal_send(const uint8_t *data, uint32_t len);

/**
 * @brief Check if RX data available
 * @return true if data available
 */
bool usb_cdc_minimal_rx_available(void);

/**
 * @brief Get one byte from RX buffer
 * @param byte Pointer to store received byte
 * @return true if byte read
 */
bool usb_cdc_minimal_rx_get(uint8_t *byte);

#endif /* USB_CDC_MINIMAL_H_ */
