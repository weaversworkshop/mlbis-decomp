#ifndef MAIN_SYSTEM_H
#define MAIN_SYSTEM_H

#include "types.h"

#include <nitro.h>

typedef struct {
    u16 hold;  // +0x0
    u16 press;  // +0x2
    u16 release;  // +0x4
    u16 repeat;  // +0x6
    u8 repeatDelay;  // +0x8
    u8 repeatRate;  // +0x9
    u8 repeatTimers[10];  // +0xa
} InputState;

typedef struct {
    u8 x;  // +0x0
    u8 y;  // +0x1
    u8 prevX;  // +0x2
    u8 prevY;  // +0x3
    union {
        struct {
            u8 active : 1;  // +0x4
            u8 hold : 1;
            u8 trigger : 1;
            u8 press : 1;
            u8 release : 1;
            u8 sampling : 1;
            u8 : 2;
        };
        u8 flags;  // +0x4
    };
    u8 pad_05[0x3];
} TouchpadStruct;

typedef struct {
    u16 pad_00_00 : 8;  // +0x0
    u16 syncPending : 1;
    s16 lastSyncVCount;  // +0x2
    u16 syncVCount;  // +0x4
    s16 frameScanlines;  // +0x6
    s16 prevFrameSkips;  // +0x8
    s16 frameSkips;  // +0xa
    s16 vblankStartVCount;  // +0xc
    s16 vblankEndVCount;  // +0xe
    s16 vblankScanlines;  // +0x10
    s16 framePeak;  // +0x12
    s16 frameHold;  // +0x14
    s16 frameHoldReload;  // +0x16
    s16 vblankPeak;  // +0x18
    s16 vblankHold;  // +0x1a
    s16 vblankHoldReload;  // +0x1c
    u8 pad_1e[0x2];
    u32 field20;
    u32 field24;
    u32 field28;
} FrameTimer;

typedef struct {
    InputState inputState;  // +0x0
    TouchpadStruct touchpad;  // +0x14
    GXOamAttr oamBuffer[128];  // +0x1c
    GXOamAttr subOamBuffer[128];  // +0x41c
    FrameTimer frameTimer;  // +0x81c
} SystemState;

extern SystemState systemState;

void initSystem(void);  // 0x02019a18
void readKeypad(void);  // 0x02019a2c
void decodeKeypad(InputState *, u16);
void waitVBlankSync(void);  // 0x02019b2c
void flushOam(u32);  // 0x02019bf4
void clearOam(void);  // 0x02019c4c
void setBackdropColor(u16, u16);  // 0x02019c7c

#endif
