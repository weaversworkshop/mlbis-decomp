// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02005fe4.h"
#include "main/unk_02007900.h"
#include "main/unk_02009ab8.h"
#include "main/heap.h"
#include "main/unk_0202f1b4.h"

#include <nnsys/snd.h>

extern void func_02009b90(void);
extern void func_0200a600(void);
extern void func_0202eea8(void);
extern void NNS_SndArcStrmInit(u32 param1, NNSSndHeapHandle param2); // DEFERRED : function interleaved with SDK

// MATCHED 100%
void func_02005fe4(BOOL param1, u32 param2, u32 param3, u32 param4) {
    func_0202eea8();
    func_0200a600();

    data_01ff9438 = -1;
    data_01ff9484 = 1;

    if (data_01ff947c.field00 == NULL) {
        data_01ff947c.field00 = func_0201a150(param2, 1, &data_02050088, 0);
    }

    data_01ff944c = param3;

    u32 r4 = 0;

    if (param3 != 0) {
        data_01ff9474.field00 = NNS_SndHeapCreate(data_01ff947c.field00, param3);
        r4 += data_01ff944c;
    }

    data_01ff9450 = param4;

    if (param4 != 0) {
        data_01ff9480.field00 = NNS_SndHeapCreate((u8*)data_01ff947c.field00 + r4, param4);
        r4 += data_01ff9450;
    }

    data_01ff9458 = param2 - r4;

    if (param2 - r4 != 0) {
        data_01ff9468.field00 = NNS_SndHeapCreate((u8*)data_01ff947c.field00 + r4, param2 - r4);
    }

    NNS_SndArcInit(&data_01ff9618, data_02050094, data_01ff9468.field00, param1);

    NNS_SndArcPlayerSetup(data_01ff9468.field00);

    NNS_SndArcStrmInit(10, data_01ff9468.field00);

    NNS_SndArcStrmCreatePrepareThread(&data_01ff9c7c, 17);

    for (s16 i = 1; i >= 0; i--) {
        func_0202f1b4(&data_01ff94a4[i]);
    }

    func_02007900();

    MI_CpuFill8(&data_01ff9504, 0, 0x34);
    func_02009ab8();

    MI_CpuFill8(&data_01ff9598, 0, 0x80);
    MI_CpuFill8(&data_01ff9538, 0, 0x60);
    
    func_02009b90();

    data_01ff9460[0] = -1;
    data_01ff9460[1] = -1;

    data_01ff9444 = 0;

    data_01ff9454[0] = 0;
    data_01ff9454[1] = 0;

    for (s16 i = 1; i >= 0; i--) {
        data_01ff94b4[i].field00 = 0x7f000;
        data_01ff94b4[i].field04 = 0x7f000;
        data_01ff94b4[i].field0c = NNS_SND_SEQ_MUTE_OFF;
        data_01ff94b4[i].field08 = 0;
    }

    data_01ff949c.field00 = 0;
    data_01ff949c.field04 = 0;

    data_01ff94ac[0] = 0x100;
    data_01ff94ac[1] = 0x100;

    NNS_SndHeapSaveState(data_01ff9468.field00);

    data_01ff9440 = 0;

    NNS_SndSetMasterVolume(97);
}
