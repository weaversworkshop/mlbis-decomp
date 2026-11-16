// objdiff-target: build/delinks/_dsd_gap@itcm_1.o

#include "itcm/uncomp.h"

// --- Function Definitions --- //

#ifndef IDE_PARSE
// MATCHED 100%
asm void uncompReadHeader(u8 ** src, u32 * recordCount, u32 * totalSize) {
    stmdb sp!, {r4, r5, r6, r7}
    ldr r3, [r0, #0x0]
    mov r5, #0x3
    ldrb r6, [r3], #0x1
    cmp r2, #0x0
    andeq r6, r5, r6, lsr #0x6
    addeq r3, r3, r6
    beq @readRecordCount
    ands r7, r5, r6, lsr #0x6
    and r4, r6, #0x3f
    beq @storeTotalSize
    ldrb r6, [r3], #0x1
    orr r4, r4, r6, lsl #0x6
    subs r7, r7, #0x1
    beq @storeTotalSize
    ldrb r6, [r3], #0x1
    orr r4, r4, r6, lsl #0xc
    subs r7, r7, #0x1
    beq @storeTotalSize
    ldrb r6, [r3], #0x1
    orr r4, r4, r6, lsl #0x12
@storeTotalSize
    str r4, [r2, #0x0]
@readRecordCount
    ldrb r6, [r3], #0x1
    ands r7, r5, r6, lsr #0x6
    and r4, r6, #0x3f
    beq @storeRecordCount
    ldrb r6, [r3], #0x1
    orr r4, r4, r6, lsl #0x6
    subs r7, r7, #0x1
    beq @storeRecordCount
    ldrb r6, [r3], #0x1
    orr r4, r4, r6, lsl #0xc
    subs r7, r7, #0x1
    beq @storeRecordCount
    ldrb r6, [r3], #0x1
    orr r4, r4, r6, lsl #0x12
@storeRecordCount
    add r4, r4, #0x1
    str r4, [r1, #0x0]
    str r3, [r0, #0x0]
    ldmia sp!, {r4, r5, r6, r7}
    bx lr
}

// MATCHED 100%
asm void uncompRecord8(u8 ** src, void ** dst) {
    stmdb sp!, {r4, r5, r6, r7, r8}
    ldr r7, [r0, #0x0]
    ldr r8, [r1, #0x0]
    mov r2, #0x0
    mov r5, #0xc
    ldr r6, =@table
@dispatch:
    mov r2, r2, lsr #0x2
    tst r2, #0x100
    ldreqb ip, [r7], #0x1
    orreq r2, ip, #0xff00
    and r3, r5, r2, lsl #0x2
    ldr pc, [r6, r3]
@storeRecordCount:
    str r7, [r0, #0x0]
    str r8, [r1, #0x0]
    ldmia sp!, {r4, r5, r6, r7, r8}
    bx lr
@literal:
    ldrb ip, [r7], #0x1
    strb ip, [r8], #0x1
    b @dispatch
@copy:
    ldrb r3, [r7], #0x1
    ldrb r4, [r7], #0x1
    and ip, r4, #0xf0
    orr r3, r3, ip, lsl #0x4
    sub ip, r4, ip
    add r4, ip, #0x2
    sub r3, r8, r3
@copy_loop:
    ldrb ip, [r3], #0x1
    strb ip, [r8], #0x1
    subs r4, r4, #0x1
    bne @copy_loop
    b @dispatch
@fill:
    ldrb r3, [r7], #0x1
    ldrb ip, [r7], #0x1
    add r3, r3, #0x2
@fill_loop:
    strb ip, [r8], #0x1
    subs r3, r3, #0x1
    bne @fill_loop
    b @dispatch
@table:
    dcd @storeRecordCount
    dcd @literal
    dcd @copy
    dcd @fill
}

// MATCHED 100%
asm void uncompRecord16(u8 ** src, void ** dst) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10}
    ldr r7, [r0, #0x0]
    ldr r8, [r1, #0x0]
    mov r2, #0x0
    mov r5, #0xc
    mov r9, #0x0
    ldr r6, =@table
@dispatch:
    mov r2, r2, lsr #0x2
    tst r2, #0x100
    ldreqb ip, [r7], #0x1
    orreq r2, ip, #0xff00
    and r3, r5, r2, lsl #0x2
    ldr pc, [r6, r3]
@storeRecordCount:
    str r7, [r0, #0x0]
    str r8, [r1, #0x0]
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10}
    bx lr
@literal:
    ldrb ip, [r7], #0x1
    eors r9, r9, #0x1
    movne r10, ip
    orreq r10, r10, ip, lsl #0x8
    streqh r10, [r8], #0x2
    b @dispatch
@copy:
    ldrb r3, [r7], #0x1
    ldrb r4, [r7], #0x1
    and ip, r4, #0xf0
    orr r3, r3, ip, lsl #0x4
    sub ip, r4, ip
    add r4, ip, #0x2
    add ip, r8, r9
    sub r3, ip, r3
@copy_loop:
    ldrb ip, [r3], #0x1
    eors r9, r9, #0x1
    movne r10, ip
    orreq r10, r10, ip, lsl #0x8
    streqh r10, [r8], #0x2
    subs r4, r4, #0x1
    bne @copy_loop
    b @dispatch
@fill:
    ldrb r3, [r7], #0x1
    ldrb r4, [r7], #0x1
    add r3, r3, #0x2
@fill_loop:
    eors r9, r9, #0x1
    movne r10, r4
    orreq r10, r10, r4, lsl #0x8
    streqh r10, [r8], #0x2
    subs r3, r3, #0x1
    bne @fill_loop
    b @dispatch
@table:
    dcd @storeRecordCount
    dcd @literal
    dcd @copy
    dcd @fill
}

#endif
