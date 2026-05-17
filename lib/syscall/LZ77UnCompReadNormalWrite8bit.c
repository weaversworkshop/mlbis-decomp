// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

asm void LZ77UnCompReadNormalWrite8bit(const void *src, void *dst, u32 ctrl) {
    SWI 0x11
    BX LR
}