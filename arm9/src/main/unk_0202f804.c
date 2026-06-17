// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202f804.h"

extern s32 func_0202ead8(UnkNodeSub * param1, UnkStruct_0202f9cc * param2);
extern void func_0202ea14(UnkNodeSub * param1, UnkStruct_0202f9cc  * param2, UnkStruct_0202f9cc * param3);

// MATCH 100%
void func_0202f804(UnkStruct_0202f9cc * param1) {
    UnkStruct_0202f9cc * r1 = func_0202ead8(&data_0205e6d8, NULL);
    while (r1 != NULL) {
        if (param1->field3d < r1->field3d)
            break;

        r1 = func_0202ead8(&data_0205e6d8, r1);
    }

    func_0202ea14(&data_0205e6d8, r1, param1);
}