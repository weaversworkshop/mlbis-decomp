// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02007344.h"

#include <nnsys/snd.h>

// MATCHED 100%
UnkStruct_01ff9470_00 * func_02007344(u32 param1) {
    UnkStruct_01ff9470_00 * r3 = data_01ff9470.field00;
    while (r3 != NULL) {
        if ((r3->field18 & 0xc0000000) == 0x40000000) {
            if (param1 == -1 || param1 == r3->field2c_11) {
                return r3;
            }
        }

        r3 = r3->field00;
    }

    return NULL;
}
