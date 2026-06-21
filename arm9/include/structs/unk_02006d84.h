#ifndef STRUCTS_UNK_02006D84_H
    #define STRUCTS_UNK_02006D84_H

    #include "types.h"

    typedef struct UnkStruct_02006d84 {
        struct UnkStruct_02006d84 * next;
        struct UnkStruct_02006d84 * prev;
        u8 tag;
        void (*fn)(u32, u32);
        u32 arg1;
        u32 arg2;
        u32 field18;
    } UnkStruct_02006d84;
#endif