// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006d14.h"
#include "main/unk_02006d84.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02006d14(u32 param1, u32 param2) {
    const NNSSndArcGroupInfo * groupInfo = NNS_SndArcGetGroupInfo(param1);

    for (u32 i = 0; i < groupInfo->count; i++) {
        NNSSndArcGroupItem * r6 = &groupInfo->item[i];

        if (r6->type == 3) {
            func_02006d84(r6->index, param2);
        }
    }
}
