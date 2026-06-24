// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02006464.h"
#include "main/unk_02006688.h"
#include "main/unk_020091e4.h"
#include "main/unk_02009bcc.h"

// MATCHED 100%
void func_02006464(u32 param1, u8 param2, u32 param3) {
    if (param1 == 0)
        return;

    if (data_01ff9504.field20 != 0) {
        UnkGlobal_01ff9464 ctx;
        ctx.field08 = 0;
        ctx.field0c = (void (*)())&func_02006464;
        ctx.field10 = param1;
        ctx.field14_b = param2;
        ctx.field18 = param3;
        func_02009bcc(&ctx);
    } else {
        if (data_01ff9460[param2] != -1 && param1 != data_01ff9460[param2]) {
            func_02006688(0, param2);
        }
        func_020091e4(param2, (s16)param1, 0, param3);
    }
}
