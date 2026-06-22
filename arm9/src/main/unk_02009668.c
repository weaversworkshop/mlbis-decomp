// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009668.h"
#include "main/unk_0200990c.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02009668(u32 seqNo, u8 param2) {
    UnkStruct_01ff94d4 * r7 = &data_01ff94d4[param2];
    const NNSSndArcSeqInfo * ret = NNS_SndArcGetSeqInfo(seqNo);
    const NNSSndArcBankInfo * ret2 = NNS_SndArcGetBankInfo(ret->param.bankNo);

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        u16 waveArcNo = ret2->waveArcNo[i];
        if (waveArcNo != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            const NNSSndArcWaveArcInfo * ret = NNS_SndArcGetWaveArcInfo(waveArcNo);
            
            u16 r3 = (u16)ret->fileId;
            if (ret->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                void * r0 = r7->field08[i];
                func_0200990c(r0, r7->field04, i, r3, (param2 == 0) ? data_01ff9474.field00 : data_01ff9480.field00);
            }
        }
    }
}
