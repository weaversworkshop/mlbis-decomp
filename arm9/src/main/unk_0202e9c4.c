// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202e944.h"
#include "main/unk_0202e9c4.h"

// MATCH 100%
void func_0202e9c4(UnkNodeSub * param1, void * param2) {
    if (param1->field00 == NULL) {
        func_0202e944(param1, param2);
        return;
    }

    u16 offset = param1->field0a;
    void ** ptr = (void **)((u8 *)param2 + offset);

    ptr[0] = NULL;
    ptr[1] = param1->field00;

    u16 offset2 = param1->field0a;
    *(void **)((u8 *)param1->field00 + offset2) = param2;

    param1->field00 = param2;
    param1->field08_lo++;
}