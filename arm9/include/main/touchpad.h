#ifndef MAIN_TOUCHPAD_H
#define MAIN_TOUCHPAD_H

#include "types.h"

#include "main/system.h"

#define TP_SAMPLING_FREQ_MIN 0
#define TP_SAMPLING_FREQ_MAX 1
#define TP_SAMPLING_FREQ_DEFAULT 1

BOOL startTouchSampling(s32);  // 0x0201e380
BOOL stopTouchSampling(void);  // 0x0201e434
BOOL readTouchpad(TouchpadStruct *);  // 0x0201e48c

#endif
