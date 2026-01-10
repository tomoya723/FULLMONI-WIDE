/*
 * usb_cdc_minimal.c
 *
 * Minimal USB CDC Implementation for Bootloader
 * Uses direct register access without FIT module interrupt handlers
 *
 * Created: 2026/01/09
 * Target: RX72N USB0 Peripheral (Full-Speed)
 */

#include "platform.h"
#include "usb_cdc_minimal.h"
#include <string.h>

/*******************************************************************************
 * USB Register Definitions (RX72N USB0)
 ******************************************************************************/
#define USB0_BASE           (0x000A0000UL)
#define USB0                (*(volatile struct st_usb0 *)USB0_BASE)

/*******************************************************************************
 * USB CDC Class Definitions
 ******************************************************************************/
#define USB_CDC_CLASS           0x02
#define USB_CDC_SUBCLASS_ACM    0x02
#define USB_CDC_PROTOCOL_V25TER 0x01

/* CDC Requests */
#define CDC_SET_LINE_CODING     0x20
#define CDC_GET_LINE_CODING     0x21
#define CDC_SET_CONTROL_LINE    0x22

/* Device Descriptor */
static const uint8_t g_device_descriptor[] = {
    18,         /* bLength */
    1,          /* bDescriptorType: Device */
    0x00, 0x02, /* bcdUSB: 2.00 */
    0x02,       /* bDeviceClass: CDC */
    0x00,       /* bDeviceSubClass */
    0x00,       /* bDeviceProtocol */
    64,         /* bMaxPacketSize0 */
    0x5B, 0x04, /* idVendor: 0x045B (Renesas) */
    0x34, 0x02, /* idProduct: 0x0234 */
    0x00, 0x01, /* bcdDevice: 1.00 */
    1,          /* iManufacturer */
    2,          /* iProduct */
    0,          /* iSerialNumber */
    1           /* bNumConfigurations */
};

/* Configuration Descriptor (CDC ACM) */
static const uint8_t g_config_descriptor[] = {
    /* Configuration Descriptor */
    9,          /* bLength */
    2,          /* bDescriptorType: Configuration */
    67, 0,      /* wTotalLength: 67 bytes */
    2,          /* bNumInterfaces: 2 */
    1,          /* bConfigurationValue */
    0,          /* iConfiguration */
    0xC0,       /* bmAttributes: Self-powered */
    50,         /* bMaxPower: 100mA */

    /* Interface 0: CDC Control */
    9,          /* bLength */
    4,          /* bDescriptorType: Interface */
    0,          /* bInterfaceNumber */
    0,          /* bAlternateSetting */
    1,          /* bNumEndpoints */
    0x02,       /* bInterfaceClass: CDC */
    0x02,       /* bInterfaceSubClass: ACM */
    0x01,       /* bInterfaceProtocol: AT commands */
    0,          /* iInterface */

    /* CDC Header Functional Descriptor */
    5,          /* bLength */
    0x24,       /* bDescriptorType: CS_INTERFACE */
    0x00,       /* bDescriptorSubtype: Header */
    0x10, 0x01, /* bcdCDC: 1.10 */

    /* CDC ACM Functional Descriptor */
    4,          /* bLength */
    0x24,       /* bDescriptorType: CS_INTERFACE */
    0x02,       /* bDescriptorSubtype: ACM */
    0x02,       /* bmCapabilities */

    /* CDC Union Functional Descriptor */
    5,          /* bLength */
    0x24,       /* bDescriptorType: CS_INTERFACE */
    0x06,       /* bDescriptorSubtype: Union */
    0,          /* bControlInterface */
    1,          /* bSubordinateInterface0 */

    /* CDC Call Management Functional Descriptor */
    5,          /* bLength */
    0x24,       /* bDescriptorType: CS_INTERFACE */
    0x01,       /* bDescriptorSubtype: Call Management */
    0x00,       /* bmCapabilities */
    1,          /* bDataInterface */

    /* Endpoint 3 IN: Interrupt (for notifications) */
    7,          /* bLength */
    5,          /* bDescriptorType: Endpoint */
    0x83,       /* bEndpointAddress: EP3 IN */
    0x03,       /* bmAttributes: Interrupt */
    8, 0,       /* wMaxPacketSize: 8 */
    16,         /* bInterval: 16ms */

    /* Interface 1: CDC Data */
    9,          /* bLength */
    4,          /* bDescriptorType: Interface */
    1,          /* bInterfaceNumber */
    0,          /* bAlternateSetting */
    2,          /* bNumEndpoints */
    0x0A,       /* bInterfaceClass: CDC Data */
    0x00,       /* bInterfaceSubClass */
    0x00,       /* bInterfaceProtocol */
    0,          /* iInterface */

    /* Endpoint 1 OUT: Bulk */
    7,          /* bLength */
    5,          /* bDescriptorType: Endpoint */
    0x01,       /* bEndpointAddress: EP1 OUT */
    0x02,       /* bmAttributes: Bulk */
    64, 0,      /* wMaxPacketSize: 64 */
    0,          /* bInterval */

    /* Endpoint 2 IN: Bulk */
    7,          /* bLength */
    5,          /* bDescriptorType: Endpoint */
    0x82,       /* bEndpointAddress: EP2 IN */
    0x02,       /* bmAttributes: Bulk */
    64, 0,      /* wMaxPacketSize: 64 */
    0           /* bInterval */
};

/* String Descriptors */
static const uint8_t g_string0[] = { 4, 3, 0x09, 0x04 };  /* Language: English */
static const uint8_t g_string1[] = { 16, 3, 'R',0, 'e',0, 'n',0, 'e',0, 's',0, 'a',0, 's',0 };
static const uint8_t g_string2[] = { 32, 3, 'F',0, 'U',0, 'L',0, 'L',0, 'M',0, 'O',0, 'N',0, 'I',0, '-',0, 'W',0, 'I',0, 'D',0, 'E',0, ' ',0, 'B',0 };

/*******************************************************************************
 * Private Variables
 ******************************************************************************/
static volatile usb_cdc_state_t s_state = USB_CDC_STATE_DETACHED;
static volatile uint8_t s_device_address = 0;
static volatile uint8_t s_pending_address = 0;  /* Address to set after status stage */
static volatile uint8_t s_configuration = 0;

/* Control transfer state for multi-packet transfers */
static const uint8_t *s_ctrl_data_ptr = NULL;
static volatile uint16_t s_ctrl_data_remaining = 0;

/* Line Coding */
static uint8_t s_line_coding[7] = {
    0x00, 0xC2, 0x01, 0x00,  /* 115200 bps */
    0x00,                     /* 1 stop bit */
    0x00,                     /* No parity */
    0x08                      /* 8 data bits */
};

/* RX/TX Buffers */
static uint8_t s_rx_buffer[256];
static volatile uint32_t s_rx_write_idx = 0;
static volatile uint32_t s_rx_read_idx = 0;

static uint8_t s_tx_buffer[64];
static volatile bool s_tx_busy = false;

/* CDC control state */
static volatile bool s_port_opened = false;  /* Host has opened the port */

/* Control Write data reception state (for SET_LINE_CODING) */
static volatile bool s_ctrl_write_pending = false;  /* Waiting for Control Write data */
static volatile uint16_t s_ctrl_write_len = 0;       /* Expected data length */

/* USB Standard Request Codes */
#define USB_GET_STATUS          0x00
#define USB_CLEAR_FEATURE       0x01
#define USB_SET_FEATURE         0x03
#define USB_SET_ADDRESS         0x05
#define USB_GET_DESCRIPTOR      0x06
#define USB_SET_DESCRIPTOR      0x07
#define USB_GET_CONFIGURATION   0x08
#define USB_SET_CONFIGURATION   0x09

/* Descriptor Types */
#define USB_DESC_DEVICE         0x01
#define USB_DESC_CONFIGURATION  0x02
#define USB_DESC_STRING         0x03

