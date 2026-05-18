// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "types.h"

extern u8 func_02009cc4(void);
extern void func_02039488(u32 a, u32 b);
extern void func_0203d2b4(UnkStruct_0203dd70_00 * a, u32 b);
extern u32 func_0203dc48(UnkStruct_0203dd70_00 * a);


#pragma thumb off

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
    data_01ff9478.field00 = &data_01ff948c;
    data_01ff948c.next = (UnkNode *)-1;
    data_01ff948c.field04 = &data_01ff9478;
}

// MATCHED 96.77%
s32 func_0203da50(UnkStruct_0203dd70_00 * param1, u32 param2, s32 param3, s32 param4) {
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
s32 func_0203dd50(UnkStruct_0203dd70_00 * param1, u32 param2, s32 param3) {
    return func_0203da50(param1, param2, param3, 1);
}

// MATCHED 100%
s32 func_0203dd60(UnkStruct_0203dd70_00 * param1, u32 param2, s32 param3) {
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
s32 func_020308b0(u32 param1, u32 param2, s32 param3, u32 param4) {
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
        param2 += result2;
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

        u32 field04 = data_01ff9504.field04;
        u32 field24 = data_01ff9504.field24;
        func_020308b0(
            data_01ff9504.field08,
            field04 + field24,
            chunk,
            field24 + data_01ff9504.field10
        );

        func_02039488(field04 + field24, chunk);

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

        if (truthy && data_01ff9478.field00 != invalid) {
            data_01ff9504.field1c = data_01ff9478.field00;
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
