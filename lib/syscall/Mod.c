// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

#pragma thumb on
asm void Mod(const void *src, void *dst, u32 ctrl) {
    SWI 0x9
    MOV r0, r1
    BX LR
}
#pragma thumb reset