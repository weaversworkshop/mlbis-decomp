// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202f9cc.h"

extern void func_0202ea78(UnkGlobal_0205e6d8 * param1, UnkStruct_0202f9cc * param2);
extern void func_0202f7b4(UnkGlobal_0205e6d8 * param1, UnkStruct_0202f9cc * param2);
extern void func_0202f804(UnkStruct_0202f9cc * param1);

void func_0202f9cc(UnkStruct_0202f9cc * param1, u32 param2) {
    UnkGlobal_0205e6d8 * r4 = param1->field04;

    if (param1->field04 != NULL) {
        func_0202ea78(param1->field04, param1);
        param1->field04 = 0;
    }

    func_0202ea78(&data_0205e6d8, param1);
    param1->field3d = param2;

    if (r4 != NULL) {
        func_0202f7b4(r4, param1);
    }

    func_0202f804(param1);
}
