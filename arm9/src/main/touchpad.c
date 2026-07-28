// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/touchpad.h"

// --- Variable Definitions --- //

TPData samplingBufs[2];  // 0x0205bac4

// --- Function Definitions --- //

// MATCHED 100%
BOOL startTouchSampling(s32 frequence) {
    if (inputState.touch.sampling) {
        return TRUE;
    }

    if (frequence <= TP_SAMPLING_FREQ_MIN || frequence > TP_SAMPLING_FREQ_MAX) {
        frequence = TP_SAMPLING_FREQ_DEFAULT;
    }

    TP_Init();

    TPCalibrateParam calibrate;
    if (TP_GetUserInfo(&calibrate)) {
        TP_SetCalibrateParam(&calibrate);
    }

    TP_RequestAutoSamplingStartAsync(GX_LCD_SIZE_Y-1, frequence, samplingBufs, 2);
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_ON);

    if (TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_ON)) {
        return FALSE;
    }

    inputState.touch.sampling = 1;

    return TRUE;
}

// MATCHED 100%
BOOL stopTouchSampling() {
    if (inputState.touch.sampling == 0) {
        return TRUE;
    }

    TP_RequestAutoSamplingStopAsync();
    TP_WaitBusy(TP_REQUEST_COMMAND_FLAG_AUTO_OFF);

    if (TP_CheckError(TP_REQUEST_COMMAND_FLAG_AUTO_OFF)) {
        return FALSE;
    }

    inputState.touch.sampling = 0;

    return TRUE;
}

// MATCHED 100%
BOOL readTouchpad(TouchpadStruct * touchpad) {
    touchpad->sampling = inputState.touch.sampling;

    if (inputState.touch.sampling == 0) {
        return FALSE;
    }

    TPData data;
    TP_GetLatestCalibratedPointInAuto(&data);

    u16 touch = data.touch;
    if (touch != 0 && data.validity != 0) {
        touchpad->active = 0;
        return FALSE;
    }

    u32 changed = touch ^ touchpad->hold;
    touchpad->press = touch & changed;
    touchpad->release = ~touch & changed;
    touchpad->active = 1;
    touchpad->hold = touch;
    touchpad->trigger = changed;

    if (touch != 0) {
        if (touchpad->press) {
            touchpad->x = data.x;
            touchpad->prevX = touchpad->x;
            touchpad->y = data.y;
            touchpad->prevY = touchpad->y;
        } else {
            touchpad->prevX = touchpad->x;
            touchpad->prevY = touchpad->y;
            touchpad->x = data.x;
            touchpad->y = data.y;
        }
    }

    return TRUE;
}
