// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006398.h"
#include "main/unk_0200650c.h"
#include "main/unk_02006688.h"
#include "main/unk_020092e0.h"

// MATCHED 100%
void func_0200650c(u32 param1, u32 param2) {
    if (param1 == data_01ff9460[0]) {
        if (param2 != 0) {
            func_02006688(0, 1);
        }
        func_020092e0(param1, 0, 0);
    } else if (param1 == data_01ff9460[1]) {
        if (param2 != 0) {
            func_02006688(0, 0);
        }
        func_020092e0(param1, 1, 0);
    } else {
        func_02006398(param1, 0, 0xc00);
    }
}
