// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009c24.h"

// MATCHED 100%
void func_02009c24() {
    UnkGlobal_01ff9464 * r1 = &data_01ff9464;
    UnkGlobal_01ff9464 * r4 = data_01ff9494.field04;

    if (r4 == r1) {
        return;
    }

    switch (r4->field08) {
        case 0:
            ((void (*)(u32, u8, u32))r4->field0c)(
                r4->field10,
                r4->field14_b,
                r4->field18
            );
            break;
        case 1:
            ((void (*)(u32, u32, u32))r4->field0c)(
                r4->field10,
                r4->field14,
                r4->field18
            );
            break;
        case 2:
            ((void (*)(u32, u32))r4->field0c)(
                r4->field10,
                r4->field14
            );
            break;
        default:
            break;
    }

    r4->field00->field04 = r4->field04;
    r4->field04->field00 = r4->field00;
    r4->field08 = -1;
}