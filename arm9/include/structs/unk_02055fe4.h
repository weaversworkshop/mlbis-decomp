#ifndef STRUCTS_UNK_02055FE4_H
    #define STRUCTS_UNK_02055FE4_H

    #include "types.h"

    typedef struct {
        u32 field000[16];
        u32 field040[5];
        u32 field054[2];
        u32 field05c[34];
        u32 field0e4[128];
        u8 field2e4[0x98];
        u8 field37c[2][7];
        u8 pad_38a[0x3b8 - 0x38a];
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
        u8 pad_43b[0x4c3 - 0x43b];
        u8 field4c3;
        u8 field4c4;
        u8 pad_4c5[0x7];
        u8 : 2;
        u8 field4cc_2 : 1;
        u8 field4cc_3 : 2; // current save slot (0/1) ?? 
        u8 : 3;
        u8 field4cd;
        u8 field4ce;
        u8 pad_4cf[0x1];
        u32 field4d0;
        u32 field4d4;
        u32 * field4d8[4];
        u16 field4e8[10];
        u16 field4fc;
        u16 field4fe;
        u16 field500;
        u16 field502;
        union {
            struct {
                u32 field504_00 : 2;
                u32 field504_02 : 11;
                u32 field504_13 : 2;
            };
            struct {
                u32 field504_0b : 1;
                u32 field504_1b : 1;
            };
        };
        s16 field508[6];
    } GameState;
#endif
