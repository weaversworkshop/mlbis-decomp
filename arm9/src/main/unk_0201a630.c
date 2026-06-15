// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_0201a630.h"

// MATCHED 100%
void func_0201a630(void) {
    UnkStruct_0205ba70_00 * r1 = data_0205ba70.field14;
    if (r1 == NULL) {
        return;
    }

    do {
        u8 r0 = r1->field11;
        if (r0 != 0 && r0 != 0xff) {
            r1->field12 = r1->field12 & ~0x1;
            r1->field12 |= 0x2;
        }
        r1 = r1->field0c;
    } while (r1 != NULL);
}
