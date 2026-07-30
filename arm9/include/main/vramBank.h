#ifndef MAIN_VRAM_BANK_H
#define MAIN_VRAM_BANK_H

#include "types.h"

#include <nitro.h>

u32 getTexVramAddr(GXVRamTex);  // 0x0201f0bc
u32 getTexPlttVramAddr(GXVRamTexPltt);  // 0x0201f13c
u32 func_0201f1b4(BOOL);  // 0x0201f1b4

#endif
