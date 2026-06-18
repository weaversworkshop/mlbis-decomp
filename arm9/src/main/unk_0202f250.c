// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202f250.h"
#include "main/unk_0203ad34.h"

// MATCH 100%
void func_0202f250(UnkGlobal_01ff94a8 * param1, u32 param2) {
    if (param1->field00 == NULL)
        return;

    func_0203ad34(param1->field00->field3c, param2);
}
