#ifndef STRUCTS_SND_LOAD_NODE_H
    #define STRUCTS_SND_LOAD_NODE_H

    #include "types.h"

    typedef struct SndLoadNode SndLoadNode;

    typedef struct SndLoadNodeHead {
        SndLoadNode * next;
    } SndLoadNodeHead;

    typedef struct SndLoadNodeTail {
        SndLoadNode * next;
        SndLoadNode * prev;
    } SndLoadNodeTail;

    struct SndLoadNode {
        SndLoadNode * next;
        void * buffer;
        u32 fileId;
        u32 size;
        u32 offset;
        void (*onComplete)(SndLoadNode *self);
        void * userData;
    };

    #ifdef __cplusplus
    struct SndLoader : SndLoadNode {
        SndLoadNode * current;
        u8 busy;
        u8 slot;
        u8 heapSel;
        u8 pad_23[0x1];
        u32 readSoFar;
        u32 seqNo;
        u32 state;
        u32 arg;
    };
    #endif

#endif
