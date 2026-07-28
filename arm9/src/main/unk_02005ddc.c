// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02005ddc.h"

#include <nitro/pad.h>

#include "main/system.h"
#include "main/task.h"
#include "main/touchpad.h"

// MATCHED 100%
void func_02005f8c(UnkStruct_02005ddc * param1) {
    param1->field00_02 = 1;
    param1->dst = param1->src;

    if (reg_OS_IE & REG_OS_IE_VB_MASK) {
        resetTasks();
    } else {
        disableVCountTasks();
    }

    disableAllTasks();
}

// MATCHED 100%
BOOL func_02005f24(UnkStruct_02005ddc * param1) {
    decodeKeypad(&param1->field08, PAD_Read());

    if (param1->field08.hold == RESET_COMBO && (param1->field08.press & RESET_COMBO)) {
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
