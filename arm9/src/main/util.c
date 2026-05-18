// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "types.h"

extern void func_02009ab8(void);
extern u8 func_02009cc4(void);
extern void func_02039488(u32 a, u32 b);
extern s32 func_0203dd60(u32* a, u32 b, u32 c);
extern u32 func_0203dd70(u32* a, u32 b, u32 c);


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

// MATCHED 100%
s32 func_020308b0(u32 param1, u32 param2, s32 param3, u32 param4) {
    // Load data_0205f670 into r4
    // Copy r2 into r9
    // Load word 0x0 of r4 into r7
    UnkStruct_020308b0_00 * r7 = data_0205f670.field00;  // TODO: Make field00 a pointer or array

    // Copy r1 into r10
    // Load word 0x84 of r7 into r2
    UnkStruct_020308b0_01 * r2 = r7->field84;  // TODO: Also make this a pointer

    // Copy r3 into r8
    // Load word 0x8 of r2 into r1
    // Compare r0,r1
    if (param1 >= r2->field08) {
        // Return boolean not of 0
        return -1;
    }

    // Load word 0x90 of r7 into r5
    s32 r4;
    UnkStruct_020308b0_02 * r11 = &(r2->entries[param1]);
    s32 r5 = r7->field90;
    // Set r1 = r2 + 0xc

    // Compare r5, 0x0
    

    // Set r11 = r1 + r0 left shifted 0x4
    

    if (r5 == 0) {
        // Set r5 equal to r9
        r5 = param3;
    }

    // Compare r9, 0x0  ; I think this is caching the compare for the loop (i.e. skip past loop if r9 <= 0)
    // Set r4 = 0x0
    r4 = 0;
    while (r4 < param3) {
        // Load word 0x4 of r11 into r0
        // Set r6 = r9 - r4
        s32 r6 = param3 - r4;

        // Compare r6, r5
        if (r6 > r5) {
            // Copy r5 into r6
            r6 = r5;
        }

        // Reduce r0 by r8
        u32 r0 = r11->field04 - param4;

        // Compare r6, r0
        if (r6 > r0) {
            // Copy r0 into r6
            r6 = r0;
        }

        // Compare r6, 0x0
        if (r6 == 0) {
            break; // Jump to end of loop
        }

        // Load word 0x0 of r11 into r1
        // Set r2 to 0
        // Set r0 to r7 + 0x34
        u32 * input0 = &(r7->field34);
        // Increment r1 by r8
        // Function call
        u32 result = func_0203dd70( // TODO: Consider changing return type to bool
            input0,
            r11->field00 + param4,
            0
        );  

        // Check result
        if (result == 0) {
            // Return boolean not of 0
            return -1;
        }

        // Set r1 to r10
        // Set r2 to r6
        // Set r0 to r7 + 0x34
        // Function call
        s32 result2 = func_0203dd60( // TODO: Check return type
            input0,
            param2,
            r6
        );
        // Compare r0, 0x0
        if (result2 < 0) {
            return result2;  // Early return
        }

        // Increment r4 by r0
        r4 += result2;
        // Compare r4, r9  ->  (likely cached for loop check)
        // Increment r8 by r0
        param4 += result2;
        // Increment r10 by r0
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
