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

SystemState systemState;  // 0x0205afa8

// --- Function Definitions --- //

// MATCHED 100%
void initSystem() {
    func_02039888();
    OS_Init();
    initHeaps();
}

// MATCHED 100%
void readKeypad() {
    decodeKeypad(&systemState.inputState, PAD_Read());
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
                input->repeatTimers[i] = input->repeatDelay;
            } else if (--input->repeatTimers[i] == 0) {
                    repeat |= held;
                    input->repeatTimers[i] = input->repeatRate;
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
    systemState.frameTimer.syncPending = 1;

    s32 vcount = reg_GX_VCOUNT;
    systemState.frameTimer.syncVCount = vcount;

    if (vcount < systemState.frameTimer.lastSyncVCount) {
        vcount += HW_LCD_LINES;
    }

    if (systemState.frameTimer.frameHold == 0 || systemState.frameTimer.framePeak < systemState.frameTimer.frameScanlines) {
        systemState.frameTimer.frameHold = systemState.frameTimer.frameHoldReload;
        systemState.frameTimer.framePeak = systemState.frameTimer.frameScanlines;
    } else {
        systemState.frameTimer.frameHold--;
    }

    systemState.frameTimer.frameScanlines = (vcount - systemState.frameTimer.lastSyncVCount) + systemState.frameTimer.frameSkips * HW_LCD_LINES;

    OS_WaitIrq(TRUE, OS_IE_V_BLANK);

    systemState.frameTimer.lastSyncVCount = reg_GX_VCOUNT;
    systemState.frameTimer.prevFrameSkips = systemState.frameTimer.frameSkips;
    systemState.frameTimer.frameSkips = 0;
}

// MATCHED 100%
void flushOam(u32 dmaNo) {
    DC_FlushRange(&systemState.oamBuffer, sizeof(systemState.oamBuffer));
    MI_DmaCopy32(dmaNo, &systemState.oamBuffer, HW_OAM, sizeof(systemState.oamBuffer));

    DC_FlushRange(&systemState.subOamBuffer, sizeof(systemState.subOamBuffer));
    MI_DmaCopy32(dmaNo, &systemState.subOamBuffer, HW_DB_OAM, sizeof(systemState.subOamBuffer));
}

// MATCHED 100%
void clearOam() {
    MIi_CpuClearFast(GX_OAM_EFFECT_NODISPLAY, &systemState.oamBuffer, sizeof(systemState.oamBuffer));
    MIi_CpuClearFast(GX_OAM_EFFECT_NODISPLAY, &systemState.subOamBuffer, sizeof(systemState.subOamBuffer));
}

// MATCHED 100%
void setBackdropColor(u16 color, u16 subColor) {
    *(vu16 *)HW_BG_PLTT = color;
    *(vu16 *)HW_DB_BG_PLTT = subColor;
}
