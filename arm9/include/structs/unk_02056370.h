#ifndef STRUCTS_UNK_02056370_H
    #define STRUCTS_UNK_02056370_H

    #include "types.h"

    typedef struct {
        u16 field00;
        u16 field02[14];
        u8 pad_1e[0x2];
        union {
            u32 field20;
            struct {
                u32 field20_00 : 8;
                u32 field20_08 : 24;
            };
        };
        union {
            u32 field24;
            struct {
                u32 field24_00 : 24;
                u32 field24_18 : 8;
            };
        };
        u8 field28[3];      // min size, not max size
        u8 pad_2b[0x1];
        u8  field2c;
        u8 pad_2d[0x3];
    } UnkGlobal_02056370;

    typedef struct {
        UnkGlobal_02056370 tbl[3];
        u8 pad_90[0x6];
        s8 field96[0x20];
        s8 fieldb6[0x81];
        u8 field_137;
    } UnkGlobalContainer_02056370;
#endif
