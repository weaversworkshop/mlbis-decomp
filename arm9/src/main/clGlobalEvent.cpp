// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/clGlobalEvent.h"
#include "main/playerStatus.h"

#include "structs/unk_020563e4.h"

#include <nitro/fx/fx_cp.h>
#include <nitro/fx/fx_trig.h>

// --- Struct Definitions --- //

typedef struct {
    u32 field00;
    u8 pad_04[0x18];
} UnkGlobal_02056504;

// --- Extern Data --- //

extern u8 data_0204ac2c[];
extern UnkGlobal_02056504 data_02056504;

// --- Extern Functions --- //

extern void func_01ff8228(u32 * param1, u32 param2, u32 param3); // itcm function

extern s32 func_020058bc(u16 id, clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2);
extern void func_02005a78(u16 id, s32 value, clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2);
extern void func_02005da0(UnkGlobal_02056504 * param1, s32 param2);

extern u32 rand_range(s32 param1);

extern s64 func_0204536c(s32 param1, s32 param2);

// --- Function Declarations --- // (local free functions; the class methods live in the classes)

void func_02015f0c(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3);  // 0x02015f0c
u32 func_020160d4(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3);  // 0x020160d4
u32 func_020161d4(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2);  // 0x020161d4
u32 func_02016254(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3);  // 0x02016254
bool func_02017634(s32 param1, s32 param2, s32 param3);  // 0x02017634
void func_02017728(UnkStruct_02015f0c_01 * param1, u8 * param2);  // 0x02017728

// --- Function Definitions --- //

// MATCHED 100%
clScriptSystem::clScriptSystem() {
    func_01ff8228(&this->field20, 0, 8);
}

// MATCHED 100%
clScriptSystem::~clScriptSystem() {}

// MATCHED 100%
void clScriptSystem::m08(UnkStruct_02015f0c_00_04 * param) {
    this->field04 = param;
}

// MATCHED 100%
void func_02015f0c(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3) {
    u8 * r4;
    u32 * dest;
    s32 r5;
    UnkStruct_02015f0c_00_04 * r6;
    s32 r7;
    u8 * src;
    u32 r11;
    

    r4 = param2->field00;
    src = &param2->field00[6];
    param3->field00 = r4[0] | (r4[1] << 8);

    r6 = &param1->field04[param3->field00];

    r11 = r4[2] | (r4[3] << 8) | (r4[4] << 16) | (r4[5] << 24);
    param3->field04 = r11;

    if ((r6->field00 & 0x80) != 0) {
        param3->field02 = src[0] | (src[1] << 8);
        src += 2;
    }

    r7 = r6->field00 & 0x7f;

    if (r7 != 0) {

        dest = (u32 *)param3->field08;

        for (r5 = 0; r5 < r7; r5++) {

            if ((r11 >> r5 & 1) != 0) {
                dest[0] = param1->fn_0c(param2, src[0] | (src[1] << 8));
                src += 2;
            } else {
                switch ((r6->opcodes[r5/2] >> (r5 & 1) * 4) & 0xf) {
                    case 0:
                        dest[0] = src[0];
                        src++;
                        break;
                    case 3:
                        dest[0] = (s8)src[0];
                        src++;
                        break;
                    case 1:
                        dest[0] = (u16)(src[0] | (src[1] << 8));
                        src += 2;
                        break;
                    case 4:
                    case 6:
                        dest[0] = (s16)(src[0] | (src[1] << 8));
                        src += 2;
                        break;
                    case 2:
                        dest[0] = (src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24));
                        src += 4;
                        break;
                    case 5:
                    case 7:
                        dest[0] = (s32)(src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24));
                        src += 4;
                        break;
                    default:
                        break;
                }
            }

            dest++;
        }
    }

    param2->field00 = src;
}

// MATCHED 100%
u32 func_020160d4(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3) {
    s32 idx;

    UnkStruct_02015f0c_00_04 * entry = &param1->field04[param3->field00];
    u8 header = entry->field00;
    s32 count = header & 0x7f;

    u32 r0 = 6;

    if ((header & 0x80) != 0) {
        r0 += 2;
    }

    for (idx = 0; idx < count; idx++) {
        if (((param3->field04 >> idx) & 1) != 0) {
            r0 += 2;
        } else {
            r0 += data_0204ac2c[entry->opcodes[idx / 2] >> (idx & 1) * 4 & 0xf];
        }
    }

    param2->field00 -= r0;
    return r0;
}

