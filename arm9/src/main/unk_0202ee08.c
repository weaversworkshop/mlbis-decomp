// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ec70.h"
#include "main/unk_0202ee08.h"

// MATCH 100%
bool func_0202ee08(UnkNode * param1, u16 param2) {
    u32 hold = param1->field24;

    UnkNode_HeapRegion * heap_region = func_0202ec70(&param1->field24, 0x10, 0x4);
    if (heap_region == NULL) {
        return false;
    }

    heap_region->field00 = param2;
    heap_region->field04 = hold;
    heap_region->field08 = param1->field28;
    heap_region->field0c = param1->field2c;

    param1->field2c_ptr = heap_region;

    return true;
}
