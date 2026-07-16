// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/playerStatus.h"

#include "main/backup.h"

#include "structs/unk_020563e4_head.h"

// --- Struct Definitions --- //

typedef struct {
    u32 field00_00 : 10;
    u32 field00_0a : 10;
    u32 field00_14 : 10;
    u32 pad_00_1e : 2;

    u32 field04_00 : 10;
    u32 field04_0a : 10;
    u32 field04_14 : 10;
    u32 pad_04_1e : 2;

    u16 field08;
    u8  pad_0a[0x2];
} UnkStruct_020526ec;

typedef struct {
    u16 field00;
    u8 pad_02[0xd];
} UnkGlobal_020526f8;

typedef struct {
    u8 pad_00[0x2];
    u8 field02;
    u8 pad_03[0x1];
    u16 field04;
    u8 pad_06[0x2];
} UnkGlobal_02052778;

typedef struct {
    u16 field00;
    u8 pad_02[0x16];
} UnkGlobal_02052a68;

typedef struct {
    u16 field00;
    u8 pad_02[0xd];
    u8 field0f_00 : 1;
    u8 field0f_01 : 4;
    u8 field0f_05 : 3;
    s16 field10[6];
    u8 field1c;
    u8 field1d;
    u8 field1e;
    u8 pad_1f[0x1];
} UnkGlobal_02053b30;

// --- Extern Data Variables --- //

extern s8 data_0204ac40[];
extern s8 data_0204ac44[];

// --- Extern Functions --- //

extern void func_01ff8228(void * param1, u32 param2, u32 param3);

// --- Variable Definitions --- // (replace with real data later)

extern s8 data_020526bc[];
extern s8 data_020526c0[];
extern s16 data_020526c4[];
extern s16 data_020526c6[];
extern s16 data_020526c8[];
extern u16 data_020526ca[];
extern s16 data_020526cc[];
extern u16 data_020526ce[];
extern s16 data_020526d0[];
extern s16 data_020526d2[];
extern u16 data_020526d4[][4];
extern u16 data_020526dc[][4];
extern u16 data_020526e4[][4];

extern UnkStruct_020526ec * data_020526ec[];
extern UnkGlobal_020526f8 data_020526f8[];
extern UnkGlobal_02052778 data_02052778[];
extern UnkGlobal_02052a68 data_020527f8[];
extern UnkGlobal_02052a68 data_02052a68[];

extern UnkGlobal_02053b30 data_02053b30[];

extern s8 data_0205af84[];
extern s8 data_0205af88[];
extern s8 data_0205af8c[];

// --- Function Declarations --- //

void func_02018048(u32 param1);
void func_020184a8(u8 param1, u8 param2);
u16 * func_02018538(s32 param1);
UnkGlobal_02052a68 * func_020185d0(u32 param1);
UnkGlobal_02052a68 * func_02018600(u32 param1);
UnkGlobal_020526f8 * func_02018630(u32 param1);
UnkGlobal_02053b30 * func_02018650(u16 param1);
UnkGlobal_02052778 * func_02018670(u32 param1);
u32 func_02018688(u32 param1, u32 * param2, u32 * param3);
s16 func_020187dc(s32 param1, s32 param2);
s32 func_02018824(u16 param1);
s8 func_02018878(u16 param1, s8 param2);
s32 func_02018974(s32 param1);
void func_020189b4(UnkGlobal_02056370 * param1, u32 param2, s16 param3, u16 param4);
BOOL func_02018ec4(u8 param1, u16 param2, u8 param3);
BOOL func_02019028(u8 param1);
void func_02019058(u8 param1, u16 param2);
BOOL func_020190a4(u8 param1, u8 param2, u8 * param3, u8 * param4);
u16 func_02019154(u8 param1);
BOOL func_02019178(u32 param1);
s32 func_020191c0(u32 param1);
void func_02019360(UnkStruct_02019360 * param1, s32 param2, u32 param3);
void func_020193a0(UnkStruct_02019360 * param1, UnkStruct_02019360 * param2);
void func_020193c4(UnkStruct_02019360 * param1, s32 param2);
s32 func_020193ec(UnkStruct_02019360 * param1, s32 param2);
s8 func_020194c4(UnkStruct_02019360 * param1);
s8 func_020194f4(u8 param1, u8 param2, s8 param3);
s32 func_02019564(UnkStruct_02019360 * param1);
s32 func_02019574(UnkStruct_02019360 * param1);
s32 func_0201957c(void);
s32 func_02019584(UnkStruct_02019360 * param1);
s32 func_020195a4(UnkStruct_02019360 * param1);
s32 func_020195d4(UnkStruct_02019360 * param1);
s32 func_02019618(UnkStruct_02019360 * param1);
s32 func_02019620(UnkStruct_02019360 * param1);

