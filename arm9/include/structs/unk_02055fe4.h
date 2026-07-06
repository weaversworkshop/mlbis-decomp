#ifndef STRUCTS_UNK_02055FE4_H
    #define STRUCTS_UNK_02055FE4_H

    #include "types.h"

    typedef struct {
        u8 pad_00[0x54];
        u32 field54[2];
        u8 pad_5c[0x35c];
        u8 field3b8;
        u8 pad_3b9[0x2f];
        u8 field3e8;
        u8 pad_3e9[0x33];
        u32 field41c;
        u8 pad_420[0x2];
        u8 field422;
        u8 field423;
        u8 pad_424[0x9];
        u8 field42d;
        u8 pad_42e[0x4];
        u8 field432;
        u8 pad_433[0x7];
        u8 field43a;
        u8 pad_43b[0x4c3-0x43b];
        u8 field4c3;
        u8 field4c4;
        u8 pad_4c5[0x7];
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
