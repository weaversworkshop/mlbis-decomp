// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009490.h"
#include "main/unk_02009714.h"
#include "main/unk_02009880.h"
#include "main/unk_02009a9c.h"
#include "main/unk_02009ad8.h"
#include "main/unk_02009fcc.h"
#include "main/unk_02009ff4.h"
#include "main/unk_0200a024.h"

#include <nnsys/snd.h>

// MATCHED 100%
bool func_02009490(NNSSndHeapHandle param1, u32 param2, UnkStruct_01ff94d4 * param3) {
    const NNSSndArcSeqInfo * seqInfo;
    const NNSSndArcBankInfo * bankInfo;
    const NNSSndArcWaveArcInfo * arcInfo;
    u16 fileId;
    u32 sz;
    s32 i;
    bool r11 = false;

    param3->field00 = 0;
    param3->field04 = 0;

    for (i = 0; i < 4; i++) {
        param3->field08[i] = 0;
    }

    seqInfo = NNS_SndArcGetSeqInfo(param2);
    fileId = (u16)seqInfo->fileId;
    sz = NNS_SndArcGetFileSize(fileId);
    void * ret = func_02009a9c(param1, sz, &func_02009fcc, (u32)&data_01ff9618, fileId);
    func_02009ad8(ret, sz, fileId, 0, NULL, NULL);
    param3->field00 = ret;

    bankInfo = NNS_SndArcGetBankInfo(seqInfo->param.bankNo);
    fileId = (u16)bankInfo->fileId;
    sz = NNS_SndArcGetFileSize(fileId);
    ret = func_02009a9c(param1, sz, &func_0200a024, (u32)&data_01ff9618, fileId);
    func_02009ad8(ret, sz, fileId, 0, NULL, NULL);
    param3->field04 = ret;

    for (i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        u16 waveNo = bankInfo->waveArcNo[i];
        if (waveNo != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            arcInfo = NNS_SndArcGetWaveArcInfo(waveNo);
            u16 fileId = (u16)arcInfo->fileId;

            if (arcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                param3->field08[i] = func_02009714(fileId, param1, 1, 0, 1, &func_02009880, 0);
                r11 = true;
            } else {
                u32 sz = NNS_SndArcGetFileSize(fileId);
                void * ret = func_02009a9c(param1, sz, &func_02009ff4, (u32)&data_01ff9618, fileId);
                func_02009ad8(ret, sz, fileId, 0, 0, 0);
                param3->field08[i] = ret;
            }
        }
    }

    return r11;
}
