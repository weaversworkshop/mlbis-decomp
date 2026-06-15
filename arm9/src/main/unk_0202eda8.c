// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ec70.h"
#include "main/unk_0202eccc.h"
#include "main/unk_0202eda8.h"

// MATCH 100%
void * func_0202eda8(UnkNode * param1, u32 param2, s32 param3) {
    if (param2 == 0) {
        param2 = 1;
    }

    param2 += 0x3;
    param2 &= ~0x3;
    
    if (param3 >= 0) {
        return func_0202ec70(&param1->field24, param2, param3);
    }

    return func_0202eccc(&param1->field24, param2, -param3);
}
