// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/main.h"

#include <nitro.h>

#include "main/system.h"
#include "main/task.h"

// --- Extern Functions --- //

extern void func_01ff85a4(void);
extern void func_01ff8b50(void);

extern void func_02035250(void);

// --- Function Definitions --- //

// MATCHED 100%
void main() {
    GX_Init();
    GX_SetPower(GX_POWER_ALL);
    GX_DispOff();
    GXS_DispOff();

    func_02035250();

    OS_SetIrqFunction(OS_IE_V_BLANK, func_01ff85a4);
    OS_EnableIrqMask(OS_IE_V_BLANK);
    OS_EnableIrq();

    GX_VBlankIntr(TRUE);

    RTC_Init();

    resetTasks();
    resetVBlankTasks();

    MI_DmaFill32(3, systemState.oamBuffer, GX_OAM_EFFECT_NODISPLAY, sizeof(systemState.oamBuffer));
    MI_DmaFill32(3, systemState.subOamBuffer, GX_OAM_EFFECT_NODISPLAY, sizeof(systemState.subOamBuffer));
    MI_CpuFill8(&systemState.inputState, 0, sizeof(systemState.inputState));

    systemState.inputState.repeatDelay = 10;
    systemState.inputState.repeatRate = 2;

    systemState.frameTimer.frameHoldReload = 60;
    systemState.frameTimer.vblankHoldReload = 60;
    systemState.frameTimer.field20 = 3;
    systemState.frameTimer.field24 = 1;
    systemState.frameTimer.field28 = 0;

    func_01ff8b50();
}