// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02018538.h"

// MATCHED 100%
u16 * func_02018538(s32 param1) {
    u32 row = param1 & 0xfff;

    switch (param1 & 0xf000) {
        case 0x1000:
            return &data_02052a68[row].field00;
        case 0x2000:
            return &data_020527f8[row].field00;
        case 0x3000:
            return &data_020526f8[row].field00;
        case 0x4000:
            return &data_02053b30[row].field00;
        default:
            return NULL;
    }
}
