// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

asm void GetCRC16(const void *src, void *dst, u32 ctrl) {
    SWI 0xe
    BX LR
}