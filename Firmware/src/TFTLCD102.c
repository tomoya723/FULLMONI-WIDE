/*
 * TFTLCD102.c
 *
 *  Created on: 2021/06/20
 *      Author: tomoya723
 */
#include "r_smc_entry.h"
#include "TFTLCD102.h"

// --------------------------------------------------------------------
// TFT LCD FadeIN
// --------------------------------------------------------------------
void LCD_FadeIN(void)
{
	static int y, z;

	for(z = 0; z <= 2999; z ++ )			// duty 0→100%
	{
		for(y = 0; y <= 500; y ++ )
		{
			MTU3.TGRD = z & 0xFFFF;
		}
	}
}

// --------------------------------------------------------------------
// TFT LCD FadeOUT
// --------------------------------------------------------------------
void LCD_FadeOUT(void)
{
	static int y, z;

	for(z = 2999; z >= 0; z -- )			// duty 0→100%
	{
		for(y = 0; y <= 2500; y ++ )
		{
			MTU3.TGRD = z & 0xFFFF;
		}
	}
}
