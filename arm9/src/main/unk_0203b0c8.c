// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0203b0c8.h"
#include "main/unk_0203b350.h"

extern void func_0203b3d8(void * param1);

// MATCH 100%
void func_0203b0c8(u32 param1, u8 param2, u32 param3, u32 param4, u32 param5) {
    void ** ret = func_0203b350(1);

    if (ret == NULL) {
        return;
    }

    ret[1] = (void *)param1;
    ret[2] = (void *)param2;
    ret[3] = (void *)param3;
    ret[4] = (void *)param4;
    ret[5] = (void *)param5;

    func_0203b3d8(ret);
}
