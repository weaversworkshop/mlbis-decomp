// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "main/cpu_mem32.h"

#ifndef IDE_PARSE

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

// Duplicate of NitroSDK's MIi_CpuCopy32.
asm void cpu_copy32(register const void *srcp, register void *destp, register u32 size)
{
    add r12, r1, r2
@30:
    cmp r1, r12
    blt @ldmltia1
    b @ldmltia2
@ldmltia1:
    ldmia r0!, {r2}
@ldmltia2:
    blt @stmltia1
    b @stmltia2
@stmltia1:
    stmia r1!, {r2}
@stmltia2:
    blt @30
    bx lr
}

#endif
