// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009bcc.h"

#include <nitro/mi.h>

// MATCHED 100%
void func_02009bcc(UnkGlobal_01ff9464 * param1) {
    UnkGlobal_01ff9464 * r4 = &data_01ff96ac;
    s32 r2 = 0xf;

    while (r2 >= 0) {
        if (r4->field08 == 0xff) {
            MI_CpuCopy8(param1, r4, 0x1c);

            UnkGlobal_01ff9464 * r0 = &data_01ff9464;
            UnkGlobal_01ff9464 * r1 = r0->field00;

            r4->field00 = r1;
            r4->field04 = r0;
            r1->field04 = r4;
            r0->field00 = r4;
            return;
        }

        r4++;
        r2--;
    }
}
