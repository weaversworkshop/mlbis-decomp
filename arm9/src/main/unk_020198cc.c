// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_020057ac.h"
#include "main/unk_020198cc.h"

#include <nitro/gx.h>

extern void func_ov008_0206406c(void);

// MATCH 100%
void func_020198cc(void) {

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
