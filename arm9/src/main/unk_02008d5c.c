// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02008d5c.h"
#include "main/unk_02008e48.h"
#include "main/unk_02009714.h"
#include "main/unk_02009a9c.h"
#include "main/unk_02009ad8.h"
#include "main/unk_02009ff4.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02008d5c(UnkNode * param1) {
    UnkStruct_01ff9598 * r5 = param1->field18;
    const NNSSndArcBankInfo * r6 = r5->field00;

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        u16 waveArcNo = r6->waveArcNo[i];

        if (waveArcNo != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            const NNSSndArcWaveArcInfo * waveArcInfo = NNS_SndArcGetWaveArcInfo(waveArcNo);
            u16 fileId = waveArcInfo->fileId;

            if (waveArcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                func_02009714(fileId, data_01ff9468.field00, 0, 1, 1, &func_02008e48, r5->field08);
            } else {
                u32 sz = NNS_SndArcGetFileSize(fileId);
                void * ret = func_02009a9c(data_01ff9468.field00, sz, &func_02009ff4, &data_01ff9618, fileId);
                func_02009ad8(ret, sz, fileId, 0, NULL, NULL);
                NNS_SndArcSetFileAddress(fileId, ret);
            }
        }
    }
}
