// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02019eac.h"

#include <nnsys/snd.h>

// MATCHED 100%
UnkStruct_0201a058 * func_02019eac(s32 param1, u32 param2, void * param3, u32 param4) {
    s32 idx;
    u32 sz;
    s32 offset;
    u32 r6;
    UnkHeader_0201a058 * r14;

    if (param2 != 0) { 
        offset = param1 / 2;
        idx = param1 & 1;
        if (param4 == 1) {
            r14 = data_0205b7f0[offset].field00[1][idx];
        } else {
            r14 = data_0205b7f0[offset].field00[0][idx];
        }

        if (idx != 0) {
            r14 = r14->field00;
        }

        param2 = (param2 + 3) & ~3;

        r6 = (idx == 0);
        do {
            sz = r14->field08_00;
            if (r14->field08_25 != 0 && sz >= param2) {
                offset = param2 + 0xc;
                if (sz > offset) {
                    if (idx == 0) {

                        UnkHeader_0201a058 * r2 = (UnkHeader_0201a058 *)((void *)r14 + 0xc + param2);
                        r2->field08_size = (sz - offset) | 0x2000000;
                        r2->field00 = r14;

                        UnkHeader_0201a058 * f4 = r14->field04;
                        r2->field04 = f4;

                        if (f4 != NULL) {
                            f4->field00 = r2;
                        }

                        r14->field08_size = param2;
                        r14->field04 = r2;

                        if (data_0205b7f0[param1 / 2].field00[1][0] < r2) {
                            data_0205b7f0[param1 / 2].field00[1][0] = r2;
                        }
                    } else {
                        UnkHeader_0201a058 * old = r14;
                        r14 = (UnkHeader_0201a058 *)((void *)r14 + (sz - param2));
                        r14->field08_size = param2;
                        r14->field00 = old;

                        UnkHeader_0201a058 * neighbor = old->field04;
                        r14->field04 = neighbor;

                        if (neighbor != NULL) {
                            neighbor->field00 = r14;
                        }

                        param2 = sz - (param2 + 0xc);

                        old->field08_size = param2 | 0x2000000;
                        old->field04 = r14;

                        if (data_0205b7f0[param1 / 2].field00[1][1] > r14) {
                            data_0205b7f0[param1 / 2].field00[1][1] = r14;
                        }
                    }
                } else {
                    r14->field08_size = sz;
                }

                r14->field08_25 = 0;
                r14->field08_26 = param1;

                return (UnkStruct_0201a058 *)((void *)r14 + 0xc);
            }
            r14 = (r6 != 0) ? r14->field04 : r14->field00;
        } while (r14 != NULL);
    }

    return NULL;
}
