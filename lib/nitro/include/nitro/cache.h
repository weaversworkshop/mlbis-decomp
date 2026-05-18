// NitroSDK cache routines (CP15 cache maintenance).
//
// Hand-authored stubs for SDK functions identified in the game's address space.
// As MLBIS decomp progresses, more identified SDK functions will land here.
// Long-term goal: replace with real NitroSDK link once the toolchain
// (MSL_C / mw-libraries) can build it.
#ifndef NITRO_CACHE_H
#define NITRO_CACHE_H

#include <types.h>

void DC_InvalidateRange(void *startAddr, u32 nBytes);
void DC_StoreRange(const void *startAddr, u32 nBytes);
void DC_FlushRange(const void *startAddr, u32 nBytes);

#endif
