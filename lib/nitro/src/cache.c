// objdiff-target: build/delinks/_dsd_gap@main_7.o
// Data cache maintenance routines.
// Cache line size on ARM946E-S is 32 bytes.
#include "nitro/cache.h"

#define HW_CACHE_LINE_SIZE 0x20

// Invalidate D-cache lines in [startAddr, startAddr + nBytes) — drop cached
// data without writing back. Use when the CPU's cached view is stale and
// authoritative data is in main RAM (e.g. after DMA-in).
asm void DC_InvalidateRange(register void *startAddr, register u32 nBytes) {
    add r1, r1, r0
    bic r0, r0, #HW_CACHE_LINE_SIZE - 1
@1:
    mcr p15, 0, r0, c7, c6, 1
    add r0, r0, #HW_CACHE_LINE_SIZE
    cmp r0, r1
    blt @1
    bx lr
}

// Clean D-cache lines in [startAddr, startAddr + nBytes) — write any dirty
// data back to main RAM, keep cached copies. Use before handing a buffer to
// another bus master (DMA, GPU, ARM7) that reads from RAM, not the ARM9 cache.
asm void DC_StoreRange(register const void *startAddr, register u32 nBytes) {
    add r1, r1, r0
    bic r0, r0, #HW_CACHE_LINE_SIZE - 1
@1:
    mcr p15, 0, r0, c7, c10, 1
    add r0, r0, #HW_CACHE_LINE_SIZE
    cmp r0, r1
    blt @1
    bx lr
}

// Clean + invalidate D-cache lines in [startAddr, startAddr + nBytes) — write
// back dirty data and drop the cached copies. Use when handing off a buffer
// AND the CPU won't reuse it (or wants to re-read fresh).
asm void DC_FlushRange(register const void *startAddr, register u32 nBytes) {
    mov r12, #0
    add r1, r1, r0
    bic r0, r0, #HW_CACHE_LINE_SIZE - 1
@1:
    mcr p15, 0, r12, c7, c10, 4
    mcr p15, 0, r0, c7, c14, 1
    add r0, r0, #HW_CACHE_LINE_SIZE
    cmp r0, r1
    blt @1
    bx lr
}
