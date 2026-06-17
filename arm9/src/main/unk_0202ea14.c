// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ea14.h"
#include "main/unk_0202e970.h"
#include "main/unk_0202e9c4.h"

// MATCH 100%
void func_0202ea14(UnkNodeSub * param1, void * param2, void * param3) {
    if (param2 == NULL) {
       func_0202e970(param1, param3);
        return;
    }

    if (param2 == param1->field00) {
        func_0202e9c4(param1, param3);
        return;
    }

    u16 offset = param1->field0a;
    void ** ptr = (void**)((u8 *)param3 + offset);
    void * next = *(void **)((u8 *)param2 + offset);

    ptr[0] = *(void **)((u8 *)param2 + offset);
    ptr[1] = param2;

    *(void**)((u8 *)next + offset + 4) = param3;

    u16 offset2 = param1->field0a;
    *(void**)((u8 *)param2 + offset2) = param3;

    param1->field08_lo++;
}