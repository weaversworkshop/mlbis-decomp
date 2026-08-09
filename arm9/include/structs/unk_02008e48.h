#ifndef STRUCTS_UNK_02008E48_H
    #define STRUCTS_UNK_02008E48_H

    #include "types.h"

    #include <nnsys/snd.h>

    #include "structs/sndLoadSlot.h"

    typedef struct {
        u8 pad_00[0x4];
        SNDWaveArc * field04;
        u8 pad_08[0x10];
        SndLoadSlot * field18;
    } UnkStruct_02008e48;
#endif
