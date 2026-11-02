// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/palAnim.h"

// --- Struct Definitions --- //

typedef s16 * (*PalAnimWorker)(s16 *, s32, u16 *, s32, u16 *);

typedef struct {
    s16 kind;  // +0x0
    s16 period;  // +0x2
    s16 delay;  // +0x4
    s16 duration;  // +0x6
    s16 cycleLen;  // +0x8
    s16 dstIdx;  // +0xa
    s16 count;  // +0xc
    s16 args[];  // +0xe
} PalAnimEntry;

typedef struct {
    s16 list[2];  // +0x0
    s16 kind;  // +0x4
    s16 period;  // +0x6
    s16 delay;  // +0x8
    s16 duration;  // +0xa
    s16 cycleLen;  // +0xc
    s16 dstIdx;  // +0xe
    s16 count;  // +0x10
    s16 args[6];  // +0x12
} PalAnimSimpleInvert;

typedef struct {
    s16 list[2];  // +0x0
    s16 kind;  // +0x4
    s16 period;  // +0x6
    s16 delay;  // +0x8
    s16 duration;  // +0xa
    s16 cycleLen;  // +0xc
    s16 dstIdx;  // +0xe
    s16 count;  // +0x10
    s16 args[8];  // +0x12
} PalAnimSimpleBlend;

// --- Extern Functions --- //

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

// --- Function Declarations --- //

s32 palAnimRun(s16 *, s32, u16 *, s32, u16 *, u32);  // 0x0201d2b4
void palAnimRunOne(s32, s16, u32, s32, u16 *, s32, u16 *, u32);  // 0x0201d448
s16 * palAnimRotate(s16 *, s32, u16 *, s32, u16 *);  // 0x0201d52c
s16 * palAnimRotateBounce(s16 *, s32, u16 *, s32, u16 *);  // 0x0201d5c4
s16 * palAnimKeySeg(s16 *, s32 *, s32 *, s32 *, s32 *);  // 0x0201d6d0
s16 * palAnimCrossFade(s16 *, s32, u16 *, s32, u16 *);  // 0x0201d768
s16 * palAnimBlendLerp(s16 *, s32, u16 *, s32, u16 *);  // 0x0201d824
s16 * palAnimBlendAdd(s16 *, s32, u16 *, s32, u16 *);  // 0x0201d910
s16 * palAnimBlendSub(s16 *, s32, u16 *, s32, u16 *);  // 0x0201d9fc
s16 * palAnimBlendLuminance(s16 *, s32, u16 *, s32, u16 *);  // 0x0201dae8
s16 * palAnimInvert(s16 *, s32, u16 *, s32, u16 *);  // 0x0201dbd4

// --- Variable Definitions --- //

PalAnimSimpleInvert palAnimSimpleInvert = {  // 0x02054d4c
    { 2, -1 },
    8, 60, 0, 60, 60, 0, 256,
    { 0, 1, 31, 60, 60, 0 }
};

PalAnimSimpleBlend palAnimSimpleBlend = {  // 0x02054d6a
    { 2, -1 }, 
    4, 60, 0, 60, 60, 0, 256, 
    { 0, 0, 1, 31, 60, 60, 0, 0 } 
};

PalAnimWorker palAnimWorkers[] = {  // 0x02054d8c
    NULL, palAnimRotate, palAnimRotateBounce, palAnimCrossFade, palAnimBlendLerp, palAnimBlendAdd, palAnimBlendSub, palAnimBlendLuminance, palAnimInvert
};

// --- Function Definitions --- //

