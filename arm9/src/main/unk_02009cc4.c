// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include <nitro/mi.h>

#include "globals.h"

#include "main/unk_02006368.h"
#include "main/unk_020092e0.h"
#include "main/unk_02009ab8.h"
#include "main/unk_02009c24.h"
#include "main/unk_02009cc4.h"
#include "main/unk_02009e2c.h"

extern void func_02009668(u32 a, u8 b);
extern void func_02009b10(u32 a, u32 b, u8 c, u32 d, u8 e);

// MATCHED 99.89% -- weird function linking issue (func_02009e2c)
void func_02009cc4() {
    if (func_02009e7c() == false) {
        return;
    }

    UnkNode * r0 = &data_01ff9504;
    if (r0->field20 != 0) {
        return;
    }

    u32 r1 = r0->field2c;
    if (r1 != 0) {
        if (r1 != 1) {
            if (r1 != 2) {
                return;
            }
        } else {
            func_02009e2c(r0);

            UnkNode * r0 = &data_01ff9504;
            func_020092e0(r0->field28, r0->field21, 0);

            MI_CpuFill8(r0, 0, 0x34);

            func_02009ab8();

            MI_CpuFill8(&data_01ff9598, 0, 0x80);
            MI_CpuFill8(&data_01ff9538, 0, 0x60);

            func_02009c24();
            return;
        }

        u32 r4 = r0->field28;
        u8 r5 = r0->field21;
        u32 r6 = r0->field30;

        MI_CpuFill8(r0, 0, 0x34);

        func_02009ab8();

        MI_CpuFill8(&data_01ff9598, 0, 0x80);
        MI_CpuFill8(&data_01ff9538, 0, 0x60);

        func_02009668(r4, r5);

        u8 byte = (&data_01ff9504)->field22;
        func_02009b10(r6 & 0xff, r4, r5, 0, byte);

        return;
    }

    func_02009e2c(r0);

    MI_CpuFill8(&data_01ff9504, 0, 0x34);

    func_02009ab8();

    MI_CpuFill8(&data_01ff9598, 0, 0x80);
    MI_CpuFill8(&data_01ff9538, 0, 0x60);

    func_02009c24();

    return;
}