// --- Function Definitions --- //

// MATCHED 100%
void func_02018048(u32 param1) {
    u16 r6;
    s32 r7;
    u32 r8;
    UnkGlobal_02056370 * r9;

    func_01ff8228(&data_02056370, 0, 0x140);

    r6 = 0;
    u16 arr[4] = { 0, 0, 0, 0 };
    r9 = data_02056370.tbl;

    for (r7 = 0; r7 < 3; r7++) {
        UnkStruct_020526ec * r0 = data_020526ec[r7];

        switch (r7) {
            case 0:
                r9->field00 = 0;
                r9->field20 = (r9->field20 & ~0xff) | (data_020526d2[param1] & 0xff);
                r9->field24 = (r9->field24 & ~0xff000000) | (data_020526cc[param1] << 0x18);

                r6 = data_020526ce[param1];

                for (s32 i = 0; i < 4; i++) {
                    arr[i] = data_020526d4[param1][i];
                }
                break;
            case 1:
                r9->field00 = 1;
                r9->field20 = (r9->field20 & ~0xff) | (data_020526d0[param1] & 0xff);
                r9->field24 = (r9->field24 & ~0xff000000) | (data_020526c6[param1] << 0x18);

                r6 = data_020526ca[param1];

                for (s32 i = 0; i < 4; i++) {
                    arr[i] = data_020526dc[param1][i];
                }
                break;

            case 2:
                r9->field00 = 2;
                r9->field20 = (r9->field20 & ~0xff) | (data_020526c4[param1] & 0xff);
                r9->field24 = (r9->field24 & ~0xff000000) | (data_020526c8[param1] << 0x18);

                for (s32 i = 0; i < 4; i++) {
                    arr[i] = data_020526e4[param1][i];
                }
        }

        r9->field02[0] = r0[r9->field20_00-1].field00_00;
        r9->field02[1] = r0[r9->field20_00-1].field00_0a;
        r9->field02[2] = r0[r9->field20_00-1].field00_14;
        r9->field02[3] = r0[r9->field20_00-1].field04_00;
        r9->field02[4] = r0[r9->field20_00-1].field04_0a;
        r9->field02[5] = r0[r9->field20_00-1].field04_14;
        r9->field24_00 = r0[r9->field20_00].field08;

        for (u32 k = 1; k < r9->field20_00; k++) {
            r9->field20_08 += r0[k-1].field08;
        }

        r9->field02[6] = r9->field02[0];
        r9->field02[8] = r9->field02[1];
        r9->field02[10] = r9->field02[2];
        r9->field02[11] = r9->field02[3];
        r9->field02[12] = r9->field02[4];
        r9->field02[13] = r9->field02[5];

        for (r8 = 0; r8 < r9->field24_18; r8++) {
            func_02018878(arr[r8], 1);
            func_02018ec4(r7, arr[r8], r8);
        }

        if (r6 != 0) {
            func_02018878(r6, 1);
            func_02019058(r7, r6);
        }

        r9->field02[7] = r9->field02[6];
        r9->field02[9] = r9->field02[8];
        
        r9++;
    }

    data_02055fe4.field41c = 0;

    data_020563e4.field20 = 0;

    data_02055fe4.field422 = data_020526c0[param1];
    data_02055fe4.field43a = data_0205af8c[param1];
    data_02055fe4.field423 = data_0205af88[param1];
    data_02055fe4.field432 = data_020526bc[param1];
    data_02055fe4.field42d = data_0205af84[param1];
}

