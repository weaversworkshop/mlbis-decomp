#ifndef STRUCTS_UNK_02005DDC_H
    #define STRUCTS_UNK_02005DDC_H

    #include "types.h"

    #include "unk_0205afa8.h"

    typedef struct {
        union {
            u32 field00;
            struct {
                u32 field00_00 : 1;
                u32 field00_01 : 1;
                u32 field00_02 : 1;
                u32 src  : 8;
                u32 mid  : 8;
                u32 dst  : 8;
                u32 top  : 5;
            };
        };
        u32 field04;
        UnkGlobal_0205afa8 field08;
    } UnkStruct_02005ddc;
#endif
