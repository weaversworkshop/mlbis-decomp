// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

#pragma thumb on
asm void VBlankIntrWait(const void *src, void *dst, u32 ctrl) {
    MOV r2, #0x0
    SWI 0x5
    BX LR
}
#pragma thumb reset