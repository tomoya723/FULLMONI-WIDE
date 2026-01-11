/*
 * USB CDC Descriptors for FIT Driver
 * Fixed: Use const to ensure proper ROM placement
 */
#include "r_usb_basic_if.h"
#include "r_usb_pcdc_if.h"

/*******************************************************************************
 * Device Descriptor (18 bytes)
 ******************************************************************************/
const uint8_t g_device_descriptor[] =
{
    18,                 /* bLength */
    0x01,               /* bDescriptorType: DEVICE */
    0x00, 0x02,         /* bcdUSB: USB 2.0 */
    0x02,               /* bDeviceClass: CDC */
    0x00,               /* bDeviceSubClass */
    0x00,               /* bDeviceProtocol */
    64,                 /* bMaxPacketSize0 */
    0x09, 0x12,         /* idVendor: 0x1209 (pid.codes) */
    0x30, 0x72,         /* idProduct: 0x7230 (FULLMONI-WIDE) */
    0x00, 0x01,         /* bcdDevice: 1.00 */
    1,                  /* iManufacturer: String index 1 */
    2,                  /* iProduct: String index 2 */
    0,                  /* iSerialNumber: None */
    1                   /* bNumConfigurations */
};

/*******************************************************************************
 * Configuration Descriptor (67 bytes total)
 ******************************************************************************/
const uint8_t g_config_descriptor[] =
{
    /* Configuration Descriptor (9 bytes) */
    9,                  /* bLength */
    0x02,               /* bDescriptorType: CONFIGURATION */
    67, 0x00,           /* wTotalLength: 67 bytes */
    2,                  /* bNumInterfaces */
    1,                  /* bConfigurationValue */
    0,                  /* iConfiguration */
    0xC0,               /* bmAttributes: Self-powered */
    50,                 /* bMaxPower: 100mA */

    /* Interface 0: CDC Control (9 bytes) */
    9,                  /* bLength */
    0x04,               /* bDescriptorType: INTERFACE */
    0,                  /* bInterfaceNumber */
    0,                  /* bAlternateSetting */
    1,                  /* bNumEndpoints */
    0x02,               /* bInterfaceClass: CDC */
    0x02,               /* bInterfaceSubClass: ACM */
    0x01,               /* bInterfaceProtocol: AT commands */
    0,                  /* iInterface */

    /* CDC Header Functional Descriptor (5 bytes) */
    5,                  /* bFunctionLength */
    0x24,               /* bDescriptorType: CS_INTERFACE */
    0x00,               /* bDescriptorSubtype: Header */
    0x10, 0x01,         /* bcdCDC: 1.10 */

    /* CDC Call Management Functional Descriptor (5 bytes) */
    5,                  /* bFunctionLength */
    0x24,               /* bDescriptorType: CS_INTERFACE */
    0x01,               /* bDescriptorSubtype: Call Management */
    0x00,               /* bmCapabilities */
    1,                  /* bDataInterface */

    /* CDC ACM Functional Descriptor (4 bytes) */
    4,                  /* bFunctionLength */
    0x24,               /* bDescriptorType: CS_INTERFACE */
    0x02,               /* bDescriptorSubtype: ACM */
    0x02,               /* bmCapabilities */

    /* CDC Union Functional Descriptor (5 bytes) */
    5,                  /* bFunctionLength */
    0x24,               /* bDescriptorType: CS_INTERFACE */
    0x06,               /* bDescriptorSubtype: Union */
    0,                  /* bControlInterface */
    1,                  /* bSubordinateInterface0 */

    /* Endpoint 3 IN - Interrupt for Notification (7 bytes) */
    7,                  /* bLength */
    0x05,               /* bDescriptorType: ENDPOINT */
    0x83,               /* bEndpointAddress: EP3 IN */
    0x03,               /* bmAttributes: Interrupt */
    8, 0x00,            /* wMaxPacketSize: 8 */
    0xFF,               /* bInterval: 255ms */

    /* Interface 1: CDC Data (9 bytes) */
    9,                  /* bLength */
    0x04,               /* bDescriptorType: INTERFACE */
    1,                  /* bInterfaceNumber */
    0,                  /* bAlternateSetting */
    2,                  /* bNumEndpoints */
    0x0A,               /* bInterfaceClass: CDC-Data */
    0x00,               /* bInterfaceSubClass */
    0x00,               /* bInterfaceProtocol */
    0,                  /* iInterface */

    /* Endpoint 2 OUT - Bulk RX (7 bytes) */
    7,                  /* bLength */
    0x05,               /* bDescriptorType: ENDPOINT */
    0x02,               /* bEndpointAddress: EP2 OUT */
    0x02,               /* bmAttributes: Bulk */
    64, 0x00,           /* wMaxPacketSize: 64 */
    0,                  /* bInterval */

    /* Endpoint 1 IN - Bulk TX (7 bytes) */
    7,                  /* bLength */
    0x05,               /* bDescriptorType: ENDPOINT */
    0x81,               /* bEndpointAddress: EP1 IN */
    0x02,               /* bmAttributes: Bulk */
    64, 0x00,           /* wMaxPacketSize: 64 */
    0                   /* bInterval */
};

/*******************************************************************************
 * String Descriptors
 ******************************************************************************/
/* String 0: Language ID */
const uint8_t g_string0[] =
{
    4,                  /* bLength */
    0x03,               /* bDescriptorType: STRING */
    0x09, 0x04          /* wLANGID: 0x0409 (US English) */
};

/* String 1: Manufacturer */
const uint8_t g_string1[] =
{
    16,                 /* bLength */
    0x03,               /* bDescriptorType: STRING */
    'R', 0, 'e', 0, 'n', 0, 'e', 0, 's', 0, 'a', 0, 's', 0
};

/* String 2: Product */
const uint8_t g_string2[] =
{
    52,                 /* bLength */
    0x03,               /* bDescriptorType: STRING */
    'F', 0, 'U', 0, 'L', 0, 'L', 0, 'M', 0, 'O', 0, 'N', 0, 'I', 0,
    '-', 0, 'W', 0, 'I', 0, 'D', 0, 'E', 0, ' ', 0, 'B', 0, 'o', 0,
    'o', 0, 't', 0, 'l', 0, 'o', 0, 'a', 0, 'd', 0, 'e', 0, 'r', 0
};

/* String Descriptor Table - MUST be non-const for FIT driver */
uint8_t * g_string_table[] =
{
    (uint8_t *)g_string0,
    (uint8_t *)g_string1,
    (uint8_t *)g_string2
};

/*******************************************************************************
 * USB Descriptor Structure for FIT
 ******************************************************************************/
usb_descriptor_t g_usb_descriptor =
{
    .p_device    = (uint8_t *)g_device_descriptor,
    .p_config_f  = (uint8_t *)g_config_descriptor,
    .p_config_h  = (uint8_t *)g_config_descriptor,
    .p_qualifier = NULL,
    .p_string    = g_string_table,
    .num_string  = 3
};
