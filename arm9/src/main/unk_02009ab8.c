// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02009ab8.h"

// MATCHED 100%
void func_02009ab8() {
    data_01ff9478.next = (UnkNode*)&data_01ff948c;
    data_01ff948c.field04 = (UnkNode*)&data_01ff9478;
    data_01ff948c.next = (UnkNode*)-1;
}
