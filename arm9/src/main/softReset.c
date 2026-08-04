// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/softReset.h"

#include "main/task.h"
#include "main/touchpad.h"


#define RESET_COMBO (PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START | PAD_BUTTON_SELECT)

// --- Extern Functions --- //

extern void func_02040670(u32);
extern u32 _u32_div_f(u32);

// --- Function Definitions --- //

// MATCHED 100%
void initSoftReset(SoftResetState * softReset) {
    softReset->inhibit = 0;
    softReset->pending = 0;
    softReset->active = 0;

    softReset->resetParam = 1;

    softReset->fadeFrames = 0;
    softReset->defaultFadeFrames = 16;
    softReset->fadeTimer = 0;
}

// MATCHED 100%
void setSoftResetFade(SoftResetState * softReset, s32 frames) {
    frames = MATH_MIN(frames, 16);
    softReset->fadeFrames = frames < 0 ? softReset->defaultFadeFrames : frames;
}

// MATCHED 100%
void updateSoftReset(SoftResetState * softReset) {
    if (softReset->active == 0) {
        if (softReset->pending != 0) {
            if (softReset->inhibit == 0) {
                startResetFade(softReset);
            }
        } else if (checkResetCombo(softReset) != 0) {
            if (softReset->inhibit != 0) {
                softReset->pending = 1;
            } else {
                startResetFade(softReset);
            }
        }
    }
}

// MATCHED 100%
void runResetFade(SoftResetState * softReset) {
    while (softReset->active != 0) {
        if (softReset->fadeTimer != 0) {
            softReset->fadeTimer--;
        }

        u32 brightness;
        if (softReset->fadeFrames == 0) {
            brightness = MASTER_BRIGHT_MAX;
        } else {
            brightness = MASTER_BRIGHT_MAX - (softReset->fadeTimer * MASTER_BRIGHT_MAX) / softReset->fadeFrames;
        }

        GX_SetMasterBrightness(brightness);
        GXS_SetMasterBrightness(brightness);

        if (softReset->fadeTimer == 0) {
            func_02040670(0);
            OS_ResetSystem(softReset->resetParam);
        }

        if (reg_OS_IE & REG_OS_IE_VB_MASK) {
            waitVBlankSync();
        } else {
            waitFrameSync();
        }
    }
}

// MATCHED 100%
BOOL checkResetCombo(SoftResetState * param1) {
    decodeKeypad(&param1->input, PAD_Read());

    if (param1->input.hold == RESET_COMBO && (param1->input.press & RESET_COMBO)) {
        return TRUE;
    }

    return FALSE;
}

// MATCHED 100%
void startResetFade(SoftResetState * param1) {
    param1->active = 1;
    param1->fadeTimer = param1->fadeFrames;

    if (reg_OS_IE & REG_OS_IE_VB_MASK) {
        disableAllVBlankTasks();
    } else {
        disableVCountTasks();
    }

    disableAllTasks();
}

// MATCHED 100% -- Noop
void func_02005fe0() {
    return;
}
