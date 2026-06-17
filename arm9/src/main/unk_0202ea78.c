// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ea78.h"

// MATCH 100%
void func_0202ea78(UnkNodeSub * param1, UnkStruct_0202f9cc * param2) {
    u16 offset = param1->field0a;
    void * r3 = *(void **)((u8 *)param2 + offset);
    void ** next = (void **)((u8 *)param2 + offset);

    if (r3 == NULL) {
        param1->field00 = next[1];
    } else {
        *(void **)((u8 *)r3 + offset + 4) = next[1];
    }

    r3 = next[1];
    if (r3 == NULL) {
        param1->field04 = next[0];
    } else {
        *(void **)((u8 *)r3 + param1->field0a) = next[0];
    }

    next[0] = NULL;
    next[1] = NULL;

    param1->field08_lo--;
}
