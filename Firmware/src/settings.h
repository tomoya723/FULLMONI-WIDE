/*
 * settings.h
 *
 *  Created on: 2025/04/20
 *      Author: tomoy
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

// emwin display resource number select
#define DISP (1)  // 1-2

#if (DISP == 1)
	#include "../aw001/Source/Generated/Resource.h"
	#include "../aw001/Source/Generated/ID_SCREEN_Telltale.h"
	#include "../aw001/Source/Generated/ID_SCREEN_01.h"
#elif (DISP == 2)
	#include "../aw002/Source/Generated/Resource.h"
	#include "../aw002/Source/Generated/ID_SCREEN_Telltale.h"
	#include "../aw002/Source/Generated/ID_SCREEN_01.h"
	#include "../aw002/Source/Generated/ID_SCREEN_02a.h"
	#include "../aw002/Source/Generated/ID_SCREEN_02b.h"
	#include "../aw002/Source/Generated/ID_SCREEN_02c.h"
#endif

extern void sch_10ms();

#endif /* SETTINGS_H_ */
