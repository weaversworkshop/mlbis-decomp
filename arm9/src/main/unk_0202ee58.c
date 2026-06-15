// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ee58.h"

// MATCH 100%
bool func_0202ee58(UnkNode * param1, u32 param2) {
    UnkNode_HeapRegion * r3 = param1->field2c_ptr;

    if (param2 != 0 && r3 != NULL) {
        do {
            if (r3->field00 == param2) break;
            r3 = r3->field0c;
        } while (r3 != NULL);
    }

    if (r3 == NULL) {
        return false;
    }

    param1->field24 = r3->field04;
    param1->field28 = r3->field08;
    param1->field2c_ptr = r3->field0c;

    return true;
}
