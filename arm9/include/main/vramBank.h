#ifndef MAIN_VRAM_BANK_H
#define MAIN_VRAM_BANK_H

#include "types.h"

#include <nitro.h>

u8 * getTexVramAddr(GXVRamTex);  // 0x0201f0bc
u8 * getTexPlttVramAddr(GXVRamTexPltt);  // 0x0201f13c
u8 * getObjExtPlttVramAddr(u32);  // 0x0201f1b4

#endif
