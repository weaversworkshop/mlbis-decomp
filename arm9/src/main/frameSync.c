// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/system.h"

#include "main/frameSync.h"

// --- Extern Functions --- //

// --- Function Definitions --- //

// MATCHED 100%
void waitFrameSync() {
    data_0205b7a8.field1c_08 = 1;

    s32 vcount = reg_GX_VCOUNT;
    data_0205b7a8.field20 = vcount;

    if (data_0205b7a8.field20 < data_0205b7a8.field1e) {
        vcount += HW_LCD_LINES;
    }

    if (data_0205b7a8.field30 == 0 || data_0205b7a8.field2e < data_0205b7a8.field22) {
        data_0205b7a8.field30 = data_0205b7a8.field32;
        data_0205b7a8.field2e = data_0205b7a8.field22;
    } else {
        data_0205b7a8.field30--;
    }

    data_0205b7a8.field22 = (vcount - data_0205b7a8.field1e) + data_0205b7a8.field26 * HW_LCD_LINES;

    OS_WaitIrq(TRUE, OS_IE_V_COUNT);

    data_0205b7a8.field1e = reg_GX_VCOUNT;
    data_0205b7a8.field24 = data_0205b7a8.field26;
    data_0205b7a8.field26 = 0;
}
