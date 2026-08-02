// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02005ca4.h"
#include "main/clGameMain.h"

// MATCHED 100% -- Gets pointer from table, add offset
u32 * func_02005ca4(u32 param1, u32 param2) {
    return (u32*)((u8*)gameState.field4d8[param1] + gameState.field4d8[param1][param2]);
}
