// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02007b00.h"
#include "main/unk_0202f1b4.h"

// MATCHED 100%
UnkStruct_01ff9470_00 * func_02007b00(UnkStruct_01ff9470_00 * param1) {
    UnkStruct_01ff9470_00 * r4 = param1->field00;

    param1->field04->field00 = param1->field00;
    if (param1->field00 != NULL) {
        param1->field00->field04 = param1->field04;
    }

    if (param1->field0c != NULL) {
        param1->field0c->field08 = param1->field08;
        if (param1->field08 != NULL) {
            param1->field08->field0c = param1->field0c;
        }
    }

    if ((param1->field18 & 0x40000000) == 0) {
        NNS_SndHandleReleaseSeq(&param1->field10);
        func_0202f1b4(&param1->field10);
    }

    param1->field00 = NULL;
    param1->field04 = NULL;
    param1->field08 = NULL;
    param1->field0c = NULL;

    if (param1->field14 != NULL) {
        param1->field14->field00 = NULL;
        param1->field14 = NULL;
    }

    param1->field18 = 0;
    param1->field1c = -1;
    param1->field1e = 0;
    param1->field20 = 0;
    param1->field26 = 0x7f;
    param1->field2e = -1;
    param1->field2c = 0;
    param1->field22 = 0;
    param1->field24 = 0;
    param1->field2a = 0;

    param1->field2c_15 = 0;
    param1->field2c_14 = 0;
    param1->field2c_13 = 0;

    return r4;
}
