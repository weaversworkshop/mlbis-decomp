// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/frameSync.h"
#include "main/system.h"

// --- Function Definitions --- //

// MATCHED 100%
void waitFrameSync() {
    systemState.frameTimer.syncPending = 1;

    s32 vcount = reg_GX_VCOUNT;
    systemState.frameTimer.syncVCount = vcount;

    if (systemState.frameTimer.syncVCount < systemState.frameTimer.lastSyncVCount) {
        vcount += HW_LCD_LINES;
    }

    if (systemState.frameTimer.frameHold == 0 || systemState.frameTimer.framePeak < systemState.frameTimer.frameScanlines) {
        systemState.frameTimer.frameHold = systemState.frameTimer.frameHoldReload;
        systemState.frameTimer.framePeak = systemState.frameTimer.frameScanlines;
    } else {
        systemState.frameTimer.frameHold--;
    }

    systemState.frameTimer.frameScanlines = (vcount - systemState.frameTimer.lastSyncVCount) + systemState.frameTimer.frameSkips * HW_LCD_LINES;

    OS_WaitIrq(TRUE, OS_IE_V_COUNT);

    systemState.frameTimer.lastSyncVCount = reg_GX_VCOUNT;
    systemState.frameTimer.prevFrameSkips = systemState.frameTimer.frameSkips;
    systemState.frameTimer.frameSkips = 0;
}
