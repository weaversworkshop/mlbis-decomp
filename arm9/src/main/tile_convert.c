// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/tile_convert.h"

// --- Function Definitions --- //

// MATCHED 100%
asm void LinearToTiles4bpp(register u32 * src, register u32 * dst, register u32 lineBytes, register u32 lineCount) {
    bics r2, r2, #0x3
    mulgts r3, r2, r3
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    add r3, r0, r3
    rsb r5, r2, r2, lsl #0x3
@loop1:
    add r4, r0, r2
@loop2:
    ldr r6, [r0, #0x0]
    add r0, r0, r2
    ldr r7, [r0, #0x0]
    add r0, r0, r2
    ldr r8, [r0, #0x0]
    add r0, r0, r2
    ldr r9, [r0, #0x0]
    add r0, r0, r2
    ldr r10, [r0, #0x0]
    add r0, r0, r2
    ldr r11, [r0, #0x0]
    add r0, r0, r2
    ldr ip, [r0, #0x0]
    add r0, r0, r2
    ldr lr, [r0], #0x4
    stmia r1!, {r6, r7, r8, r9, r10, r11, ip, lr}
    sub r0, r0, r5
    cmp r0, r4
    bne @loop2
    add r0, r0, r5
    cmp r0, r3
    bne @loop1
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void LinearToTiles8bpp(register u32 * src, register u32 * dst, register u32 lineBytes, register u32 lineCount) {
    bics r2, r2, #0x7
    mulgts r3, r2, r3
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    add r3, r0, r3
    rsb r5, r2, r2, lsl #0x3
@loop1:
    add r4, r0, r2
@loop2:
    ldmia r0, {r6, r7}
    add r0, r0, r2
    ldmia r0, {r8, r9}
    add r0, r0, r2
    ldmia r0, {r10, r11}
    add r0, r0, r2
    ldmia r0, {ip, lr}
    add r0, r0, r2
    stmia r1!, {r6, r7, r8, r9, r10, r11, ip, lr}
    ldmia r0, {r6, r7}
    add r0, r0, r2
    ldmia r0, {r8, r9}
    add r0, r0, r2
    ldmia r0, {r10, r11}
    add r0, r0, r2
    ldmia r0!, {ip, lr}
    stmia r1!, {r6, r7, r8, r9, r10, r11, ip, lr}
    sub r0, r0, r5
    cmp r0, r4
    bne @loop2
    add r0, r0, r5
    cmp r0, r3
    bne @loop1
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void TilesToLinear4bpp(register u32 * src, register u32 * dst, register u32 lineBytes, register u32 lineCount) {
    bics r2, r2, #0x3
    mulgts r3, r2, r3
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    add r3, r1, r3
    rsb r5, r2, r2, lsl #0x3
@loop1:
    add r4, r1, r2
@loop2:
    ldmia r0!, {r6, r7, r8, r9, r10, r11, ip, lr}
    str r6, [r1, #0x0]
    add r1, r1, r2
    str r7, [r1, #0x0]
    add r1, r1, r2
    str r8, [r1, #0x0]
    add r1, r1, r2
    str r9, [r1, #0x0]
    add r1, r1, r2
    str r10, [r1, #0x0]
    add r1, r1, r2
    str r11, [r1, #0x0]
    add r1, r1, r2
    str ip, [r1, #0x0]
    add r1, r1, r2
    str lr, [r1], #0x4
    sub r1, r1, r5
    cmp r1, r4
    bne @loop2
    add r1, r1, r5
    cmp r1, r3
    bne @loop1
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void TilesToLinear8bpp(register u32 * src, register u32 * dst, register u32 lineBytes, register u32 lineCount) {
    bics r2, r2, #0x7
    mulgts r3, r2, r3
    bxle lr
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    add r3, r1, r3
    rsb r5, r2, r2, lsl #0x3
@loop1:
    add r4, r1, r2
@loop2:
    ldmia r0!, {r6, r7, r8, r9, r10, r11, ip, lr}
    stmia r1, {r6, r7}
    add r1, r1, r2
    stmia r1, {r8, r9}
    add r1, r1, r2
    stmia r1, {r10, r11}
    add r1, r1, r2
    stmia r1, {ip, lr}
    add r1, r1, r2
    ldmia r0!, {r6, r7, r8, r9, r10, r11, ip, lr}
    stmia r1, {r6, r7}
    add r1, r1, r2
    stmia r1, {r8, r9}
    add r1, r1, r2
    stmia r1, {r10, r11}
    add r1, r1, r2
    stmia r1!, {ip, lr}
    sub r1, r1, r5
    cmp r1, r4
    bne @loop2
    add r1, r1, r5
    cmp r1, r3
    bne @loop1
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}
