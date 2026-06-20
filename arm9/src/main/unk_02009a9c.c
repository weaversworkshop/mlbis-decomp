// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009a9c.h"

#include <nnsys/snd.h>

// MATCHED 100%
void * func_02009a9c(NNSSndHeapHandle param1, u32 param2, NNSSndHeapDisposeCallback callback, u32 param4, u32 param5) {
    if (param2 < 0x1c) {
        param2 = 0x1c;
    }

    NNS_SndHeapAlloc(param1, param2, callback, param4, param5);
}
