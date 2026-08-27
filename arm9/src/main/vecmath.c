// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/vecmath.h"

// --- Function Definitions --- //
#ifndef IDE_PARSE

// MATCHED 100%
asm s32 idiv(register s32 dividend, register s32 divisor) {
    cmp r1, #0x0
    bxeq lr
    mov ip, #0x1
    rsbmi ip, ip, #0x0
    rsbmi r1, r1, #0x0
    movs r2, r0
    rsbmi ip, ip, #0x0
    rsbmi r2, r2, #0x0
    clz r0, r1
    clz r3, r2
    subs r3, r0, r3
    mov r0, #0x0
    bxmi lr
    add r3, r3, r3, lsl #0x1
    rsb r3, r3, #0x5c
    add pc, pc, r3, lsl #0x2
    cmp r1, r2, lsr #0x1f
    suble r2, r2, r1, lsl #0x1f
    addle r0, r0, ip, lsl #0x1f
    cmp r1, r2, lsr #0x1e
    suble r2, r2, r1, lsl #0x1e
    addle r0, r0, ip, lsl #0x1e
    cmp r1, r2, lsr #0x1d
    suble r2, r2, r1, lsl #0x1d
    addle r0, r0, ip, lsl #0x1d
    cmp r1, r2, lsr #0x1c
    suble r2, r2, r1, lsl #0x1c
    addle r0, r0, ip, lsl #0x1c
    cmp r1, r2, lsr #0x1b
    suble r2, r2, r1, lsl #0x1b
    addle r0, r0, ip, lsl #0x1b
    cmp r1, r2, lsr #0x1a
    suble r2, r2, r1, lsl #0x1a
    addle r0, r0, ip, lsl #0x1a
    cmp r1, r2, lsr #0x19
    suble r2, r2, r1, lsl #0x19
    addle r0, r0, ip, lsl #0x19
    cmp r1, r2, lsr #0x18
    suble r2, r2, r1, lsl #0x18
    addle r0, r0, ip, lsl #0x18
    cmp r1, r2, lsr #0x17
    suble r2, r2, r1, lsl #0x17
    addle r0, r0, ip, lsl #0x17
    cmp r1, r2, lsr #0x16
    suble r2, r2, r1, lsl #0x16
    addle r0, r0, ip, lsl #0x16
    cmp r1, r2, lsr #0x15
    suble r2, r2, r1, lsl #0x15
    addle r0, r0, ip, lsl #0x15
    cmp r1, r2, lsr #0x14
    suble r2, r2, r1, lsl #0x14
    addle r0, r0, ip, lsl #0x14
    cmp r1, r2, lsr #0x13
    suble r2, r2, r1, lsl #0x13
    addle r0, r0, ip, lsl #0x13
    cmp r1, r2, lsr #0x12
    suble r2, r2, r1, lsl #0x12
    addle r0, r0, ip, lsl #0x12
    cmp r1, r2, lsr #0x11
    suble r2, r2, r1, lsl #0x11
    addle r0, r0, ip, lsl #0x11
    cmp r1, r2, lsr #0x10
    suble r2, r2, r1, lsl #0x10
    addle r0, r0, ip, lsl #0x10
    cmp r1, r2, lsr #0xf
    suble r2, r2, r1, lsl #0xf
    addle r0, r0, ip, lsl #0xf
    cmp r1, r2, lsr #0xe
    suble r2, r2, r1, lsl #0xe
    addle r0, r0, ip, lsl #0xe
    cmp r1, r2, lsr #0xd
    suble r2, r2, r1, lsl #0xd
    addle r0, r0, ip, lsl #0xd
    cmp r1, r2, lsr #0xc
    suble r2, r2, r1, lsl #0xc
    addle r0, r0, ip, lsl #0xc
    cmp r1, r2, lsr #0xb
    suble r2, r2, r1, lsl #0xb
    addle r0, r0, ip, lsl #0xb
    cmp r1, r2, lsr #0xa
    suble r2, r2, r1, lsl #0xa
    addle r0, r0, ip, lsl #0xa
    cmp r1, r2, lsr #0x9
    suble r2, r2, r1, lsl #0x9
    addle r0, r0, ip, lsl #0x9
    cmp r1, r2, lsr #0x8
    suble r2, r2, r1, lsl #0x8
    addle r0, r0, ip, lsl #0x8
    cmp r1, r2, lsr #0x7
    suble r2, r2, r1, lsl #0x7
    addle r0, r0, ip, lsl #0x7
    cmp r1, r2, lsr #0x6
    suble r2, r2, r1, lsl #0x6
    addle r0, r0, ip, lsl #0x6
    cmp r1, r2, lsr #0x5
    suble r2, r2, r1, lsl #0x5
    addle r0, r0, ip, lsl #0x5
    cmp r1, r2, lsr #0x4
    suble r2, r2, r1, lsl #0x4
    addle r0, r0, ip, lsl #0x4
    cmp r1, r2, lsr #0x3
    suble r2, r2, r1, lsl #0x3
    addle r0, r0, ip, lsl #0x3
    cmp r1, r2, lsr #0x2
    suble r2, r2, r1, lsl #0x2
    addle r0, r0, ip, lsl #0x2
    cmp r1, r2, lsr #0x1
    suble r2, r2, r1, lsl #0x1
    addle r0, r0, ip, lsl #0x1
    cmp r1, r2
    addle r0, r0, ip
    bx lr
}

