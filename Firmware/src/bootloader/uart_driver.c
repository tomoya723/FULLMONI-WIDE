/*
 * UART Driver for SCI9 (Based on Smart Configurator)
 * RX72N Bootloader
 *
 * PB7 = TXD9 (Physical pin 53)
 * PB6 = RXD9 (Physical pin 54)
 * 115200bps, 8N1
 */

#include <stdint.h>
#include <stdbool.h>
#include "iodefine.h"

/* SCI9 Register Definitions */
#define SCI9_BASE   ((volatile struct st_sci9 *)0x0008A120)
#define MSTP_SCI9_BIT  (1 << 22)  /* MSTPCRB bit 22 */

/* グローバル変数 */
static volatile uint8_t uart_tx_busy = 0;

/*
 * UART初期化 (Smart Configuratorの設定を完全移植)
 */
void uart_init(void)
{
    /* Protect off for system registers and MPC */
    SYSTEM.PRCR.WORD = 0xA503;
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;

    /* Cancel SCI stop state */
    MSTP(SCI9) = 0U;

    /* Clear the control register */
    SCI9.SCR.BYTE = 0x00U;

    /* Set clock enable */
    SCI9.SCR.BYTE = 0x00U;  /* _00_SCI_INTERNAL_SCK_UNUSED */

    /* Clear the SIMR1.IICM, SPMR.CKPH, and CKPOL bit, and set SPMR */
    SCI9.SIMR1.BIT.IICM = 0U;
    SCI9.SPMR.BYTE = 0x00U;  /* _00_SCI_RTS | _00_SCI_CLOCK_NOT_INVERTED | _00_SCI_CLOCK_NOT_DELAYED */

    /* Set control registers */
    SCI9.SMR.BYTE = 0x00U;   /* PCLK, no parity, 1 stop, 8-bit, async */
    SCI9.SCMR.BYTE = 0x72U;  /* F2 = 0x72 = _00_SCI_SERIAL_MODE | _00_SCI_DATA_INVERT_NONE | _00_SCI_DATA_LSB_FIRST | _10_SCI_DATA_LENGTH_8_OR_7 | _62_SCI_SCMR_DEFAULT */
    SCI9.SEMR.BYTE = 0x10U;  /* _10_SCI_8_BASE_CLOCK */

    /* Set bit rate */
    SCI9.BRR = 0x40U;

    /* Set RXD9 pin (PB6) */
    MPC.PB6PFS.BYTE = 0x0AU;
    PORTB.PMR.BYTE |= 0x40U;

    /* Set TXD9 pin (PB7) */
    MPC.PB7PFS.BYTE = 0x0AU;
    PORTB.PODR.BYTE |= 0x80U;
    PORTB.PDR.BYTE |= 0x80U;

    /* Protect on */
    MPC.PWPR.BIT.PFSWE = 0;
    MPC.PWPR.BIT.B0WI = 1;
    SYSTEM.PRCR.WORD = 0xA500;

    /* Wait for at least 1 bit period after BRR setup (約8.7μs@115200bps) */
    /* 60MHz = 16.67ns/cycle, 8.7μs = 522 cycles */
    for (volatile int i = 0; i < 600; i++);

    /* Enable transmit and receive */
    SCI9.SCR.BYTE = 0x30U;  /* TE=1, RE=1 */
}

/*
 * 1バイト送信
 */
void uart_putc(char c)
{
    /* Wait for transmit data register empty (TDRE) */
    while (SCI9.SSR.BIT.TDRE == 0);

    /* Write data */
    SCI9.TDR = (uint8_t)c;

    /* Clear TDRE flag */
    SCI9.SSR.BIT.TDRE = 0;
}

/*
 * 文字列送信
 */
void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') {
            uart_putc('\r');
        }
        uart_putc(*str++);
    }
}

/*
 * 1バイト受信 (ポーリング)
 */
bool uart_getc(uint8_t *data)
{
    /* Check if data available */
    if (SCI9.SSR.BIT.RDRF == 0) {
        return false;
    }

    /* Read data */
    *data = SCI9.RDR;

    /* Clear RDRF flag */
    SCI9.SSR.BIT.RDRF = 0;

    return true;
}

/*
 * 受信データ有無チェック
 */
bool uart_data_available(void)
{
    return (SCI9.SSR.BIT.RDRF != 0);
}
