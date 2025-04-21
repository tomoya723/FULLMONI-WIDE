/*
 * settings.h
 *
 *  Created on: 2025/04/20
 *      Author: tomoy
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

// emwin display resource number select
#define DISP (2)  // 1-2

#if (DISP == 1)
	#include "../aw001/Source/Generated/Resource.h"
	#include "../aw001/Source/Generated/ID_SCREEN_Telltale.h"
	#include "../aw001/Source/Generated/ID_SCREEN_01.h"
#elif (DISP == 2)
	#include "../aw002/Source/Generated/Resource.h"
	#include "../aw002/Source/Generated/ID_SCREEN_Telltale.h"
	#include "../aw002/Source/Generated/ID_SCREEN_01a.h"
	#include "../aw002/Source/Generated/ID_SCREEN_01b.h"
	#include "../aw002/Source/Generated/ID_SCREEN_01c.h"
#endif

extern void sch_10ms();

#endif /* SETTINGS_H_ */
