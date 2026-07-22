// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "main/unk_020057ac.h"

// TODO: Reintegrate as part of clGameMain : clTask

extern func_0201a610(UnkStruct_020057ac_00 * param1);

// MATCHED 100%
void func_020057ac(UnkStruct_020057ac_00 * param1, u16 param2) {
    param1->field2a = param1->field28;
    param1->field28 = param2;
    param1->field10 = (u8)0x1;

    func_0201a610(param1);
}
