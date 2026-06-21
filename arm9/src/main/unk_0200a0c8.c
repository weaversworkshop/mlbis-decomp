// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200a0c8.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_0200a0c8(void * mem, u32 data1, u32 data2) {
    NNSSndArc * arc;
    OSIntrMode mode;
    
    mode = OS_DisableInterrupts();
    arc = NNS_SndArcSetCurrent(data1);

    if (mem == NNS_SndArcGetFileAddress(data2)) {
        NNS_SndArcSetFileAddress(data2, NULL);
    }

    NNS_SndArcSetCurrent(arc);
    OS_RestoreInterrupts(mode);
}
