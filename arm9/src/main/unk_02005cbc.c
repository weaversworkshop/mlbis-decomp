// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02005ca4.h"
#include "main/unk_02005cbc.h"
#include "main/unk_tu_02015ebc.h"

// MATCHED 100%
u32 * func_02005cbc(s32 param1) {
    u16 * ret;
    switch (param1 & 0xf000) {
        case 0x1000:
            ret = func_02018538(param1);
            return func_02005ca4(0, *ret);
        case 0x2000:
            ret = func_02018538(param1);
            return func_02005ca4(1, *ret);
        case 0x3000:
            ret = func_02018538(param1);
            return func_02005ca4(2, *ret);
        case 0x4000:
            ret = func_02018538(param1);
            return func_02005ca4(3, *ret);
        default:
            return NULL;
    }
}
