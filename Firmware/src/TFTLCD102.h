/*
 * TFTLCD102.h
 *
 *  Created on: 2021/06/20
 *      Author: tomoya723
 */

#ifndef TFTLCD102_HEADER_FILE
#define TFTLCD102_HEADER_FILE

// --------------------------------------------------------------------
// Prototype declaration
// --------------------------------------------------------------------
void LCD_FadeIN(unsigned int target);  /* target: ディマー収束値 (e.g. 600=暗, 2998=明) */
void LCD_FadeOUT(void);

#endif /* TFTLCD102_HEADER_FILE */
