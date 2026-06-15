// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "main/cpu_clear32.h"

// Duplicate of NitroSDK's MIi_CpuClear32.
asm void cpu_clear32(register u32 data, register void *destp, register u32 size)
{
    add r12, r1, r2
@20:
    cmp r1, r12
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r0}
@stmltia2:
    blt @20
    bx lr
}
