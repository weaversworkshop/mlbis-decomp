// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/unk_tu_02017758.h"

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

// func_02017758: ; 0x02017758
//     stmdb sp!, {r4, lr}
//     mov r4, r0
//     bl CARD_IsAvailable
//     cmp r0, #0x0
//     bne .L_02017770
//     bl CARD_Init
// .L_02017770:
//     ldr r0, .L_02017824
//     ldrb r1, [r0, #0x0]
//     cmp r1, #0x0
//     ldmneia sp!, {r4, pc}
//     mvn r1, #0x2
//     str r1, [r0, #0xc]
//     bl func_02017a88
//     cmp r4, #0x0
//     beq .L_020177a8
//     cmp r4, #0x1
//     beq .L_020177c8
//     cmp r4, #0x2
//     beq .L_020177e8
//     b .L_02017804
// .L_020177a8:
//     ldr r0, .L_02017828
//     bl func_020420b0
//     ldr r2, .L_0201782c
//     ldr r0, .L_02017824
//     ldr r1, .L_02017830
//     str r2, [r0, #0x10]
//     str r1, [r0, #0x14]
//     b .L_02017804
// .L_020177c8:
//     ldr r0, .L_02017834
//     bl func_020420b0
//     ldr r2, .L_0201782c
//     ldr r0, .L_02017824
//     ldr r1, .L_02017830
//     str r2, [r0, #0x10]
//     str r1, [r0, #0x14]
//     b .L_02017804
// .L_020177e8:
//     ldr r0, .L_02017838
//     bl func_020420b0
//     ldr r2, .L_0201783c
//     ldr r0, .L_02017824
//     ldr r1, .L_02017840
//     str r2, [r0, #0x10]
//     str r1, [r0, #0x14]
// .L_02017804:
//     bl func_02017ac0
//     ldr r0, .L_02017824
//     mov r1, #0x0
//     str r1, [r0, #0x8]
//     str r1, [r0, #0x4]
//     mov r1, #0x1
//     strb r1, [r0, #0x0]
//     ldmia sp!, {r4, pc}
// .L_02017824: .word data_02058f6c
// .L_02017828: .word 0x901
// .L_0201782c: .word func_02017844
// .L_02017830: .word func_0201788c
// .L_02017834: .word 0xd01
// .L_02017838: .word 0x1202
// .L_0201783c: .word func_020178c0
// .L_02017840: .word func_02017908
//     arm_func_end func_02017758



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
