// objdiff-target: build/delinks/_dsd_gap@main_7.o
#include "types.h"

#pragma thumb on
asm void LZ77UnCompReadByCallbackWrite16bit(const void *src, void *dst, u32 ctrl) {
    SWI 0x12
    BX LR
}
#pragma thumb reset