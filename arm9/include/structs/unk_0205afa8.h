#ifndef STRUCTS_UNK_0205AFA8_H
    #define STRUCTS_UNK_0205AFA8_H

    #include "types.h"

    typedef struct {
        u8 x;
        u8 y;
        u8 prevX;
        u8 prevY;
        union {
            struct {
                u8 active : 1;
                u8 hold : 1;
                u8 trigger : 1;
                u8 press : 1;
                u8 release : 1;
                u8 sampling : 1;
                u8 : 2;
            };
            u8 flags;
        };
        u8 pad_05[0x3];
    } TouchpadStruct;

    typedef struct {
        u16 field00;
        u16 field02;
        u16 field04;
        u16 field06;
        u8 field08;
        u8 field09;
        u8 field0a[10];
        TouchpadStruct touch;
    } UnkGlobal_0205afa8;
#endif
