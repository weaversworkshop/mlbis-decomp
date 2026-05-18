// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "types.h"

extern void func_02009ab8(void);
extern u8 func_02009cc4(void);
extern void func_020308b0(u32 a, u32 b, u32 c, u32 d);
extern void func_02039488(u32 a, u32 b);


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

/*
r0: <garbage data>
r1: <garbage data>
r2: <garbage data>
r3: <garbage data>
r4: <garbage data>
r5: <garbage data>
r6: <garbage data>
r7: <garbage data>
r8: <garbage data>
r9: <garbage data>
r10: <garbage data>
r11: <garbage data>

N: false
Z: false
C: false
V: false
*/
