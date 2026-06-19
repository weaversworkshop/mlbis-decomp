// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202f7b4.h"
#include "main/unk_0202f804.h"
#include "main/unk_0202f9cc.h"

// MATCH 100%
void func_0202f9cc(UnkStruct_0202f9cc * param1, u32 param2) {
    NNSFndList * r4 = param1->field04;

    if (param1->field04 != NULL) {
        NNS_FndRemoveListObject(param1->field04, param1);
        param1->field04 = 0;
    }

    NNS_FndRemoveListObject(&data_0205e6d8, param1);
    param1->field3d = param2;

    if (r4 != NULL) {
        func_0202f7b4(r4, param1);
    }

    func_0202f804(param1);
}
