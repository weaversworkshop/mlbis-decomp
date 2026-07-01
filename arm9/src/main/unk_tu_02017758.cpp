// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include <nitro/card.h>

#include "main/unk_tu_02017758.h"

// --- Struct Definitions --- //

typedef struct {
    u8 field00;
    u8 pad_01[0x3];
    u32 field04;
    u32 field08;
    u32 field0c;
    void (*fn_10)(void);
    void (*fn_14)(void);
} UnkGlobal_02058f6c;

// --- Extern Data --- //

extern UnkGlobal_02058f6c data_02058f6c;

// --- Extern Functions --- //

extern void func_020420b0(CARDBackupType type);

// --- Function Declarations --- //

void func_02017758(u32 param1);
// func_02017844
void func_02017844(void);
// func_0201788c
void func_0201788c(void);
// func_020178c0
void func_020178c0(void);
// func_02017908
void func_02017908(void);
// func_0201793c
// func_020179a4
// func_020179d8
// func_02017a54
// func_02017a88
void func_02017a88(void);
// func_02017ac0
void func_02017ac0(void);
// func_02017b0c
// func_02017b40
// func_02017b50
// func_02017b60
// func_02017ba4

// --- Function Definitions --- //

void func_02017758(u32 param1) {
    if (!CARD_IsAvailable()) {
        CARD_Init();
    }

    if (data_02058f6c.field00 == 0) {
        data_02058f6c.field0c = ~2;

        func_02017a88();

        switch (param1) {
            case 0:
                func_020420b0(CARD_BACKUP_TYPE_EEPROM_4KBITS);
                data_02058f6c.fn_10 = &func_02017844;
                data_02058f6c.fn_14 = &func_0201788c;
                break;
            case 1:
                func_020420b0(CARD_BACKUP_TYPE_EEPROM_64KBITS);
                data_02058f6c.fn_10 = &func_02017844;
                data_02058f6c.fn_14 = &func_0201788c;
                break;
            case 2:
                func_020420b0(CARD_BACKUP_TYPE_FLASH_2MBITS);
                data_02058f6c.fn_10 = &func_020178c0;
                data_02058f6c.fn_14 = &func_02017908;
        }

        func_02017ac0();

        data_02058f6c.field08 = 0;
        data_02058f6c.field04 = 0;
        data_02058f6c.field00 = 1;
    }
}

//     .global func_02017844
//     arm_func_start func_02017844
// func_02017844: ; 0x02017844
//     stmdb sp!, {lr}
//     sub sp, sp, #0x14
//     ldr ip, [sp, #0x18]
//     mov lr, r0
//     str ip, [sp, #0x0]
//     mov r0, #0x1
//     str r0, [sp, #0x4]
//     mov ip, #0x8
//     mov r0, r1
//     str ip, [sp, #0x8]
//     mov ip, #0xa
//     mov r1, lr
//     str ip, [sp, #0xc]
//     mov ip, #0x2
//     str ip, [sp, #0x10]
//     bl func_02041fb4
//     add sp, sp, #0x14
//     ldmia sp!, {pc}
//     arm_func_end func_02017844

//     .global func_0201788c
//     arm_func_start func_0201788c
// func_0201788c: ; 0x0201788c
//     stmdb sp!, {lr}
//     sub sp, sp, #0x14
//     ldr ip, [sp, #0x18]
//     mov lr, #0x1
//     stmia sp, {ip, lr}
//     mov ip, #0x6
//     str ip, [sp, #0x8]
//     str lr, [sp, #0xc]
//     mov ip, #0x0
//     str ip, [sp, #0x10]
//     bl func_02041fb4
//     add sp, sp, #0x14
//     ldmia sp!, {pc}
//     arm_func_end func_0201788c

//     .global func_020178c0
//     arm_func_start func_020178c0
// func_020178c0: ; 0x020178c0
//     stmdb sp!, {lr}
//     sub sp, sp, #0x14
//     ldr ip, [sp, #0x18]
//     mov lr, r0
//     str ip, [sp, #0x0]
//     mov r0, #0x1
//     str r0, [sp, #0x4]
//     mov ip, #0x7
//     mov r0, r1
//     str ip, [sp, #0x8]
//     mov ip, #0xa
//     mov r1, lr
//     str ip, [sp, #0xc]
//     mov ip, #0x2
//     str ip, [sp, #0x10]
//     bl func_02041fb4
//     add sp, sp, #0x14
//     ldmia sp!, {pc}
//     arm_func_end func_020178c0

