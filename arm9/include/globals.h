#ifndef GLOBALS_H
    #define GLOBALS_H

    #include "types.h"

    typedef struct UnkGlobal_01ff9504 UnkGlobal_01ff9504;
    typedef struct UnkNode  UnkNode;

    struct UnkNode {
        UnkNode * next;
        u32 field04;
        u32 field08;
        u32 field0c;
        u32 field10;
        void (*field14)(UnkGlobal_01ff9504 *self);
        u32 field18;
    };

    struct UnkGlobal_01ff9504 {
        UnkNode * field00;
        u32 field04;
        u32 field08;
        u32 field0c;
        u32 field10;
        void (*field14)(UnkGlobal_01ff9504 *self);
        u32 field18;
        UnkNode * field1c;
        u8 field20;
        u8 pad_21[0x3];
        u32 field24;
    };

    typedef struct {
        UnkNode * field00;
    } UnkGlobal_01ff9478;

    typedef struct {
        u32 field00;
    } UnkGlobal_01ff9488;


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
        u32 field34;
        u8 pad_38[0x4c];
        UnkStruct_020308b0_01 * field84;
        u8 pad_88[0x8];
        s32 field90;
    } UnkStruct_020308b0_00;

    

    typedef struct {
        UnkStruct_020308b0_00 * field00;
    } UnkGlobal_0205f670;

    extern UnkGlobal_01ff9504 data_01ff9504;
    extern UnkGlobal_01ff9478 data_01ff9478;
    extern UnkGlobal_01ff9488 data_01ff9488;
    extern UnkGlobal_0205f670 data_0205f670;
#endif