// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006274.h"
#include "main/unk_020068c4.h"
#include "main/unk_02006c34.h"
#include "main/unk_02007398.h"

extern void func_0201a174(void *);  // delete func from heap.cpp, need to change this to C++ when the TU is formed

// MATCHED 100%
void func_02006274(u32 param1) {
    func_02006c34();

    func_02007398(0, 0);
    func_02007398(1, 0);

    func_020068c4(0);
    func_020068c4(1);

    if (data_01ff9468.field00 != NULL) {
        NNS_SndHeapClear(data_01ff9468.field00);
        NNS_SndHeapDestroy(data_01ff9468.field00);
        data_01ff9468.field00 = NULL;
    }

    if (data_01ff9474.field00 != NULL) {
        NNS_SndHeapClear(data_01ff9474.field00);
        NNS_SndHeapDestroy(data_01ff9474.field00);
        data_01ff9474.field00 = NULL;
    }

    if (data_01ff9480.field00 != NULL) {
        NNS_SndHeapClear(data_01ff9480.field00);
        NNS_SndHeapDestroy(data_01ff9480.field00);
        data_01ff9480.field00 = NULL;
    }

    if (data_01ff947c.field00 != NULL && param1 == 0) {
        func_0201a174(data_01ff947c.field00);
        data_01ff947c.field00 = NULL;
    }
}
