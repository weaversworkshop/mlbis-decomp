// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006c34.h"
#include "main/unk_02007900.h"
#include "main/unk_02007f40.h"
#include "main/unk_0202f1b4.h"

// MATCHED 100%
void func_02006c34() {
    UnkStruct_01ff9470_00 * ptr = data_01ff9470.field00;

    while (ptr != 0) {
        if ((ptr->field18 & 0xc0000000) != 0x40000000) {
            func_02007f40(ptr, 0);
            NNS_SndHandleReleaseSeq(&ptr->field10);
            func_0202f1b4(&ptr->field10);
        }
        ptr = ptr->field00;
    }

    func_02007900();
}