// MATCHED 100%
clGlobalEvent::clGlobalEvent() {
    func_01ff8228(&this->field08, 0, 0x80);
}

// MATCHED 100%
clGlobalEvent::~clGlobalEvent() {}

// MATCHED 100%
u32 func_020161d4(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2) {
    u32 ret = 1;

    if (param2->field00 != NULL) {
        if (param2->fieldaa != 0) {
            if (--param2->fieldaa != 0) {
                return 3;
            }
        }

        UnkStruct_02015f0c_02 r4;
        ret = 0;
        while (ret == 0) {
            func_02015f0c(param1, param2, &r4);
            ret = func_02016254(param1, param2, &r4);
        }
    }

    return ret;
}

// MATCHED 100%
u32 func_02016254(clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3) {
    s32 ret;

    switch (param3->field00) {
        case 0:
            param2->field00 = NULL;
            return 1;
        case 1:
            if (param2->fielda8 == 0) {
                param2->field00 = NULL;
                return 2;
            }
            param2->fielda8--;
            param2->field00 = param2->field68[param2->fielda8];
            return 0;
        case 3:
            if (param3->field08[0] == 1) {
                param2->field68[param2->fielda8] = param2->field00;
                param2->fielda8++;
            }

            param2->field00 += param3->field08[1];
            return 0;
        case 4:
            param2->fieldaa = param3->field08[0];
            return 3;
        case 2:
            ret = func_02017634(param3->field08[0], param3->field08[1], param3->field08[2]);
            if (param3->field08[3] == ret) {
                param2->field00 += param3->field08[4];
            }
            return 0;
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
            return 0;
        case 5:
            param2->field68[param2->fielda8] = (u8 *)param3->field08[0];
            param2->fielda8++;
            return 0;
        case 6:
            param2->fielda8--;
            func_02005a78(param3->field02, (s32)param2->field68[param2->fielda8], param1, param2);
            return 0;
        case 7: {
            u32 r5 = param2->fielda8;
            u8 ** r6 = param2->field64;
            u32 r7 = param3->field08[0] & 0xf;

            if (r7 == 3) {
                r6[r5]++;
            } else if (r7 == 4) {
                r6[r5]--;
            }

            ret = func_02017634(param3->field08[1], param3->field08[2], (s32)r6[r5]);

            if (ret != 0) {
                param2->field00 += param3->field08[3];
                if ((param3->field08[0] & 0x30) == 0x10) {
                    param2->fielda8--;
                    return 0;
                }
            } else if ((param3->field08[0] & 0x30) == 0x20) {
                param2->fielda8--;
                return 0;
            }

            if (r7 == 1) {
                r6[r5]++;
            } else if (r7 == 2) {
                r6[r5]--;
            }
            return 0;
        }
        case 57: {
            u8 * rec = &param2->field00[param3->field08[0]];
            u16 header = rec[0] | (rec[1] << 8);
            s32 type = (header & 0xf00) >> 8;
            u8 * elem = (rec + 2) + data_0204ac2c[type] * (param3->field08[1] & 0xff);

            switch (type) {
                case 0:
                    ret = elem[0];
                    break;
                case 3:
                    ret = (s8)elem[0];
                    break;
                case 1:
                    ret = (u16)(elem[0] | (elem[1] << 8));
                    break;
                case 4:
                case 6:
                    ret = (s16)(elem[0] | (elem[1] << 8));
                    break;
                case 2:
                    ret = (elem[0] | (elem[1] << 8) | (elem[2] << 16) | (elem[3] << 24));
                    break;
                case 5:
                case 7:
                    ret = (s32)(elem[0] | (elem[1] << 8) | (elem[2] << 16) | (elem[3] << 24));
                    break;
                default:
                    ret = 0;
                    break;
            }

            func_02005a78(param3->field02, ret, param1, param2);
            return 0;
        }
        case 58: {
            u8 * ptr = &param2->field00[param3->field08[0]];
            func_02005a78(param3->field02, (u16)(*ptr | (*(ptr+1) << 8)) & 0xff, param1, param2);
            return 0;
        }
        case 8:
            func_02005a78(param3->field02, param3->field08[0], param1, param2);
            return 0;
        case 9:
            func_02005a78(param3->field02, param3->field08[0] + param3->field08[1], param1, param2);
            return 0;
        case 10:
            func_02005a78(param3->field02, param3->field08[0] - param3->field08[1], param1, param2);
            return 0;
        case 11:
            func_02005a78(param3->field02, param3->field08[0] * param3->field08[1], param1, param2);
            return 0;
        case 12: {
            s64 divmod_val = func_0204536c(param3->field08[0], param3->field08[1]);
            func_02005a78(param3->field02, (s32)divmod_val, param1, param2);
            return 0;
        }
        case 13: {
            s64 divmod_val = func_0204536c(param3->field08[0], param3->field08[1]);
            func_02005a78(param3->field02, (s32)(divmod_val >> 32), param1, param2);
            return 0;
        }
        case 14:
            func_02005a78(param3->field02, param3->field08[0] << param3->field08[1], param1, param2);
            return 0;
        case 15:
            func_02005a78(param3->field02, param3->field08[0] >> param3->field08[1], param1, param2);
            return 0;
        case 16:
            func_02005a78(param3->field02, param3->field08[0] & param3->field08[1], param1, param2);
            return 0;
        case 17:
            func_02005a78(param3->field02, param3->field08[0] | param3->field08[1], param1, param2);
            return 0;
        case 18:
            func_02005a78(param3->field02, param3->field08[0] ^ param3->field08[1], param1, param2);
            return 0;
        case 19:
            func_02005a78(param3->field02, -param3->field08[0], param1, param2);
            return 0;
        case 20:
            func_02005a78(param3->field02, (param3->field08[0] == 0), param1, param2);
            return 0;
        case 21:
            func_02005a78(param3->field02, ~param3->field08[0], param1, param2);
            return 0;
        case 22:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, ret+1, param1, param2);
            return 0;
        case 23:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, ret-1, param1, param2);
            return 0;
        case 24:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, param3->field08[0]+ret, param1, param2);
            return 0;
        case 25:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, ret-param3->field08[0], param1, param2);
            return 0;
        case 26:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, param3->field08[0]*ret, param1, param2);
            return 0;
        case 27: {
            ret = func_020058bc(param3->field02, param1, param2);
            s64 divmod_val = func_0204536c(ret, param3->field08[0]);
            func_02005a78(param3->field02, (s32)divmod_val, param1, param2);
            return 0;
        }
        case 28: {
            ret = func_020058bc(param3->field02, param1, param2);
            s64 divmod_val = func_0204536c(ret, param3->field08[0]);
            func_02005a78(param3->field02, (s32)(divmod_val >> 32), param1, param2);
            return 0;
        }
        case 29:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, ret << param3->field08[0], param1, param2);
            return 0;
        case 30:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, ret >> param3->field08[0], param1, param2);
            return 0;
        case 31:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, param3->field08[0] & ret, param1, param2);
            return 0;
        case 32:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, param3->field08[0] | ret, param1, param2);
            return 0;
        case 33:
            ret = func_020058bc(param3->field02, param1, param2);
            func_02005a78(param3->field02, param3->field08[0] ^ ret, param1, param2);
            return 0;
        case 34:
            ret = FX_Sqrt(param3->field08[0] << FX32_SHIFT);
            func_02005a78(param3->field02, ret / FX32_ONE, param1, param2);
            return 0;
        case 35:
            ret = FX_InvSqrt(param3->field08[0] << FX32_SHIFT);
            func_02005a78(param3->field02, ret / FX32_ONE, param1, param2);
            return 0;
        case 36:
            ret = FX_Inv(param3->field08[0] << FX32_SHIFT);
            func_02005a78(param3->field02, ret / FX32_ONE, param1, param2);
            return 0;
        case 37:
            func_02005a78(param3->field02, FX_SinIdx((u16)((param3->field08[0] << 16) / 360)), param1, param2);
            return 0;
        case 38:
            func_02005a78(param3->field02, FX_CosIdx((u16)((param3->field08[0] << 16) / 360)), param1, param2);
            return 0;
        case 39:
            func_02005a78(param3->field02, FX_AtanIdx(param3->field08[0]) * 360 / (1 << 16), param1, param2);
            return 0;
        case 40:
            func_02005a78(param3->field02, FX_Atan2Idx(param3->field08[0], param3->field08[1]) * 360 / (1 << 16), param1, param2);
            return 0;
        case 41:
            ret = rand_range(param3->field08[0]);
            func_02005a78(param3->field02, ret, param1, param2);
            return 0;
        case 42:
            func_02005a78(param3->field02, param3->field08[0], param1, param2);
            return 0;
        case 48:
            func_02005a78(param3->field02, FX_Whole(param3->field08[0]), param1, param2);
            return 0;
        case 49:
            func_02005a78(param3->field02, FX_Floor(param3->field08[0]), param1, param2);
            return 0;
        case 50:
            func_02005a78(param3->field02, FX_Sqrt(param3->field08[0]), param1, param2);
            return 0;
        case 51:
            func_02005a78(param3->field02, FX_InvSqrt(param3->field08[0]), param1, param2);
            return 0;
        case 52:
            func_02005a78(param3->field02, FX_Inv(param3->field08[0]), param1, param2);
            return 0;
        case 53:
            func_02005a78(param3->field02, FX_SinIdx((u16)((param3->field08[0] << 4) / 360)), param1, param2);
            return 0;
        case 54:
            func_02005a78(param3->field02, FX_CosIdx((u16)((param3->field08[0] << 4) / 360)), param1, param2);
            return 0;
        case 55: 
            func_02005a78(param3->field02, FX_AtanIdx(param3->field08[0]) * 360 / (1 << 4), param1, param2);
            return 0;
        case 43:
            func_02005a78(param3->field02, param3->field08[0] + param3->field08[1], param1, param2);
            return 0;
        case 44:
            func_02005a78(param3->field02, param3->field08[0] - param3->field08[1], param1, param2);
            return 0;
        case 45:
            func_02005a78(param3->field02, FX_MUL(param3->field08[0], param3->field08[1]), param1, param2);
            return 0;
        case 46:
            func_02005a78(param3->field02, FX_Div(param3->field08[0], param3->field08[1]), param1, param2);
            return 0;
        case 47:
            func_02005a78(param3->field02, FX_ModS32(param3->field08[0], param3->field08[1]), param1, param2);
            return 0;
        case 56: 
            func_02005a78(param3->field02, FX_Atan2Idx(param3->field08[0], param3->field08[1]) * 360 / (1 << 4), param1, param2);
            return 0;
        case 64:
            func_02005da0(&data_02056504, -1);
            data_02056504.field00 |= 2;
            return 0;
        case 65:
            ret = func_02018974(param3->field08[0]);
            func_02005a78(param3->field02, ret, param1, param2);
            return 0;
        case 66: {
            u16 old = data_020563e4.field20;
            u32 sum = old + param3->field08[0];

            if (sum > 999) {
                data_020563e4.field20 = 999;
            } else {
                data_020563e4.field20 = sum;
            }

            func_02005a78(param3->field02, data_020563e4.field20 - old, param1, param2);
            return 0;
        }
        case 67:
            if ((param3->field08[0] & 0x3000) == 0x3000) {
                ret = (data_02055fe4.field4c3 >> (param3->field08[0] & 0xfff)) & 1;
            } else {
                ret = func_02018824(param3->field08[0]);
            }

            func_02005a78(param3->field02, ret, param1, param2);
            return 0;
        case 68:
            ret = func_02018878(param3->field08[0], param3->field08[1]);
            func_02005a78(param3->field02, ret, param1, param2);
            return 0;
        case 69: {
            UnkGlobal_02056370 *obj = &data_02056370.tbl[param3->field08[0]];
            UnkStruct_02019360 local;

            switch (param3->field08[1]) {
                case 0:
                    ret = obj->field02[0];
                    break;
                case 1:
                    ret = obj->field02[1];
                    break;
                case 2:
                    ret = obj->field02[2];
                    break;
                case 3:
                    ret = obj->field02[3];
                    break;
                case 4:
                    ret = obj->field02[4];
                    break;
                case 5:
                    ret = obj->field02[5];
                    break;
                case 6:
                    ret = obj->field02[6];
                    break;
                case 7:
                    ret = obj->field02[7];
                    break;
                case 8:
                    ret = obj->field02[8];
                    break;
                case 9:
                    ret = obj->field02[9];
                    break;
                case 10:
                    ret = obj->field02[10];
                    break;
                case 11:
                    ret = obj->field02[11];
                    break;
                case 12:
                    ret = obj->field02[12];
                    break;
                case 13:
                    ret = obj->field02[13];
                    break;
                case 14:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_020194c4(&local);
                    break;
                case 15:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_02019564(&local);
                    break;
                case 16:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_02019574(&local);
                    break;
                case 17:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_020195a4(&local);
                    break;
                case 18:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_02019618(&local);
                    break;
                case 19:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_020195d4(&local);
                    break;
                case 20:
                    func_02019360(&local, param3->field08[0], obj->field20 >> 8);
                    ret = func_02019620(&local);
                    break;
                case 21:
                    ret = obj->field24 >> 24;
                    break;
                case 22:
                    ret = obj->field28[0] | 0x4000;
                    break;
                case 23:
                    ret = obj->field28[1] | 0x4000;
                    break;
                case 24:
                    ret = obj->field28[2] | 0x4000;
                    break;
                case 25:
                    ret = obj->field2c | 0x3000;
                    break;
            }

            func_02005a78(param3->field02, ret, param1, param2);
            return 0;
        }
        case 70: {
            s32 id = param3->field08[0];
            s32 val = param3->field08[2];
            UnkGlobal_02056370 *obj = &data_02056370.tbl[id];
            UnkStruct_02019360 local;

            switch (param3->field08[1]) {
                case 0:
                    obj->field02[0] = val;
                    func_02019028(id);
                    break;
                case 1:
                    obj->field02[1] = val;
                    func_02019028(id);
                    break;
                case 2:
                    obj->field02[2] = val;
                    func_02019028(id);
                    break;
                case 3:
                    obj->field02[3] = val;
                    func_02019028(id);
                    break;
                case 4:
                    obj->field02[4] = val;
                    func_02019028(id);
                    break;
                case 5:
                    obj->field02[5] = val;
                    func_02019028(id);
                    break;
                case 7:
                    obj->field02[7] = val;
                    func_02019028(id);
                    break;
                case 9:
                    obj->field02[9] = val;
                    func_02019028(id);
                    break;
                case 17: {
                    obj->field20 = (obj->field20 & 0xff) | (val << 8);
                    func_02019360(&local, id, obj->field20 >> 8);

                    obj->field20 = (obj->field20 & ~0xff) | (func_02019564(&local) & 0xff);
                    func_02019028(id);
                    break;
                }
                case 21:
                    obj->field24 = (obj->field24 & 0x00ffffff) | (val << 24);
                    break;
                case 22:
                    func_02018ec4(id, val, 0);
                    break;
                case 23:
                    func_02018ec4(id, val, 1);
                    break;
                case 24:
                    func_02018ec4(id, val, 2);
                    break;
                case 25:
                    func_02019058(id, val);
                    break;
            }
            return 0;
        }
        default:
            break;

    }

    return param1->fn_10(param2, param3);
}

