// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200990c.h"
#include "main/unk_020099b4.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_0200990c(void * param1, SNDBankData * bank, u32 param3, u16 param4, NNSSndHeapHandle param5) {
    SNDInstPos pos = SND_GetFirstInstDataPos(bank);
    SNDInstData inst;

    while (SND_GetNextInstData(bank, &inst, &pos)) {
        if (inst.type == 1 && param3 == inst.param.wave[1]) {
            func_020099b4(param1, inst.param.wave[0], param4, param5, 1);
        }
    }
}