//     .global func_02017908
//     arm_func_start func_02017908
// func_02017908: ; 0x02017908
//     stmdb sp!, {lr}
//     sub sp, sp, #0x14
//     ldr ip, [sp, #0x18]
//     mov lr, #0x1
//     stmia sp, {ip, lr}
//     mov ip, #0x6
//     str ip, [sp, #0x8]
//     str lr, [sp, #0xc]
//     mov ip, #0x0
//     str ip, [sp, #0x10]
//     bl func_02041fb4
//     add sp, sp, #0x14
//     ldmia sp!, {pc}
//     arm_func_end func_02017908

//     .global func_0201793c
//     arm_func_start func_0201793c
// func_0201793c: ; 0x0201793c
//     stmdb sp!, {r3, r4, r5, r6, lr}
//     sub sp, sp, #0x4
//     mov r6, r0
//     mov r5, r1
//     mov r4, r2
//     bl func_02017a88
//     cmp r0, #0x0
//     addeq sp, sp, #0x4
//     moveq r0, #0x0
//     ldmeqia sp!, {r3, r4, r5, r6, pc}
//     mov r1, #0x0
//     ldr r0, .L_0201799c
//     str r1, [sp, #0x0]
//     ldr ip, [r0, #0x14]
//     ldr r3, .L_020179a0
//     mov r0, r6
//     mov r1, r5
//     mov r2, r4
//     blx ip
//     ldr r1, .L_0201799c
//     mov r0, #0x1
//     str r0, [r1, #0x8]
//     add sp, sp, #0x4
//     ldmia sp!, {r3, r4, r5, r6, pc}
// .L_0201799c: .word data_02058f6c
// .L_020179a0: .word func_020179a4
//     arm_func_end func_0201793c

//     .global func_020179a4
//     arm_func_start func_020179a4
// func_020179a4: ; 0x020179a4
//     stmdb sp!, {r3, lr}
//     bl CARD_GetResultCode
//     ldr r1, .L_020179d4
//     str r0, [r1, #0x4]
//     bl func_02017ac0
//     ldr r0, .L_020179d4
//     ldr r1, [r0, #0x4]
//     cmp r1, #0x0
//     movne r1, #0x2
//     moveq r1, #0x0
//     str r1, [r0, #0x8]
//     ldmia sp!, {r3, pc}
// .L_020179d4: .word data_02058f6c
//     arm_func_end func_020179a4

//     .global func_020179d8
//     arm_func_start func_020179d8
// func_020179d8: ; 0x020179d8
//     stmdb sp!, {r3, r4, r5, r6, r7, lr}
//     mov r6, r0
//     mov r5, r1
//     mov r4, r2
//     mov r7, r3
//     bl func_02017a88
//     cmp r0, #0x0
//     moveq r0, #0x0
//     ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
//     cmp r7, #0x0
//     beq .L_02017a18
//     mov r0, r5
//     mov r1, r4
//     bl func_02017b40
//     add r1, r5, r4
//     strh r0, [r1, #-0x2]
// .L_02017a18:
//     mov r1, #0x0
//     ldr r0, .L_02017a4c
//     str r1, [sp, #0x0]
//     ldr ip, [r0, #0x10]
//     ldr r3, .L_02017a50
//     mov r0, r6
//     mov r1, r5
//     mov r2, r4
//     blx ip
//     ldr r1, .L_02017a4c
//     mov r0, #0x1
//     str r0, [r1, #0x8]
//     ldmia sp!, {r3, r4, r5, r6, r7, pc}
// .L_02017a4c: .word data_02058f6c
// .L_02017a50: .word func_02017a54
//     arm_func_end func_020179d8

//     .global func_02017a54
//     arm_func_start func_02017a54
// func_02017a54: ; 0x02017a54
//     stmdb sp!, {r3, lr}
//     bl CARD_GetResultCode
//     ldr r1, .L_02017a84
//     str r0, [r1, #0x4]
//     bl func_02017ac0
//     ldr r0, .L_02017a84
//     ldr r1, [r0, #0x4]
//     cmp r1, #0x0
//     movne r1, #0x2
//     moveq r1, #0x0
//     str r1, [r0, #0x8]
//     ldmia sp!, {r3, pc}
// .L_02017a84: .word data_02058f6c
//     arm_func_end func_02017a54

