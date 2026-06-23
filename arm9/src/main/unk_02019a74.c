// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02019a74.h"

// MATCHED 100%
void func_02019a74(UnkGlobal_0205afa8 * param1, u16 param2) {
    u32 r3 = 0;

    param1->field02 = ~param1->field00 & param2;
    param1->field04 = param1->field00 & ~param2;

    u16 r14 = param1->field00;
    for (u32 i = 0; i < 10; i++) {
        u16 r7 = (param2 & data_0204ac4c[i]);
        if (r7 != 0) {
            if ((r14 & r7) == 0) {
                r3 |= r7;
                param1->field0a[i] = param1->field08;
            } else if (--param1->field0a[i] == 0) {
                    r3 |= r7;
                    param1->field0a[i] = param1->field09;
            }
        } else {
            param1->field0a[i] = 0;
        }
    }

    param1->field06 = r3;
    param1->field00 = param2;
}
