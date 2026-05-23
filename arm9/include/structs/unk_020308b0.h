#ifndef STRUCTS_UNK_020308B0_H
    #define STRUCTS_UNK_020308B0_H

    #include "types.h"

    #include "unk_0203dd70.h"

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