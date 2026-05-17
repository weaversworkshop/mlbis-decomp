// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

asm void IsDebugger(const void *src, void *dst, u32 ctrl) {
    SWI 0xf
    BX LR
}