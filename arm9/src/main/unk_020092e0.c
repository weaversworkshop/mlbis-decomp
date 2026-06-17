// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200907c.h"

// TODO: Possibly combine these, they seem to have similar fields
#include "structs/unk_02030540.h"
#include "structs/unk_02030608.h"

#include "structs/unk_0203066c.h"

#include "main/unk_020092e0.h"

extern void func_0202f194(UnkStruct_01ff94a4 ** a, u32 b);
extern void func_0202f270(UnkStruct_01ff94a4 ** a, u32 b, u32 c);
extern void func_0202f290(UnkStruct_01ff94a4 ** a, u32 b, u32 c);
extern void func_0202f300(UnkStruct_01ff94a4 ** a, u32 b);
extern UnkStruct_02030540 * func_02030540(u32 a);
extern UnkStruct_02030608 * func_02030608(u32 a);
extern UnkStruct_0203066c * func_0203066c(u32 a);
extern void func_020309c0(u32 a, u32 b);
extern void func_02031614(UnkStruct_01ff94a4 ** a, bool b, u32 c, u32 d, u32 e);
extern s32 func_020348fc(s32 a, u32 b);

// MATCH 100%
void func_020092e0(u32 param1, u8 param2, u32 param3) {
    UnkStruct_01ff94d4 * r6 = &data_01ff94d4[param2];
    UnkStruct_02030540 * r4 = func_02030540(param1);
    UnkStruct_02030608 * r7 = func_02030608(r4->field04);

    func_020309c0(r4->field00, r6->field00);
    func_020309c0(r7->field00, r6->field04);

    for (s32 r4 = 0; r4 < 4; r4++) {
        if (r6->field08[r4] != 0) {
            UnkStruct_0203066c * ret = func_0203066c(r7->field04[r4]);
            func_020309c0((u32)(ret->field00 << 0x8) >> 0x8, r6->field08[r4]);
        }
    }

    func_0202f194(&data_01ff94a4[param2], 0);

    UnkStruct_01ff94b4 * struct_01ff94b4 = &data_01ff94b4[param2];
    struct_01ff94b4->field04 = struct_01ff94b4->field00;
    struct_01ff94b4->field0c = 0;

    func_0202f270(&data_01ff94a4[param2], 0xffff, 0);

    if (param3 != 0) {
        data_01ff94b8[param2].field00 = 0;
    }

    func_02031614(
        &data_01ff94a4[param2], 
        (param2 != 0) ? true : false, 
        -1, -1, param1
    );

    func_0202f290(
        &data_01ff94a4[param2], 0xffff,
        0xfff & (u32)(data_01ff94b8[param2].field00 >> 0xc)
    );

    if (data_01ff94ac[param2] != 0x100) {
        func_0202f300(&data_01ff94a4[param2], data_01ff94ac[param2]);
        data_01ff94ac[param2] = 0x100;
    }

    if (param3 != 0) {
        data_01ff94c0[param2].field00 = 2;
        data_01ff94b8[param2].field00 = data_01ff94b4[param2].field00;
        data_01ff94bc[param2].field00 = func_020348fc(data_01ff94b4[param2].field00, 0x1000);
        func_0200907c();
    }

    data_01ff949c.field00 = param1;
    data_01ff949c.field04 = param2;
}
