// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009e2c.h"
#include "main/unk_02030bac.h"

#pragma thumb off

// MATCH 100%
void func_02009e2c(UnkNode* param1) {
    UnkNode* r0 = param1;

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

    func_02030bac(r0);
}

#pragma thumb reset