// NONMATCHING 98.01% -- callback param5 is stored late
s32 palAnimRun(s16 * list, s32 time, u16 * dstPal, s32 palCount, u16 * srcPal, u32 loop) {
    u16 * pal = dstPal;

    if (pal != NULL && srcPal != NULL && srcPal != pal) {
        if (srcPal < pal) {
            func_01ff8380(srcPal, pal, palCount * sizeof(u16));
        } else {
            func_01ff82ac(srcPal, pal, palCount * sizeof(u16));
        }
    }

    if (list == NULL) {
        return -1;
    }

    s16 * cur = list;
    s32 delay;
    s32 maxPeriod = 0;

    while (TRUE) {
        s16 idx = *cur++;
        if (idx == -1) {
            return maxPeriod;
        }
    
        PalAnimEntry * entry = (PalAnimEntry *)&list[idx];
        s32 absPeriod = list[idx+1];
        if (absPeriod < 0) {
            absPeriod = -absPeriod;
        }
        if (maxPeriod < absPeriod) {
            maxPeriod = absPeriod;
        }

        if (pal != NULL) {
            s32 period = entry->period << 8;
            s32 duration = entry->duration << 8;
            s32 cycleLen = entry->cycleLen << 8;
            s32 kind = entry->kind;
            delay = entry->delay << 8;
            s32 phase = time;
            if (loop == 0) {
                phase = time - delay;
                if (phase < 0) {
                    continue;
                }
                if (duration < phase) {
                    phase = duration;
                }
            } else {
                if (period < 0) {
                    if (-period < time) {
                        phase = duration;
                    }
                } else {
                    s32 rem = time % period;
                    if (rem < 0) {
                        rem += period;
                    }
                    rem -= delay;

                    if (duration <= rem) {
                        phase = cycleLen;
                    } else if (rem < 0) {
                        continue;
                    } else {
                        phase = rem % cycleLen;
                    }
                }
            }
            
            u16 * out = &pal[entry->dstIdx];
            s32 n = entry->count;
            if (palCount - entry->dstIdx < n) {
                n = palCount - entry->dstIdx;
            }

            palAnimWorkers[kind](entry->args, phase, out, n, pal);
        }
    }
}

// NONMATCHING 96.40% -- param8 loaded early from stack before use in function call
void palAnimRunOne(s32 kind, s16 color, u32 dur, s32 time, u16 * dstPal, s32 palCount, u16 * srcPal, u32 loop) {
    BOOL reverse = FALSE;

    if (kind < 0) {
        kind = -kind;
        reverse = TRUE;
    }

    s16 * list;
    if (kind == 8) {
        PalAnimSimpleInvert * rec;
        palAnimSimpleInvert.kind = kind;

        rec = &palAnimSimpleInvert;
        if (reverse == TRUE) {
            rec->args[2] = 0;
            rec->args[5] = 0x1f;
        } else {
            rec->args[2] = 0x1f;
            rec->args[5] = 0;
        }

        rec->period = dur;
        rec->duration = dur;
        rec->cycleLen = dur;
        rec->args[3] = dur;
        rec->args[4] = dur;
        list = rec->list;
    } else {
        PalAnimSimpleBlend * rec;
        palAnimSimpleBlend.kind = kind;
        palAnimSimpleBlend.args[1] = color;

        rec = &palAnimSimpleBlend;
        if (reverse == 1) {
            rec->args[3] = 0;
            rec->args[6] = 0x1f;
        } else {
            rec->args[3] = 0x1f;
            rec->args[6] = 0;
        }

        rec->period = dur;
        rec->duration = dur;
        rec->cycleLen = dur;
        rec->args[4] = dur;
        rec->args[5] = dur;
        list = rec->list;
    }

    palAnimRun(list, time, dstPal, palCount, srcPal, loop);
}

// MATCHED 100%
s16 * palAnimRotate(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s16 len = args[1];
    u16 buf[0x100];

    MIi_CpuCopy16(&pal[args[0]], buf, len * sizeof(s16));

    s32 i = 0;
    while (TRUE) {
        if (i == len) {
            i = 0;
            break;
        }

        time -= (args[i+2] << 8);
        if (time < 0) {
            break;
        }
        i++;
    }

    while (TRUE) {
        if (--count < 0) {
            return &args[2] + len;
        }

        *dst++ = buf[i++];

        if (i == len) {
            i = 0;
        }
    }
}

// MATCHED 100%
s16 * palAnimRotateBounce(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 len = args[1];
    s32 cycle = len*2;
    u16 buf[0x100];

    MIi_CpuCopy16(&pal[args[0]], buf, cycle);

    s32 i = 0;
    while (TRUE) {
        if (i == cycle) {
            i = 0;
            break;
        }

        time -= (args[i+2] << 8);
        if (time < 0) {
            break;
        }

        i++;
    }

    if (len-1 < i) {
        i = cycle-1 - i;
        while (TRUE) {
            if (--count < 0) {
                return &args[2] + cycle;
            }

            *dst++ = buf[i];

            if (i == 0) {
                break;
            }
            i--;
        }
    }

    while (TRUE) {
        while (TRUE) {
            if (--count < 0) {
                return &args[2] + cycle;
            }

            *dst++ = buf[i];

            if (i == len-1) {
                break;
            }
            i++;
        }

        while (TRUE) {
            if (--count < 0) {
                return &args[2] + cycle;
            }

            *dst++ = buf[i];

            if (i == 0) {
                break;
            }
            i--;
        }
    }
}

