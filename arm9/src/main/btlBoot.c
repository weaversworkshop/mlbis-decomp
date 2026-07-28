// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/btlBoot.h"
#include "main/heap.h"

#include "main/unk_020057ac.h"

// --- Extern Data Variables --- //

#include "structs/unk_020563e4.h"

typedef struct {
    u8 pad_00[0x1cf4];
    s16 field1cf4;
    s16 field1cf6;
} UnkStruct_ov008_02064f0c;

extern UnkStruct_ov008_02064f0c * data_ov008_02064f0c;

FS_EXTERN_OVERLAY(OVY_8);
FS_EXTERN_OVERLAY(OVY_9);
FS_EXTERN_OVERLAY(OVY_118);
FS_EXTERN_OVERLAY(OVY_119);
FS_EXTERN_OVERLAY(OVY_120);
FS_EXTERN_OVERLAY(OVY_121);

// --- Extern Functions --- //

extern u32 func_0201a138(u32, u32, u32, u32);
extern u32 fsLoadOverlay(FSOverlayID, u32);

extern void func_ov008_0206406c(void);
extern void func_ov009_02064f30(void);

// --- Variable Definitions --- //

u32 data_0205af98;  // 0x0205af98

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
    fsLoadOverlay(FS_OVERLAY_ID(OVY_8), 0);
    btlSetup();
}

// MATCHED 100%
void btlSetup() {
    switch ((s16)data_020563e4.fieldfc) {
        case 0x3001:
        case 0x3002:
        case 0x3003:
        case 0x3004:
            data_02055fe4.field504_00 = 1;
            break;
        case 0x3005:
        case 0x3009:
            data_02055fe4.field504_00 = 3;
            break;
        default:
        if (data_020563e4.field102 != 0) {
            data_02055fe4.field504_00 = 2;
        } else if (data_02055fe4.field504_0b != 0) {
            data_02055fe4.field504_00 = 1;
        } else {
            data_02055fe4.field504_00 = 0;
        }
    }

    switch (data_02055fe4.field504_00) {
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

    fsLoadOverlay(FS_OVERLAY_ID(OVY_9), 0);

    if (func_0201a138(0x1d20, 0, 0, 0)) {
        func_ov009_02064f30();
    }

    data_ov008_02064f0c->field1cf4 = FS_OVERLAY_ID(OVY_9);
    data_ov008_02064f0c->field1cf6 = data_ov008_02064f0c->field1cf4;
}

// MATCHED 100%
void btlExit() {

    func_ov008_0206406c();

    data_01ff9440 = 0;

    data_020563e4.field102 = 0;

    if (data_02055fe4.field504_13 == 3) {
        GX_SetMasterBrightness(-0x10);
        GXS_SetMasterBrightness(~0xf);

        u16 hold = data_02055fe0->field2a;
        func_020057ac(data_02055fe0, 2);
        data_02055fe0->field2a = hold;
    } else {
        data_0205af98 = 0;

        GX_SetMasterBrightness(-0x10);
        GXS_SetMasterBrightness(~0xf);

        if (data_02055fe0->field2a == 5) {
            data_020563e4.fieldfc &= ~1;
        }

        func_020057ac(data_02055fe0, data_02055fe0->field2a);
    }
}