// MATCHED 100%
void func_020184a8(u8 param1, u8 param2) {
    UnkGlobal_02056370 * r4 = &data_02056370.tbl[param1];
    r4->field20 = (r4->field20 & ~0xff) | (param2 & 0xff);
    
    if (param1 < 2) {
        s8 idx = func_020194f4(param1, param2-1, -1);
        r4->field24 = (r4->field24 & ~0xff000000) | (data_0204ac44[idx-1] << 0x18);
    } else {
        s8 idx = func_020194f4(param1, param2-1, -1);
        r4->field24 = (r4->field24 & ~0xff000000) | (data_0204ac40[idx-1] << 0x18);
    }
}

// MATCHED 100%
u16 * func_02018538(s32 param1) {
    u32 row = param1 & 0xfff;

    switch (param1 & 0xf000) {
        case 0x1000:
            return &data_02052a68[row].field00;
        case 0x2000:
            return &data_020527f8[row].field00;
        case 0x3000:
            return &data_020526f8[row].field00;
        case 0x4000:
            return &data_02053b30[row].field00;
        default:
            return NULL;
    }
}

// MATCHED 100%
UnkGlobal_02052a68 * func_020185d0(u32 param1) {
    if ((param1 & 0x1000) != 0x1000) {
        return NULL;
    }

    return &data_02052a68[param1 & 0xfff];
}

// MATCHED 100%
UnkGlobal_02052a68 * func_02018600(u32 param1) {
    if ((param1 & 0x2000) != 0x2000) {
        return NULL;
    }

    return &data_020527f8[param1 & 0xfff];
}

// MATCHED 100%;
UnkGlobal_020526f8 * func_02018630(u32 param1) {
    if ((param1 & 0x3000) != 0x3000) {
        return NULL;
    }
    
    return &data_020526f8[param1 & 0xfff];
}

// MATCHED 100%
UnkGlobal_02053b30 * func_02018650(u16 param1) {
    if ((param1 & 0xf000) != 0x4000) {
        return NULL;
    }
    
    return &data_02053b30[param1 & 0xfff];
}

// MATCHED 100%
UnkGlobal_02052778 * func_02018670(u32 param1) {
    if (param1 >= 0x10) {
        return NULL;
    }

    return &data_02052778[param1];
}

// MATCHED 100%
u32 func_02018688(u32 param1, u32 * param2, u32 * param3) {
    UnkGlobal_02052778 * ptr = func_02018670(param1);

    switch (ptr->field02) {
        case 0:
            if (param2 != NULL) {
                *param2 = ptr->field04 & ~0x8000;
            }

            if (param3 != NULL && (ptr->field04 & 0x8000) != 0) {
                *param3 = 1;
            } else {
                *param3 = 0;
            }
            break;
        case 1:
            if (param2 != NULL) {
                *param2 = ptr->field04 << 8;
            }
            
            if (param3 != NULL) {
                *param3 = 0;
            }
            break;
        case 2:
            if (param2 != NULL) {
                *param2 = (u32)ptr->field04 << 0x18 >> 0x10;
            }

            if (param3 != NULL) {
                *param3 = ptr->field04 & 0xff00;
            }
            break;
        case 3:
            if (param2 != NULL) {
                *param2 = ptr->field04;
            }
            
            if (param3 != NULL) {
                *param3 = 0;
            }
            break;
        case 4:
            if (param2 != NULL) {
                *param2 = 0;
            }

            if (param3 != NULL) {
                *param3 = 0;
            }
            break;
        case 5:
            if (param2 != NULL) {
                *param2 = ptr->field04;
            }
            
            if (param3 != NULL) {
                *param3 = 0;
            }
            break;
        case 6:
            if (param2 != NULL) {
                *param2 = ptr->field04 & ~0x8000;
            }

            if (param3 != NULL && (ptr->field04 & 0x8000) != 0) {
                *param3 = 1;
            } else {
                *param3 = 0;
            }
            break;
    }

    return ptr->field02;
}

