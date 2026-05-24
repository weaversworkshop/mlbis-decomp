// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_020308b0.h"

#include <nitro/os.h>

extern UnkStruct_0203dd70_02 * func_02038848(UnkStruct_0203dd70_01 * a);
extern void func_020389a8(void);
extern u32 func_02038dc4(UnkStruct_0203dd70_01 * a);
extern void func_0203d1dc(UnkStruct_0203dd70_00 * a);
extern bool func_0203d270(UnkStruct_0203dd70_00 * a);

#pragma thumb off

// MATCHED 100%
OSIntrMode func_02038e14(UnkStruct_0203dd70_01 * param1) {
    OSIntrMode mode = OS_DisableInterrupts();

    if (param1->field00 != NULL) {
        while (param1->field00 != NULL) {
            UnkStruct_0203dd70_02 * node = func_02038848(param1);
            node->field64 = true;
            node->field78 = false;
            node->field80 = false;
            node->field7c = false;
        }

        param1->field04 = NULL;
        param1->field00 = NULL;
        func_020389a8();
    }

    return OS_RestoreInterrupts(mode);
}

// MATCHED 100%
OSIntrMode func_0203c390(UnkStruct_0203dd70_00 * param1, u32 param2) {
    OSIntrMode mode = OS_DisableInterrupts();

    UnkStruct_0203dd70_00 * r0 = param1->field04;
    UnkStruct_0203dd70_00 * r1 = param1->field00;

    if (r1 != NULL) {
        r1->field04 = r0;
    }
    if (r0 != NULL) {
        r0->field00 = r1;
    }

    param1->field00 = NULL;
    param1->field04 = NULL;
    param1->field0c &= ~0x4f;
    param1->field14 = param2;
    func_02038e14(&param1->field18);

    return OS_RestoreInterrupts(mode);
}

// MATCHED 100%
bool func_0203dc48(UnkStruct_0203dd70_00 * param1) {
    UnkStruct_0203dd70_00 * r6 = param1;
    u32 check = false;

    OSIntrMode mode = OS_DisableInterrupts();

    bool run = (param1->field0c & 1) ? true : false;
    if (run) {
        check = ((r6->field0c & 0x44) == 0);

        if (check != false) {
            r6->field0c |= 0x4;
            do {
                func_02038dc4(&r6->field18);
            } while ((r6->field0c & 0x40) == 0);
        } else {
            do {
                func_02038dc4(&r6->field18);
                run = ((r6->field0c & 0x1) != 0);
            } while (run != false);
        }
    }

    OS_RestoreInterrupts(mode);

    if (check != false) {
        return func_0203d270(r6);
    }

    return (r6->field14 == 0);
}

// MATCHED 100%
bool func_0203d2b4(UnkStruct_0203dd70_00 * param1, u32 param2) {
    UnkStruct_0203d2b4_00 * r4 = param1->field08;

    param1->field10 = param2;
    param1->field14 = 0x2;

    u32 r7 = 1 << param2;
    param1->field0c |= 1;

    OSIntrMode mode = OS_DisableInterrupts();

    if ((r4->field1c & 0x80) != 0) {
        func_0203c390(param1, 0x3);
        OS_RestoreInterrupts(mode);
        return false;
    }

    if ((r7 & 0x1fc) != 0) {
        param1->field0c |= 0x4;
    }

    UnkStruct_0203dd70_00 * r0 = (UnkStruct_0203dd70_00 *)&r4->field20;
    UnkStruct_0203dd70_00 * r1 = param1->field00;
    UnkStruct_0203dd70_00 * r2 = param1->field04;

    if (r1 != 0) {
        r1->field04 = r2;
    }

    if (r2 != 0) {
        r2->field00 = r1;
    }

    while (r0->field04 != 0) {
        r0 = r0->field04;
    }

    r0->field04 = param1;
    param1->field00 = r0;
    param1->field04 = NULL;

    bool flag = false;
    if ((r4->field1c & 0x8) != 0) {
        flag = true;
    }

    if (flag == false) {
        bool truthy = (r4->field1c & 0x10) ? true : false;
        if (truthy == false) {
            r4->field1c |= 0x10;

            OS_RestoreInterrupts(mode);

            u32 flags = r4->field20.field38;
            if ((flags & 0x200) != 0) {
                r4->field20.field34(param1, 0x9);
            }

            OSIntrMode mode2 = OS_DisableInterrupts();

            param1->field0c |= 0x40;

            bool truthy = (param1->field0c & 0x4) ? true : false;
            if (truthy == false) {
                OS_RestoreInterrupts(mode2);
                func_0203d1dc(param1);
                return true;
            }

            OS_RestoreInterrupts(mode2);
            goto L_0203d44c;
        }
    }

    bool truthy = (param1->field0c & 0x4) ? true : false;
    if (truthy == false) {
        OS_RestoreInterrupts(mode);
        return true;
    }

    do {
        func_02038dc4(&param1->field18);
    } while ((param1->field0c & 0x40) == 0);

    OS_RestoreInterrupts(mode);

L_0203d44c:
    return func_0203d270(param1);
}

