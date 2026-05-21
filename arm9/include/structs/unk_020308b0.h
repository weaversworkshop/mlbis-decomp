#ifndef STRUCTS_UNK_020308B0_H
    #define STRUCTS_UNK_020308B0_H

    #include "types.h"

    typedef struct {
        u32 field00;
        s32 field04;
        u8 pad_00[0x8];
    } UnkStruct_020308b0_02;

    typedef struct {
        u8 pad_00[0x8];
        u32 field08;
        UnkStruct_020308b0_02 entries[/*??*/];
    } UnkStruct_020308b0_01;

    typedef struct UnkStruct_0203dd70_00 UnkStruct_0203dd70_00;
    typedef struct UnkStruct_0203d2b4_00 UnkStruct_0203d2b4_00;

    typedef struct UnkStruct_0203d2b4_01 {
        struct UnkStruct_0203d2b4_01 * field00;
        struct UnkStruct_0203d2b4_01 * field04;
        u8 pad_08[0x2c];
        void (*field34)(UnkStruct_0203dd70_00 * node, u32 event);
        u32 field38;
    } UnkStruct_0203d2b4_01;

    struct UnkStruct_0203dd70_00 {
        struct UnkStruct_0203dd70_00 * field00; // We think this may be a doubly-linked list?
        struct UnkStruct_0203dd70_00 * field04; // Tentative assignment for `prev` and `next` ptrs
        UnkStruct_0203d2b4_00 * field08;
        volatile s32 field0c;
        u32 field10;
        u32 field14;
        u32 field18;
        u8 pad_1c[0x8];
        s32 field24;
        s32 field28;
        s32 field2c;
        u32 * field30;
        s32 field34;
        s32 field38;
    };

    typedef struct UnkStruct_0203d2b4_00 {
        u8 pad_00[0x1c];
        volatile u32 field1c;
        UnkStruct_0203d2b4_01 field20;
    };

    typedef struct {
        u8 pad_00[0x34];
        UnkStruct_0203dd70_00 field34;
        u8 pad_70[0x14];
        UnkStruct_020308b0_01 * field84;
        u8 pad_88[0x8];
        s32 field90;
    } UnkStruct_020308b0_00;

    typedef struct {
        UnkStruct_020308b0_00 * field00;
    } UnkGlobal_0205f670;
#endif
