// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006d84.h"
#include "main/unk_02009a9c.h"
#include "main/unk_02009ad8.h"
#include "main/unk_02009b10.h"

#include "structs/unk_01ff9504.h"
#include "structs/unk_02006d84.h"

#include <nnsys/snd.h>

void func_02008c6c(UnkNode * param1);
void * func_02009bcc(UnkStruct_02006d84 * param1);
void func_02009fcc(void *mem, u32 size, u32 data1, u32 data2);

// MATCHED 100%
void func_02006d84(u32 param1, u32 param2) {
    if (data_01ff9504.field20 != 0) {
        UnkStruct_02006d84 data;
        data.tag = 2;
        data.fn = &func_02006d84;
        data.arg1 = param1;
        data.arg2 = param2;

        func_02009bcc(&data);
        return;
    }

    const NNSSndArcSeqArcInfo * info = NNS_SndArcGetSeqArcInfo(param1);
    if (NNS_SndArcGetFileAddress(info->fileId) != NULL) {
        return;
    }

    u32 sz = NNS_SndArcGetFileSize(info->fileId);
    void * ret = func_02009a9c(data_01ff9468.field00, sz, &func_02009fcc, &data_01ff9618, info->fileId);

    func_02009ad8(ret, sz, info->fileId, 0, &func_02008c6c, ret);

    NNS_SndArcSetFileAddress(info->fileId, ret);

    data_01ff9488.field00 = param2;

    func_02009b10(0, param1, 0xff, 0, 1);
}
