#ifndef MAIN_SOFT_RESET_H
#define MAIN_SOFT_RESET_H

#include "types.h"

#include "main/system.h"

#define MASTER_BRIGHT_MAX 16

typedef struct {
    u32 inhibit : 1;
    u32 pending : 1;
    u32 active : 1;
    u32 fadeFrames : 8;
    u32 defaultFadeFrames : 8;
    u32 fadeTimer : 8;
    u32 : 5;
    u32 resetParam;
    InputState input;
} SoftResetState;

extern SoftResetState softResetState;  // 0x02056504

void initSoftReset(SoftResetState *);  // 0x02005d50
void setSoftResetFade(SoftResetState *, s32);  // 0x02005da0
void updateSoftReset(SoftResetState *);  // 0x02005ddc
void runResetFade(SoftResetState *);  // 0x02005e44
BOOL checkResetCombo(SoftResetState *);  // 0x02005f24
void startResetFade(SoftResetState *);  // 0x02005f8c
void func_02005fe0(void);  // 0x02005fe0

#endif
