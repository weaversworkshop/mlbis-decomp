// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/system.h"

#include <nitro.h>

#include "main/heap.h"

// --- Extern Data --- //

extern void ** data_0205afc4;
extern void ** data_0205b3c4;

// --- Extern Functions --- //

extern void func_02039888(void);

// --- Variable Definitions --- // (replace with real data later)

UnkStruct_0205b7a8 data_0205b7a8;

// --- Function Declarations --- //

void func_02019a18(void);  // 0x02019a18
void func_02019a2c(void);  // 0x02019a2c
void func_02019a74(UnkGlobal_0205afa8 * param1, u16 param2);
void waitVBlankSync(void);  // 0x02019b2c
void func_02019bf4(u32 dmaNo);  // 0x02019bf4
void func_02019c4c(void);  // 0x02019c4c
void func_02019c7c(u16 param1, u16 param2);  // 0x02019c7c

// --- Function Definitions --- //

// MATCHED 100%
void func_02019a18() {
    func_02039888();
    OS_Init();
    func_02019c90();
}

// MATCHED 100%
void func_02019a2c() {
    func_02019a74(&data_0205afa8, PAD_Read());
}

// MATCHED 100%
void func_02019a74(UnkGlobal_0205afa8 * param1, u16 param2) {
    u32 r3 = 0;

    param1->field02 = ~param1->field00 & param2;
    param1->field04 = param1->field00 & ~param2;

    u16 r14 = param1->field00;
    for (u32 i = 0; i < 10; i++) {
        u16 r7 = (param2 & data_0204ac4c[i]);
        if (r7 != 0) {
            if ((r14 & r7) == 0) {
                r3 |= r7;
                param1->field0a[i] = param1->field08;
            } else if (--param1->field0a[i] == 0) {
                    r3 |= r7;
                    param1->field0a[i] = param1->field09;
            }
        } else {
            param1->field0a[i] = 0;
        }
    }

    param1->field06 = r3;
    param1->field00 = param2;
}

// MATCHED 100%
void waitVBlankSync() {
    data_0205b7a8.field1c_08 = 1;

    s32 vcount = reg_GX_VCOUNT;
    data_0205b7a8.field20 = vcount;

    if (vcount < data_0205b7a8.field1e) {
        vcount += HW_LCD_LINES;
    }

    if (data_0205b7a8.field30 == 0 || data_0205b7a8.field2e < data_0205b7a8.field22) {
        data_0205b7a8.field30 = data_0205b7a8.field32;
        data_0205b7a8.field2e = data_0205b7a8.field22;
    } else {
        data_0205b7a8.field30--;
    }

    data_0205b7a8.field22 = (vcount - data_0205b7a8.field1e) + data_0205b7a8.field26 * HW_LCD_LINES;

    OS_WaitIrq(TRUE, OS_IE_V_BLANK);

    data_0205b7a8.field1e = reg_GX_VCOUNT;
    data_0205b7a8.field24 = data_0205b7a8.field26;
    data_0205b7a8.field26 = 0;
}

// MATCHED 100%
void func_02019bf4(u32 dmaNo) {
    DC_FlushRange(&data_0205afc4, 0x400);
    MI_DmaCopy32(dmaNo, &data_0205afc4, HW_OAM, 0x400);

    DC_FlushRange(&data_0205b3c4, 0x400);
    MI_DmaCopy32(dmaNo, &data_0205b3c4, HW_DB_OAM, 0x400);
}

// MATCHED 100%
void func_02019c4c() {
    MIi_CpuClearFast(0x200, &data_0205afc4, 0x400);
    MIi_CpuClearFast(0x200, &data_0205b3c4, 0x400);
}

// MATCHED 100%
void func_02019c7c(u16 param1, u16 param2) {
    *(vu16 *)HW_BG_PLTT = param1;
    *(vu16 *)HW_DB_BG_PLTT = param2;
}
