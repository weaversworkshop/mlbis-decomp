// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/cpu_mem16.h"

#include "itcm/cpu_mem.h"

// --- Function Definitions --- //

// MATCHED 100%
void cpu_fill16(u16 * src, u16 value, u32 size) {
    if (size != 0) {
        if (((u32)src & 2) != 0) {
            size -= 2;
            *src++ = value;
        }

        if ((size & 2) != 0) {
            size -= 2;
            src[(size & ~1) / 2] = value;
        }

        cpu_fill32(src, value | (value << 16), size & ~2);
    }
}

// MATCHED 100%
void cpu_copy16(u16 * src, u16 * dst, u32 size) {
    if (src != dst && size != 0) {
        u16 * newSrc = src;

        if ((((u32)src ^ (u32)dst) & 2) != 0) {
            if (dst < src) {
                cpu_copy16_fwd(src, dst, size);
            } else {
                cpu_copy16_rev(src, dst, size);
            }
        } else if (dst < src) {
            if (((u32)dst & 2) != 0) {
                newSrc = src+1;
                size -= 2;
                *dst++ = src[0];
            }

            cpu_copy32_fwd(newSrc, dst, size & ~2);

            if((size & 2) != 0) {
                dst[size/2-1] = newSrc[size/2-1];
            }
        } else {
            if (((u32)((u8*)dst + size) & 2) != 0) {
                src += size / 2;
                dst[size/2-1] = src[-1];
                size -= 2;
            }

            if (((u32)dst & 2) != 0) {
                cpu_copy32_rev(&newSrc[1], &dst[1], size-2);
                dst[0] = newSrc[0];
            } else {
                cpu_copy32_rev(newSrc, dst, size);
            }
        }
    }
}
