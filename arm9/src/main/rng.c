// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/rng.h"

// --- Struct Definitions --- //

// --- Extern Data --- //

extern u32 data_0205bac0;

// --- Extern Functions --- //

extern s64 func_0204536c(s32 param1, s32 param2);

// --- Function Declarations --- //

void srand();  // 0x0201cf1c
u32 rand();  // 0x0201cf2c
u16 rand_range(s32 param1);  // 0x0201cf78

// --- Function Definitions --- //

// MATCHED 100%
void srand(u32 param1) {
    data_0205bac0 = param1;
}

// MATCHED 100%
u32 rand() {
    u32 seed = data_0205bac0;
    if (seed == 0) {
        seed = DEFAULT_SEED;
    }

    u32 val = seed + ((seed * 5) << 3);
    val = ((val >> 16) | (val << 16)) + val;
    val = (val & 0x1fffe) << 15;
    data_0205bac0 = val;

    return val >> 16;
}

// MATCHED 100%
u16 rand_range(s32 param1) {
    return func_0204536c(rand(), param1) >> 32;
}