//     .global func_02017a88
//     arm_func_start func_02017a88
// func_02017a88: ; 0x02017a88
//     stmdb sp!, {r3, lr}
//     bl func_02038708
//     ldr r2, .L_02017abc
//     mvn r1, #0x2
//     str r0, [r2, #0xc]
//     cmp r0, r1
//     moveq r0, #0x0
//     ldmeqia sp!, {r3, pc}
//     mov r0, r0, lsl #0x10
//     mov r0, r0, lsr #0x10
//     bl CARD_LockBackup
//     mov r0, #0x1
//     ldmia sp!, {r3, pc}
// .L_02017abc: .word data_02058f6c
//     arm_func_end func_02017a88

//     .global func_02017ac0
//     arm_func_start func_02017ac0
// func_02017ac0: ; 0x02017ac0
//     stmdb sp!, {r3, lr}
//     ldr r1, .L_02017b08
//     mvn r0, #0x2
//     ldr r1, [r1, #0xc]
//     cmp r1, r0
//     ldmeqia sp!, {r3, pc}
//     mov r0, r1, lsl #0x10
//     mov r0, r0, lsr #0x10
//     bl CARD_UnlockBackup
//     ldr r0, .L_02017b08
//     ldr r0, [r0, #0xc]
//     mov r0, r0, lsl #0x10
//     mov r0, r0, lsr #0x10
//     bl func_02038770
//     ldr r0, .L_02017b08
//     mvn r1, #0x2
//     str r1, [r0, #0xc]
//     ldmia sp!, {r3, pc}
// .L_02017b08: .word data_02058f6c
//     arm_func_end func_02017ac0

//     .global func_02017b0c
//     arm_func_start func_02017b0c
// func_02017b0c: ; 0x02017b0c
//     stmdb sp!, {r3, lr}
//     bl func_020421e8
//     cmp r0, #0x0
//     beq .L_02017b34
//     ldr r0, .L_02017b3c
//     ldr r0, [r0, #0x8]
//     cmp r0, #0x2
//     movne r0, #0x1
//     moveq r0, #0x0
//     ldmia sp!, {r3, pc}
// .L_02017b34:
//     mov r0, #0x0
//     ldmia sp!, {r3, pc}
// .L_02017b3c: .word data_02058f6c
//     arm_func_end func_02017b0c

//     .global func_02017b40
//     arm_func_start func_02017b40
// func_02017b40: ; 0x02017b40
//     ldr ip, .L_02017b4c
//     sub r1, r1, #0x2
//     bx ip
// .L_02017b4c: .word MATH_CalcChecksum16
//     arm_func_end func_02017b40

//     .global func_02017b50
//     arm_func_start func_02017b50
// func_02017b50: ; 0x02017b50
//     ldr r0, .L_02017b5c
//     ldr r0, [r0, #0x8]
//     bx lr
// .L_02017b5c: .word data_02058f6c
//     arm_func_end func_02017b50

//     .global func_02017b60
//     arm_func_start func_02017b60
// func_02017b60: ; 0x02017b60
//     stmdb sp!, {r3, lr}
//     cmp r0, #0x2
//     movhs r0, #0x0
//     ldmhsia sp!, {r3, pc}
//     ldr r1, .L_02017b98
//     ldr r3, .L_02017b9c
//     mul r1, r0, r1
//     ldr r2, .L_02017ba0
//     add r0, r1, #0x10
//     add r1, r3, r1
//     mov r3, #0x1
//     bl func_020179d8
//     mov r0, #0x1
//     ldmia sp!, {r3, pc}
// .L_02017b98: .word 0xfd8
// .L_02017b9c: .word data_02058f94
// .L_02017ba0: .word 0x5f4
//     arm_func_end func_02017b60

//     .global func_02017ba4
//     arm_func_start func_02017ba4
// func_02017ba4: ; 0x02017ba4
//     stmdb sp!, {r3, lr}
//     cmp r0, #0x2
//     movhs r0, #0x0
//     ldmhsia sp!, {r3, pc}
//     ldr r1, .L_02017be0
//     ldr r3, .L_02017be4
//     mul r1, r0, r1
//     add r0, r1, #0x3fc
//     ldr r2, .L_02017be8
//     add r0, r0, #0x400
//     add r1, r3, r1
//     mov r3, #0x1
//     bl func_020179d8
//     mov r0, #0x1
//     ldmia sp!, {r3, pc}
// .L_02017be0: .word 0xfd8
// .L_02017be4: .word data_02058f94
// .L_02017be8: .word 0x5f4
//     arm_func_end func_02017ba4

//     .global func_02017bec
//     arm_func_start func_02017bec

} // extern "C"
