// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_020068c4.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_020068c4(u32 param1) {
    NNS_SndPlayerStopSeq(&data_01ff94a4[param1], 0);

    NNSSndHeapHandle heap = (param1 == 0) ? data_01ff9474.field00 : data_01ff9480.field00;
    NNS_SndHeapClear(heap);

    data_01ff9460[param1] = -1;
}
