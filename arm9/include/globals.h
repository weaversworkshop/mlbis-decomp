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

    extern UnkGlobal_01ff9504 data_01ff9504;
    extern UnkGlobal_01ff9478 data_01ff9478;
    extern UnkGlobal_01ff9488 data_01ff9488;
#endif