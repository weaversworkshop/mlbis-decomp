// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02007398.h"
#include "main/unk_020079bc.h"
#include "main/unk_02007b00.h"
#include "main/unk_02007f40.h"
#include "main/unk_0202f1b4.h"

// MATCHED 100%
UnkStruct_01ff9470_00 * func_020079bc(UnkStruct_01ff9470_00 * param1) {
    UnkStruct_01ff9470_00 * r4 = data_01ff986c;
    s32 r2 = 19;

    while (r2 >= 0) {
        if (r4->field2c_00 == 0) {
            break;
        }

        r2--;
        r4++;
    }

    if (r2 < 0) {
        UnkStruct_01ff9470_00 * r12 = data_01ff9470.field00;
        u8 r14 = 0xff;

        r4 = NULL;

        while (r12 != NULL) {
            u16 r1 = 0xffff;
            if (r12->field2e != r1) {
                if (r12->field2e_b + r12->field2f < r14 && r12 != param1) {
                    r4 = r12;
                    r14 = (u8)(r12->field2e_b + r12->field2f);
                }
            }
            r12 = r12->field00;
        }

        if (r4 == NULL) {
            return NULL;
        }

        if ((r4->field18 & 0xc0000000) == 0x40000000) {
            func_02007398((s8)(s16)r4->field2c_11, 2);
        } else {
            func_02007f40(r4, 0);
            NNS_SndHandleReleaseSeq(&r4->field10);
            func_0202f1b4(&r4->field10);
        }

        func_02007b00(r4);
    }

    return r4;
}
