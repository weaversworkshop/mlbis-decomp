// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "structs/unk_02030540.h"

#include "main/unk_0202f234.h"
#include "main/unk_0202f9cc.h"

// MATCH 100%
void func_0202f234(UnkGlobal_01ff94a8 * param1, u32 param2) {
    if (param1->field00 == NULL)
        return;

    func_0202f9cc(param1->field00, param2);
}
