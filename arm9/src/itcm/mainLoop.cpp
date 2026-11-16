// objdiff-target: build/delinks/_dsd_gap@itcm_1.o

#include "globals.h"

#include "itcm/mainLoop.h"

#include "main/clGameMain.h"
#include "main/clVCountTask.h"
#include "main/fs.h"
#include "main/softReset.h"
#include "main/sound.h"
#include "main/task.h"

extern "C" {

#include <nitro.h>

#include "main/sound.h"
#include "main/system.h"
#include "main/unk_02047c84.h"

// --- Extern Data Variables --- //

FS_EXTERN_OVERLAY(OVY_0);
FS_EXTERN_OVERLAY(OVY_1);

// --- Variable Definitions --- //

#include <nitro/itcm_begin.h>
u8 irqTasksEnabled;  // 0x01ff93c0
u8 blockSleep;  // 0x01ff93c4
#include <nitro/itcm_end.h>

// --- Extern Functions --- //

extern void func_ov001_02069820(u32);

// --- Function Declarations --- //

void runTasks(void);  // 0x01ff8664
void runVBlankTasks(void);  // 0x01ff8834
void runVCountTasks(void);  // 0x01ff88f0
void mainLoop(void);  // 0x01ff8bb8
void vblankUpdate(void);  // 0x01ff8cc0
void vcountUpdate(void);  // 0x01ff8cec

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void vblankIntr() {
    systemState.frameTimer.vblankStartVCount = GX_GetVCount();

    vblankUpdate();

    systemState.frameTimer.frameSkips++;

    s32 vcount = GX_GetVCount();
    systemState.frameTimer.vblankEndVCount = vcount;

    if (vcount < systemState.frameTimer.vblankStartVCount) {
        vcount += HW_LCD_LINES;
    }

    systemState.frameTimer.vblankScanlines = vcount - systemState.frameTimer.vblankStartVCount;

    if (systemState.frameTimer.vblankHold == 0 || systemState.frameTimer.vblankPeak < systemState.frameTimer.vblankScanlines) {
        systemState.frameTimer.vblankHold = systemState.frameTimer.vblankHoldReload;
        systemState.frameTimer.vblankPeak = systemState.frameTimer.vblankScanlines;
    } else {
        systemState.frameTimer.vblankHold--;
    }

    systemState.frameTimer.syncPending = 0;

    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

// MATCHED 100%
void runTasks() {
    if (taskHead != NULL) {
        BOOL done;
        
        do {
            done = TRUE;

            clTask * task = taskHead;
            while (task != NULL) {
                taskNext = task->next;
                taskCur = task;

                if (task->enabled && !task->hasRan) {
                    s32 rate = task->rate;
                    if (rate & 8) {
                        task->counter++;

                        if (-rate <= task->counter) {
                            task->run();
                            task->counter = 0;
                        }

                        if (taskCur != NULL) {
                            task->numRuns++;
                            task->hasRan = 1;
                        }
                    } else {
                        task->run();

                        if (taskCur != NULL) {

                            task->numRuns++;

                            task->counter++;

                            if (task->counter >= task->rate) {
                                task->hasRan = 1;
                            } else {
                                done = FALSE;
                            }
                        }
                    }
                }
                task = taskNext;
            }
        } while (done == FALSE);
    }

    clTask * node = taskHead;
    while (node != NULL) {
        node->hasRan = 0;

        if ((node->rate & 8) == 0) {
            node->counter = 0;
        }

        node = node->next;
    }
}

// MATCHED 100%
void runVBlankTasks() {
    clVBlankTask * task = vblHead;
    while (task != NULL) {
        vblNext = task->next;
        vblCur = task;

        if (task->enabled && !task->hasRan) {
            task->func_ov008_02064558();

            if (vblCur != NULL) {
                task->numRuns++;
                task->hasRan = 1;
            }
        }

        task = vblNext;
    }

    clVBlankTask * node = vblHead;
    while (node != NULL) {
        node->hasRan = 0;
        node = node->next;
    }
}

// MATCHED 100%
void runVCountTasks() {
    if (vcountHook != NULL && vcountHookLine == GX_GetVCountEqVal()) {
        vcountHook();
    }

    if (vcountHead != NULL) {
        clVCountTask * task = vcountNext;

        while (task != NULL) {
            vcountNext = task->next;
            vcountCur = task;

            if (task->enabled && !task->hasRan) {
                task->func_ov003_02072638();

                if (vcountCur != NULL) {
                    task->numRuns++;
                    task->hasRan = 1;
                }
            }

            task = vcountNext;

            if (task != NULL && task->line > GX_GetVCount()) {
                GX_SetVCountEqVal(task->line);
                return;
            }
        }
    }

    clVCountTask * node = vcountHead;
    while (node != NULL) {
        node->hasRan = 0;
        node = node->next;
    }

    vcountNext = vcountHead;
}

// MATCHED 100%
void vcountIntr() {
    vcountUpdate();

    if (vcountNext == vcountHead) {
        systemState.frameTimer.vblankStartVCount = HW_LCD_HEIGHT;

        systemState.frameTimer.frameSkips++;

        s32 vcount = GX_GetVCount();
        systemState.frameTimer.vblankEndVCount = vcount;

        if (vcount < systemState.frameTimer.vblankStartVCount) {
            vcount += HW_LCD_LINES;
        }

        systemState.frameTimer.vblankScanlines = vcount - systemState.frameTimer.vblankStartVCount;

        if (systemState.frameTimer.vblankHold == 0 || systemState.frameTimer.vblankPeak < systemState.frameTimer.vblankScanlines) {
            systemState.frameTimer.vblankHold = systemState.frameTimer.vblankHoldReload;
            systemState.frameTimer.vblankPeak = systemState.frameTimer.vblankScanlines;
        } else {
            systemState.frameTimer.vblankHold--;
        }

        systemState.frameTimer.syncPending = 0;

        OS_SetIrqCheckFlag(OS_IE_V_COUNT);

        if (vcountHook != NULL) {
            GX_SetVCountEqVal(vcountHookLine);
        } else if (vcountHead != NULL) {
            GX_SetVCountEqVal(vcountHead->line);
        }
    }
}

// MATCHED 100%
void bootGame() {
    u32 tableSize = fsInit(-1, NULL, 0);

    fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
    fsLoadOverlay(FS_OVERLAY_ID(OVY_1), MI_PROCESSOR_ARM9);

    func_ov001_02069820(tableSize);

    fsUnloadOverlay(FS_OVERLAY_ID(OVY_1), MI_PROCESSOR_ARM9);
    fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);

    mainLoop();
}

// MATCHED 100%
void mainLoop() {
    while (TRUE) {
        #ifndef IDE_PARSE
        asm {
            stmdb sp!, {r0, r1, r2, r3, r4, r5, lr}
            ldr r5, =sndTick
            ldr lr, =data_02054bfc
            ldmia r5, {r0, r1, r2, r3, r4}
            stmdb sp!, {r0, r1, r2, r3, r4}
            ldmia lr, {r0, r1, r2, r3, r4}
            stmia r5, {r0, r1, r2, r3, r4}
            bl sndTick
            ldr r5, =sndTick
            ldmia sp!, {r0, r1, r2, r3, r4}
            stmia r5, {r0, r1, r2, r3, r4}
            ldmia sp!, {r0, r1, r2, r3, r4, r5, lr}
        }
        #endif

        runTasks();

        runResetFade(&softResetState);

        if (gameState.field4cc_0) {
            if (gameState.field4d0 < -1) {
                gameState.field4d0++;
            }
        }

        if (blockSleep == 0 && PAD_DetectFold() == TRUE) {
            enterSleep();
        }
    }
}

// MATCHED 100%
void enterSleep() {
    sndStopAlt(-1);

    sndPauseSlot(TRUE, 0);
    sndPauseSlot(TRUE, 1);

    PM_GoSleepMode(PM_TRIGGER_COVER_OPEN | PM_TRIGGER_CARD, 0, 0);

    sndPauseSlot(FALSE, 0);
    sndPauseSlot(FALSE, 1);
}

// MATCHED 100%
void vblankUpdate() {
    if (irqTasksEnabled != 0) {
        updateSoftReset(&softResetState);
        runVBlankTasks();
    }
}

// MATCHED 100%
void vcountUpdate() {
    if (irqTasksEnabled != 0) {
        updateSoftReset(&softResetState);
        runVCountTasks();
    }
}
