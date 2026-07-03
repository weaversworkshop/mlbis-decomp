#ifndef STRUCTS_UNK_02055FE4_H
    #define STRUCTS_UNK_02055FE4_H

    #include "types.h"

    typedef struct {
        u8 pad_00[0x4c3];
        u8 field4c3;
        u8 pad_4c4[0x8];
        u8 field4cc_0 : 3;
        u8 field4cc_3_4 : 2; // current save slot (0/1) ?? 
        u8 field4cc_5 : 3;
        u8 pad_4cd[0x4e8 - 0x4cd];
        u16 field4e8[10];
        u8 pad_4fc[0x504 - 0x4fc];
        u32 field504_00 : 13;
        u32 field504_13 : 2;
    } UnkGlobal_02055fe4;
#endif
