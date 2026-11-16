// objdiff-target: build/delinks/_dsd_gap@itcm_1.o

#include "itcm/cpu_mem.h"

// --- Function Definitions --- //

#ifndef IDE_PARSE
// MATCHED 100%
asm void cpu_fill32(void * dst, u32 value, u32 size) {
    cmp r2, #0x0
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    mov r3, r1
    mov r4, r1
    mov r5, r1
    mov r6, r1
    mov r7, r1
    mov r8, r1
    mov r9, r1
    mov r10, r1
    mov r11, r1
    mov ip, r1
    mov lr, r1
@fillBlock:
    subs r2, r2, #0x30
    stmgtia r0!, {r1, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    bgt @fillBlock
    sub pc, pc, r2
    sub pc, pc, #0x8
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    str r1, [r0], #0x4
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void cpu_copy32_fwd(void * src, void * dst, u32 size) {
    cmp r2, #0x0
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
@copyBlock:
    subs r2, r2, #0x2c
    ldmgeia r0!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    stmgeia r1!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    bgt @copyBlock
    sub pc, pc, r2, lsl #0x2
    sub pc, pc, #0x8
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    sub pc, pc, #0x8
    sub pc, pc, #0x8
    ldmia r0!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip}
    stmia r1!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5, r6, r7, r8, r9, r10, r11}
    stmia r1!, {r3, r4, r5, r6, r7, r8, r9, r10, r11}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5, r6, r7, r8, r9, r10}
    stmia r1!, {r3, r4, r5, r6, r7, r8, r9, r10}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5, r6, r7, r8, r9}
    stmia r1!, {r3, r4, r5, r6, r7, r8, r9}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5, r6, r7, r8}
    stmia r1!, {r3, r4, r5, r6, r7, r8}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5, r6, r7}
    stmia r1!, {r3, r4, r5, r6, r7}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5, r6}
    stmia r1!, {r3, r4, r5, r6}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4, r5}
    stmia r1!, {r3, r4, r5}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmia r0!, {r3, r4}
    stmia r1!, {r3, r4}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldr r3, [r0, #0x0]
    str r3, [r1, #0x0]
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void cpu_copy32_rev(void * src, void * dst, u32 size) {
    cmp r2, #0x0
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    add r0, r0, r2
    add r1, r1, r2
@copyBlock:
    subs r2, r2, #0x2c
    ldmgedb r0!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    stmgedb r1!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    bgt @copyBlock
    sub pc, pc, r2, lsl #0x2
    sub pc, pc, #0x8
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    sub pc, pc, #0x8
    sub pc, pc, #0x8
    ldmdb r0!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip}
    stmdb r1!, {r3, r4, r5, r6, r7, r8, r9, r10, r11, ip}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5, r6, r7, r8, r9, r10, r11}
    stmdb r1!, {r3, r4, r5, r6, r7, r8, r9, r10, r11}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5, r6, r7, r8, r9, r10}
    stmdb r1!, {r3, r4, r5, r6, r7, r8, r9, r10}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5, r6, r7, r8, r9}
    stmdb r1!, {r3, r4, r5, r6, r7, r8, r9}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5, r6, r7, r8}
    stmdb r1!, {r3, r4, r5, r6, r7, r8}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5, r6, r7}
    stmdb r1!, {r3, r4, r5, r6, r7}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5, r6}
    stmdb r1!, {r3, r4, r5, r6}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4, r5}
    stmdb r1!, {r3, r4, r5}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3, r4}
    stmdb r1!, {r3, r4}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
    ldmdb r0!, {r3}
    stmdb r1!, {r3}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void cpu_copy16_fwd(u16 * src, u16 * dst, u32 size) {
    mov ip, pc
    b @nextBlock
@copyBlock:
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
    ldrh r3, [r0], #0x2
    strh r3, [r1], #0x2
@nextBlock:
    subs r2, r2, #0x20
    bpl @copyBlock
    adds r3, r2, #0x20
    bxle lr
    rsb r3, r3, #0x20
    add pc, ip, r3, lsl #0x2
}

// MATCHED 100%
asm void cpu_copy16_rev(u16 * src, u16 * dst, u32 size) {
    add r0, r0, r2
    add r1, r1, r2
    mov ip, pc
    b @nextBlock
@copyBlock:
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
    ldrh r3, [r0, #-0x2]!
    strh r3, [r1, #-0x2]!
@nextBlock:
    subs r2, r2, #0x20
    bpl @copyBlock
    adds r3, r2, #0x20
    bxle lr
    rsb r3, r3, #0x20
    add pc, ip, r3, lsl #0x2
}
#endif