// MATCHED 96.77%
s32 func_0203da50(UnkStruct_0203dd70_00 * param1, u32 * param2, s32 param3, s32 param4) {
    s32 r4 = param1->field2c;
    s32 r6 = param3;

    if (r6 > param1->field28 - r4) {
        r6 = param1->field28 - r4;
    }

    param1->field30 = param2;

    if (r6 < 0) {
        r6 = 0;
    }

    param1->field34 = param3;
    param1->field38 = r6;

    if (param4 == 0) {
        param1->field0c = (param1->field0c | 0x4);
    }

    func_0203d2b4(param1, 0);

    if (param4 == 0) {
        u32 result = func_0203dc48(param1);
        if (result != 0) {
            r6 = param1->field2c - r4;
        } else {
            r6 = -1;
        }
    }

    return r6;
}

// MATCHED 100%
s32 func_0203dd50(UnkStruct_0203dd70_00 * param1, u32 * param2, s32 param3) {
    return func_0203da50(param1, param2, param3, 1);
}

// MATCHED 100%
s32 func_0203dd60(UnkStruct_0203dd70_00 * param1, u32 * param2, s32 param3) {
    return func_0203da50(param1, param2, param3, 0);
}

// MATCHED 100%
bool func_0203dd70(UnkStruct_0203dd70_00* param1, s32 param2, u32 param3) {
    switch (param3) {
        case 0:
            param2 += param1->field24;
            break;
        case 1:
            param2 += param1->field2c;
            break;
        case 2:
            param2 += param1->field28;
            break;
        default:
            return false;
    }

    if (param2 < param1->field24) {
        param2 = param1->field24;
    }

    if (param2 > param1->field28) {
        param2 = param1->field28;
    }

    param1->field2c = param2;

    return true;
}

// MATCHED 100%
s32 func_020308b0(u32 param1, u32 * param2, s32 param3, u32 param4) {
    UnkStruct_020308b0_00 * r7 = data_0205f670.field00;
    UnkStruct_020308b0_01 * r2 = r7->field84;

    if (param1 >= r2->field08) {
        return -1;
    }

    s32 r4;
    UnkStruct_020308b0_02 * r11 = &(r2->entries[param1]);
    s32 r5 = r7->field90;

    if (r5 == 0) {
        r5 = param3;
    }

    r4 = 0;
    while (r4 < param3) {
        s32 r6 = param3 - r4;

        if (r6 > r5) {
            r6 = r5;
        }

        u32 r0 = r11->field04 - param4;

        if (r6 > r0) {
            r6 = r0;
        }

        if (r6 == 0) {
            break;
        }

        UnkStruct_0203dd70_00 * input0 = &(r7->field34);

        bool result = func_0203dd70(
            input0,
            r11->field00 + param4,
            0
        );

        if (result == false) {
            return -1;
        }

        s32 result2 = func_0203dd60( // TODO: Check return type
            input0,
            param2,
            r6
        );

        if (result2 < 0) {
            return result2;
        }

        r4 += result2;
        param4 += result2;
        param2 = (u32*)((u8*)param2 + result2);
    }

    return r4;
}

#pragma thumb reset
