// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02005ddc.h"

#include <nitro/os.h>
#include <nitro/pad.h>

#include "main/system.h"
#include "main/task.h"

// MATCHED 100%
void func_0201e8b4() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);
    UnkStruct_0205bad4_00 * r3 = data_0205bad4.field08;

    while (r3 != NULL) {
        if (r3->field15 != 0 && r3->field15 != 0xff) {
            r3->field16 = r3->field16 & ~1;
            r3->field16 = r3->field16 | 2;
        }
        r3 = r3->field0c;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void func_02005f8c(UnkStruct_02005ddc * param1) {
    param1->field00_02 = 1;
    param1->dst = param1->src;

    if (reg_OS_IE & REG_OS_IE_VB_MASK) {
        func_0201a91c();
    } else {
        func_0201e8b4();
    }

    func_0201a630();
}

// MATCHED 100%
BOOL func_02005f24(UnkStruct_02005ddc * param1) {
    func_02019a74(&param1->field08, PAD_Read());

    if (param1->field08.field00 == 0x30c && (param1->field08.field02 & 0x30c)) {
        return TRUE;
    }

    return FALSE;
}

// MATCHED 100%
void func_02005ddc(UnkStruct_02005ddc * param1) {
    if (param1->field00_02) {
        return;
    }

    if (param1->field00_01) {
        if (param1->field00_00) {
            return;
        }

        func_02005f8c(param1);
        return;
    }

    if (!func_02005f24(param1)) {
        return;
    }

    if (param1->field00_00) {
        param1->field00_01 = 1;
        return;
    }

    func_02005f8c(param1);
}
