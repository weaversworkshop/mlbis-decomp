// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009e2c.h"

#include <nnsys/snd.h>

// MATCH 100%
void func_02009e2c(UnkNode* param1) {
    NNSSndHeapHandle r0 = (NNSSndHeapHandle)param1;

    if (data_01ff9504.field22 != 0) {
        if (data_01ff9504.field22 == 1) {
            r0 = data_01ff9468.field00;
        }
    } else {
        UnkGlobal_01ff9468 * src;
        if (data_01ff9504.field21 == 0) {
            src = &data_01ff9474;
        } else {
            src = &data_01ff9480;
        }
        r0 = src->field00;
    }

    NNS_SndHeapSaveState(r0);
}