/* SETUP packet structure */
typedef struct {
    uint8_t  bmRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_setup_t;

/*******************************************************************************
 * Debug output (use extern functions from main_fit_cdc.c)
 ******************************************************************************/
extern void debug_print(const char *msg);
extern void debug_hex(uint32_t val);

#if 0  /* Use debug functions from main_fit_cdc.c instead */
static void debug_print_local(const char *msg)
{
    char buf[12];
    static const char hex[] = "0123456789ABCDEF";
    buf[0] = '0';
    buf[1] = 'x';
    buf[2] = hex[(val >> 28) & 0xF];
    buf[3] = hex[(val >> 24) & 0xF];
    buf[4] = hex[(val >> 20) & 0xF];
    buf[5] = hex[(val >> 16) & 0xF];
    buf[6] = hex[(val >> 12) & 0xF];
    buf[7] = hex[(val >> 8) & 0xF];
    buf[8] = hex[(val >> 4) & 0xF];
    buf[9] = hex[val & 0xF];
    buf[10] = '\0';
    debug_print(buf);
}
#endif /* #if 0 */

/*******************************************************************************
 * USB Hardware Functions
 ******************************************************************************/

/* Enable USB0 module */
static void usb_hw_init(void)
{
    debug_print("[USB] usb_hw_init start\r\n");

    /* Unlock register protection */
    SYSTEM.PRCR.WORD = 0xA502;

    /* Check and configure USB clock (48MHz required) */
    debug_print("[USB] SCKCR2=");
    debug_hex(SYSTEM.SCKCR2.WORD);
    debug_print("\r\n");

    /* USB clock source selection (UCLKSEL in SCKCR2)
     * For RX72N: USB clock from PLL or HOCO
     * SCKCR2.UCLKSEL: 0=Not used, 1=PLL/4, etc. */

    /* Enable USB0 module clock */
    MSTP(USB0) = 0;             /* Enable USB0 */

    SYSTEM.PRCR.WORD = 0xA500;  /* Lock */

    debug_print("[USB] Module clock enabled, MSTPCRA=");
    debug_hex(SYSTEM.MSTPCRA.LONG);
    debug_print("\r\n");

    /* Wait for module to stabilize */
    for (volatile int i = 0; i < 10000; i++);

    /* Set USB pins (USB0_VBUS: P16) */
    MPC.PWPR.BIT.B0WI = 0;      /* Enable PFS write */
    MPC.PWPR.BIT.PFSWE = 1;
    MPC.P16PFS.BYTE = 0x12;     /* USB0_VBUS */
    MPC.PWPR.BIT.PFSWE = 0;     /* Disable PFS write */
    MPC.PWPR.BIT.B0WI = 1;
    PORT1.PMR.BIT.B6 = 1;       /* P16 as peripheral */

    debug_print("[USB] Pins configured\r\n");

    /* USB0 software reset */
    USB0.SYSCFG.WORD = 0x0000;
    for (volatile int i = 0; i < 1000; i++);

    /* Configure USB clock (UCKSEL) - check SYSCFG register */
    debug_print("[USB] Before USBE: SYSCFG=");
    debug_hex(USB0.SYSCFG.WORD);
    debug_print("\r\n");

    /* Enable SCKE (USB clock enable) first, then USBE */
    USB0.SYSCFG.BIT.SCKE = 1;   /* USB clock enable */
    for (volatile int i = 0; i < 1000; i++);

    debug_print("[USB] After SCKE: SYSCFG=");
    debug_hex(USB0.SYSCFG.WORD);
    debug_print("\r\n");

    /* Configure as peripheral, Full-Speed */
    USB0.SYSCFG.BIT.USBE = 1;   /* USB module enable */
    USB0.SYSCFG.BIT.DPRPU = 0;  /* D+ pull-up disabled initially */
    USB0.SYSCFG.BIT.DRPD = 0;   /* D+/D- pull-down disabled */
    USB0.SYSCFG.BIT.DCFM = 0;   /* Peripheral mode */

    debug_print("[USB] SYSCFG=");
    debug_hex(USB0.SYSCFG.WORD);
    debug_print("\r\n");

    /* Disable all USB interrupts (polling mode) */
    USB0.INTENB0.WORD = 0x0000;
    USB0.INTENB1.WORD = 0x0000;
    USB0.BRDYENB.WORD = 0x0000;
    USB0.NRDYENB.WORD = 0x0000;
    USB0.BEMPENB.WORD = 0x0000;

    debug_print("[USB] Interrupts disabled\r\n");

    /* Clear all status bits */
    USB0.INTSTS0.WORD = 0x0000;
    USB0.INTSTS1.WORD = 0x0000;
    USB0.BRDYSTS.WORD = 0x0000;
    USB0.NRDYSTS.WORD = 0x0000;
    USB0.BEMPSTS.WORD = 0x0000;

    /* Initialize DCP (Default Control Pipe) */
    USB0.DCPCFG.WORD = 0x0000;
    USB0.DCPMAXP.WORD = 64;     /* Max packet size = 64 bytes */
    USB0.DCPCTR.WORD = 0x0000;  /* NAK */

    debug_print("[USB] usb_hw_init done\r\n");
}

/*******************************************************************************
 * Control Transfer Handling
 ******************************************************************************/

/* Write data to CFIFO for control IN transfer */
static void usb_write_cfifo(const uint8_t *data, uint16_t len)
{
    uint16_t i;
    volatile uint16_t timeout;

    debug_print("[USB] CFIFO write start, len=");
    debug_hex(len);
    debug_print("\r\n");

    /* First, set NAK to stop any transfer */
    USB0.DCPCTR.WORD = 0x0000;  /* PID=NAK */

    /* Select CFIFO for DCP with write (ISEL=1) */
    USB0.CFIFOSEL.WORD = 0x8020;  /* ISEL=1(write), MBW=00(8bit), CURPIPE=0(DCP) */

    /* Wait for CFIFO access ready (FRDY) */
    timeout = 10000;
    while (!(USB0.CFIFOCTR.WORD & 0x2000)) {  /* Wait FRDY */
        if (--timeout == 0) {
            debug_print("[USB] CFIFO timeout!\r\n");
            return;
        }
    }

    debug_print("[USB] CFIFO ready, CFIFOCTR=");
    debug_hex(USB0.CFIFOCTR.WORD);
    debug_print("\r\n");

    /* Clear buffer */
    USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR */
    for (volatile int w = 0; w < 100; w++);

    /* Limit to max packet size (64 bytes for control endpoint) */
    uint16_t write_len = (len > 64) ? 64 : len;

    /* Write data to FIFO (8-bit access) */
    for (i = 0; i < write_len; i++) {
        USB0.CFIFO.BYTE.L = data[i];
    }

    /* Set BVAL - always set for control transfers */
    /* For short packet (< 64) this marks end of transfer */
    /* For full packet (= 64) host will request more data */
    USB0.CFIFOCTR.WORD = 0x8000;  /* BVAL */

    debug_print("[USB] CFIFO write done (");
    debug_hex(write_len);
    debug_print(" bytes)\r\n");
}

/* Read SETUP packet from USBREQ register */
static void usb_read_setup(usb_setup_t *setup)
{
    uint16_t req = USB0.USBREQ.WORD;
    uint16_t val = USB0.USBVAL;       /* Direct access (no .WORD) */
    uint16_t idx = USB0.USBINDX;      /* Direct access (no .WORD) */
    uint16_t len = USB0.USBLENG;      /* Direct access (no .WORD) */

    setup->bmRequestType = (uint8_t)(req & 0xFF);
    setup->bRequest = (uint8_t)(req >> 8);
    setup->wValue = val;
    setup->wIndex = idx;
    setup->wLength = len;

    debug_print("[USB] SETUP: bmReqType=");
    debug_hex(setup->bmRequestType);
    debug_print(" bReq=");
    debug_hex(setup->bRequest);
    debug_print(" wVal=");
    debug_hex(setup->wValue);
    debug_print(" wIdx=");
    debug_hex(setup->wIndex);
    debug_print(" wLen=");
    debug_hex(setup->wLength);
    debug_print("\r\n");
}

/* Send next chunk of control IN data (called on BEMP for multi-packet) */
static void usb_ctrl_send_next(void)
{
    if (s_ctrl_data_remaining == 0 || s_ctrl_data_ptr == NULL) {
        return;
    }

    uint16_t send_len = (s_ctrl_data_remaining > 64) ? 64 : s_ctrl_data_remaining;

    debug_print("[USB] CTRL Send next, remaining=");
    debug_hex(s_ctrl_data_remaining);
    debug_print("\r\n");

    /* Write to CFIFO */
    USB0.DCPCTR.WORD = 0x0000;  /* PID=NAK */
    USB0.CFIFOSEL.WORD = 0x8020;

    volatile uint16_t timeout = 10000;
    while (!(USB0.CFIFOCTR.WORD & 0x2000)) {
        if (--timeout == 0) {
            debug_print("[USB] CTRL next timeout\r\n");
            s_ctrl_data_ptr = NULL;
            s_ctrl_data_remaining = 0;
            return;
        }
    }

    USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR */
    for (volatile int w = 0; w < 100; w++);

    for (uint16_t i = 0; i < send_len; i++) {
        USB0.CFIFO.BYTE.L = s_ctrl_data_ptr[i];
    }

    USB0.CFIFOCTR.WORD = 0x8000;  /* BVAL */

    s_ctrl_data_ptr += send_len;
    s_ctrl_data_remaining -= send_len;

    USB0.DCPCTR.WORD = 0x0001;  /* PID=BUF */
}

/* Send control IN response (supports multi-packet) */
static void usb_ctrl_send(const uint8_t *data, uint16_t len, uint16_t max_len)
{
    uint16_t send_len = (len < max_len) ? len : max_len;
    uint16_t first_len = (send_len > 64) ? 64 : send_len;

    debug_print("[USB] CTRL Send len=");
    debug_hex(send_len);
    debug_print("\r\n");

    /* Store remaining data for multi-packet transfer */
    if (send_len > 64) {
        s_ctrl_data_ptr = data + 64;
        s_ctrl_data_remaining = send_len - 64;
    } else {
        s_ctrl_data_ptr = NULL;
        s_ctrl_data_remaining = 0;
    }

    /* Ensure DCP is in NAK state and ready */
    USB0.DCPCTR.WORD = 0x0000;  /* PID=NAK, clear all flags */
    for (volatile int w = 0; w < 500; w++);  /* Wait for state change */

    /* Select CFIFO for DCP write */
    USB0.CFIFOSEL.WORD = 0x0000;  /* Deselect first */
    for (volatile int w = 0; w < 100; w++);
    USB0.CFIFOSEL.WORD = 0x8020;  /* ISEL=1, CURPIPE=0 */

    volatile uint16_t timeout = 50000;  /* Longer timeout */
    while (!(USB0.CFIFOCTR.WORD & 0x2000)) {
        if (--timeout == 0) {
            debug_print("[USB] CFIFO timeout! DCPCTR=");
            debug_hex(USB0.DCPCTR.WORD);
            debug_print(" CFIFOCTR=");
            debug_hex(USB0.CFIFOCTR.WORD);
            debug_print("\r\n");
            return;
        }
    }

    USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR */
    for (volatile int w = 0; w < 100; w++);

    for (uint16_t i = 0; i < first_len; i++) {
        USB0.CFIFO.BYTE.L = data[i];
    }

    USB0.CFIFOCTR.WORD = 0x8000;  /* BVAL */

    /* Set PID to BUF for data stage */
    USB0.DCPCTR.WORD = 0x0001;  /* PID=BUF - enable transmission */

    debug_print("[USB] DCPCTR=");
    debug_hex(USB0.DCPCTR.WORD);
    debug_print("\r\n");
}

/* Send ZLP (Zero Length Packet) for status stage */
static void usb_ctrl_send_zlp(void)
{
    volatile uint16_t timeout;

    debug_print("[USB] CTRL ZLP\r\n");

    /* Set NAK first */
    USB0.DCPCTR.WORD = 0x0000;

    /* Select CFIFO for write */
    USB0.CFIFOSEL.WORD = 0x8020;  /* ISEL=1, MBW=00, CURPIPE=0 */

    /* Wait for CFIFO ready */
    timeout = 10000;
    while (!(USB0.CFIFOCTR.WORD & 0x2000)) {
        if (--timeout == 0) {
            debug_print("[USB] ZLP timeout\r\n");
            return;
        }
    }

    /* Clear and set BVAL (empty packet) */
    USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR */
    for (volatile int w = 0; w < 100; w++);
    USB0.CFIFOCTR.WORD = 0x8000;  /* BVAL */

    /* Set PID to BUF */
    USB0.DCPCTR.WORD = 0x0001;
}

/* STALL control endpoint */
static void usb_ctrl_stall(void)
{
    debug_print("[USB] CTRL STALL\r\n");
    USB0.DCPCTR.WORD = 0x0002;  /* PID=STALL */
}

/* Handle GET_DESCRIPTOR request */
static void usb_handle_get_descriptor(usb_setup_t *setup)
{
    uint8_t desc_type = (uint8_t)(setup->wValue >> 8);
    uint8_t desc_index = (uint8_t)(setup->wValue & 0xFF);

    debug_print("[USB] GET_DESC type=");
    debug_hex(desc_type);
    debug_print(" idx=");
    debug_hex(desc_index);
    debug_print("\r\n");

    switch (desc_type) {
    case USB_DESC_DEVICE:
        usb_ctrl_send(g_device_descriptor, sizeof(g_device_descriptor), setup->wLength);
        break;

    case USB_DESC_CONFIGURATION:
        usb_ctrl_send(g_config_descriptor, sizeof(g_config_descriptor), setup->wLength);
        break;

    case USB_DESC_STRING:
        switch (desc_index) {
        case 0:
            usb_ctrl_send(g_string0, sizeof(g_string0), setup->wLength);
            break;
        case 1:
            usb_ctrl_send(g_string1, sizeof(g_string1), setup->wLength);
            break;
        case 2:
            usb_ctrl_send(g_string2, sizeof(g_string2), setup->wLength);
            break;
        default:
            usb_ctrl_stall();
            break;
        }
        break;

    default:
        usb_ctrl_stall();
        break;
    }
}

/* Handle SET_ADDRESS request */
static void usb_handle_set_address(usb_setup_t *setup)
{
    /* Save address - will be set after status stage completes */
    s_pending_address = (uint8_t)(setup->wValue & 0x7F);
    debug_print("[USB] SET_ADDRESS pending=");
    debug_hex(s_pending_address);
    debug_print("\r\n");

    /* For Control Write (No Data), just set CCPL to complete */
    /* The USB controller will automatically send ZLP status */
    USB0.DCPCTR.BIT.PID = 1;   /* PID=BUF */
    USB0.DCPCTR.BIT.CCPL = 1;  /* Control transfer complete */

    debug_print("[USB] SET_ADDR CCPL set, DCPCTR=");
    debug_hex(USB0.DCPCTR.WORD);
    debug_print("\r\n");

    /* Address will be set when we detect idle state (ctsq=0) with pending address */
}

/* Handle SET_CONFIGURATION request */
static void usb_handle_set_configuration(usb_setup_t *setup)
{
    s_configuration = (uint8_t)(setup->wValue & 0xFF);
    debug_print("[USB] SET_CONFIG config=");
    debug_hex(s_configuration);
    debug_print("\r\n");

    if (s_configuration == 1) {
        /* Configure endpoints for CDC */
        /* RX72N USB0 (USBF) - FIT Standard: PIPE1=Bulk IN, PIPE2=Bulk OUT */

        /*
         * Attempt 23: CRITICAL FIX for BSTS=1 issue
         *
         * Root cause analysis:
         * - Previous attempts: ACLRM alone doesn't clear BSTS=1
         * - USB0 lacks CSCLR bit (only USB1/USBA has it)
         * - FIT module uses CFIFO buffer clear + ACLRM + CSCLR
         *
         * Solution: Use CFIFO BCLR to force buffer clear
         * For IN pipes (PIPE1-9), ISEL=0 in CFIFOSEL
         *
         * FIT sequence (usb_cstd_pipe_init):
         * 1. Clear interrupts
         * 2. PID=NAK + clear STALL
         * 3. Configure PIPE registers
         * 4. SQCLR
         * 5. ACLRM
         * 6. CSCLR (not available on USB0!)
         * 7. Clear interrupt status
         *
         * Our workaround: CFIFO + BCLR to clear buffer contents
         */

        /*=== PIPE1: Bulk IN (EP2 IN) - for TX to host ===*/
        /*
         * FIT driver sequence (usb_cstd_pipe_init):
         * 1. Clear interrupts (BRDY, NRDY, BEMP)
         * 2. PID=NAK + clear STALL
         * 3. PIPECFG, PIPEMAXP configuration  <-- Configure FIRST!
         * 4. SQCLR
         * 5. ACLRM  <-- ACLRM AFTER configuration!
         * 6. Clear interrupt status
         *
         * KEY INSIGHT: ACLRM must be done AFTER PIPECFG is set!
         * The buffer is allocated based on PIPECFG.TYPE, so clearing
         * before configuration has no effect.
         *
         * CRITICAL FIX: For USB0 (USBF type), must set TYPE=00 first
         * to fully release the PIPE buffer before reconfiguration.
         */

        /* Step 1: Clear interrupt enables for PIPE1 */
        USB0.BRDYENB.WORD &= ~0x0002;  /* Disable BRDY */
        USB0.NRDYENB.WORD &= ~0x0002;  /* Disable NRDY */
        USB0.BEMPENB.WORD &= ~0x0002;  /* Disable BEMP */
        for (volatile int w = 0; w < 100; w++);

        /* Step 2: PID=NAK first */
        USB0.PIPE1CTR.WORD = 0x0000;  /* PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Step 2b: CRITICAL - Set PIPECFG.TYPE=00 to release PIPE buffer completely */
        USB0.PIPESEL.WORD = 0x0001;   /* Select PIPE1 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPECFG.WORD = 0x0000;   /* TYPE=00 (unused) - releases buffer */
        for (volatile int w = 0; w < 1000; w++);  /* Wait for buffer release */
        USB0.PIPESEL.WORD = 0x0000;   /* Deselect */
        for (volatile int w = 0; w < 100; w++);

        /* Step 3: Configure PIPE registers (FIT does this BEFORE ACLRM) */
        USB0.PIPESEL.WORD = 0x0001;   /* Select PIPE1 */
        for (volatile int w = 0; w < 100; w++);

        /* Note: USB0 (USBF type) does NOT have PIPEBUF register.
         * Buffer allocation is fixed per PIPE type.
         */

        /* PIPECFG: TYPE=01(Bulk), BFRE=0, DBLB=0, SHTNAK=0, DIR=1(IN), EPNUM=2 */
        USB0.PIPECFG.WORD = 0x4012;
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPEMAXP.WORD = 64;
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPESEL.WORD = 0x0000;   /* Deselect */
        for (volatile int w = 0; w < 100; w++);

        /* Step 4: SQCLR - Clear sequence bit to DATA0 */
        USB0.PIPE1CTR.WORD = 0x0100;  /* SQCLR=1 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPE1CTR.WORD = 0x0000;  /* Clear SQCLR, keep PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Step 5: ACLRM - Buffer clear AFTER configuration (FIT sequence)
         * Note: FIT driver does NOT use CFIFO BCLR for initialization.
         * Only ACLRM is used to clear the pipe buffer.
         */
        USB0.PIPE1CTR.WORD = 0x0200;  /* ACLRM=1, PID=NAK */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPE1CTR.WORD = 0x0000;  /* ACLRM=0, PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Note: USB0 does NOT have CSCLR bit (only USB1/USBA has it) */

        /* Verify BSTS status after buffer clear */
        {
            uint16_t ctr = USB0.PIPE1CTR.WORD;
            char dbg[80];
            sprintf(dbg, "[USB] PIPE1 after ACLRM: CTR=%04X (BSTS=%d)\r\n", ctr, (ctr >> 15) & 1);
            debug_print(dbg);
        }

        /* Step 6: Clear interrupt status for PIPE1 */
        USB0.BRDYSTS.WORD = ~0x0002;  /* Clear BRDY status */
        USB0.NRDYSTS.WORD = ~0x0002;  /* Clear NRDY status */
        USB0.BEMPSTS.WORD = ~0x0002;  /* Clear BEMP status */
        for (volatile int w = 0; w < 100; w++);

        /* Read back for verification */
        {
            uint16_t ctr = USB0.PIPE1CTR.WORD;
            char dbg[80];
            USB0.PIPESEL.WORD = 0x0001;
            sprintf(dbg, "[USB] PIPE1: CFG=%04X MAXP=%04X CTR=%04X\r\n",
                    USB0.PIPECFG.WORD, USB0.PIPEMAXP.WORD, ctr);
            debug_print(dbg);
            USB0.PIPESEL.WORD = 0x0000;
        }

        /*=== PIPE2: Bulk OUT (EP1 OUT) - for RX from host ===*/
        /* Step 1: Reset CTR first */
        USB0.PIPE2CTR.WORD = 0x0000;
        for (volatile int w = 0; w < 100; w++);

        /* Step 2: Set TYPE=00 first to release buffer completely */
        USB0.PIPESEL.WORD = 0x0002;   /* Select PIPE2 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPECFG.WORD = 0x0000;   /* TYPE=00 (unused) - releases buffer */
        for (volatile int w = 0; w < 1000; w++);  /* Wait for buffer release */
        USB0.PIPESEL.WORD = 0x0000;   /* Deselect */
        for (volatile int w = 0; w < 100; w++);

        /* Step 3: Configure PIPE */
        USB0.PIPESEL.WORD = 0x0002;   /* Select PIPE2 */
        for (volatile int w = 0; w < 100; w++);

        /* Note: USB0 does NOT have PIPEBUF register */

        USB0.PIPECFG.WORD = 0x4001;   /* TYPE=Bulk(01), DIR=OUT(0), EPNUM=1 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPEMAXP.WORD = 64;
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPESEL.WORD = 0x0000;   /* Deselect */

        /* Step 4: ACLRM buffer clear AFTER configuration (FIT sequence) */
        USB0.PIPE2CTR.WORD = 0x0200;  /* ACLRM=1 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPE2CTR.WORD = 0x0000;  /* ACLRM=0, PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Note: USB0 does NOT have CSCLR bit */

        /* Step 5: SQCLR then PID=BUF for receiving */
        USB0.PIPE2CTR.WORD = 0x0100;  /* SQCLR=1 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPE2CTR.WORD = 0x0001;  /* PID=BUF - ready to receive */
        for (volatile int w = 0; w < 100; w++);

        /* Read back for verification */
        {
            uint16_t ctr = USB0.PIPE2CTR.WORD;
            char dbg[80];
            USB0.PIPESEL.WORD = 0x0002;
            sprintf(dbg, "[USB] PIPE2: CFG=%04X MAXP=%04X CTR=%04X\r\n",
                    USB0.PIPECFG.WORD, USB0.PIPEMAXP.WORD, ctr);
            debug_print(dbg);
            USB0.PIPESEL.WORD = 0x0000;
        }

        /*=== PIPE6: Interrupt IN (EP3 IN) - for notifications ===*/
        /* Same approach as PIPE1 - Attempt 23 */

        /* Step 1: PID=NAK first */
        USB0.PIPE6CTR.WORD = 0x0000;  /* PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Step 1b: Set TYPE=00 first to release buffer completely */
        USB0.PIPESEL.WORD = 0x0006;   /* Select PIPE6 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPECFG.WORD = 0x0000;   /* TYPE=00 (unused) - releases buffer */
        for (volatile int w = 0; w < 1000; w++);  /* Wait for buffer release */
        USB0.PIPESEL.WORD = 0x0000;   /* Deselect */
        for (volatile int w = 0; w < 100; w++);

        /* Step 2: Configure PIPE registers */
        USB0.PIPESEL.WORD = 0x0006;
        for (volatile int w = 0; w < 100; w++);

        /* Note: USB0 does NOT have PIPEBUF register */

        USB0.PIPECFG.WORD = 0xC013;   /* TYPE=11(Int), DIR=1(IN), EPNUM=3, DBLB=0 (bit9=0) */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPEMAXP.WORD = 8;
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPESEL.WORD = 0x0000;   /* Deselect */
        for (volatile int w = 0; w < 100; w++);

        /* Step 3: SQCLR - Clear sequence bit */
        USB0.PIPE6CTR.BIT.SQCLR = 1;
        for (volatile int w = 0; w < 100; w++);

        /* Step 4: ACLRM - Auto buffer clear AFTER configuration
         * Note: FIT driver does NOT use CFIFO BCLR for initialization.
         */
        USB0.PIPE6CTR.WORD = 0x0200;  /* ACLRM=1, PID=NAK */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPE6CTR.WORD = 0x0000;  /* ACLRM=0, PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Note: USB0 does NOT have CSCLR bit */

        /* Apply SQCLR again */
        USB0.PIPE6CTR.WORD = 0x0100;  /* SQCLR=1 */
        for (volatile int w = 0; w < 100; w++);
        USB0.PIPE6CTR.WORD = 0x0000;  /* Clear SQCLR, keep PID=NAK */
        for (volatile int w = 0; w < 100; w++);

        /* Step 7: Keep PID=NAK - IN pipe not ready */

        /* Read back for verification */
        {
            uint16_t ctr = USB0.PIPE6CTR.WORD;
            char dbg[80];
            USB0.PIPESEL.WORD = 0x0006;
            sprintf(dbg, "[USB] PIPE6: CFG=%04X MAXP=%04X CTR=%04X\r\n",
                    USB0.PIPECFG.WORD, USB0.PIPEMAXP.WORD, ctr);
            debug_print(dbg);
            USB0.PIPESEL.WORD = 0x0000;
        }

        /* Enable BEMP interrupt for PIPE1 (bit1) at config time */
        USB0.BEMPENB.WORD |= 0x0002;
        USB0.NRDYENB.WORD |= 0x0002;

        s_state = USB_CDC_STATE_READY;
        debug_print("[USB] CDC Ready!\r\n");
    }

    usb_ctrl_send_zlp();
}

/* Handle standard device requests */
static void usb_handle_std_request(usb_setup_t *setup)
{
    switch (setup->bRequest) {
    case USB_GET_DESCRIPTOR:
        usb_handle_get_descriptor(setup);
        break;

    case USB_SET_ADDRESS:
        usb_handle_set_address(setup);
        break;

    case USB_SET_CONFIGURATION:
        usb_handle_set_configuration(setup);
        break;

    case USB_GET_CONFIGURATION:
        {
            uint8_t cfg = (uint8_t)s_configuration;
            usb_ctrl_send(&cfg, 1, setup->wLength);
        }
        break;

    case USB_GET_STATUS:
        {
            static const uint8_t status[2] = {0, 0};
            usb_ctrl_send(status, 2, setup->wLength);
        }
        break;

    case USB_CLEAR_FEATURE:
        {
            /* Handle CLEAR_FEATURE for endpoints (clear HALT)
             * FIT Standard: PIPE1=Bulk IN (EP2), PIPE2=Bulk OUT (EP1), PIPE6=Int IN (EP3)
             *
             * CRITICAL: Use WORD access for all register writes to ensure atomicity.
             * CLEAR_FEATURE(ENDPOINT_HALT) requires:
             * 1. Clear STALL/Reset PID to NAK
             * 2. Reset data toggle (SQCLR) to DATA0
             * 3. Optionally clear buffer (ACLRM)
             */
            uint8_t ep = setup->wIndex & 0x0F;
            uint8_t dir_in = (setup->wIndex & 0x80) ? 1 : 0;

            /* Only log first CLEAR_FEATURE to reduce debug spam */
            static uint8_t cf_count = 0;
            if (cf_count < 3) {
                debug_print("[USB] CLR_FEAT EP");
                debug_hex(ep);
                debug_print(dir_in ? " IN\r\n" : " OUT\r\n");
                cf_count++;
            }

            if (ep == 1 && !dir_in) {
                /* EP1 OUT = PIPE2 - Ready to receive */
                USB0.PIPE2CTR.WORD = 0x0000;  /* PID=NAK */
                for (volatile int w = 0; w < 100; w++);
                USB0.PIPE2CTR.WORD = 0x0100;  /* SQCLR=1 */
                for (volatile int w = 0; w < 100; w++);
                USB0.PIPE2CTR.WORD = 0x0200;  /* ACLRM=1 */
                for (volatile int w = 0; w < 500; w++);
                USB0.PIPE2CTR.WORD = 0x0000;  /* ACLRM=0 */
                for (volatile int w = 0; w < 100; w++);
                USB0.PIPE2CTR.WORD = 0x0001;  /* PID=BUF - ready to receive */
            } else if (ep == 2 && dir_in) {
                /* EP2 IN = PIPE1 - Bulk IN for TX
                 * Host is sending CLEAR_FEATURE because endpoint is not responding.
                 *
                 * CRITICAL FIX: Do NOT reset PIPE1 during active transmission!
                 * If we reset during tx, the data in the buffer will be lost.
                 * Just acknowledge the request but don't disturb the pipe.
                 * The host will retry after we ACK this request.
                 */
                if (!s_tx_busy) {
                    /* Only reset if not transmitting */
                    USB0.PIPE1CTR.WORD = 0x0000;  /* PID=NAK first */
                    for (volatile int w = 0; w < 500; w++);
                    USB0.PIPE1CTR.WORD = 0x0100;  /* SQCLR=1 - Reset sequence bit to DATA0 */
                    for (volatile int w = 0; w < 100; w++);
                    USB0.PIPE1CTR.WORD = 0x0200;  /* ACLRM=1 - Clear buffer */
                    for (volatile int w = 0; w < 500; w++);
                    USB0.PIPE1CTR.WORD = 0x0000;  /* ACLRM=0, PID=NAK */
                    for (volatile int w = 0; w < 100; w++);
                }
                /* If tx_busy, just ACK the CLEAR_FEATURE without disturbing the pipe */
            } else if (ep == 3 && dir_in) {
                /* EP3 IN = PIPE6 - Interrupt IN for notifications */
                USB0.PIPE6CTR.WORD = 0x0000;  /* PID=NAK first */
                for (volatile int w = 0; w < 500; w++);
                USB0.PIPE6CTR.WORD = 0x0100;  /* SQCLR=1 */
                for (volatile int w = 0; w < 100; w++);
                USB0.PIPE6CTR.WORD = 0x0200;  /* ACLRM=1 */
                for (volatile int w = 0; w < 500; w++);
                USB0.PIPE6CTR.WORD = 0x0000;  /* ACLRM=0, PID=NAK */
                for (volatile int w = 0; w < 100; w++);
                /* Keep PID=NAK for IN pipe */
            }

            /* Ensure CFIFO is set back to DCP before sending status */
            USB0.CFIFOSEL.WORD = 0x0000;
            for (volatile int w = 0; w < 100; w++);

            /* ACK the request */
            USB0.DCPCTR.BIT.PID = 1;
            USB0.DCPCTR.BIT.CCPL = 1;
        }
        break;

    default:
        debug_print("[USB] Unknown STD request: ");
        debug_hex(setup->bRequest);
        debug_print("\r\n");
        usb_ctrl_stall();
        break;
    }
}

/* Handle CDC class requests */
static void usb_handle_class_request(usb_setup_t *setup)
{
    volatile uint16_t timeout;

    switch (setup->bRequest) {
    case CDC_GET_LINE_CODING:
        debug_print("[USB] CDC GET_LINE_CODING\r\n");

        /* Direct CFIFO write for LINE_CODING */
        USB0.DCPCTR.WORD = 0x0000;  /* NAK */
        for (volatile int w = 0; w < 1000; w++);

        USB0.CFIFOSEL.WORD = 0x0000;
        for (volatile int w = 0; w < 500; w++);
        USB0.CFIFOSEL.WORD = 0x8020;  /* ISEL=1, CURPIPE=0 */

        timeout = 50000;
        while (!(USB0.CFIFOCTR.WORD & 0x2000)) {
            if (--timeout == 0) {
                debug_print("[USB] GET_LINE_CODING CFIFO timeout\r\n");
                USB0.DCPCTR.WORD = 0x0002;  /* STALL */
                return;
            }
        }

        USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR */
        for (volatile int w = 0; w < 200; w++);

        /* Write 7 bytes of line coding */
        for (int i = 0; i < 7; i++) {
            USB0.CFIFO.BYTE.L = s_line_coding[i];
        }
        USB0.CFIFOCTR.WORD = 0x8000;  /* BVAL */
        USB0.DCPCTR.WORD = 0x0001;  /* PID=BUF */

        debug_print("[USB] GET_LINE_CODING sent\r\n");
        break;

    case CDC_SET_LINE_CODING:
        debug_print("[USB] CDC SET_LINE_CODING\r\n");
        /* Control Write with data - need to receive 7 bytes in DATA OUT stage */
        if (setup->wLength > 0) {
            /* Prepare to receive data in DATA OUT stage */
            s_ctrl_write_pending = true;
            s_ctrl_write_len = setup->wLength;
            /* Set PID=BUF to accept DATA OUT packets */
            USB0.DCPCTR.BIT.PID = 1;  /* BUF - ready to receive */
            /* Do NOT set CCPL yet - wait for data reception */
        } else {
            /* No data - just ACK */
            USB0.DCPCTR.BIT.PID = 1;
            USB0.DCPCTR.BIT.CCPL = 1;
        }
        break;

    case CDC_SET_CONTROL_LINE:
        {
            /* wValue bit0 = DTR, bit1 = RTS */
            /* DTR = 1 means host opened the COM port */
            char dbg[48];
            sprintf(dbg, "[USB] SET_CONTROL_LINE wVal=%04X\r\n", setup->wValue);
            debug_print(dbg);

            if (setup->wValue & 0x0001) {
                /* DTR=1: Host opened the port */
                s_port_opened = true;
                debug_print("[USB] DTR=1 -> Port OPENED\r\n");
            } else {
                /* DTR=0: Host closed the port */
                s_port_opened = false;
                debug_print("[USB] DTR=0 -> Port closed\r\n");
            }

            /* Control Write no data - just ACK */
            USB0.DCPCTR.BIT.PID = 1;
            USB0.DCPCTR.BIT.CCPL = 1;
        }
        break;

    default:
        debug_print("[USB] Unknown CDC request: ");
        debug_hex(setup->bRequest);
        debug_print("\r\n");
        usb_ctrl_stall();
        break;
    }
}

/* Process SETUP packet */
static void usb_process_setup(void)
{
    usb_setup_t setup;

    usb_read_setup(&setup);

    uint8_t type = setup.bmRequestType & 0x60;  /* Request type */

    if (type == 0x00) {
        /* Standard request */
        usb_handle_std_request(&setup);
    } else if (type == 0x20) {
        /* Class request */
        usb_handle_class_request(&setup);
    } else {
        usb_ctrl_stall();
    }
}

/* Enable D+ pull-up (connect to host) */
static void usb_attach(void)
{
    debug_print("[USB] Attaching (D+ pull-up ON)\r\n");
    USB0.SYSCFG.BIT.DPRPU = 1;  /* D+ pull-up enable */
    s_state = USB_CDC_STATE_ATTACHED;

    /* Force attached state (bypass VBUS detection) */
    debug_print("[USB] Forcing ATTACHED state (VBUS bypass)\r\n");
}

/*******************************************************************************
 * Public Functions
 ******************************************************************************/

usb_cdc_err_t usb_cdc_minimal_init(void)
{
    debug_print("\r\n[USB CDC Minimal] Init start\r\n");

    usb_hw_init();

    /* Short delay before enabling pull-up */
    for (volatile int i = 0; i < 100000; i++);

    usb_attach();

    debug_print("[USB CDC Minimal] Init complete\r\n");
    return USB_CDC_SUCCESS;
}

void usb_cdc_minimal_task(void)
{
    static uint32_t loop_cnt = 0;
    static uint16_t prev_intsts0 = 0xFFFF;
    uint16_t intsts0 = USB0.INTSTS0.WORD;

    /* Debug: Show INTSTS0 changes */
    if (intsts0 != prev_intsts0) {
        debug_print("[USB] INTSTS0=");
        debug_hex(intsts0);
        debug_print("\r\n");
        prev_intsts0 = intsts0;
    }

    /* Periodic debug output */
    loop_cnt++;
    if (loop_cnt >= 5000000) {
        loop_cnt = 0;
        debug_print("[USB] State: SYSCFG=");
        debug_hex(USB0.SYSCFG.WORD);
        debug_print(" DCPCTR=");
        debug_hex(USB0.DCPCTR.WORD);
        debug_print("\r\n");
    }

    /* Check for USB reset */
    if (intsts0 & 0x4000) {  /* DVST: Device State Transition (Bus Reset) */
        debug_print("[USB] Bus Reset detected\r\n");
        USB0.INTSTS0.WORD = (uint16_t)~0x4000;  /* Clear bit */
        s_device_address = 0;
        s_configuration = 0;
        s_state = USB_CDC_STATE_ATTACHED;

        /* Reset DCP (Default Control Pipe) */
        USB0.DCPCFG.WORD = 0x0000;
        USB0.DCPMAXP.WORD = 64;      /* Max packet size 64 */
        USB0.DCPCTR.WORD = 0x0000;   /* PID=NAK */
    }

    /* Check for SETUP packet (VALID bit in INTSTS0) */
    if (intsts0 & 0x0008) {  /* VALID: Setup packet received */
        debug_print("[USB] SETUP packet received\r\n");
        USB0.INTSTS0.WORD = (uint16_t)~0x0008;  /* Clear VALID */

        /* Clear any pending control transfer state */
        s_ctrl_data_ptr = NULL;
        s_ctrl_data_remaining = 0;
        s_ctrl_write_pending = false;
        s_ctrl_write_len = 0;

        /* Reset DCP completely for new control transfer */
        USB0.DCPCTR.WORD = 0x0000;  /* PID=NAK, clear CCPL */
        for (volatile int w = 0; w < 500; w++);

        /* Clear CFIFO completely */
        USB0.CFIFOSEL.WORD = 0x0000;  /* Deselect CFIFO */
        for (volatile int w = 0; w < 500; w++);
        USB0.CFIFOSEL.WORD = 0x8020;  /* ISEL=1, CURPIPE=0 */
        for (volatile int w = 0; w < 1000; w++);

        /* Wait for FRDY and clear buffer */
        volatile uint16_t timeout = 10000;
        while (!(USB0.CFIFOCTR.WORD & 0x2000)) {  /* Wait FRDY */
            if (--timeout == 0) break;
        }
        USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR - clear buffer */
        for (volatile int w = 0; w < 500; w++);

        usb_process_setup();
    }

    /* Check for Control Transfer stage transition (CTRT) */
    if (intsts0 & 0x0800) {  /* CTRT: Control Transfer Stage Transition */
        uint8_t ctsq = (uint8_t)(intsts0 & 0x07);  /* CTSQ[2:0] */
        USB0.INTSTS0.WORD = (uint16_t)~0x0800;  /* Clear CTRT */

        debug_print("[USB] CTRT ctsq=");
        debug_hex(ctsq);
        debug_print("\r\n");

        switch (ctsq) {
        case 0:  /* Idle or Setup stage */
            /* Apply pending address when returning to idle after SET_ADDRESS */
            if (s_pending_address != 0) {
                USB0.USBADDR.WORD = s_pending_address;
                s_device_address = s_pending_address;
                debug_print("[USB] Address set to ");
                debug_hex(s_device_address);
                debug_print("\r\n");
                s_pending_address = 0;
            }
            /* Clear Control Write pending state if somehow still set */
            s_ctrl_write_pending = false;
            break;
        case 1:  /* Control Read Data stage */
            break;
        case 2:  /* Control Read Status stage - host sent ZLP ACK */
            debug_print("[USB] Control Read complete\r\n");
            USB0.DCPCTR.BIT.CCPL = 1;  /* Control transfer complete */
            break;
        case 3:  /* Control Write Data stage - ready to receive data from host */
            debug_print("[USB] Control Write Data stage\r\n");
            /* If SET_LINE_CODING pending, data will be received in DCP BRDY */
            /* Make sure PID=BUF so we can receive data */
            if (s_ctrl_write_pending) {
                USB0.DCPCTR.BIT.PID = 1;  /* BUF - ready to receive */
            }
            break;
        case 4:  /* Control Write Status stage */
            debug_print("[USB] Control Write complete\r\n");
            USB0.DCPCTR.BIT.CCPL = 1;
            break;
        case 5:  /* Control Write (no data) Status stage */
            debug_print("[USB] Control NoData complete\r\n");
            /* Note: CCPL already set in SET_ADDRESS handler */
            /* Apply pending address */
            if (s_pending_address != 0) {
                USB0.USBADDR.WORD = s_pending_address;
                s_device_address = s_pending_address;
                debug_print("[USB] Address set to ");
                debug_hex(s_device_address);
                debug_print("\r\n");
                s_pending_address = 0;
            }
            break;
        default:
            break;
        }
    }

    /* Check for BEMP (Buffer Empty) - transmission complete */
    if (USB0.BEMPSTS.WORD & 0x0001) {  /* DCP BEMP */
        USB0.BEMPSTS.WORD = (uint16_t)~0x0001;  /* Clear */
        debug_print("[USB] DCP BEMP (TX complete)\r\n");

        /* Send remaining data if multi-packet transfer */
        if (s_ctrl_data_remaining > 0) {
            usb_ctrl_send_next();
        }
    }

    /* Check for PIPE1 BEMP (Bulk IN transmission complete) */
    if (USB0.BEMPSTS.WORD & 0x0002) {  /* PIPE1 BEMP */
        USB0.BEMPSTS.WORD = (uint16_t)~0x0002;  /* Clear */
        debug_print("[USB] PIPE1 BEMP (Bulk IN sent)\r\n");
        s_tx_busy = false;
    }

    /* Check for DCP BRDY (Control OUT data received - for SET_LINE_CODING etc.) */
    if (USB0.BRDYSTS.WORD & 0x0001) {  /* DCP BRDY */
        USB0.BRDYSTS.WORD = (uint16_t)~0x0001;  /* Clear */
        debug_print("[USB] DCP BRDY (Control Write data)\r\n");

        if (s_ctrl_write_pending) {
            /* Read data from CFIFO */
            USB0.CFIFOSEL.WORD = 0x0000;  /* ISEL=0 for reading, CURPIPE=0 (DCP) */
            for (volatile int w = 0; w < 500; w++);

            /* Wait for FRDY */
            volatile uint16_t timeout = 10000;
            while (!(USB0.CFIFOCTR.WORD & 0x2000)) {
                if (--timeout == 0) break;
            }

            if (timeout > 0) {
                /* Read data length */
                uint16_t rcvLen = USB0.CFIFOCTR.WORD & 0x01FF;  /* DTLN bits */
                char dbg[64];
                sprintf(dbg, "[USB] Control Write data len=%d\r\n", rcvLen);
                debug_print(dbg);

                /* Read data (for SET_LINE_CODING, it's 7 bytes) */
                if (rcvLen <= 7) {
                    for (uint16_t i = 0; i < rcvLen; i++) {
                        s_line_coding[i] = USB0.CFIFO.BYTE.L;
                    }
                    debug_print("[USB] Line coding received\r\n");
                }

                /* Clear buffer */
                USB0.CFIFOCTR.WORD = 0x4000;  /* BCLR */
            }

            s_ctrl_write_pending = false;
            s_ctrl_write_len = 0;

            /* Send status stage (ZLP) - CCPL */
            USB0.DCPCTR.BIT.CCPL = 1;
            debug_print("[USB] Control Write status stage\r\n");
        }
    }

    /* Check for PIPE2 BRDY (Bulk OUT data received) */
    if (USB0.BRDYSTS.WORD & 0x0004) {  /* PIPE2 BRDY */
        USB0.BRDYSTS.WORD = (uint16_t)~0x0004;  /* Clear */
        debug_print("[USB] PIPE2 BRDY (RX data)\r\n");
        /* TODO: Read received data from D1FIFO */
    }
}

usb_cdc_state_t usb_cdc_minimal_get_state(void)
{
    return s_state;
}

bool usb_cdc_minimal_is_ready(void)
{
    return (s_state == USB_CDC_STATE_READY);
}

usb_cdc_err_t usb_cdc_minimal_send(const uint8_t *data, uint32_t len)
{
    volatile uint32_t timeout;

    if (s_state != USB_CDC_STATE_READY) {
        return USB_CDC_ERR_NOT_OPEN;
    }

    if (!s_port_opened) {
        return USB_CDC_ERR_NOT_OPEN;
    }

    if (len == 0 || len > 64) {
        return USB_CDC_ERR_FAILED;
    }

    debug_print("[USB] Send len=");
    debug_hex(len);
    debug_print("\r\n");

    /*
     * FIT driver uses CFIFO for all PIPEs when DMA is disabled.
     * Key insight: CFIFOSEL.ISEL=0 for PIPE1-9 (ISEL is only for DCP direction)
     *
     * Sequence based on FIT r_usb_plibusbip.c usb_pstd_write_data():
     * 1. Select CFIFO for PIPE1 with ISEL=0
     * 2. Wait for FRDY
     * 3. Write data to D0FIFO (NOT CFIFO - CFIFO is for DCP only)
     * 4. Set BVAL if short packet
     * 5. Clear BEMP status
     * 6. Set PID=BUF
     */

    /* Step 1: Clear interrupt status */
    USB0.BEMPSTS.WORD = (uint16_t)~0x0002;
    USB0.BRDYSTS.WORD = (uint16_t)~0x0002;
    USB0.NRDYSTS.WORD = (uint16_t)~0x0002;

    /* Step 2: Set NAK and clear buffer (fresh start for each transmission) */
    USB0.PIPE1CTR.WORD = 0x0000;  /* PID=NAK */
    for (volatile int w = 0; w < 100; w++);

    /* Step 2b: ACLRM - Clear any stale data in pipe buffer */
    USB0.PIPE1CTR.WORD = 0x0200;  /* ACLRM=1, PID=NAK */
    for (volatile int w = 0; w < 100; w++);
    USB0.PIPE1CTR.WORD = 0x0000;  /* ACLRM=0, PID=NAK */
    for (volatile int w = 0; w < 100; w++);

    /* Step 2c: SQCLR - Reset sequence bit to DATA0 */
    USB0.PIPE1CTR.WORD = 0x0100;  /* SQCLR=1 */
    for (volatile int w = 0; w < 100; w++);
    USB0.PIPE1CTR.WORD = 0x0000;  /* Clear SQCLR */
    for (volatile int w = 0; w < 100; w++);

    /* Debug: Check BSTS after ACLRM */
    {
        char dbg[80];
        sprintf(dbg, "[USB] After ACLRM: PIPE1=%04X (BSTS=%d SQMON=%d)\r\n",
                USB0.PIPE1CTR.WORD, (USB0.PIPE1CTR.WORD >> 15) & 1, (USB0.PIPE1CTR.WORD >> 6) & 1);
        debug_print(dbg);
    }

    /* Step 3: Select D0FIFO for PIPE1 (FIT driver uses D0FIFO/D1FIFO for Bulk, not CFIFO)
     * D0FIFOSEL: MBW=01(bits11-10=16bit), CURPIPE=1(bits3-0)
     * CFIFO is for DCP (Control transfers) only!
     */
    USB0.D0FIFOSEL.WORD = 0x0000;  /* First disconnect */
    for (volatile int w = 0; w < 100; w++);
    while ((USB0.D0FIFOSEL.WORD & 0x000F) != 0x0000);  /* Wait for disconnect */

    USB0.D0FIFOSEL.WORD = 0x0401;  /* MBW=16bit, CURPIPE=1 */
    for (volatile int w = 0; w < 100; w++);

    /* Step 3b: Wait for FIFO port access to be ready */
    {
        uint32_t port_timeout = 10000;
        while ((USB0.D0FIFOSEL.WORD & 0x000F) != 0x0001 && --port_timeout > 0);
        if (port_timeout == 0) {
            debug_print("[USB] D0FIFO port select failed!\r\n");
            USB0.D0FIFOSEL.WORD = 0x0000;
            return USB_CDC_ERR_FAILED;
        }
    }

    /* Step 3c: Clear FIFO buffer using BCLR (FIT uses this for receive, works for TX too)
     * This clears any stale data and resets BSTS
     */
    USB0.D0FIFOCTR.WORD = 0x4000;  /* BCLR=1 */
    for (volatile int w = 0; w < 100; w++);

    /* Step 4: Wait for FRDY */
    {
        uint32_t frdy_timeout = 10000;
        while (!(USB0.D0FIFOCTR.WORD & 0x2000) && --frdy_timeout > 0) {
            /* Check for CURPIPE change */
            if ((USB0.D0FIFOSEL.WORD & 0x000F) != 0x0001) {
                USB0.D0FIFOSEL.WORD = 0x0401;  /* Re-select with MBW=16bit */
            }
        }
        if (frdy_timeout == 0) {
            char dbg[80];
            sprintf(dbg, "[USB] D0FIFO FRDY timeout! SEL=%04X CTR=%04X\r\n",
                    USB0.D0FIFOSEL.WORD, USB0.D0FIFOCTR.WORD);
            debug_print(dbg);
            USB0.D0FIFOSEL.WORD = 0x0000;
            return USB_CDC_ERR_FAILED;
        }
    }

    /* Step 5: Write data to D0FIFO using 16-bit access (like FIT driver)
     * FIT uses 16-bit writes for even bytes, then 8-bit for odd byte
     */
    {
        uint32_t i;
        /* Write 16-bit words */
        for (i = 0; i + 1 < len; i += 2) {
            USB0.D0FIFO.WORD = (uint16_t)data[i] | ((uint16_t)data[i+1] << 8);
        }
        /* Write remaining odd byte if any */
        if (i < len) {
            /* Switch to 8-bit access for last byte */
            uint16_t sel = USB0.D0FIFOSEL.WORD;
            USB0.D0FIFOSEL.WORD = (sel & ~0x0C00);  /* MBW=8bit, preserve other bits */
            USB0.D0FIFO.BYTE.L = data[i];
            USB0.D0FIFOSEL.WORD = sel;  /* Restore MBW=16bit */
        }
    }

    /* Step 6: Set BVAL for short packet (less than max packet size)
     * For D0FIFO, write to D0FIFOCTR
     */
    if (len < 64) {
        USB0.D0FIFOCTR.WORD |= 0x8000;  /* BVAL=1 */
    }

    /* Debug: Check D0FIFO state after BVAL */
    {
        char dbg[100];
        sprintf(dbg, "[USB] After BVAL: D0SEL=%04X D0CTR=%04X PIPE1=%04X\r\n",
                USB0.D0FIFOSEL.WORD, USB0.D0FIFOCTR.WORD, USB0.PIPE1CTR.WORD);
        debug_print(dbg);
    }

    /* Step 7: Keep D0FIFO connected - FIT driver keeps FIFO connected during transmission
     * Hardware automatically transfers data when IN token is received
     */

    /* Step 8: Clear BEMP status before PID=BUF */
    USB0.BEMPSTS.WORD = (uint16_t)~0x0002;

    /* Step 9: Enable BEMP interrupt */
    USB0.BEMPENB.WORD |= 0x0002;
    USB0.NRDYENB.WORD |= 0x0002;

    /* Step 10: Set PID=BUF to start transmission
     * D0FIFO is connected, data is in FIFO ready to send
     */
    USB0.PIPE1CTR.BIT.PID = 1;  /* PID=BUF */
    s_tx_busy = true;

    {
        char dbg[80];
        sprintf(dbg, "[USB] PID=BUF: PIPE1=%04X D0CTR=%04X NRDY=%04X\r\n",
                USB0.PIPE1CTR.WORD, USB0.D0FIFOCTR.WORD, USB0.NRDYSTS.WORD);
        debug_print(dbg);
    }

    /* Wait for transmission complete (BEMP) */
    timeout = 500000;
    {
        uint32_t nrdy_count = 0;
        while (s_tx_busy && --timeout > 0) {
            usb_cdc_minimal_task();

            if (USB0.BEMPSTS.WORD & 0x0002) {
                USB0.BEMPSTS.WORD = (uint16_t)~0x0002;
                s_tx_busy = false;
                /* Disconnect D0FIFO after transmission */
                USB0.D0FIFOSEL.WORD = 0x0000;
                debug_print("[USB] TX done\r\n");
                break;
            }

            if (USB0.NRDYSTS.WORD & 0x0002) {
                USB0.NRDYSTS.WORD = (uint16_t)~0x0002;
                nrdy_count++;
                if (nrdy_count <= 3) {
                    char dbg[80];
                    sprintf(dbg, "[USB] NRDY#%lu PIPE1=%04X D0CTR=%04X SQMON=%d\r\n",
                            nrdy_count, USB0.PIPE1CTR.WORD, USB0.D0FIFOCTR.WORD,
                            (USB0.PIPE1CTR.WORD >> 6) & 1);
                    debug_print(dbg);
                }
                /* NRDY means host sent IN token but we NAK'd.
                 * Ensure PID=BUF to respond to next IN token */
                if ((USB0.PIPE1CTR.WORD & 0x0003) != 0x0001) {
                    USB0.PIPE1CTR.BIT.PID = 1;  /* Re-set PID=BUF */
                }
            }
        }
    }

    if (timeout == 0) {
        char dbg[80];
        sprintf(dbg, "[USB] TX timeout! PIPE1=%04X D0CTR=%04X\r\n",
                USB0.PIPE1CTR.WORD, USB0.D0FIFOCTR.WORD);
        debug_print(dbg);
        s_tx_busy = false;
        USB0.D0FIFOSEL.WORD = 0x0000;  /* Disconnect D0FIFO */
        USB0.PIPE1CTR.WORD = 0x0000;
        return USB_CDC_ERR_FAILED;
    }

    return USB_CDC_SUCCESS;
}

bool usb_cdc_minimal_rx_available(void)
{
    return (s_rx_write_idx != s_rx_read_idx);
}

bool usb_cdc_minimal_rx_get(uint8_t *byte)
{
    if (s_rx_write_idx == s_rx_read_idx) {
        return false;
    }
    *byte = s_rx_buffer[s_rx_read_idx];
    s_rx_read_idx = (s_rx_read_idx + 1) % sizeof(s_rx_buffer);
    return true;
}