// MATCHED 100%
u32 clGlobalEvent::fn_10(UnkStruct_02015f0c_01 * param2, UnkStruct_02015f0c_02 * param3) {
    return 1;
}

// MATCHED 100%
u32 clGlobalEvent::fn_14() {
    return 0;
}

// MATCHED 100%
void clGlobalEvent::fn_18() {}

// MATCHED 100%
s32 clGlobalEvent::fn_0c(UnkStruct_02015f0c_01 * param2, u32 param3) {
    return func_020058bc(param3, this, param2);
}

// MATCHED 100%
bool func_02017634(s32 param1, s32 param2, s32 param3) {
    switch (param1) {
        case 0:
            return (param2 == param3);
        case 1:
            return (param2 != param3);
        case 2:
            return (param2 < param3);
        case 3:
            return (param2 > param3);
        case 4:
            return (param2 <= param3);
        case 5:
            return (param2 >= param3);
        case 6:
            return ((param2 & param3) != 0);
        case 7:
            return ((param2 | param3) != 0);
        case 8:
            return ((param2 ^ param3) != 0);
        case 9:
            return (param2 == 0);
        case 10:
            return (param2 != -1);
    }

    return 0;
}

// MATCHED 100%
void func_02017728(UnkStruct_02015f0c_01 * param1, u8 * param2) {
    param1->field00 = param2;

    func_01ff8228(param1->field24, 0, 0x40);

    param1->field64[0] = 0;
    param1->fielda8 = 0;
    param1->fieldaa = 0;
}

} // extern "C"
