// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/system.h"

#include "main/heap.h"

// --- Extern Functions --- //

extern void func_02039888(void);

// --- Variable Definitions --- //

const u16 repeatKeyMasks[10] = {  // 0x0204ac4c
    PAD_KEY_UP | PAD_KEY_DOWN,
    PAD_KEY_RIGHT | PAD_KEY_LEFT,
    PAD_BUTTON_A, PAD_BUTTON_B, PAD_BUTTON_X, PAD_BUTTON_Y,
    PAD_BUTTON_START, PAD_BUTTON_SELECT, PAD_BUTTON_L, PAD_BUTTON_R,
};

InputState inputState;  // 0x0205afa8
GXOamAttr oamBuffer[128];  // 0x0205afc4

// ROM gives this only 0x3e4 bytes; retail flush/clear use full HW size 0x400, overrunning 0x1c bytes into frameTimer (dead bytes -- see FrameTimer.oamOverrunPad)
GXOamAttr subOamBuffer[124];  // 0x0205b3c4
FrameTimer frameTimer;  // 0x0205b7a8

// --- Function Definitions --- //

// MATCHED 100%
void initSystem() {
    func_02039888();
    OS_Init();
    initHeaps();
}

// MATCHED 100%
void readKeypad() {
    decodeKeypad(&inputState, PAD_Read());
}

// MATCHED 100%
void decodeKeypad(InputState * input, u16 keys) {
    u32 repeat = 0;

    input->press = ~input->hold & keys;
    input->release = input->hold & ~keys;

    u16 prevHold = input->hold;
    for (u32 i = 0; i < 10; i++) {
        u16 held = (keys & repeatKeyMasks[i]);
        if (held != 0) {
            if ((prevHold & held) == 0) {
                repeat |= held;
                input->repeatTimers[i] = input->field08;
            } else if (--input->repeatTimers[i] == 0) {
                    repeat |= held;
                    input->repeatTimers[i] = input->field09;
            }
        } else {
            input->repeatTimers[i] = 0;
        }
    }

    input->repeat = repeat;
    input->hold = keys;
}

// MATCHED 100%
void waitVBlankSync() {
    frameTimer.syncPending = 1;

    s32 vcount = reg_GX_VCOUNT;
    frameTimer.syncVCount = vcount;

    if (vcount < frameTimer.lastSyncVCount) {
        vcount += HW_LCD_LINES;
    }

    if (frameTimer.frameHold == 0 || frameTimer.framePeak < frameTimer.frameScanlines) {
        frameTimer.frameHold = frameTimer.frameHoldReload;
        frameTimer.framePeak = frameTimer.frameScanlines;
    } else {
        frameTimer.frameHold--;
    }

    frameTimer.frameScanlines = (vcount - frameTimer.lastSyncVCount) + frameTimer.frameSkips * HW_LCD_LINES;

    OS_WaitIrq(TRUE, OS_IE_V_BLANK);

    frameTimer.lastSyncVCount = reg_GX_VCOUNT;
    frameTimer.prevFrameSkips = frameTimer.frameSkips;
    frameTimer.frameSkips = 0;
}

// MATCHED 100%
void flushOam(u32 dmaNo) {
    DC_FlushRange(&oamBuffer, sizeof(oamBuffer));
    MI_DmaCopy32(dmaNo, &oamBuffer, HW_OAM, sizeof(oamBuffer));

    DC_FlushRange(&subOamBuffer, sizeof(oamBuffer));
    MI_DmaCopy32(dmaNo, &subOamBuffer, HW_DB_OAM, sizeof(oamBuffer));
}

// MATCHED 100%
void clearOam() {
    MIi_CpuClearFast(GX_OAM_EFFECT_NODISPLAY, &oamBuffer, sizeof(oamBuffer));
    MIi_CpuClearFast(GX_OAM_EFFECT_NODISPLAY, &subOamBuffer, sizeof(oamBuffer));
}

// MATCHED 100%
void setBackdropColor(u16 color, u16 subColor) {
    *(vu16 *)HW_BG_PLTT = color;
    *(vu16 *)HW_DB_BG_PLTT = subColor;
}
