// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02008f2c.h"

#include <nnsys/snd.h>

// MATCHED 100%
extern void * func_02008f2c(const NNSSndArcBankInfo * param1, void * param2, u32 param3) {
    UnkStruct_01ff9598 * r12 = &data_01ff9598[0];
    s32 r14 = 7;

    while (r14 >= 0) {
        if (r12->field0c == 0)
            break;
        r14--;
        r12++;
    }

    r12->field00 = param1;
    r12->field04 = param2;
    r12->field08 = param3;
    r12->field0c = 1;
    return r12;
}
