// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009b10.h"

// MATCHED 100%
void func_02009b10(u32 param1, u32 param2, u8 param3, u32 param4, u8 param5) {
    UnkNode * r14 = data_01ff9478.next;
    UnkNode * r12 = r14->next;

    data_01ff9504.next = r12;
    data_01ff9504.field04 = r14->field04;
    data_01ff9504.field08 = r14->field08;
    data_01ff9504.field0c = r14->field0c;
    data_01ff9504.field10 = r14->field10;
    data_01ff9504.field14 = r14->field14;
    data_01ff9504.field18 = r14->field18;
    data_01ff9504.field1c = r12;
    data_01ff9504.field2c = param1;
    data_01ff9504.field28 = param2;
    data_01ff9504.field21 = param3;
    data_01ff9504.field24 = 0;
    data_01ff9504.field20 = 1;
    data_01ff9504.field30 = param4;
    data_01ff9504.field22 = param5;
}
