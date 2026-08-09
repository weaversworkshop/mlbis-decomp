#ifndef STRUCTS_SND_LOAD_SLOT_H
    #define STRUCTS_SND_LOAD_SLOT_H

    #include "types.h"

    typedef struct {
        u32 tableSize;
        u32 userData;
        u32 inUse : 1;
        u32 offsetsFixed : 1;
    } SndLoadSlot;
#endif
