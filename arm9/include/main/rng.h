#ifndef MAIN_RNG_H
#define MAIN_RNG_H

#include "types.h"

#define DEFAULT_SEED 0x2a6d365a

void srand();  // 0x0201cf1c
u32 rand();  // 0x0201cf2c
u16 rand_range(s32);  // 0x0201cf78

#endif
