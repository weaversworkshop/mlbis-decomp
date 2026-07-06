#ifndef STRUCTS_UNK_020563E4_HEAD_H
    #define STRUCTS_UNK_020563E4_HEAD_H

    #ifdef STRUCTS_UNK_020563E4_H
        #error "unk_020563e4.h (full 0x114) and unk_020563e4_head.h (<=0x40) both included: data_020563e4 would get conflicting types."
    #endif

    #include "types.h"

    typedef struct {
        u8 pad_00[0x20];
        u16 field20;
    } UnkGlobal_020563e4_head;
    extern UnkGlobal_020563e4_head data_020563e4;
#endif
