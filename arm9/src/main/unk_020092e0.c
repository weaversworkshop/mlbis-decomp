// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200907c.h"
#include "main/unk_020092e0.h"

#include <nnsys/snd.h>

extern s32 func_020348fc(s32 param1, u32 param2);

// MATCH 100%
void func_020092e0(u32 param1, u8 param2, u32 param3) {
    UnkStruct_01ff94d4 * r6 = &data_01ff94d4[param2];
    const NNSSndArcSeqInfo * r4 = NNS_SndArcGetSeqInfo(param1);
    const NNSSndArcBankInfo * r7 = NNS_SndArcGetBankInfo(r4->param.bankNo);

    NNS_SndArcSetFileAddress(r4->fileId, r6->field00);
    NNS_SndArcSetFileAddress(r7->fileId, r6->field04);

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        if (r6->field08[i] != 0) {
            const NNSSndArcWaveArcInfo * ret = NNS_SndArcGetWaveArcInfo(r7->waveArcNo[i]);
            NNS_SndArcSetFileAddress(ret->fileId, r6->field08[i]);
        }
    }

    NNS_SndPlayerStopSeq(&data_01ff94a4[param2], 0);

    UnkStruct_01ff94b4 * struct_01ff94b4 = &data_01ff94b4[param2];
    struct_01ff94b4->field04 = struct_01ff94b4->field00;
    struct_01ff94b4->field0c = 0;

    NNS_SndPlayerSetTrackMuteEx(&data_01ff94a4[param2], 0xffff, 0);

    if (param3 != 0) {
        data_01ff94b8[param2].field00 = 0;
    }

    NNS_SndArcPlayerStartSeqEx(
        &data_01ff94a4[param2], 
        (param2 != 0) ? true : false, 
        -1, -1, param1
    );

    NNS_SndPlayerSetTrackVolume(
        &data_01ff94a4[param2], 0xffff,
        0xfff & (u32)(data_01ff94b8[param2].field00 >> 0xc)
    );

    if (data_01ff94ac[param2] != 0x100) {
        NNS_SndPlayerSetTempoRatio(&data_01ff94a4[param2], data_01ff94ac[param2]);
        data_01ff94ac[param2] = 0x100;
    }

    if (param3 != 0) {
        data_01ff94c0[param2].field00 = 2;
        data_01ff94b8[param2].field00 = data_01ff94b4[param2].field00;
        data_01ff94bc[param2].field00 = func_020348fc(data_01ff94b4[param2].field00, 0x1000);
        func_0200907c();
    }

    data_01ff949c.field00 = param1;
    data_01ff949c.field04 = param2;
}
