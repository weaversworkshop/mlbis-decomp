#ifndef STRUCTS_UNK_01FF9464_H
    #define STRUCTS_UNK_01FF9464_H

    #include "types.h"

    typedef struct UnkGlobal_01ff9464 {
        u32 * field00;
        u32 * field04;
        u8 field08;
        u8 pad_09[0x3];
        void (*field0c)(void);
        u32 field10;
        union { u32 field14; u8  field14_b; };
        u32 field18;
    } UnkGlobal_01ff9464;
#endif
