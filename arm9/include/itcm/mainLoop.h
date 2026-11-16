#ifndef ITCM_MAIN_LOOP_H
#define ITCM_MAIN_LOOP_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>

typedef struct {
    BOOL hasTouchCalib;
    TPCalibrateParam touchCalib;  // +0x04
    OSOwnerInfo owner;  // +0x0c
    u8 macAddress[6];  // +0x60
    s64 rtcOffset;  // +0x68
} ConsoleInfo;

void vblankIntr(void);  // 0x01ff85a4
void vcountIntr(void);  // 0x01ff8a38
void bootGame(void);  // 0x01ff8b50
void enterSleep(void);  // 0x01ff8c70

extern u8 irqTasksEnabled;
extern ConsoleInfo consoleInfo;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
