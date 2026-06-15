// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ee58.h"
#include "main/unk_02030bac.h"
#include "main/unk_02030d10.h"

extern bool func_0202ee08(UnkNode * a, u16 b);

// MATCH 100%
u32 func_02030bac(UnkNode* param1) {
    bool r0 = func_0202ee08(
        param1->next,
        param1->field0c_lo
    );

    if (r0 == false) {
        return -1;
    }

    r0 = func_02030d10(param1);
    if (r0 != false) {
        return param1->field0c_lo - 1;
    }

    func_0202ee58(
        param1->next,
        0
    );

    return -1;
}
