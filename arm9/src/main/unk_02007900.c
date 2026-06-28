// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02007900.h"
#include "main/unk_0202f1b4.h"

// MATCHED 100%
void func_02007900(void) {
    s32 r9;
    UnkStruct_01ff9470_00 * r10 = data_01ff986c;

    data_01ff9470.field00 = NULL;

    for (r9 = 0; r9 < 20; r9++) {
        r10->field00 = NULL;
        r10->field04 = NULL;
        r10->field08 = NULL;
        r10->field0c = NULL;

        if (r10->field14 != NULL) {
            r10->field14->field00 = NULL;
            r10->field14 = NULL;
        }
        r10->field18 = 0;
        r10->field1c = -1;
        r10->field1e = 0;
        r10->field20 = 0;
        r10->field26 = 0x7f;
        r10->field2e = -1;
        r10->field2c = 0;
        r10->field22 = 0;
        r10->field24 = 0;
        r10->field2a = 0;

        r10->field2c_15 = 0;
        r10->field2c_14 = 0;
        r10->field2c_13 = 0;

        r10->field1d = r9;

        func_0202f1b4(&r10->field10);

        r10++;
    }
}
