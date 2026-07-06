#ifndef STRUCTS_UNK_020563E4_H
    #define STRUCTS_UNK_020563E4_H

    #ifdef STRUCTS_UNK_020563E4_HEAD_H
        #error "unk_020563e4.h (full 0x114) and unk_020563e4_head.h (<=0x40) both included: data_020563e4 would get conflicting types."
    #endif

    #include "types.h"

    typedef struct {
        u8 pad_00[0x20];
        u16 field20;
        u8 pad_22[0xc6 - 0x22];
        s16 fieldc6;
        s16 fieldc8;
        u8 pad_ca[0xfc - 0xca];
        u16 fieldfc;
        u16 fieldfe;
        u16 field100;
        u16 field102;
        u32 field104;
        u32 field108;
        u8 pad_10c[0x114 - 0x10c];
    } UnkGlobal_020563e4;
    extern UnkGlobal_020563e4 data_020563e4;
#endif
