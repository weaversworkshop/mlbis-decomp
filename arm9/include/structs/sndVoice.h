#ifndef STRUCTS_UNK_01FF9470_H
    #define STRUCTS_UNK_01FF9470_H

    #include "types.h"

    #include <nnsys/snd.h>

    typedef struct UnkStruct_01ff9470_00 {
        struct UnkStruct_01ff9470_00 * field00;
        struct UnkStruct_01ff9470_00 * field04;
        struct UnkStruct_01ff9470_00 * field08;
        struct UnkStruct_01ff9470_00 * field0c;
        union {
            NNSSndHandle field10;
            NNSSndStrmHandle field10_strm;
        };
        struct UnkStruct_01ff9470_00 * field14;
        u32 field18;
        s8 field1c;
        u8 field1d;
        u16 field1e;
        u16 field20;
        u16 field22;
        u16 field24;
        u16 field26;
        u8 pad_28[0x2];
        u8 field2a;
        u8 pad_2b;
        union {
            u16 field2c;
            struct {
                u16 field2c_00 : 1;
                u16 field2c_01 : 8;
                u16 field2c_09 : 1;
                u16 field2c_10 : 1;
                u16 field2c_11 : 2;
                u16 field2c_13 : 1;
                u16 field2c_14 : 1;
                u16 field2c_15 : 1;
            };
        };
        union {
            u16 field2e;
            struct {
                s8 field2e_b;
                s8 field2f;
            };
        };
        u8 pad_30[0x4];
    } UnkStruct_01ff9470_00;

    typedef struct {
        UnkStruct_01ff9470_00 * field00;
    } UnkGlobal_01ff9470;
#endif
