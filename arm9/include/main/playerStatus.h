#ifndef MAIN_PLAYER_STATUS_H
#define MAIN_PLAYER_STATUS_H

#include "types.h"

typedef struct {
    s16 field00;
    u16 field02;
    u32 field04;
    u32 field08;
    u8 field0c_b0 : 1;
    u8 field0c : 7;
    u8 pad_0d[0x3];
} UnkStruct_02019360;

s32 func_02018824(u16 param1);  // 0x02018824
s8 func_02018878(u16 param1, s8 param2);  // 0x02018878
s32 func_02018974(s32 param1);  // 0x02018974
BOOL func_02018ec4(u8 param1, u16 param2, u8 param3);  // 0x02018ec4
BOOL func_02019028(u8 param1);  // 0x02019028
void func_02019058(u8 param1, u16 param2);  // 0x02019058
void func_02019360(UnkStruct_02019360 * param1, s32 param2, u32 param3);  // 0x02019360
s8 func_020194c4(UnkStruct_02019360 * param1);  // 0x020194c4
s32 func_02019564(UnkStruct_02019360 * param1);  // 0x02019564
s32 func_02019574(UnkStruct_02019360 * param1);  // 0x02019574
s32 func_020195a4(UnkStruct_02019360 * param1);  // 0x020195a4
s32 func_020195d4(UnkStruct_02019360 * param1);  // 0x020195d4
s32 func_02019618(UnkStruct_02019360 * param1);  // 0x02019618
s32 func_02019620(UnkStruct_02019360 * param1);  // 0x02019620

#endif
