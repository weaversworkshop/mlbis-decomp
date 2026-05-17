
#include "types.h"

asm void CpuFastSet(const void *src, void *dst, u32 ctrl) {
    SWI 0xc
    BX LR
}