#ifndef MAIN_SYSTEM_H
#define MAIN_SYSTEM_H

#include "types.h"

#include <nitro.h>

typedef struct {
    u16 hold;
    u16 press;
    u16 release;
    u16 repeat;
    u8 repeatDelay;
    u8 repeatRate;
    u8 repeatTimers[10];
} InputState;

typedef struct {
    u8 x;
    u8 y;
    u8 prevX;
    u8 prevY;
    union {
        struct {
            u8 active : 1;
            u8 hold : 1;
            u8 trigger : 1;
            u8 press : 1;
            u8 release : 1;
            u8 sampling : 1;
            u8 : 2;
        };
        u8 flags;
    };
    u8 pad_05[0x3];
} TouchpadStruct;

typedef struct {
    u16 pad_00_00 : 8;
    u16 syncPending : 1;
    s16 lastSyncVCount;
    u16 syncVCount;
    s16 frameScanlines;
    s16 prevFrameSkips;
    s16 frameSkips;
    s16 vblankStartVCount;
    s16 vblankEndVCount;
    s16 vblankScanlines;
    s16 framePeak;
    s16 frameHold;
    s16 frameHoldReload;
    s16 vblankPeak;
    s16 vblankHold;
    s16 vblankHoldReload;
    u8 pad_1e[0x2];
    u32 field20;
    u32 field24;
    u32 field28;
} FrameTimer;

typedef struct {
    InputState inputState;
    TouchpadStruct touchpad;
    GXOamAttr oamBuffer[128];
    GXOamAttr subOamBuffer[128];
    FrameTimer frameTimer;
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
