// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/frameSync.h"
#include "main/system.h"

// --- Function Definitions --- //

// MATCHED 100%
void waitFrameSync() {
    frameTimer.syncPending = 1;

    s32 vcount = reg_GX_VCOUNT;
    frameTimer.syncVCount = vcount;

    if (frameTimer.syncVCount < frameTimer.lastSyncVCount) {
        vcount += HW_LCD_LINES;
    }

    if (frameTimer.frameHold == 0 || frameTimer.framePeak < frameTimer.frameScanlines) {
        frameTimer.frameHold = frameTimer.frameHoldReload;
        frameTimer.framePeak = frameTimer.frameScanlines;
    } else {
        frameTimer.frameHold--;
    }

    frameTimer.frameScanlines = (vcount - frameTimer.lastSyncVCount) + frameTimer.frameSkips * HW_LCD_LINES;

    OS_WaitIrq(TRUE, OS_IE_V_COUNT);

    frameTimer.lastSyncVCount = reg_GX_VCOUNT;
    frameTimer.prevFrameSkips = frameTimer.frameSkips;
    frameTimer.frameSkips = 0;
}
