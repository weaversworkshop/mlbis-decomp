#ifndef STRUCTS_UNK_01FF9504_H
    #define STRUCTS_UNK_01FF9504_H

    #include "types.h"

    typedef struct UnkNode UnkNode;

    typedef struct UnkNodeHeader {
        UnkNode * next;
        UnkNode * field04;
    } UnkNodeHeader;

    struct UnkNode {
        UnkNode * next;
        UnkNode * field04;
        u32 field08;
        union {
            u32 field0c;
            struct { u16 field0c_lo; u16 field0e; };
        };
        // u32 field0c;
        u32 field10;
        void (*field14)(UnkNode *self);
        u32 field18;
        UnkNode * field1c;
        u8 field20;
        u8 field21;
        u8 field22;
        u8 field23;
        u32 field24;
        u32 field28;
        u32 field2c;
        u32 field30;
    };

#endif
