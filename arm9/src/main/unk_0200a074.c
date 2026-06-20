// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200a074.h"

#include <nnsys/snd.h>

// MATCHED 100%  --  modified SingleWaveDisposeCallback
void func_0200a074(void * mem, u32 size, u32 data1, u32 data2) {
    SNDWaveArc * waveArc = (SNDWaveArc *)data1;

    if (waveArc->fileHeader.headerSize != 0) {
        if (mem == SND_GetWaveDataAddress(waveArc, data2)) {
            SND_SetWaveDataAddress(waveArc, data2, NULL);
        }
    }

    SND_InvalidateWaveData(mem, mem + size);
}