// MATCHED 100%
s16 * palAnimKeySeg(s16 * keys, s32 * time, s32 * dur, s32 * from, s32 * to) {
    s32 t = *time;
    s16 * last = &keys[keys[0] * 3] - 2;

    keys++;

    while (TRUE) {
        if (t < (keys[1] << 8)) {
            *time = t;
            *dur = (keys[1] << 8);
            *from = keys[0];
            *to = keys[3];
            return last;
        }
            
        t -= (keys[2] << 8);

        if (t < 0 || keys == last) {
            *time = 1;
            *dur = 1;
            *from = keys[0];
            *to = keys[3];
            return last;
        }

        keys += 3;
    }
}

// MATCHED 100%
s16 * palAnimCrossFade(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 dur;
    s32 fromIdx;
    s32 toIdx;
    u16 buf[0x100];

    s16 * next = palAnimKeySeg(args, &time, &dur, &fromIdx, &toIdx);

    if (pal < buf) {
        func_01ff8380(pal, buf, sizeof(buf));
    } else {
        func_01ff82ac(pal, buf, sizeof(buf));
    }

    u16 * a = &buf[fromIdx];
    u16 * b = &buf[toIdx];
    s16 w = (time << 5) / dur;

    while (TRUE) {
        if (--count < 0) {
            return next;
        }

        *dst++ = colorLerp(*a++, *b++, w);
    }
}

// MATCHED 100%
s16 * palAnimBlendLerp(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 dur;
    s32 from;
    s32 to;
    s16 * next;

    u16 * src = &pal[args[0]];
    u16 color = args[1];

    next = palAnimKeySeg(&args[2], &time, &dur, &from, &to);

    s16 w = from + time * (to - from) / dur;

    if ((s32)dst < (s32)src) {
        while (TRUE) {
            if (--count < 0) {
                return next;
            }

            *dst++ = colorLerp(*src, color, w);
            src++;
        }
    }

    while (TRUE) {
        if (--count < 0) {
            return next;
        }

        dst[count] = colorLerp(src[count], color, w);
    }
}

// MATCHED 100%
s16 * palAnimBlendAdd(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 dur;
    s32 from;
    s32 to;
    s16 * next;

    u16 * src = &pal[args[0]];
    u16 color = args[1];

    next = palAnimKeySeg(&args[2], &time, &dur, &from, &to);

    s16 w = from + time * (to - from) / dur;

    if ((s32)dst < (s32)src) {
        while (TRUE) {
            if (--count < 0) {
                return next;
            }

            *dst++ = colorAddSaturation(*src, color, w);
            src++;
        }
    }

    while (TRUE) {
        if (--count < 0) {
            return next;
        }

        dst[count] = colorAddSaturation(src[count], color, w);
    }
}

// MATCHED 100%
s16 * palAnimBlendSub(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 dur;
    s32 from;
    s32 to;
    s16 * next;

    u16 * src = &pal[args[0]];
    u16 color = args[1];

    next = palAnimKeySeg(&args[2], &time, &dur, &from, &to);

    s16 w = from + time * (to - from) / dur;

    if ((s32)dst < (s32)src) {
        while (TRUE) {
            if (--count < 0) {
                return next;
            }

            *dst++ = colorSubSaturation(*src, color, w);
            src++;
        }
    }

    while (TRUE) {
        if (--count < 0) {
            return next;
        }

        dst[count] = colorSubSaturation(src[count], color, w);
    }
}

// MATCHED 100%
s16 * palAnimBlendLuminance(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 dur;
    s32 from;
    s32 to;
    s16 * next;

    u16 * src = &pal[args[0]];
    u16 color = args[1];

    next = palAnimKeySeg(&args[2], &time, &dur, &from, &to);

    s16 w = from + time * (to - from) / dur;

    if ((s32)dst < (s32)src) {
        while (TRUE) {
            if (--count < 0) {
                return next;
            }

            *dst++ = colorLuminanceBlend(*src, color, w);
            src++;
        }
    }

    while (TRUE) {
        if (--count < 0) {
            return next;
        }

        dst[count] = colorLuminanceBlend(src[count], color, w);
    }
}

