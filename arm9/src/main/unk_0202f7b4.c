// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ea14.h"
#include "main/unk_0202ead8.h"
#include "main/unk_0202f7b4.h"

// MATCH 100%
void func_0202f7b4(UnkNodeSub * param1, UnkStruct_0202f9cc * param2) {
    UnkStruct_0202f9cc * r1 = func_0202ead8(param1, NULL);
    while (r1 != NULL) {
        if (param2->field3d < r1->field3d)
            break;

        r1 = func_0202ead8(param1, r1);
    }

    func_0202ea14(param1, r1, param2);
    param2->field04 = param1;
}
