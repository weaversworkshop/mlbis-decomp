// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009880.h"

#include <nitro/mi.h>

// MATCHED 100%
void func_02009880(UnkStruct_02008e48 * param1) {
    UnkGlobal_01ff9538 * r1 = param1->field18;
    SNDWaveArc * r4 = param1->field04;
    u32 r5 = r1->field00;

    if (r1->field08_01 == 0) {
        MI_CpuCopy8(r4->waveOffset, &r4->waveOffset[r4->waveCount], r5);
        MI_CpuFill8(r4->waveOffset, 0, r5);
    }
}
