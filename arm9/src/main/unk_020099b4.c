// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_020099b4.h"
#include "main/unk_02009ad8.h"
#include "main/unk_02009a9c.h"
#include "main/unk_0200a074.h"

#include <nnsys/snd.h>

// MATCHED 100%  -- LoadSingleWave modified
BOOL func_020099b4(const SNDWaveArc * waveArc, s32 waveNo, u32 fileId, NNSSndHeapHandle heap, u32 param5) {
    if (SND_GetWaveDataAddress(waveArc, waveNo) != NULL) {
        return FALSE;
    }

    u32 waveCount = SND_GetWaveDataCount(waveArc);
    u32 begin = waveArc->waveOffset[waveArc->waveCount + waveNo];
    u32 end = (waveNo < --waveCount) ? waveArc->waveOffset[waveArc->waveCount + waveNo + 1] : waveArc->fileHeader.fileSize;

    void * ret = func_02009a9c(heap, end-begin, &func_0200a074, (u32)waveArc, waveNo);

    u32 offset = waveArc->waveOffset[waveArc->waveCount + waveNo];

    if (param5 != 0) {
        func_02009ad8(ret, end-begin, fileId, offset, NULL, 0);
    } else {
        NNS_SndArcReadFile(fileId, ret, end-begin, offset);
        DC_StoreRange(ret, end-begin);
    }

    SND_SetWaveDataAddress((SNDWaveArc *)waveArc, waveNo, ret);
    return TRUE;
}
