// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009714.h"
#include "main/unk_020098c8.h"
#include "main/unk_02009ad8.h"
#include "main/unk_0200a054.h"

#include <nnsys/snd.h>

static SNDWaveArc data_02056520;

// MATCHED 100%
void * func_02009714(u32 fileId, NNSSndHeapHandle heap, u32 param3, u32 param4, u32 param5, void (*fn)(UnkStruct_02008e48 *), u32 param7) {
    SNDWaveArc * waveArc;
    u32 len;
    u32 tableSize;
    s32 readSize;

    waveArc = NNS_SndArcGetFileAddress(fileId);
    if (waveArc != NULL && param3 == 0) {
        if (fn != NULL && param5 != 0) {
            UnkGlobal_01ff9538 * ret = func_020098c8(waveArc->waveCount * 4, param7);
            ret->field08_01 = 1;

            UnkStruct_02008e48 ctx;
            ctx.field04 = waveArc;
            ctx.field18 = ret;

            fn(&ctx);
        }

        return waveArc;
    }

    NNS_SndArcReadFile(fileId, &data_02056520, sizeof(data_02056520), 0);
    tableSize = sizeof(u32) * data_02056520.waveCount;
    len = sizeof(data_02056520) + tableSize * 2;

    waveArc = NNS_SndHeapAlloc(heap, len + 0x20, &func_0200a054, &data_01ff9618, fileId);

    if (param5 == 0) {
        NNS_SndArcReadFile(fileId, waveArc, tableSize + sizeof(data_02056520), 0);

        MI_CpuCopy8(waveArc->waveOffset, &waveArc->waveOffset[waveArc->waveCount], tableSize);
        MI_CpuFill8(waveArc->waveOffset, 0, tableSize);
        DC_StoreRange(waveArc, len);
    } else {
        func_02009ad8(waveArc, tableSize + sizeof(data_02056520), fileId, 0, (void (*)(UnkNode *)) fn, func_020098c8(tableSize, param7));
    }

    if (param4 != 0) {
        NNS_SndArcSetFileAddress(fileId, waveArc);
    }

    return waveArc;
}
