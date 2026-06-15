// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202eccc.h"

// MATCH 100%
void * func_0202eccc(u32 * param1, u32 param2, s32 param3) {
    u32 r5 = ~(param3 - 1) & (param1[1] - param2);

    if (r5 < param1[0]) {
        return 0;
    }

    u32 r0 = param1[-1];
    u32 r2 = param1[1] - r5;

    if (((u8)r0 & 0x1) != 0) {
        cpu_clear32(0, (void *)r5, r2);
    }

    param1[1] = r5;
    return r5;
}
