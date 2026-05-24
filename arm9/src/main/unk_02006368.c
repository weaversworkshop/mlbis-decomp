// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02006368.h"
#include "main/unk_020308b0.h"

#include <nitro/os.h>

extern u8 func_02009cc4(void);

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
    data_01ff9478.next = &data_01ff948c;
    data_01ff948c.next = (UnkNode *)-1;
    data_01ff948c.field04 = &data_01ff9478;
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
