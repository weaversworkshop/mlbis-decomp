// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/main.h"

#include <nitro.h>

#include "itcm/mainLoop.h"

#include "main/system.h"
#include "main/task.h"

// --- Function Definitions --- //

// MATCHED 100%
void main() {
    GX_Init();
    GX_SetPower(GX_POWER_ALL);
    GX_DispOff();
    GXS_DispOff();

    FX_Init();

    OS_SetIrqFunction(OS_IE_V_BLANK, vblankIntr);
    OS_EnableIrqMask(OS_IE_V_BLANK);
    OS_EnableIrq();

    GX_VBlankIntr(TRUE);

    RTC_Init();

    resetTasks();
    resetVBlankTasks();

    MI_DmaFill32(3, &systemState.oamBuffers[0], GX_OAM_EFFECT_NODISPLAY, sizeof(systemState.oamBuffers[0]));
    MI_DmaFill32(3, &systemState.oamBuffers[1], GX_OAM_EFFECT_NODISPLAY, sizeof(systemState.oamBuffers[1]));
    MI_CpuFill8(&systemState.inputState, 0, sizeof(systemState.inputState));

    systemState.inputState.repeatDelay = 10;
    systemState.inputState.repeatRate = 2;

    systemState.frameTimer.frameHoldReload = 60;
    systemState.frameTimer.vblankHoldReload = 60;
    systemState.frameTimer.field20 = 3;
    systemState.frameTimer.field24 = 1;
    systemState.frameTimer.field28 = 0;

    bootGame();
}