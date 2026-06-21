// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02009ad8.h"

// MATCHED 100%
void func_02009ad8(void * param1, u32 param2, u32 param3, u32 param4, void (*fn)(UnkNode *), u32 param6) {
    UnkNode * node = (UnkNode *)param1;
    node->field04 = node;
    node->field08 = param3; 
    node->field0c = param2;
    node->field10 = param4;
    node->field14 = fn;
    node->field18 = param6;
    data_01ff948c.field04->next = node;
    node->next = (UnkNode *)&data_01ff948c;
    data_01ff948c.field04 = param1;
}
