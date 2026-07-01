// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_020091e4.h"
#include "main/unk_020092e0.h"
#include "main/unk_02009490.h"
#include "main/unk_02009b10.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_020091e4(u8 param1, s16 param2, u32 param3, u32 param4) {
    if (param2 == data_01ff9460[param1]) {
        if (param3 != 0)  {
            func_020092e0(param2, param1, 0);
        }
    } else {
        NNSSndHeapHandle heap = param1 ? data_01ff9480.field00 : data_01ff9474.field00;
        
        data_01ff9460[param1] = param2;

        NNS_SndHeapClear(heap);

        BOOL truthy = func_02009490(heap, param2, &data_01ff94d4[param1]);
        u32 r3 = (param3 != 0);

        data_01ff9488.field00 = param4;

        if (truthy) {
            func_02009b10(2, param2, param1, r3, 0);
        } else {
            func_02009b10((u8)r3, param2, param1, 0, 0);
        }
    }
}
