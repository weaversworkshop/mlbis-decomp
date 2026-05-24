#ifndef STRUCTS_UNK_0205BA70_H
    #define STRUCTS_UNK_0205BA70_H

    #include "types.h"

    typedef struct UnkStruct_0205ba70_00 {
        u8 pad_00[0xc];
        struct UnkStruct_0205ba70_00 * field0c;
        u8 field10;
        u8 field11;
        u8 field12;
        u8 field13;
    } UnkStruct_0205ba70_00;

    typedef struct {
        u8 pad_00[0x18];
        UnkStruct_0205ba70_00 * field18;
    } UnkGlobal_0205ba70;
#endif