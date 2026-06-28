// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02007344.h"
#include "main/unk_02007398.h"
#include "main/unk_02031be4.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02007398(u32 param1, u32 param2) {
    UnkStruct_01ff9470_00 * ret = func_02007344(param1);
    if (ret != NULL) {
        NNS_SndArcStrmStop(&ret->field10_strm, param2);
        NNS_SndStrmHandleRelease(&ret->field10_strm);
        func_02031be4(&ret->field10_strm);

        ret->field2c_09 = 1;
    }
}
