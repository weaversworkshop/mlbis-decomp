// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0201a180.h"

// MATCHED 100%
void func_0201a180(UnkHeader_0201a058 * param1, UnkHeader_0201a058 * param2) {
    param1->field08_00 = param2->field08_00 + 0xc + param1->field08_00;

    UnkHeader_0201a058 * next = param2->field04;
    param1->field04 = next;
    if (next != NULL) {
        next->field00 = param1;
    }
}
