// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "types.h"

#include <nitro/os.h>
#include <nitro/pad.h>

extern u8 func_02009cc4(void);
// extern void func_0201a91c(void);
extern void func_0201e8b4(void);
extern UnkStruct_0203dd70_02 * func_02038848(UnkStruct_0203dd70_01 * a);
extern void func_020389a8(void);
extern u32 func_02038dc4(UnkStruct_0203dd70_01 * a);
extern OSIntrMode func_02038e14(UnkStruct_0203dd70_01 * a);
extern void func_0203d1dc(UnkStruct_0203dd70_00 * a);
extern bool func_0203d270(UnkStruct_0203dd70_00 * a);


#pragma thumb off

// MATCHED 100% -- Noop
void func_02004b98() {
    return;
}

// MATCHED 100%
void func_0201a91c() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);
    UnkStruct_0205ba70_00 * node = data_0205ba70.field18;

    if (node != NULL) {
        do {
            u8 flag = node->field11;
            if (flag != 0 && flag != 0xff) {
                node->field12 = node->field12 & ~0x1;
                node->field12 = node->field12 | 0x2;
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
void func_0201a610(UnkStruct_020057ac_00 * param1) {
    param1->field12 = (param1->field12 & ~0x1) | 0x1;
    param1->field12 |= 0x2;
}

// MATCHED 100%
void func_020057ac(UnkStruct_020057ac_00 * param1, u16 param2) {
    param1->field2a = param1->field28;
    param1->field28 = param2;
    param1->field10 = (u8)0x1;

    func_0201a610(param1);
}

// MATCHED 100% -- Gets pointer from table, add offset
u32 * func_02005ca4(u32 param1, u32 param2) {
    return (u32*)((u8*)data_020564bc[param1] + data_020564bc[param1][param2]);
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

// MATCHED 100%
u8 func_02006368(void) {
    if (data_01ff9504.field20 == 0) {
        return data_01ff9504.field20;
    }
    return func_02009cc4();
}

// MATCHED 100%
u8 func_02006388(void) {
    return data_01ff9504.field20;
}

// NOT MATCHED: stmia not forming, 3 strs instead  (77.25%)
void func_02009ab8() {
    data_01ff9478.next = &data_01ff948c;
    data_01ff948c.next = (UnkNode *)-1;
    data_01ff948c.field04 = &data_01ff9478;
}

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

// MATCHED 94.67%
bool func_02009e7c(void) {
    u32 chunk;
    u32 accumulator = 0;
    UnkNode *invalid = (UnkNode *)-1;
    
    do {
        chunk = data_01ff9488.field00 - accumulator;

        if (data_01ff9504.field0c - data_01ff9504.field24 < chunk) {
            chunk = data_01ff9504.field0c - data_01ff9504.field24;
        }

        // UnkGlobal_01ff9478 * field04 = data_01ff9504.field04;
        u32 field24 = data_01ff9504.field24;
        u32 * ptr = (u32*)((u8*)data_01ff9504.field04 + field24);
        func_020308b0(
            data_01ff9504.field08,
            ptr,
            chunk,
            field24 + data_01ff9504.field10
        );

        DC_StoreRange(ptr, chunk);

        data_01ff9504.field24 += chunk;
        if (data_01ff9504.field24 < data_01ff9504.field0c) {
            goto false_exit;
            // break; // Early return false?
        }

        data_01ff9504.field24 = 0;

        bool truthy = false;
        if (data_01ff9504.field1c != invalid 
            && data_01ff9504.field1c->next == invalid) {
            truthy = true;
            func_02009ab8();
        }

        if (data_01ff9504.field14 != NULL) {
            data_01ff9504.field14(&data_01ff9504);
        }

        if (truthy && data_01ff9478.next != invalid) {
            data_01ff9504.field1c = data_01ff9478.next;
        }

        UnkNode *next = data_01ff9504.field1c->next;
        data_01ff9504.field00 = next;
        data_01ff9504.field04 = data_01ff9504.field1c->field04;
        data_01ff9504.field08 = data_01ff9504.field1c->field08;
        data_01ff9504.field0c = data_01ff9504.field1c->field0c;
        data_01ff9504.field10 = data_01ff9504.field1c->field10;
        data_01ff9504.field14 = data_01ff9504.field1c->field14;
        data_01ff9504.field18 = data_01ff9504.field1c->field18;
        data_01ff9504.field1c = data_01ff9504.field00;

        // UnkNode *first = data_01ff9504.field00;
        if (data_01ff9504.field00 == invalid) {
            data_01ff9504.field20 = 0;
            goto true_exit;
        }

        // Check loop conditions
    } while (chunk < data_01ff9488.field00 && (accumulator += chunk) < data_01ff9488.field00);

    true_exit:
        return true;
    false_exit:
        return false;
}


#pragma thumb reset
