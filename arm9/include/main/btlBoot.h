#ifndef MAIN_BTL_BOOT_H
#define MAIN_BTL_BOOT_H

#include "types.h"

void btlEnter(u32, u32);  // 0x02019628
void btlSetup(void);  // 0x02019684
void btlExit(void);  // 0x020198cc

#endif
