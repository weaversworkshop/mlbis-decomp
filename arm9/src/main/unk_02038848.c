// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "main/unk_02038848.h"

// MATCHED 100%
UnkStruct_0203dd70_02 * func_02038848(UnkStruct_0203dd70_01 * param1) {
    UnkStruct_0203dd70_02 * r2 = param1->field00;

    if (r2 != NULL) {
        UnkStruct_0203dd70_02 * r1 = r2->field80;
        param1->field00 = r1;

        if (r1 != NULL) {
            r1->field7c = NULL;
        } else {
            param1->field04 = NULL;
            r2->field78 = NULL;
        }
    }

    return r2;
}
