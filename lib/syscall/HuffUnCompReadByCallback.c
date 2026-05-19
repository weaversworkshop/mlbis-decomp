// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

#pragma thumb on
asm void HuffUnCompReadByCallback(const void *src, void *dst, u32 ctrl) {
    SWI 0x13
    BX LR
}
#pragma thumb reset