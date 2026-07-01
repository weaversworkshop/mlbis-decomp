// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/backup.h"
#include "main/unk_tu_02017bec.h"

// --- Struct Definitions --- //

typedef struct {
    s16 field00;
    u16 field02;
    u32 field04;
    u32 field08;
    u8 field0c_b0 : 1;
    u8 field0c : 7;
    u8 pad_0d[0x3];
} UnkStruct_02019360;

typedef struct {
    u8  pad_00[0x8];
    u16 field08;
    u8  pad_0a[0x2];
} UnkStruct_020526ec;

typedef struct {
    u16 field00;
    u8 pad_02[0xd];
} UnkGlobal_020526f8;

typedef struct {
    u16 field00;
    u8 pad_02[0x16];
} UnkGlobal_02052a68;

typedef struct {
    u16 field00;
    u8 pad_02[0x1d];
} UnkGlobal_02053b30;

// --- Extern Data Variables --- //

typedef struct { u8 data[SAVE_SLOT_SIZE]; } SaveSlot;
extern SaveSlot data_02058f94[SAVE_SLOT_COUNT];

// --- Extern Functions --- //

// --- Variable Definitions --- // (replace with real data later)

extern UnkStruct_020526ec * data_020526ec[];
extern UnkGlobal_020526f8 data_020526f8[];
extern UnkGlobal_02052a68 data_020527f8[];
extern UnkGlobal_02052a68 data_02052a68[];
extern UnkGlobal_02053b30 data_02053b30[];

// --- Function Declarations --- //

// `func_02018048`
u16 * func_02018538(s32 param1);
// `func_020185d0`
// `func_02018600`
// `func_02018630`
// `func_02018650`
// `func_020187dc`
s32 func_02018824(u16 param1);
s32 func_02018878(u16 param1, s8 param2);
s32 func_02018974(s32 param1);
// `func_020189b4`
void func_02018ec4(u8 param1, u16 param2, u8 param3);
void func_02019028(u8 param1);
void func_02019058(u8 param1, u16 param2);
// `func_020190a4`
// `func_02019154`
void func_02019360(UnkStruct_02019360 * param1, s32 param2, u32 param3);
// `func_020193c4`
// `func_020193ec`
s32 func_020194c4(UnkStruct_02019360 * param1);
s32 func_020194f4(u8 param1, u8 param2, s8 param3);
s32 func_02019564(UnkStruct_02019360 * param1);
s32 func_02019574(UnkStruct_02019360 * param1);
s32 func_0201957c(void);
s32 func_02019584(UnkStruct_02019360 * param1);
s32 func_020195a4(UnkStruct_02019360 * param1);
s32 func_020195d4(UnkStruct_02019360 * param1);
s32 func_02019618(UnkStruct_02019360 * param1);
s32 func_02019620(UnkStruct_02019360 * param1);

// --- Function Definitions --- //

