// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202e944.h"

// MATCH 100%
void func_0202e944(UnkNodeSub * param1, void * param2) {
    u16 offset = param1->field0a;

    *(u32 *)((u8*)param2 + offset + 4) = 0;
    *(u32 *)((u8*)param2 + offset) = 0;

    param1->field00 = param2;
    param1->field04 = param2;
    param1->field08_lo++;
}
