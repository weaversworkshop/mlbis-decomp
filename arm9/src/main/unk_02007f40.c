// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02007f40.h"

// MATCHED 100%
void func_02007f40(UnkStruct_01ff9470_00 * param1, u32 param2) {
    NNS_SndPlayerStopSeq(&param1->field10, param2);
    param1->field2c_09 = 1;
}
