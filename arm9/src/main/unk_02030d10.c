// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202eda8.h"
#include "main/unk_02030ccc.h"
#include "main/unk_02030d10.h"

extern void func_0202e970(UnkNode ** a, u32 b);

// MATCH 100%
bool func_02030d10(UnkNode* param1) {
    u32 ret = func_0202eda8(
        param1->next,
        0x14,
        0x4
    );

    if (ret == 0) {
        return false;
    }

    func_02030ccc(ret);

    func_0202e970(
        &param1->field04,
        ret
    );

    return true;
}
