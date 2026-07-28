// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/rng.h"

// --- Extern Functions --- //

extern s64 func_0204536c(s32, s32);

// --- Variable Definitions --- //

u32 randState;  // 0x0205bac0

// --- Function Definitions --- //

// MATCHED 100%
void srand(u32 seed) {
    randState = seed;
}

// MATCHED 100%
u32 rand() {
    u32 seed = randState;
    if (seed == 0) {
        seed = DEFAULT_SEED;
    }

    u32 val = seed + ((seed * 5) << 3);
    val = ((val >> 16) | (val << 16)) + val;
    val = (val & 0x1fffe) << 15;
    randState = val;

    return val >> 16;
}

// MATCHED 100%
u16 rand_range(s32 range) {
    return func_0204536c(rand(), range) >> 32;
}
