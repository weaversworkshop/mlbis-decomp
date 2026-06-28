// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006594.h"
#include "main/unk_02006688.h"
#include "main/unk_020068ac.h"
#include "main/unk_020092e0.h"

// MATCHED 100%
void func_02006594(u32 param1, u32 param2, u32 param3) {
    u32 val = func_020068ac();

    if (param1 == 0) {
        if (param2 != 0) {
            func_02006688(0, 1);
        }

        if ((u16)val != (u16)-1) {
            func_020092e0((u16)val, 0, param3);
        }
    } else {
        if (param2 != 0) {
            func_02006688(0, 0);
        }

        if (val >> 16 != (u16)-1) {
            func_020092e0(val >> 16, 1, param3);
        }
    }
}
