// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202eda8.h"
#include "main/unk_02030ccc.h"
#include "main/unk_02030d10.h"

// MATCH 100%
bool func_02030d10(UnkNode* param1) {
    void * ret = func_0202eda8(
        param1->next,
        0x14,
        0x4
    );

    if (ret == NULL) {
        return false;
    }

    func_02030ccc(ret);

    NNS_FndAppendListObject(
        (NNSFndList*)(&param1->field04),
        ret
    );

    return true;
}
