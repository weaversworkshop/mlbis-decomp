// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0202ead8.h"

extern void * func_0202ead8(UnkNodeSub * param1, void * param2) {
    if (param2 == NULL) {
        return param1->field00;
    } else {
        return *(void **)((u8 *)param2 + param1->field0a + 4);
    }
}