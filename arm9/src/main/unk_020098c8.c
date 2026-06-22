// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_020098c8.h"

// MATCHED 100%
void * func_020098c8(u32 param1, u32 param2) {
    UnkGlobal_01ff9538 * r3 = &data_01ff9538[0];
    s32 r12 = 7;

    while (r12 >= 0) {
        if (r3->field08_00 == 0) 
            break;
        r12--;
        r3++;
    }

    r3->field00 = param1;
    r3->field04 = param2;
    r3->field08_00 = 1;
    return r3;
}