// MATCHED 100%
s16 * palAnimInvert(s16 * args, s32 time, u16 * dst, s32 count, u16 * pal) {
    s32 dur;
    s32 from;
    s32 to;
    s16 * next;

    u16 * src = &pal[*args++];

    next = palAnimKeySeg(args, &time, &dur, &from, &to);

    s16 w = from + time * (to - from) / dur;

    if ((s32)dst < (s32)src) {
        while (TRUE) {
            if (--count < 0) {
                return next;
            }

            *dst++ = colorInvertLerp(*src, w);
            src++;
        }
    }

    while (TRUE) {
        if (--count < 0) {
            return next;
        }

        dst[count] = colorInvertLerp(src[count], w);
    }
}

#ifndef IDE_PARSE
// MATCHED 100%
asm u16 colorLerp(u16 a, u16 b, s32 w) {
    ldr r12, =0x1f07c1f
    orr r1, r1, r1, lsl #0xf
    and r1, r1, r12
    mul r1, r2, r1
    and r1, r12, r1, lsr #0x5
    rsb r2, r2, #0x20
    orr r0, r0, r0, lsl #0xf
    and r0, r0, r12
    mul r0, r2, r0
    and r0, r12, r0, lsr #0x5
    add r0, r0, r1
    orr r0, r0, r0, lsl #0xf
    mov r0, r0, lsr #0xf
    bx lr
}

// MATCHED 100%
asm u16 colorAddSaturation(u16 a, u16 b, s32 w) {
    ldr r12, =0x1f07c1f
    orr r1, r1, r1, lsl #0xf
    and r1, r1, r12
    mul r1, r2, r1
    and r1, r12, r1, lsr #0x5
    orr r0, r0, r0, lsl #0xf
    and r0, r0, r12
    add r0, r0, r1
    bic r1, r0, r12
    sub r1, r1, r1, lsr #0x5
    orr r0, r0, r1
    and r0, r0, r12
    orr r0, r0, r0, lsl #0xf
    mov r0, r0, lsr #0xf
    bx lr
}

// MATCHED 100%
asm u16 colorSubSaturation(u16 a, u16 b, s32 w) {
    ldr r12, =0x1f07c1f
    orr r1, r1, r1, lsl #0xf
    and r1, r1, r12
    mul r1, r2, r1
    and r1, r12, r1, lsr #0x5
    ldr r3, =0x2008020
    sub r1, r3, r1
    orr r0, r0, r0, lsl #0xf
    and r0, r0, r12
    add r0, r0, r1
    bic r1, r0, r12
    sub r1, r1, r1, lsr #0x5
    and r0, r0, r1
    orr r0, r0, r0, lsl #0xf
    mov r0, r0, lsr #0xf
    bx lr
}

// MATCHED 100%
asm u16 colorLuminanceBlend(u16 a, u16 b, s32 w) {
    ldr r12, =0x1f07c1f
    orr r0, r0, r0, lsl #0xf
    and r0, r0, r12
    add r3, r0, r0, lsr #0x14
    add r3, r3, r3, lsr #0xa
    mov r3, r3, lsr #0x1
    tst r3, #0x20
    orrne r3, r3, #0x1f
    and r3, r3, #0x1f
    orr r1, r1, r1, lsl #0xf
    and r1, r1, r12
    mul r1, r3, r1
    and r1, r12, r1, lsr #0x5
    mul r1, r2, r1
    and r1, r12, r1, lsr #0x5
    rsb r2, r2, #0x20
    mul r0, r2, r0
    and r0, r12, r0, lsr #0x5
    add r0, r0, r1
    orr r0, r0, r0, lsl #0xf
    mov r0, r0, lsr #0xf
    bx lr
}

// MATCHED 100%
asm u16 colorInvertLerp(u16 a, s32 w) {
    ldr r12, =0x1f07c1f
    orr r0, r0, r0, lsl #0xf
    and r0, r0, r12
    sub r3, r12, r0
    mul r3, r1, r3
    and r3, r12, r3, lsr #0x5
    rsb r1, r1, #0x20
    mul r0, r1, r0
    and r0, r12, r0, lsr #0x5
    add r0, r0, r3
    orr r0, r0, r0, lsl #0xf
    mov r0, r0, lsr #0xf
    bx lr
}
#endif
