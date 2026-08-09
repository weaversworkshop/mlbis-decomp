#ifndef MAIN_BTL_BOOT_H
#define MAIN_BTL_BOOT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

void btlEnter(u32, u32);  // 0x02019628
void btlSetup(void);  // 0x02019684
void btlExit(void);  // 0x020198cc

#ifdef __cplusplus
} // extern "C"
#endif

#endif
