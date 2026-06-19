// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202f804.h"

// MATCH 100%
void func_0202f804(UnkStruct_0202f9cc * param1) {
    UnkStruct_0202f9cc * r1 = NNS_FndGetNextListObject(&data_0205e6d8, NULL);
    while (r1 != NULL) {
        if (param1->field3d < r1->field3d)
            break;

        r1 = NNS_FndGetNextListObject(&data_0205e6d8, r1);
    }

    NNS_FndInsertListObject(&data_0205e6d8, r1, param1);
}