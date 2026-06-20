// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_020099b4.h"
#include "main/unk_02009a9c.h"

#include <nnsys/snd.h>

// ???
void func_02009ad8(void * param1, u32 param2, u32 param3, u32 param4, u32 param5, u32 param6);
// SingleWaveDisposeCallback
void func_0200a074(void * mem, u32 size, u32 data1, u32 data2);

// MATCHED 100%  ?? LoadSingleWave modified
bool func_020099b4(const SNDWaveArc * waveArc, s32 waveNo, u32 fileId, NNSSndHeapHandle heap, u32 param5) {
    if (SND_GetWaveDataAddress(waveArc, waveNo) != NULL) {
        return false;
    }

    u32 waveCount = SND_GetWaveDataCount(waveArc);
    u32 begin = waveArc->waveOffset[waveArc->waveCount + waveNo];
    u32 end = (waveNo < --waveCount) ? waveArc->waveOffset[waveArc->waveCount + waveNo + 1] : waveArc->fileHeader.fileSize;

    void * ret = func_02009a9c(heap, end-begin, &func_0200a074, waveArc, waveNo);

    u32 offset = waveArc->waveOffset[waveArc->waveCount + waveNo];

    if (param5 != 0) {
        func_02009ad8(ret, end-begin, fileId, offset, 0, 0);
    } else {
        NNS_SndArcReadFile(fileId, ret, end-begin, offset);
        DC_StoreRange(ret, end-begin);
    }

    SND_SetWaveDataAddress(waveArc, waveNo, ret);
    return true;
}
