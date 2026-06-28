// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02008c6c.h"
#include "main/unk_02008d5c.h"
#include "main/unk_02008f2c.h"
#include "main/unk_02009a9c.h"
#include "main/unk_02009ad8.h"
#include "main/unk_0200a024.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02008c6c(UnkNode * param1) {
    NNSSndSeqArc * r7 = param1->field18;

    for (u32 i = 0; i < r7->count; i++) {
        NNSSndSeqArcSeqInfo * r9 = &r7->info[i];

        if (r9->offset != NNS_SND_SEQ_ARC_INVALID_OFFSET) {
            NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(r9->param.bankNo);
            u16 fileId = bankInfo->fileId;

            if (NNS_SndArcGetFileAddress(fileId) == NULL) {
                u32 sz = NNS_SndArcGetFileSize(fileId);
                void * ret = func_02009a9c(data_01ff9468.field00, sz, &func_0200a024, &data_01ff9618, fileId);

                void * ret2 = func_02008f2c(bankInfo, ret, i);
                func_02009ad8(ret, sz, fileId, 0, &func_02008d5c, ret2);
                NNS_SndArcSetFileAddress(fileId, ret);
            }
        }

        r9++;
    }
}
