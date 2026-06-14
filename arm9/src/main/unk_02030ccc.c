// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02030ccc.h"

extern u32 func_0202e92c(u32 a, u32 b);

// MATCH 100%
u32 func_02030ccc(u32 param1) {
    return func_0202e92c(param1, 0);
}
