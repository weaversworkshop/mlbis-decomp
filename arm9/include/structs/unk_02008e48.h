#ifndef STRUCTS_UNK_02008E48_H
    #define STRUCTS_UNK_02008E48_H

    #include "types.h"

    #include "structs/unk_01ff9538.h"

    #include <nnsys/snd.h>

    typedef struct UnkStruct_02008e48 {
        u8 pad_00[0x4];
        SNDWaveArc * field04;
        u8 pad_08[0x10];
        UnkGlobal_01ff9538 * field18;
    } UnkStruct_02008e48;
#endif