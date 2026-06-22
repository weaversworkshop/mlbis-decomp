#ifndef STRUCTS_UNK_0205BAD4_H
    #define STRUCTS_UNK_0205BAD4_H

    #include "types.h"

    typedef struct UnkStruct_0205bad4_00 {
        u8 pad_00[0xc];
        struct UnkStruct_0205bad4_00 * field0c;
        u8 pad_10[0x5];
        u8 field15;
        u8 field16;
        u8 pad_17[0x2];
    } UnkStruct_0205bad4_00;

    typedef struct UnkGlobal_0205bad4 {
        u8 pad_00[0x8];
        UnkStruct_0205bad4_00 * field08;
    } UnkGlobal_0205bad4;
#endif
