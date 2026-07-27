#ifndef MAIN_SYSTEM_H
#define MAIN_SYSTEM_H

#include "types.h"

#include "structs/unk_0205afa8.h"

typedef struct {
    u16 field00;
    u8 pad_02[0x1a];
    u16 field1c_00 : 8;
    u16 field1c_08 : 1;
    s16 field1e;
    u16 field20;
    s16 field22;
    s16 field24;
    s16 field26;
    u8 pad_28[0x6];
    s16 field2e;
    s16 field30;
    s16 field32;
    u8 pad_34[0x14];
} UnkStruct_0205b7a8;

extern UnkStruct_0205b7a8 data_0205b7a8;

void func_02019a74(UnkGlobal_0205afa8 * param1, u16 param2);  // 0x02019a74

#endif
