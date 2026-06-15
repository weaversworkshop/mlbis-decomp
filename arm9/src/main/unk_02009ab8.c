// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02009ab8.h"

// NOT MATCHED: stmia not forming, 3 strs instead  (77.25%)
void func_02009ab8() {
    data_01ff9478.next = &data_01ff948c;
    data_01ff948c.next = (UnkNode *)-1;
    data_01ff948c.field04 = &data_01ff9478;
}