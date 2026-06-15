// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202e944.h"
#include "main/unk_0202e970.h"

// MATCH 100%
void func_0202e970(UnkNodeSub * param1, u8 * param2) {
    if (param1->field00 == NULL) {
        func_0202e944(param1, param2);
        return;
    }

    u16 offset = param1->field0a;

    *(u32 *)(param2 + offset) = (u32)param1->field04;
    *(u32 *)(param2 + offset + 4) = 0;

    u16 offset2 = param1->field0a;
    *(u32 *)((u8*)param1->field04 + offset2 + 4) = param2;

    param1->field04 = param2;
    param1->field08_lo++;
}
