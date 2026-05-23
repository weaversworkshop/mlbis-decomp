#ifndef STRUCTS_UNK_0203DD70_H
    #define STRUCTS_UNK_0203DD70_H

    #include "types.h"

    typedef struct UnkStruct_0203d2b4_00 UnkStruct_0203d2b4_00;

    typedef struct UnkStruct_0203dd70_02 {
        u8 pad_00[0x64];
        u32 field64;
        u8 pad_68[0x10];
        u32 field78;
        u32 field7c;
        u32 field80;
    } UnkStruct_0203dd70_02;

    typedef struct UnkStruct_0203dd70_01 {
        UnkStruct_0203dd70_02 * field00;
        UnkStruct_0203dd70_02 * field04;    // Tentative assignment
    } UnkStruct_0203dd70_01;

    typedef struct UnkStruct_0203dd70_00 {
        struct UnkStruct_0203dd70_00 * field00; // We think this may be a doubly-linked list?
        struct UnkStruct_0203dd70_00 * field04; // Tentative assignment for `prev` and `next` ptrs
        UnkStruct_0203d2b4_00 * field08;
        volatile s32 field0c;
        u32 field10;
        u32 field14;
        UnkStruct_0203dd70_01 field18;  // Definitely a struct -> see func_0203c390()
        u8 pad_20[0x4];
        s32 field24;
        s32 field28;
        s32 field2c;
        u32 * field30;
        s32 field34;
        s32 field38;
    } UnkStruct_0203dd70_00;
#endif