// MATCHED 100%
BOOL func_02017b60(u32 slot_idx) {
    if (slot_idx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    func_020179d8((void *)(slot_idx * SAVE_SLOT_SIZE + SAVE_SLOT_1_OFFSET), &data_02058f94[slot_idx], SAVE_PAYLOAD_SIZE, 1);

    return TRUE;
}


BOOL func_02017ba4(u32 slot_idx) {
//     .global func_02017ba4
//     arm_func_start func_02017ba4
// func_02017ba4: ; 0x02017ba4
//     stmdb sp!, {r3, lr}
//     cmp r0, #0x2
//     movhs r0, #0x0
//     ldmhsia sp!, {r3, pc}
    if (slot_idx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }
//     ldr r1, .L_02017be0
//     ldr r3, .L_02017be4
//     mul r1, r0, r1
//     add r0, r1, #0x3fc
//     ldr r2, .L_02017be8
//     add r0, r0, #0x400
//     add r1, r3, r1
//     mov r3, #0x1
//     bl func_020179d8
    func_020179d8((void *)(slot_idx * SAVE_SLOT_SIZE + SAVE_SLOT_2_OFFSET), &data_02058f94[slot_idx], SAVE_PAYLOAD_SIZE, 1);
//     mov r0, #0x1
//     ldmia sp!, {r3, pc}
// .L_02017be0: .word 0xfd8
// .L_02017be4: .word data_02058f94
// .L_02017be8: .word 0x5f4
//     arm_func_end func_02017ba4
    return TRUE;
}

// func_02017bec: ; 0x02017bec
//     stmdb sp!, {r3, r4, r5, r6, r7, lr}
//     ldr r5, .L_02017fd8
//     ldr r4, .L_02017fdc
//     ldrb r7, [r5, #0x4cc]
//     ldr r6, [r4, #0x8]
//     mov r5, #0x1
//     mov r4, r7, lsl #0x1b
//     mov r4, r4, lsr #0x1e
//     mov r7, r4, lsl #0x1
//     mov ip, r5, lsl r7
//     mov lr, r6, lsl #0x18
//     tst ip, lr, lsr #0x1c
//     mov r6, r3
//     moveq r0, r5
//     ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
//     ldr r3, .L_02017fe0
//     add r5, r7, r6
//     mul r3, r5, r3
//     ldr ip, .L_02017fe4
//     ldr r5, [ip, r3]
//     adds r5, r5, r0
//     ldr r0, .L_02017fe8
//     movmi r5, #0x0
//     cmp r5, r0
//     movgt r5, r0
//     str r5, [ip, r3]
//     cmp r1, #0x0
//     beq .L_02017c7c
//     ldr r5, .L_02017fec
//     ldr r0, .L_02017ff0
//     add r5, r5, r3
//     and r1, r1, r0
//     ldrsb r0, [r5, r1]
//     cmp r0, #0x9
//     addlt r0, r0, #0x1
//     strltb r0, [r5, r1]
// .L_02017c7c:
//     cmp r2, #0x0
//     beq .L_02017ca4
//     ldr r1, .L_02017fec
//     ldr r0, .L_02017ff0
//     add r5, r1, r3
//     and r1, r2, r0
//     ldrsb r0, [r5, r1]
//     cmp r0, #0x9
//     addlt r0, r0, #0x1
//     strltb r0, [r5, r1]
// .L_02017ca4:
//     ldr r0, .L_02017fdc
//     ldr r2, .L_02017fd8
//     mov r5, #0x0
//     add r7, r0, r3
// .L_02017cb4:
//     add r0, r2, r5, lsl #0x1
//     add r0, r0, #0x400
//     ldrh r1, [r0, #0xe8]
//     add r0, r7, r5, lsl #0x1
//     add r0, r0, #0x100
//     add r5, r5, #0x1
//     strh r1, [r0, #0x1e]
//     cmp r5, #0xa
//     blt .L_02017cb4
//     ldr r5, .L_02017ff4
//     mov r1, #0x0
//     ldr r0, .L_02017ff8
//     mov r2, r1
//     add r5, r5, r3
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x80000
//     biceq r0, r0, #0x80000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02017ffc
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x100000
//     biceq r0, r0, #0x100000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018000
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x200000
//     biceq r0, r0, #0x200000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018004
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x400000
//     biceq r0, r0, #0x400000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018008
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x800000
//     biceq r0, r0, #0x800000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_0201800c
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x1000000
//     biceq r0, r0, #0x1000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018010
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x2000000
//     biceq r0, r0, #0x2000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018014
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x4000000
//     biceq r0, r0, #0x4000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018018
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x8000000
//     biceq r0, r0, #0x8000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_0201801c
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     orrne r0, r0, #0x10000000
//     biceq r0, r0, #0x10000000
//     str r0, [r5, #0xe0]
//     mov r1, #0x0
//     ldr r0, .L_02018020
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x20000000
//     biceq r0, r0, #0x20000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018024
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x40000000
//     biceq r0, r0, #0x40000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_02018028
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe0]
//     mov r1, #0x0
//     orrne r0, r0, #0x80000000
//     biceq r0, r0, #0x80000000
//     str r0, [r5, #0xe0]
//     ldr r0, .L_0201802c
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     mov r1, #0x0
//     orrne r0, r0, #0x1
//     biceq r0, r0, #0x1
//     str r0, [r5, #0xe4]
//     ldr r0, .L_02018030
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     mov r1, #0x0
//     orrne r0, r0, #0x2
//     biceq r0, r0, #0x2
//     str r0, [r5, #0xe4]
//     ldr r0, .L_02018034
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     mov r1, #0x0
//     orrne r0, r0, #0x4
//     biceq r0, r0, #0x4
//     str r0, [r5, #0xe4]
//     ldr r0, .L_02018038
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     mov r1, #0x0
//     orrne r0, r0, #0x8
//     biceq r0, r0, #0x8
//     str r0, [r5, #0xe4]
//     ldr r0, .L_0201803c
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     mov r1, #0x0
//     orrne r0, r0, #0x10
//     biceq r0, r0, #0x10
//     str r0, [r5, #0xe4]
//     ldr r0, .L_02018040
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     mov r1, #0x0
//     orrne r0, r0, #0x20
//     biceq r0, r0, #0x20
//     str r0, [r5, #0xe4]
//     ldr r0, .L_02018044
//     mov r2, r1
//     bl func_020058bc
//     cmp r0, #0x0
//     ldr r0, [r5, #0xe4]
//     orrne r0, r0, #0x40
//     biceq r0, r0, #0x40
//     str r0, [r5, #0xe4]
//     cmp r6, #0x0
//     mov r0, r4
//     beq .L_02017fc4
//     bl func_02017ba4
//     b .L_02017fc8
// .L_02017fc4:
//     bl func_02017b60
// .L_02017fc8:
//     cmp r0, #0x0
//     movne r0, #0x0
//     moveq r0, #0x2
//     ldmia sp!, {r3, r4, r5, r6, r7, pc}
// .L_02017fd8: .word data_02055fe4
// .L_02017fdc: .word data_02058f84
// .L_02017fe0: .word 0x7ec
// .L_02017fe4: .word data_02058fe8
// .L_02017fe8: .word 0xf423f
// .L_02017fec: .word data_0205900c
// .L_02017ff0: .word 0xfff
// .L_02017ff4: .word data_02059148
// .L_02017ff8: .word 0xe713
// .L_02017ffc: .word 0xe714
// .L_02018000: .word 0xe715
// .L_02018004: .word 0xe716
// .L_02018008: .word 0xe717
// .L_0201800c: .word 0xe718
// .L_02018010: .word 0xe719
// .L_02018014: .word 0xe71a
// .L_02018018: .word 0xe71b
// .L_0201801c: .word 0xe71c
// .L_02018020: .word 0xe71d
// .L_02018024: .word 0xe71e
// .L_02018028: .word 0xe71f
// .L_0201802c: .word 0xe720
// .L_02018030: .word 0xe721
// .L_02018034: .word 0xe722
// .L_02018038: .word 0xe723
// .L_0201803c: .word 0xe724
// .L_02018040: .word 0xe725
// .L_02018044: .word 0xe726
//     arm_func_end func_02017bec



