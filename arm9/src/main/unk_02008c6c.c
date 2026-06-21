// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02008c6c.h"
#include "main/unk_02009a9c.h"
#include "main/unk_02009ad8.h"
#include "main/unk_0200a024.h"

#include <nnsys/snd.h>

extern void func_02008d5c(UnkNode * param1);
extern const NNSSndArcBankInfo * func_02008f2c(const NNSSndArcBankInfo * param1, void * param2, u32 param3);

// MATCHED 100%
void func_02008c6c(UnkNode * param1) {
    NNSSndSeqArc * r7 = param1->field18;

    for (u32 r8 = 0; r8 < r7->count; r8++) {
        NNSSndSeqArcSeqInfo * r9 = &r7->info[r8];

        if (r9->offset != -1) {
            NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(r9->param.bankNo);
            u16 fileId = bankInfo->fileId;

            if (NNS_SndArcGetFileAddress(fileId) == NULL) {
                u32 sz = NNS_SndArcGetFileSize(fileId);
                void * ret = func_02009a9c(data_01ff9468.field00, sz, &func_0200a024, (u32)&data_01ff9618, fileId);

                bankInfo = func_02008f2c(bankInfo, ret, r8);
                func_02009ad8(ret, sz, fileId, 0, &func_02008d5c, bankInfo);
                NNS_SndArcSetFileAddress(fileId, ret);
            }
        }

        r9++;
    }
}
