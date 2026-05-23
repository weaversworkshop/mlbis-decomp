#ifndef STRUCTS_UNK_0203D2B4_H
    #define STRUCTS_UNK_0203D2B4_H

    #include "types.h"

    typedef struct UnkStruct_0203dd70_00 UnkStruct_0203dd70_00;

    typedef struct UnkStruct_0203d2b4_01 {
        struct UnkStruct_0203d2b4_01 * field00;
        struct UnkStruct_0203d2b4_01 * field04;
        u8 pad_08[0x2c];
        void (*field34)(UnkStruct_0203dd70_00 * node, u32 event);
        u32 field38;
    } UnkStruct_0203d2b4_01;

    typedef struct UnkStruct_0203d2b4_00 {
        u8 pad_00[0x1c];
        volatile u32 field1c;
        UnkStruct_0203d2b4_01 field20;
    } UnkStruct_0203d2b4_00;
#endif