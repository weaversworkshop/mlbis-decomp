// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/menuAI.h"

extern "C" {

extern void func_0201a168(void * param1);

}

namespace nsMenuAI {

// Key functions: out-of-line so the vtables + dtor pairs emit in THIS TU. Do not inline.

// MATCHED 100%
void clBtlMoveBase::fn_10() {}

// MATCHED 100%
void clBtlMove::fn_10() {}

}
