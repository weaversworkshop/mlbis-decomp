// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/btlBoot.h"

#include "main/clGameMain.h"
#include "main/fs.h"
#include "main/heap.h"
#include "main/sound.h"

#include "ov008/clBtlSys.h"

extern "C" {

#include <nitro.h>

// --- Struct Definitions --- //

typedef struct {
    u8 pad_00[0x1d20];
} UnkStruct_ov009_02064f30;

// --- Extern Data Variables --- //

FS_EXTERN_OVERLAY(OVY_8);
FS_EXTERN_OVERLAY(OVY_9);
FS_EXTERN_OVERLAY(OVY_118);
FS_EXTERN_OVERLAY(OVY_119);
FS_EXTERN_OVERLAY(OVY_120);
FS_EXTERN_OVERLAY(OVY_121);

// --- Extern Functions --- //

extern void func_ov009_02064f30(UnkStruct_ov009_02064f30 *);

// --- Variable Definitions --- //

u32 data_0205af98;  // 0x0205af98

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void btlEnter(u32 param1, u32 param2) {
    FSOverlayInfo info;
    void * ret;
    if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_8))) {
        ret = (void*)((u8*)FS_GetOverlayAddress(&info) + FS_GetOverlayTotalSize(&info));
    } else {
        ret = NULL;
    }

    setMainHeapBase(ret);
    fsLoadOverlay(FS_OVERLAY_ID(OVY_8), MI_PROCESSOR_ARM9);
    btlSetup();
}

// MATCHED 100%
void btlSetup() {
    switch ((s16)gameWork.field4fc) {
        case 0x3001:
        case 0x3002:
        case 0x3003:
        case 0x3004:
            gameWork.field504_0 = 1;
            break;
        case 0x3005:
        case 0x3009:
            gameWork.field504_0 = 3;
            break;
        default: {
            if (gameWork.field502 != 0) {
                gameWork.field504_0 = 2;
            } else if (gameWork.field504_0b != 0) {
                gameWork.field504_0 = 1;
            } else {
                gameWork.field504_0 = 0;
            }
        }
    }

    switch (gameWork.field504_0) {
        case 0: {
            FSOverlayInfo info;
            void * ret;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_118))) {
                ret = (void*)((u8*)FS_GetOverlayAddress(&info) + FS_GetOverlayTotalSize(&info));
            } else {
                ret = NULL;
            }

            setMainHeapBase(ret);
            break;
        }
        case 1: {
            FSOverlayInfo info;
            void * ret;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_120))) {
                ret = (void*)((u8*)FS_GetOverlayAddress(&info) + FS_GetOverlayTotalSize(&info));
            } else {
                ret = NULL;
            }

            setMainHeapBase(ret);
            break;
        }
        case 2: {
            FSOverlayInfo info;
            void * ret;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_119))) {
                ret = (void*)((u8*)FS_GetOverlayAddress(&info) + FS_GetOverlayTotalSize(&info));
            } else {
                ret = NULL;
            }

            setMainHeapBase(ret);
            break;
        }
        case 3: {
            FSOverlayInfo info;
            void * ret;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_121))) {
                ret = (void*)((u8*)FS_GetOverlayAddress(&info) + FS_GetOverlayTotalSize(&info));
            } else {
                ret = NULL;
            }

            setMainHeapBase(ret);
            break;
        }
    }

    fsLoadOverlay(FS_OVERLAY_ID(OVY_9), MI_PROCESSOR_ARM9);

    UnkStruct_ov009_02064f30 * ptr = new(HEAP_MAIN_LO, NULL, FALSE) UnkStruct_ov009_02064f30;
    if (ptr != NULL) {
        func_ov009_02064f30(ptr);
    }

    btlMainSys->field1cf4 = FS_OVERLAY_ID(OVY_9);
    btlMainSys->field1cf6 = btlMainSys->field1cf4;
}

// MATCHED 100%
void btlExit() {

    btlDisableVramBanks();

    sndEchoMode = 0;

    gameWork.field502 = 0;

    if (gameWork.field504_13 == 3) {
        GX_SetMasterBrightness(-0x10);
        GXS_SetMasterBrightness(~0xf);

        u16 hold = gameMain->prevGameMode;
        gameMain->change(2);
        gameMain->prevGameMode = hold;
    } else {
        data_0205af98 = 0;

        GX_SetMasterBrightness(-0x10);
        GXS_SetMasterBrightness(~0xf);

        if (gameMain->prevGameMode == 5) {
            gameWork.field4fc &= ~1;
        }

        gameMain->change(gameMain->prevGameMode);
    }
}
