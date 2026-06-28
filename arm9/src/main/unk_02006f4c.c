// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006f4c.h"
#include "main/unk_020099b4.h"
#include "main/unk_02009b10.h"
#include "main/unk_02009bcc.h"

#include <nnsys/snd.h>

// MATCH 100%
void func_02006f4c(u16 * param1, u32 param2, u32 param3) {
    u16 * prgNos = param1;
    u16 successes;
    const NNSSndArcWaveArcInfo * arcInfo;
    const NNSSndArcBankInfo * bankInfo;
    const SNDWaveArc * arcAddr;
    void * bankAddr;

    if (data_01ff9504.field20 != 0) {
        UnkGlobal_01ff9464 ctx;
        ctx.field08 = 1;
        ctx.field0c = (void (*)())&func_02006f4c;
        ctx.field10 = param1;
        ctx.field14 = param2;
        ctx.field18 = param3;
        func_02009bcc(&ctx);
        return;
    }

    successes = 0;
    bankInfo = NNS_SndArcGetBankInfo(0);
    arcInfo = NNS_SndArcGetWaveArcInfo(bankInfo->waveArcNo[0]);
    arcAddr = NNS_SndArcGetFileAddress(arcInfo->fileId);
    bankAddr = NNS_SndArcGetFileAddress(bankInfo->fileId);

    for (s16 r9 = *prgNos++; r9 > 0; r9--) {
        if (*prgNos != 0) {
            SNDInstData data;
            SND_ReadInstData(bankAddr, *prgNos, 0, &data);

            successes += func_020099b4(arcAddr, data.param.wave[0], arcInfo->fileId, data_01ff9468.field00, param2);
        }

        prgNos++;
    }

    if (successes != 0 && param2 != 0) {
        data_01ff9488.field00 = param3;
        func_02009b10(0, 0, -1, 0, 1);
    }
}
