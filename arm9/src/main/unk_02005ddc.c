// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02005ddc.h"
#include "main/unk_0201a630.h"

#include <nitro/os.h>
#include <nitro/pad.h>

extern void func_0201e8b4(void);

// MATCHED 100%
void func_0201a91c() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);
    UnkStruct_0205ba70_00 * node = data_0205ba70.field18;

    if (node != NULL) {
        do {
            u8 flag = node->field11;
            if (flag != 0 && flag != 0xff) {
                node->field12_b = node->field12_b & ~0x1;
                node->field12_b = node->field12_b | 0x2;
            }
            node = node->field0c;
        } while (node != NULL);
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void func_02005f8c(UnkStruct_02005ddc_00 * param1) {
    param1->bit2 = 1;
    param1->dst = param1->src;

    if (reg_OS_IE & REG_OS_IE_VB_MASK) {
        func_0201a91c();
    } else {
        func_0201e8b4();
    }

    func_0201a630();
}

// MATCHED 100%
void func_02019a74(UnkStruct_02019a74 * param1, u16 param2) {
    u16 r4 = param1->field00;
    u32 r3 = 0;
    u32 r5 = r3;

    param1->field02 = (~r4) & param2;
    param1->field04 = param1->field00 & (~param2);

    u16 lr = param1->field00;

    for (u32 r12 = r3; r12 < 10; r12++) {
        u16 *r2 = data_0204ac4c;
        u16 r7 = param2 & r2[r12];

        if (r7 != 0) {
            if ((lr & r7) == 0) {
                param1->field0a[r12] = param1->field08;
                r3 |= r7;
            } else {
                param1->field0a[r12] -= 1;
                if ((param1->field0a[r12] & 0xff) == 0) {
                    r3 |= r7;
                    param1->field0a[r12] = param1->field09;
                }
            }
        } else {
            param1->field0a[r12] = r5;
        }
    }

    param1->field06 = r3;
    param1->field00 = param2;
}

// MATCHED 100%
bool func_02005f24(UnkStruct_02005ddc_00 * param1) {
    func_02019a74(&param1->field08, PAD_Read());

    if (param1->field08.field00 == 0x30c && (param1->field08.field02 & 0x30c)) {
        return true;
    }

    return false;
}

// MATCHED 100%
void func_02005ddc(UnkStruct_02005ddc_00 * param1) {
    if (param1->bit2) {
        return;
    }

    if (param1->bit1) {
        if (param1->bit0) {
            return;
        }

        func_02005f8c(param1);
        return;
    }

    if (func_02005f24(param1) == false) {
        return;
    }

    if (param1->bit0) {
        param1->bit1 = 1;
        return;
    }

    func_02005f8c(param1);
}
