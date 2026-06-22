// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02008e48.h"
#include "main/unk_0200990c.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02008e48(UnkStruct_02008e48 * param1) {
    UnkGlobal_01ff9538 * r1 = param1->field18;
    SNDWaveArc * r4 = param1->field04;

    u32 r5 = r1->field00;
    u32 r6 = r1->field04;

    if (r1->field08_01 == 0) {
        MI_CpuCopy8(r4->waveOffset, &r4->waveOffset[r4->waveCount], r5);
        MI_CpuFill8(r4->waveOffset, 0, r5);
    }

    const NNSSndArcSeqArcInfo * arcInfo = NNS_SndArcGetSeqArcInfo(data_01ff9504.field28);
    const NNSSndSeqArc * seqArc = NNS_SndArcGetFileAddress(arcInfo->fileId);
    const NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(seqArc->info[r6].param.bankNo);
    SNDBankData * fileAddr = NNS_SndArcGetFileAddress(bankInfo->fileId);

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        u32 waveArcNo = bankInfo->waveArcNo[i];
        if (waveArcNo != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            const NNSSndArcWaveArcInfo * arcInfo = NNS_SndArcGetWaveArcInfo(waveArcNo);
            u16 fileId = arcInfo->fileId;
            if (arcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                func_0200990c(r4, fileAddr, i, fileId, data_01ff9468.field00);
            }
        }
    }
}
