// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/copyRect.h"

// --- Extern Functions --- //

extern void func_01ff82ac(u16 * param1, u16 * param2, u32 param3);
extern void func_01ff8380(u16 * param1, u16 * param2, u32 param3);

// --- Function Definitions --- //

// MATCHED 100%
asm void copy_rect_blk128(void * src, void * dst, s32 rows, s32 srcStride, s32 dstStride) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11}
    ldr r4, [sp, #0x20]
    sub r3, r3, #0x80
    sub r4, r4, #0x80
@loop:
    ldmneia r0!, {r5, r6, r7, r8, r9, r10, r11, ip}
    stmneia r1!, {r5, r6, r7, r8, r9, r10, r11, ip}
    ldmneia r0!, {r5, r6, r7, r8, r9, r10, r11, ip}
    stmneia r1!, {r5, r6, r7, r8, r9, r10, r11, ip}
    ldmneia r0!, {r5, r6, r7, r8, r9, r10, r11, ip}
    stmneia r1!, {r5, r6, r7, r8, r9, r10, r11, ip}
    ldmneia r0!, {r5, r6, r7, r8, r9, r10, r11, ip}
    stmneia r1!, {r5, r6, r7, r8, r9, r10, r11, ip}
    add r0, r0, r3
    add r1, r1, r4
    subs r2, r2, #0x1
    bgt @loop
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11}
    bx lr
}

// MATCHED 100%
asm void copy_rect_blk64(void * src, void * dst, s32 rows, s32 srcStride, s32 dstStride) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ldr r4, [sp, #0x24]
    sub r3, r3, #0x40
    sub r4, r4, #0x40
@loop:
    ldmneia r0!, {r6, r7, r8, r9, r10, r11, ip, lr}
    stmneia r1!, {r6, r7, r8, r9, r10, r11, ip, lr}
    ldmneia r0!, {r6, r7, r8, r9, r10, r11, ip, lr}
    stmneia r1!, {r6, r7, r8, r9, r10, r11, ip, lr}
    add r0, r0, r3
    add r1, r1, r4
    subs r2, r2, #0x1
    bgt @loop
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void copy_rect_rem(void * src, void * dst, s32 rows, s32 srcStride, s32 dstStride) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ldr r4, [sp, #0x24]
    ldr r5, [sp, #0x28]
    sub r4, r4, r3
    sub r5, r5, r3
@loop:
    tst r3, #0x20
    ldmneia r0!, {r6, r7, r8, r9, r10, r11, ip, lr}
    stmneia r1!, {r6, r7, r8, r9, r10, r11, ip, lr}
    tst r3, #0x10
    ldmneia r0!, {r6, r7, r8, r9}
    stmneia r1!, {r6, r7, r8, r9}
    tst r3, #0x8
    ldmneia r0!, {r6, r7}
    stmneia r1!, {r6, r7}
    tst r3, #0x4
    ldmneia r0!, {r6}
    stmneia r1!, {r6}
    add r0, r0, r4
    add r1, r1, r5
    subs r2, r2, #0x1
    bgt @loop
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}

// MATCHED 100%
asm void copy_rect(void * src, void * dst, s32 rows, s32 width, s32 srcStride, s32 dstStride) {
    stmdb sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
    sub sp, sp, #0x8
    mov r7, r2
    mov r6, r3
    cmp r7, #0x0
    ldr r5, [sp, #0x28]
    ldr r4, [sp, #0x2c]
    cmpgt r6, #0x0
    mov r9, r0
    mov r8, r1
    addle sp, sp, #0x8
    ldmleia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
    cmp r7, #0x1
    beq @contiguous
    cmp r6, r5
    cmpeq r5, r4
    bne @strided
@contiguous:
    mul r2, r7, r6
    cmp r9, r8
    mov r0, r9
    bhs @copy_forward
    mov r1, r8
    bl func_01ff8380
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
@copy_forward:
    mov r1, r8
    bl func_01ff82ac
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
@strided:
    cmp r6, #0x80
    blt @blk64
@blk128:
    mov r0, r9
    mov r1, r8
    mov r2, r7
    mov r3, r5
    str r4, [sp, #0x0]
    bl copy_rect_blk128
    sub r6, r6, #0x80
    cmp r6, #0x80
    add r9, r9, #0x80
    add r8, r8, #0x80
    bge @blk128
@blk64:
    cmp r6, #0x40
    blt @rem
    mov r0, r9
    mov r1, r8
    mov r2, r7
    mov r3, r5
    str r4, [sp, #0x0]
    bl copy_rect_blk64
    add r9, r9, #0x40
    add r8, r8, #0x40
    sub r6, r6, #0x40
@rem:
    str r5, [sp, #0x0]
    mov r0, r9
    mov r1, r8
    mov r2, r7
    mov r3, r6
    str r4, [sp, #0x4]
    bl copy_rect_rem
    add sp, sp, #0x8
    ldmia sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
}
