#ifndef STRUCTS_UNK_01FF9504_H
    #define STRUCTS_UNK_01FF9504_H

    #include "types.h"

    typedef struct UnkNode            UnkNode;

    struct UnkNode {
        UnkNode * next;
        UnkNode * field04;
        u32 field08;
        u32 field0c;
        u32 field10;
        void (*field14)(UnkNode *self);
        u32 field18;
        UnkNode * field1c;
        u8 field20;
        u8 pad_21[0x3];
        u32 field24;
    };

#endif