// MATCHED 100%
asm u32 isqrt(register u32 val) {
    cmp r0, #0x80000000
    addeq r0, r0, #0x1
    rsbs r1, r0, #0x0
    bgt @start
    clz r3, r0
    mov r3, r3, lsr #0x1
    add r3, r3, r3, lsl #0x1
    movle r0, #0x0
    add pc, pc, r3, lsl #0x2
@start:
    movgt r0, #0x8000
    add r2, r0, #0x8000
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x4000
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x2000
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x1000
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x800
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x400
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x200
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x100
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x80
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x40
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x20
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x10
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x8
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x4
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x2
    mlas r3, r2, r2, r1
    movle r0, r2
    add r2, r0, #0x1
    mlas r3, r2, r2, r1
    movle r0, r2
    bx lr
}

// MATCHED 100%
asm u32 func_0201ed3c() {
    ldr r0, =0
    bx lr
}

// MATCHED 100%
asm void vec_cross_product(VecFx32 * outVec, VecFx32 * vecA, VecFx32 * vecB) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ldr r3, [r1, #0x4]
    ldr r4, [r2, #0x8]
    ldr r5, [r2, #0x4]
    ldr r6, [r1, #0x8]
    rsb r5, r5, #0x0
    smull r7, r8, r3, r4
    smlal r7, r8, r5, r6
    ldr r3, [r1, #0x8]
    ldr r4, [r2, #0x0]
    ldr r5, [r2, #0x8]
    ldr r6, [r1, #0x0]
    rsb r5, r5, #0x0
    smull r9, r10, r3, r4
    smlal r9, r10, r5, r6
    ldr r3, [r1, #0x0]
    ldr r4, [r2, #0x4]
    ldr r5, [r2, #0x0]
    ldr r6, [r1, #0x4]
    rsb r5, r5, #0x0
    smull r11, ip, r3, r4
    smlal r11, ip, r5, r6
    cmp r8, #0x0
    mvnmi r8, r8
    clz r4, r8
    mvnmi r8, r8
    cmp r10, #0x0
    mvnmi r10, r10
    clz r5, r10
    mvnmi r10, r10
    cmp r5, r4
    movmi r4, r5
    cmp ip, #0x0
    mvnmi ip, ip
    clz r5, ip
    mvnmi ip, ip
    cmp r5, r4
    movmi r4, r5
    cmp r4, #0x20
    beq @return
    sub r4, r4, #0x1
    mov r8, r8, lsl r4
    mov r10, r10, lsl r4
    mov ip, ip, lsl r4
    rsb r4, r4, #0x20
    orr r7, r8, r7, lsr r4
    orr r9, r10, r9, lsr r4
    orr r11, ip, r11, lsr r4
@return:
    stmia r0, {r7, r9, r11}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm fx32 vector_magnitude(VecFx32 * vec) {
    stmdb sp!, {r4, lr}
    ldmia r0, {r1, r2, r3}
    smull r0, ip, r1, r1
    smull r1, r4, r2, r2
    adds r0, r0, r1
    adc ip, ip, r4
    smull r1, r4, r3, r3
    adds r0, r0, r1
    adc ip, ip, r4
    clz r4, ip
    tst r4, #0x1
    subne r4, r4, #0x1
    mov ip, ip, lsl r4
    rsb r4, r4, #0x20
    orr r0, ip, r0, lsr r4
    bl isqrt
    mov r4, r4, lsr #0x1
    mov r0, r0, lsl r4
    ldmia sp!, {r4, lr}
    bx lr
}

// MATCHED 100%
asm s32 mul_div(s32 a, s32 b, s32 c) {
    smull r3, ip, r0, r1
    cmp ip, #0x0
    mvnmi ip, ip
    clz r0, ip
    mvnmi ip, ip
    cmp r0, #0x20
    beq @overflow
    sub r0, r0, #0x1
    mov ip, ip, lsl r0
    rsb r0, r0, #0x20
    movs r1, r2, lsr r0
    beq @return
    orr r0, ip, r3, lsr r0
    stmdb sp!, {lr}
    bl idiv
    ldmia sp!, {lr}
    bx lr
@overflow:
    cmp ip, #0x0
    mvnmi r3, r3
    clz r0, r3
    mvnmi r3, r3
    clz ip, r2
    cmp ip, r0
    bmi @return
    sub r0, r0, #0x1
    mov r1, r2, lsl r0
    mov r0, r3, lsl r0
    stmdb sp!, {lr}
    bl idiv
    ldmia sp!, {lr}
    bx lr
@return:
    mov r0, #0x0
    bx lr
}

#endif
