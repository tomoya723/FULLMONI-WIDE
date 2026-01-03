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
 * UART初期化 (Smart Configuratorの設定を移植)
 */
void uart_init(void)
{
    /* Protect off for system registers */
    SYSTEM.PRCR.WORD = 0xA503;
    
    /* Cancel SCI9 stop state */
    SYSTEM.MSTPCRB.LONG &= ~MSTP_SCI9_BIT;
    
    /* Protect off */
    MPC.PWPR.BIT.B0WI = 0;
    MPC.PWPR.BIT.PFSWE = 1;
    
    /* Clear the control register */
    SCI9.SCR.BYTE = 0x00U;
    
    /* Set clock enable */
    SCI9.SCR.BYTE = 0x00U;  /* Internal clock unused */
    
    /* Clear SIMR1.IICM, set SPMR */
    SCI9.SIMR1.BIT.IICM = 0U;
    SCI9.SPMR.BYTE = 0x00U;
    
    /* Set control registers */
    SCI9.SMR.BYTE = 0x00U;   /* PCLK, 8N1, async mode */
    SCI9.SCMR.BYTE = 0x72U;  /* Serial mode, LSB first, 8-bit */
    SCI9.SEMR.BYTE = 0x10U;  /* 8 base clock */
    
    /* Set bit rate: 115200bps @ PCLKB=60MHz */
    /* BRR = (60000000 / (32 * 115200)) - 1 = 15.28 ≈ 15 (実測値は0x40=64が正しい) */
    SCI9.BRR = 0x40U;  /* Smart Configuratorの設定値 */
    
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
    
    /* Enable transmit and receive */
    SCI9.SCR.BYTE = 0x30U;  /* TE=1, RE=1 */
}

/*
 * 1バイト送信
 */
void uart_putc(char c)
{
    /* Wait for transmit buffer empty */
    while (SCI9.SSR.BIT.TEND == 0);
    
    /* Write data */
    SCI9.TDR = (uint8_t)c;
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
