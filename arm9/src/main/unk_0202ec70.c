// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ec70.h"
#include "main/cpu_clear32.h"

// MATCH 100%
u32 func_0202ec70(u32 * param1, u32 param2, u32 param3) {
    u32 r3 = param1[0];
    u32 r0 = param3 - 1;
    u32 r2 = ~r0;

    r0 += r3;
    u32 r5 = r2 & r0;

    r0 = param1[1];

    u32 r6 = param2 + r5;

    if (r6 > r0) {
        return 0;
    }

    r0 = param1[-1];
    r2 = r6 - r3;

    if (((u8)r0 & 0x1) != 0) {
        cpu_clear32(0, (void *)r3, r2);
    }

    param1[0] = r6;

    return r5;
}