// MATCHED 100%
u16 * func_02018538(s32 param1) {
    u32 row = param1 & 0xfff;

    switch (param1 & 0xf000) {
        case 0x1000:
            return &data_02052a68[row].field00;
        case 0x2000:
            return &data_020527f8[row].field00;
        case 0x3000:
            return &data_020526f8[row].field00;
        case 0x4000:
            return &data_02053b30[row].field00;
        default:
            return NULL;
    }
}

// MATCHED 100%
s32 func_020194c4(UnkStruct_02019360 * param1) {
    return func_020194f4(param1->field00, param1->field02, param1->field0c_b0);
}

// MATCHED 100%
s32 func_020194f4(u8 param1, u8 param2, s8 param3) {
    if (param1 == 2) {
        if (param2 < 9) {
            return 1;
        } else if (param2 < 19) {
            return 2;
        } else if (param2 < 39) {
            return 3;
        } else {
            return 4;
        }
    } else {
        if (param2 < 5) {
            return 1;
        } else if (param2 < 11) {
            return 2;
        } else if (param2 < 17) {
            return 3;
        } else if (param2 < 24) {
            return 4;
        } else if (param2 < 39) {
            return 5;
        } else {
            return 6;
        }
    }
}

// MATCHED 100%
s32 func_02019564(UnkStruct_02019360 * param1) {
    return func_02019574(param1) + 1;
}

// MATCHED 100%
s32 func_02019574(UnkStruct_02019360 * param1) {
    return param1->field02;
}

// MATCHED 100%
s32 func_0201957c() {
    return 98;
}

// MATCHED 100%
s32 func_02019584(UnkStruct_02019360 * param1) {
    s32 val1 = func_02019574(param1);
    val1++;

    s32 val2 = func_0201957c();

    if (val2 < val1) {
        val1 = val2;
    }

    return val1 + 1;
}

// MATCHED 100%
s32 func_020195a4(UnkStruct_02019360 * param1) {
    return func_02019618(param1) - (func_02019620(param1) - func_020195d4(param1));
}

// MATCHED 100%
s32 func_020195d4(UnkStruct_02019360 * param1) {
    if (func_02019574(param1) >= func_0201957c()) {
        return 0;
    }

    return data_020526ec[param1->field00][param1->field02].field08;
}

// MATCHED 100%
s32 func_02019618(UnkStruct_02019360 * param1) {
    return param1->field04;
}

// MATCHED 100%
s32 func_02019620(UnkStruct_02019360 * param1) {
    return param1->field08;
}

} // extern "C"
