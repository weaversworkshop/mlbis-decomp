// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202f7b4.h"

// MATCH 100%
void func_0202f7b4(NNSFndList * param1, UnkStruct_0202f9cc * param2) {
    UnkStruct_0202f9cc * r1 = NNS_FndGetNextListObject(param1, NULL);
    while (r1 != NULL) {
        if (param2->field3d < r1->field3d)
            break;

        r1 = NNS_FndGetNextListObject(param1, r1);
    }

    NNS_FndInsertListObject(param1, r1, param2);
    param2->field04 = param1;
}
