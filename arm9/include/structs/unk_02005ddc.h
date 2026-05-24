#ifndef STRUCTS_UNK_02005DDC_H
    #define STRUCTS_UNK_02005DDC_H

    #include "types.h"

    #include "unk_02019a74.h"

    typedef struct {
        union {
            u32 field00;
            struct {
                u32 bit0 : 1;
                u32 bit1 : 1;
                u32 bit2 : 1;
                u32 src  : 8;
                u32 mid  : 8;
                u32 dst  : 8;
                u32 top  : 5;
            };
        };
        u32 field04;
        UnkStruct_02019a74 field08;
    } UnkStruct_02005ddc_00;
#endif
