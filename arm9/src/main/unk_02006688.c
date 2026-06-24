// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006688.h"

#include <nnsys/snd.h>

// MATCH 100%
void func_02006688(u32 param1, u8 param2) {
    if (param1 == 0) {
        param1 = 8;
    }

    NNS_SndPlayerStopSeq(&data_01ff94a4[param2], param1);

    UnkStruct_01ff94b4 * r3 = &data_01ff94b4[param2];
    r3->field04 = r3->field00; 
    r3->field0c = NNS_SND_SEQ_MUTE_OFF;

    NNS_SndPlayerSetTrackMuteEx(&data_01ff94a4[param2], 0xffff, NNS_SND_SEQ_MUTE_OFF);
}