// MATCHED 100%
s16 func_020187dc(s32 param1, s32 param2) {
    if ((param1 & 0xfff) >= 4) {
        return -1;
    }

    s32 val = param2 & 0xfff;
    if (val < 4 || val >= 8) {
        return -1;
    }

    return val - 4 + (param1 & 0xfff) * 4;
}

// MATCHED 100%
s32 func_02018824(u16 param1) {
    s32 ret = 0;

    switch ((param1 & 0xf000)) {
        case 0x2000:
            ret = data_02056370.field96[param1 & 0xfff];
            break;
        case 0x4000:
            ret = data_02056370.fieldb6[param1 & 0xfff];
    }
    return ret;
}

// MATCHED 100%
s8 func_02018878(u16 param1, s8 param2) {
    s8 ret = 0;
    u16 r2 = (param1 & 0xfff);
    
    UnkGlobalContainer_02056370 * lr = &data_02056370;

    switch ((u16)(param1 & 0xf000)) {
        case 0x3000: {
            if (param2 < 0) {
                if (lr->field_137 & (1 << r2)) {
                    ret = -1;
                }

                lr->field_137 &= ~(1 << r2);

                return ret;
            } else {
                if (!(lr->field_137 & (1 << r2))) {
                    ret = 1;
                }

                lr->field_137 |= (1 << r2);

                return ret;
            }
        }
        case 0x4000: {
            s8 old = lr->fieldb6[r2];
            s16 r1 = param2 + old;
            if (r1 < 0) {
                r1 = 0;
            }
            if (r1 > 9) {
                r1 = 9;
            }

            lr->fieldb6[r2] = r1;

            return (s8)(r1 - old);
        }
        case 0x2000: {
            s8 old = lr->field96[r2];
            s16 r1 = param2 + old;
            if (r1 < 0) {
                r1 = 0;
            }
            if (r1 > 99) {
                r1 = 99;
            }

            lr->field96[r2] = r1;

            return (s8)(r1 - old);
        }
    }

    return 0;
}

// MATCHED 100%
s32 func_02018974(s32 param1) {
    u32 old = data_02055fe4.field41c;
    s32 coins = old + param1;
    if (coins < 0) {
        data_02055fe4.field41c = 0;
    } else {
        if (coins > 999999) {
            data_02055fe4.field41c = 999999;
        } else {
            data_02055fe4.field41c = coins;
        }
    }

    return (data_02055fe4.field41c - old);
}

