// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_tu_02019628.h"

#include <nitro/fs.h>
#include <nitro/gx.h>

// --- Struct Definitions --- //

typedef struct {
    u8 pad_00[0x20];
    u16 field20;
    u8 pad_24[0xda];
    u16 fieldfc;
} UnkGlobal_020563e4;

typedef struct {
    u8 pad_00[0x2];
    u16 field02;
} UnkGlobal_020564e4;

// --- Extern Data Variables --- //


// --- Extern Functions --- //
#include "main/unk_020057ac.h"

extern u32 func_0201a2bc(void * param2);
extern u32 func_0201b180(u32 param1, u32 param2);

extern void func_ov008_0206406c(void);

// --- Variable Definitions --- // (replace with real data later)

extern UnkGlobal_020563e4 data_020563e4;
extern UnkGlobal_020564e4 data_020564e4;
extern u32 data_0205af98;

// --- Function Declarations --- //

void func_02019628(u32 param1, u32 param2);
void func_02019684(void);
void func_020198cc(void);

// --- Function Definitions --- //

// MATCHED 100%
FS_EXTERN_OVERLAY(OVY_8);
void func_02019628(u32 param1, u32 param2) {
    FSOverlayInfo data;
    void * ret;
    if (FS_LoadOverlayInfo(&data, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_8))) {
        ret = (void*)((u8*)FS_GetOverlayAddress(&data) + FS_GetOverlayTotalSize(&data));
    } else {
        ret = NULL;
    }

    func_0201a2bc(ret);
    func_0201b180(FS_OVERLAY_ID(OVY_8), 0);
    func_02019684();
}

// MATCHED 100%
void func_020198cc() {

    func_ov008_0206406c();

    data_01ff9440 = 0;

    data_020564e4.field02 = 0;

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