// MATCHED 100%
void func_020189b4(UnkGlobal_02056370 * param1, u32 param2, s16 param3, u16 param4) {
    UnkGlobal_02056370 * r10 = &data_02056370.tbl[param2];

    s32 r5 = r10->field02[0];
    param1->field02[0] = r5;

    s32 r6 = r10->field02[1];
    param1->field02[1] = r6;

    s32 r7 = r10->field02[2];
    param1->field02[2] = r7;

    s32 r8 = r10->field02[3];
    param1->field02[3] = r8;

    s32 r9 = r10->field02[4];
    param1->field02[4] = r9;

    s32 stat6 = r10->field02[5];
    param1->field02[5] = stat6;

    for (u32 i = 0; i < r10->field24_18; i++) {
        UnkGlobal_02053b30 * ptr;
        if (param3 == i) {
            ptr = func_02018650(param4);
        } else {
            ptr = func_02018650(r10->field28[i] | 0x4000);
        }

        r5 += ptr->field10[0];
        r6 += ptr->field10[1];
        r7 += ptr->field10[2];
        r8 += ptr->field10[3];
        r9 += ptr->field10[4];
        stat6 += ptr->field10[5];
    }

    s32 val = 999;
    if (r5 <= 999) {
        val = (r5 < 0) ? 0 : r5;
    }
    if (val == 0) {
        r5 = 1;
        param1->field02[6] = r5;
    } else {
        if (r5 > 999) {
            r5 = 999;
        } else if (r5 < 0) {
            r5 = 0;
        }
        param1->field02[6] = r5;
    }

    val = 999;
    if (r6 > val) {
        r6 = val;
    } else if (r6 < 0) {
        r6 = 0;
    }
    param1->field02[8] = r6;

    if (r7 > 999) {
        r7 = 999;
    } else if (r7 < 0) {
        r7 = 0;
    }
    param1->field02[10] = r7;

    val = 999;
    if (r8 <= 999) {
        val = (r8 < 0) ? 0 : r8;
    }

    if (val == 0) {
        r8 = 1;
        param1->field02[11] = r8;
    } else {
        if (r8 > 999) {
            r8 = 999;
        } else if (r8 < 0) {
            r8 = 0;
        }
        param1->field02[11] = r8;
    }

    if (r9 > 999) {
        r9 = 999;
    } else if (r9 < 0) {
        r9 = 0;
    }
    param1->field02[12] = r9;

    if (stat6 > 999) {
        stat6 = 999;
    } else if (stat6 < 0) {
        stat6 = 0;
    }
    param1->field02[13] = stat6;

    for (u32 i = 0; i < r10->field24_18; i++) {
        UnkGlobal_02053b30 * ptr;
        if (param3 == i) {
            ptr = func_02018650(param4);
        } else {
            ptr = func_02018650(r10->field28[i] | 0x4000);
        }

        switch (ptr->field1c) {
            case 1: {
                s32 v = r5 * (ptr->field1d + 100);
                r5 = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 2: {
                s32 v = r6 * (ptr->field1d + 100);
                r6 = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 3: {
                s32 v = r7 * (ptr->field1d + 100);
                r7 = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 0x46: { // 70
                s32 v = r8 * (ptr->field1d + 100);
                r8 = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 4: {
                s32 v = r9 * (ptr->field1d + 100);
                r9 = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
        }
    }

    val = 999;
    if (r5 <= 999) {
        val = (r5 < 0)  ? 0 : r5;
    }

    if (val == 0) {
        param1->field02[6] = 1;
    } else {
        if (r5 > 999) {
            r5 = 999;
        } else if (r5 < 0) {
            r5 = 0;
        }
        param1->field02[6] = r5;
    }

    if (r6 > 999) {
        r6 = 999;
    } else if (r6 < 0) {
        r6 = 0;
    }
    param1->field02[8] = r6;

    if (r7 > 999) {
        r7 = 999;
    } else if (r7 < 0) {
        r7 = 0;
    }
    param1->field02[10] = r7;

    val = 999;
    if (r8 <= 999) {
        val = (r8 < 0) ? 0 : r8;
    }

    if (val == 0) {
        param1->field02[11] = 1;
    } else {
        if (r8 > 999) {
            r8 = 999;
        } else if (r8 < 0) {
            r8 = 0;
        }
        param1->field02[11] = r8;
    }

    if (r9 > 999) {
        r9 = 999;
    } else if (r9 < 0) {
        r9 = 0;
    }
    param1->field02[12] = r9;

    if (stat6 > 999) {
        stat6 = 999;
    } else if (stat6 < 0) {
        stat6 = 0;
    }
    param1->field02[13] = stat6;
}

// MATCHED 100%
BOOL func_02018ec4(u8 param1, u16 param2, u8 param3) {
    UnkGlobal_02053b30 * ptr;
    u32 i;

    UnkGlobal_02056370 * r6 = &data_02056370.tbl[param1];

    ptr = func_02018650(param2);

    if (ptr->field0f_01 != 0) {
        for (i = 0; i < r6->field24_18; i++) {
            UnkGlobal_02053b30 * ret = func_02018650(r6->field28[i] | 0x4000);
            if (param3 != i && ptr->field0f_01 == ret->field0f_01) {
                return FALSE;
            }
        }

        if (ptr->field0f_00 != 0 && param1 != 2) {
            return FALSE;
        }

        if (ptr->field0f_00 == 0 && param1 == 2) {
            return FALSE;
        }

        if (param3 >= r6->field24_18) {
            return FALSE;
        }

        ptr = func_02018650(param2);

        if (func_020194f4(param1, r6->field20_00-1, 0) < ptr->field0f_05) {
            return FALSE;
        }
    }

    func_020189b4(r6, param1, param3, param2);

    r6->field28[param3] = param2 & 0xfff;

    if (r6->field02[7] > r6->field02[6]) {
        r6->field02[7] = r6->field02[6];
    }

    if (r6->field02[9] > r6->field02[8]) {
        r6->field02[9] = r6->field02[8];
    }

    return TRUE;
}

// MATCHED 100%
BOOL func_02019028(u8 param1) {
    return func_02018ec4(param1, data_02056370.tbl[param1].field28[0] | 0x4000, 0);
}

// MATCHED 100%
void func_02019058(u8 param1, u16 param2) {
    data_02056370.tbl[param1].field2c = param2 & 0xfff;

    s16 ret = func_020187dc(data_02055fe4.field3b8, data_02055fe4.field3e8);
    if (ret != -1) {
        data_02055fe4.field4c4 = ret;
    }
}

// MATCHED 100%
BOOL func_020190a4(u8 param1, u8 param2, u8 * param3, u8 * param4) {

    UnkGlobal_02056370 * r4 = &data_02056370.tbl[param1];
    BOOL r11 = FALSE;
    u8 r5 = 0;
    u8 r6 = 0;

    for (u32 i = 0; i < r4->field24_18; i++) {
        UnkGlobal_02053b30 * ret = func_02018650(r4->field28[i] | 0x4000);

        if (ret->field1c == param2) {
            if (ret->field1d > r5 || (ret->field1d == r5 && ret->field1e > r6)) {
                r6 = ret->field1e;
                r5 = ret->field1d;

                if (param3 != NULL) {
                    *param3 = ret->field1d;
                }

                if (param4 != NULL) {
                    *param4 = r6;
                }
            }

            r11 = TRUE;
        }
    }

    return r11;
}

// MATCHED 100%
u16 func_02019154(u8 param1) {
    return data_02056370.tbl[param1].field2c | 0x3000;
}

// MATCHED 100%
BOOL func_02019178(u32 param1) {
    s32 ret = func_020191c0(param1);
    if (ret == 0) {
        return 0;
    }

    u32 val = data_02055fe4.field54[(ret & 0xfff) >> 5] >> (ret & 0x1f);
    return ((val & 1) != 0);
}

// MATCHED 100%
s32 func_020191c0(u32 param1) {
    switch (param1 - 0x1000) {
        case 20:
            return 0;
        case 21:
            return 0;
        case 22:
            return 0;
        case 6:
            return 0x2011;
        case 3:
            return 0x2012;
        case 13:
            return 0x201d;
        case 10:
            return 0x201c;
        case 11:
            return 0x201e;
        case 0:
            return 0x2010;
        case 23:
            return 0;
        case 12:
            return 0x201f;
        case 24:
            return 0;
        case 25:
            return 0;
        case 2:
            return 0x2016;
        case 4:
            return 0x2017;
        case 26:
            return 0;
        case 5:
            return 0x2018;
        case 1:
            return 0x2019;
        case 27:
            return 0;
        case 8:
            return 0x201b;
        case 14:
            return 0x2021;
        case 7:
            return 0x201a;
        case 9:
            return 0;
        case 15:
            return 0x2022;
        case 16:
            return 0x2013;
        case 17:
            return 0;
        case 18:
            return 0;
        case 19:
            return 0;
    }
    return param1;
}

// MATCHED 100%
void func_02019360(UnkStruct_02019360 * param1, s32 param2, u32 param3) {
    param1->field00 = param2;
    param1->field02 = 0;
    param1->field04 = 0;
    param1->field08 = func_020195d4(param1);
    param1->field0c_b0 = 0;

    func_020193c4(param1, param3);
}

// MATCHED 100%
void func_020193a0(UnkStruct_02019360 * param1, UnkStruct_02019360 * param2) {
    param1->field00 = param2->field00;
    param1->field02 = param2->field02;
    param1->field04 = param2->field04;
    param1->field08 = param2->field08;
}

// MATCHED 100%
void func_020193c4(UnkStruct_02019360 * param1, s32 param2) {
    while (param2 > 0) {
        param2 = func_020193ec(param1, param2);
    }
}

// MATCHED 100%
s32 func_020193ec(UnkStruct_02019360 * param1, s32 param2) {
    s32 diff = func_02019620(param1) - func_02019618(param1);

    if (param2 - diff < 0 || func_0201957c() <= func_02019574(param1)) {
        s32 val = func_02019618(param1);
        if (val + param2 < 0) {
            param1->field04 = 0;
        } else if (val + param2 > 0x98967f) {
            param1->field04 = 0x98967f;
        } else {
            param1->field04 = val + param2;
        }
        return 0;
    }

    param1->field0c_b0 = 1;
    param1->field02++;
    param1->field04 = func_02019620(param1);

    s32 val = func_02019620(param1) + func_020195d4(param1);
    if (val < 0) {
        param1->field08 = 0;
    } else if (val > 0x98967f) {
        param1->field08 = 0x98967f;
    } else {
        param1->field08 = val;
    }

    return param2 - diff;
}

// MATCHED 100%
s8 func_020194c4(UnkStruct_02019360 * param1) {
    return func_020194f4(param1->field00, param1->field02, param1->field0c_b0);
}

// MATCHED 100%
s8 func_020194f4(u8 param1, u8 param2, s8 param3) {
    if (param1 == 2) {
        if (param2 < 9) {
            return 1;
        } else if (param2 < 19) {
            return 2;
        } else if (param2 < 39) {
            return 3;
        } else {
            return 4;
        }
    } else {
        if (param2 < 5) {
            return 1;
        } else if (param2 < 11) {
            return 2;
        } else if (param2 < 17) {
            return 3;
        } else if (param2 < 24) {
            return 4;
        } else if (param2 < 39) {
            return 5;
        } else {
            return 6;
        }
    }
}

// MATCHED 100%
s32 func_02019564(UnkStruct_02019360 * param1) {
    return func_02019574(param1) + 1;
}

// MATCHED 100%
s32 func_02019574(UnkStruct_02019360 * param1) {
    return param1->field02;
}

// MATCHED 100%
s32 func_0201957c() {
    return 98;
}

// MATCHED 100%
s32 func_02019584(UnkStruct_02019360 * param1) {
    s32 val1 = func_02019574(param1);
    val1++;

    s32 val2 = func_0201957c();

    if (val2 < val1) {
        val1 = val2;
    }

    return val1 + 1;
}

// MATCHED 100%
s32 func_020195a4(UnkStruct_02019360 * param1) {
    return func_02019618(param1) - (func_02019620(param1) - func_020195d4(param1));
}

// MATCHED 100%
s32 func_020195d4(UnkStruct_02019360 * param1) {
    if (func_02019574(param1) >= func_0201957c()) {
        return 0;
    }

    return data_020526ec[param1->field00][param1->field02].field08;
}

// MATCHED 100%
s32 func_02019618(UnkStruct_02019360 * param1) {
    return param1->field04;
}

// MATCHED 100%
s32 func_02019620(UnkStruct_02019360 * param1) {
    return param1->field08;
}
