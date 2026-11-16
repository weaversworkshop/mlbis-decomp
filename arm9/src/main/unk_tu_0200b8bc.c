// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_tu_0200b8bc.h"

// --- Struct Definitions --- //

typedef union {
    u32 field00;
    struct {
        u32 field00_0 : 4;  // +0x0
        u32 field00_4 : 4;
        u32 : 16;
        u32 field00_24 : 8;
    };
} UnkStruct_0200cb30;

typedef struct {
    u16 field00;
    u16 field02;
} UnkStruct_0200bce8_1C;

typedef struct {
    UnkStruct_0200bce8_18 struct18;  // +0x0
    UnkStruct_0200bce8_1C struct1c;  // +0x4
    UnkStruct_0200bce8_20 struct20;  // +0x8
    UnkStruct_0200bce8_24 out;  // +0xc
} UnkStruct_0200bce8;

// --- Extern Functions --- //

extern void cpu_copy32_fwd(void *, void *, u32);
extern void cpu_copy32_rev(void *, void *, u32);

// --- Variable Definitions --- //

const u32 data_0204a4cc[4] = { 0x11111111, 0x22222222, 0x44444444, 0x88888888 };
const u32 data_0204a4dc[4] = { 0x00001111, 0x00002222, 0x00004444, 0x00008888 };
const u32 data_0204a4ec[4] = { 0x11110000, 0x22220000, 0x44440000, 0x88880000 };

// --- Function Declarations --- //

u16 func_0200bc48(UnkStruct_0200b8bc *);  // 0x0200bc48
u32 func_0200c4b8(UnkStruct_0200b8bc *, UnkStruct_0200bce8_24 *, BOOL);  // 0x0200c4b8

void func_0200cb30(u32 *, u16, u16, u16, u32 *, u8, u8);  // 0x0200cb30
void func_0200ce04(u32 *, s32, u16, u16, u32 *, u8, u8);  // 0x0200ce04
void func_0200d0d8(u32 *, u16, u16, s32, u16, u32 *, u8, u8);  // 0x0200d0d8
void func_0200d63c(u32 *, s32, u16, s32, u16, u32 *, u8, u8);  // 0x0200d63c

// --- Function Definitions --- //

// MATCHED 100%
void func_0200b8bc(UnkStruct_0200b8bc * param1, UnkStruct_0200b8bc_10 ** param2, u32 param3, const char * param4, u8 param5, u8 param6, u8 param7, u8 param8, u8 param9, u8 param10, u8 param11, u8 param12, u8 param13, u8 param14, UnkStruct_0200b8bc_2c * param15) {
    param1->field10 = param2;
    param1->field00 = param3;
    param1->field04 = param4;
    param1->field08_0 = param5;
    param1->field08_10 = param6;
    param1->field08_20 = param7;
    param1->field1c_17 = param7;
    param1->field08_24 = param8;
    param1->field08_28 = param9;
    param1->field0c_0 = param9;
    param1->field0c_4 = param10;
    param1->field0c_9 = param11;
    param1->field14_2 = param13;
    param1->field14_10 = param14;

    if (param12 == 0) {
        param1->field0c_16 = param11;
    } else {
        param1->field0c_16 = param12;
    }

    param1->field18_0 = 0;
    param1->field14_0 = 0;
    param1->field2c = param15;

    func_0200b9f4(param1);
}

// MATCHED 100%
void func_0200b9f4(UnkStruct_0200b8bc * param1) {
    param1->field1c_11 = 0;
    param1->field1c_8 = 0;
    param1->field1c_21 = 0;
    param1->field18_17 = 0;
    param1->field1c_10 = 0;
    param1->field18_22 = param1->field08_0;
    param1->field1c_0 = param1->field08_10;
    param1->field20_0 = 0;
    param1->field20_10 = 0;
    param1->field1c_22 = 0x3ff;
    param1->unkStruct24.field24_0 = 1;
    param1->unkStruct24.field28_0 = 0;
    param1->unkStruct24.field28_16 = 0;
    param1->unkStruct24.field24_8 = 0x3ff;
    param1->unkStruct24.field24_18 = param1->field08_10;
}

// MATCHED 100%
void func_0200baf0(UnkStruct_0200b8bc_2c * param1, u32 param2, u8 param3, UnkCallback_0200b8bc_2c param4, void * param5) {
    u16 idx = param2-1;
    param1[idx].field00 = param3;
    param1[idx].field04 = param4;
    param1[idx].field08 = param5;
}

// MATCHED 100%
void func_0200bb1c(UnkStruct_0200b8bc * param1, UnkStruct_0200b8bc_24 * param2) {
    UnkStruct_0200b8bc local;

    if (param1 < &local) {
        cpu_copy32_rev(param1, &local, sizeof(local));
    } else {
        cpu_copy32_fwd(param1, &local, sizeof(local));
    }

    while (local.field04[0] != '\0') {
        func_0200bce8(&local, 1, 1);
    }

    if (&local.unkStruct24 < param2) {
        cpu_copy32_rev(&local.unkStruct24, param2, sizeof(UnkStruct_0200b8bc_24));
    } else {
        cpu_copy32_fwd(&local.unkStruct24, param2, sizeof(UnkStruct_0200b8bc_24));
    }
}

// MATCHED 100%
u16 func_0200bbb0(UnkStruct_0200b8bc * param1) {
    UnkStruct_0200b8bc local;

    if (param1 < &local) {
        cpu_copy32_rev(param1, &local, sizeof(local));
    } else {
        cpu_copy32_fwd(param1, &local, sizeof(local));
    }

    local.field1c_21 = 1;

    while (local.field04[0] != 0) {
        func_0200bce8(&local, 1, 1);
    }

    return local.unkStruct24.field28_16 + (local.unkStruct24.field24_0 << 8);
}

// MATCHED 100%
u16 func_0200bc48(UnkStruct_0200b8bc * param1) {
    UnkStruct_0200b8bc local;

    if (param1 < &local) {
        cpu_copy32_rev(param1, &local, sizeof(local));
    } else {
        cpu_copy32_fwd(param1, &local, sizeof(local));
    }

    local.field1c_21 = 1;

    while (TRUE) {
        if ((local.field04[0] == 0xff && local.field04[1] == 0) || local.field04[0] == 0) {
            break;
        }

        func_0200bce8(&local, 1, 1);
    }

    return local.field08_0 - param1->field08_0;
}


UnkStruct_0200bce8_24 func_0200bce8(UnkStruct_0200b8bc * param1, u32 param2, u32 param3) {
    UnkStruct_0200bce8_24 outv = {0};
    UnkStruct_0200bce8_20 s20;
    struct {
        UnkStruct_0200bce8_18 struct18;
        UnkStruct_0200bce8_1C struct1c;
    } w;

    switch (param1->field04[0]) {
        case 0:
            break;
        case 0x20: {
            u8 val = param1->field0c_4 - 1;

            param1->field20_0 = param1->field08_0;
            param1->field20_10 = param1->field08_10;

            if (param1->field1c_22 == 0x3ff) {
                param1->field1c_22 = param1->field08_0;
            }

            param1->field08_0 = val+1 + param1->field1c_17 + param1->field08_0;
            param1->field20_20 = val;

            param1->field04++;

            outv.field00h = 0xff20;
            outv.field02h = 0xff20;

            UnkStruct_0200b8bc_10 * r3 = param1->field10[(u16)param1->field18_0 * 2];
            u32 r2 = WORD_10(r3, r3->field08[0]/4);

            val = (r2 & 0xf) * 4;

            if (val > param1->field1c_11) {
                param1->field1c_11 = val;
            }
            break;
        }
        case 0xff: {
            param1->field04 += func_0200c4b8(param1, &outv, param3) + 2;
            outv.field02h = outv.field00h;
            break;
        }
        default: {
            outv.field04 = param1->field04;

            param1->field20_10 = param1->field08_10;

            UnkStruct_0200b8bc_10 ** tbl = param1->field10;
            u16 j = param1->field18_0;
            u16 r0 = param1->field04[0];
            UnkStruct_0200b8bc_10 * r12;

            if (r0 < 0x100) {
                r12 = tbl[j*2];
            } else {
                r12 = tbl[j*2+1];
                r0 -= 0x100;
            }

            s32 val = ((u16 *)PTR_10(r12, r12->field04))[r0];

            outv.field00h = val;
            outv.field02h = param1->field04[0];

            if (param3 == 0 && param2 == 0) {
                BOOL flag = (param1->field1c_10) != 0;
                u16 idx = param1->field18_0;

                UnkStruct_0200b8bc_10 ** tbl = param1->field10;
                UnkStruct_0200b8bc_10 * r2;

                if (outv.field02h < 0x100) {
                    r2 = tbl[idx*2];
                } else {
                    r2 = tbl[idx*2+1];
                }

                u32 * ptr = PTR_10(r2, WORD_10(r2, (u8)val + 2));

                w.struct18.field00 = func_0200c330(param1->field08_0, param1->field08_10, param1->field14_0, val >> 8, (u16)param1->field0c_9, param1->field00, ptr, param1->field0c_0, flag, param1->field14_10);
                w.struct1c.field02 = w.struct18.field00;
                s20.field00 = w.struct18.field00;
            } else {
                BOOL flag = (param1->field1c_10) != 0;
                u16 idx = param1->field18_0;
                
                UnkStruct_0200b8bc_10 ** tbl = param1->field10;
                UnkStruct_0200b8bc_10 * r12;

                if (outv.field02h < 0x100) {
                    r12 = tbl[idx*2];
                } else {
                    r12 = tbl[idx*2+1];
                }

                u32 * sec = PTR_10(r12, WORD_10(r12, (u8)val + 2));
                u8 mode = param1->field14_0;

                u32 a;
                u32 hdr;
                if (flag) {
                    hdr = sec[0];
                    a = ((hdr & 0xf0) >> 4) * 4 - 1;
                } else {
                    u8 c = val >> 8;
                    u8 wi = (c >> 3) + 1;
                    u8 sh = (c & 7) * 4;

                    hdr = sec[0];
                    a = (u8)((sec[wi] >> sh) & 0xf);

                    if (((hdr & 0xf0) >> 4) >= 5) {
                        u8 wi2 = wi + ((hdr & 0xff000000) >> 25);
                        a = (u8)(a | ((sec[wi2] >> sh) << 4));
                    }
                }

                w.struct18.field02[0] = a;
                w.struct18.field02[1] = (hdr & 0xf) * 4 - 1;

                if (mode & 1) {
                    w.struct18.field02[0] = (w.struct18.field02[0] + 1) * 2 - 1;
                }

                if (mode & 2) {
                    w.struct18.field02[1] = (w.struct18.field02[1] + 1) * 2 - 1;
                }

                w.struct1c.field00 = *(u16 *)w.struct18.field02;
                s20.field00 = *(u16 *)w.struct18.field02;
            }

            param1->field20_0 = param1->field08_0;
            param1->field20_10 = param1->field08_10;

            if (param1->field1c_22 == 0x3ff) {
                param1->field1c_22 = param1->field08_0;
            }

            param1->field08_0 = s20.field00b+1 + param1->field1c_17 + param1->field08_0;
            param1->field20_20 = s20.field00b;
            param1->field20_26 = s20.field01b;

            s20.field00 += 0x10001;

            if (s20.field01b > param1->field1c_11) {
                param1->field1c_11 = s20.field01b;
            }

            param1->field04++;
            
            if (param1->unkStruct24.field24_8 > param1->field1c_22) {
                param1->unkStruct24.field24_8 = param1->field1c_22;
            }

            if (param1->field08_0 - param1->unkStruct24.field24_8 > param1->unkStruct24.field28_0) {
                param1->unkStruct24.field28_0 = param1->field08_0 - param1->unkStruct24.field24_8;
            }

            param1->unkStruct24.field28_16 = param1->field08_10 - param1->unkStruct24.field24_18 + param1->field1c_11;
            break;
        }
    }

    return outv;
}

// MATCHED 100%
s16 func_0200c330(u16 param1, u16 param2, u8 param3, u8 param4, u32 param5, u32 param6, u32 * param7, u8 param8, BOOL param9, u8 param10) {
    u16 n = param5 + 1;
    u8 pair[2];

    switch (param10) {
        case 0:
            if (param3 == 0) {
                func_0200cb30((u32 *)param6, param1, param2, n, param7, param4, param8);
            } else {
                func_0200d0d8((u32 *)param6, param1, param2, param3, n, param7, param4, param8);
            }
            break;
        case 1:
            if (param3 == 0) {
                func_0200ce04((u32 *)param6, param1, param2, n, param7, param4, param8);
            } else {
                func_0200d63c((u32 *)param6, param1, param2, param3, n, param7, param4, param8);
            }
            break;
    }

    u32 a;
    u32 hdr;
    
    if (param9 != 0) {
        hdr = param7[0];
        a = ((hdr & 0xf0) >> 4) * 4 - 1;
    } else {
        u8 wi = (param4 >> 3) + 1;
        u8 sh = (param4 & 0x7) * 4;

        hdr = param7[0];
        a = (u8)((param7[wi] >> sh) & 0xf);

        if (((hdr & 0xf0) >> 4) >= 5) {
            u8 wi2 = wi + ((hdr & 0xff000000) >> 25);
            a = (u8)(a | ((param7[wi2] >> sh) << 4));
        }
    }

    pair[0] = a;
    pair[1] = (hdr & 15) * 4 - 1;

    if (param3 & 1) {
        pair[0] = (pair[0] + 1) * 2 - 1;
    }

    if (param3 & 2) {
        pair[1] = (pair[1] + 1) * 2 - 1;
    }

    return *(s16 *)pair;
}

// MATCHED 100%
u32 func_0200c4b8(UnkStruct_0200b8bc * param1, UnkStruct_0200bce8_24 * param2, BOOL param3) {
    u8 cmd = param1->field04[1];
    u32 ret = 0;

    if (cmd == 0) {
        param1->unkStruct24.field24_0 += 1;
        param1->field08_10 = param1->field1c_11 + param1->field08_24 + param1->field08_10;
        param1->field08_0 = param1->field18_22;
        param1->field1c_11 = 0;
        param1->field1c_22 = 0x3ff;

        if (param1->field1c_21 == 0) {
            param1->field04 += 2;

            switch (param1->field1c_8) {
                case 1:
                    param1->field08_0 = (param1->field0c_16 + 1) * 4 - (func_0200bc48(param1) >> 1);
                    param1->field08_0 = param1->field14_2 + param1->field08_0;
                    break;
                case 2:
                    param1->field08_0 = (param1->field0c_16 + 1) * 8 - func_0200bc48(param1);
                    param1->field08_0 = param1->field08_0 - (param1->field18_22 - param1->field14_2);
                    param1->field08_0 = param1->field14_2 + param1->field08_0;
                    break;
            }

            param1->field04 -= 2;
        }

        param2->field00h = 0xff00;
    } else if (cmd < 0x20) {
        UnkStruct_0200b8bc_2c * rec;

        if (param1->field2c == NULL) {
            return ret;
        }

        rec = &param1->field2c[cmd - 1];

        if (rec->field04 != NULL && rec->field01 == 0) {
            if (rec->field00 < 0) {
                ret = rec->field04(rec->field08, param1, param3);
            } else {
                rec->field04(rec->field08, param1, param3);
                ret = rec->field00;
            }
        } else {
            ret = rec->field00;
        }

        if (!param3) {
            param2->field00h = (cmd & 31) | 0xff00;
            param2->field04 = param1->field04 + 2;
        }
    } else if (cmd < 0x30) {
        if (!param3) {
            param1->field0c_0 = (cmd & 15) + param1->field08_28;

            if (param1->field0c_0 > 14) {
                param1->field0c_0 = 14;
            }
        }
    } else if (cmd < 0x40) {
        u32 sub = cmd & 15;

        if (sub < 4) {
            if ((sub & 3) == 0) {
                param1->field14_0a = 0;
                param1->field14_0b = 0;
            } else {
                if (sub & 1) {
                    param1->field14 |= 2;
                }
                if (sub & 2) {
                    param1->field14_0a = 1;
                }
            }
        } else if (sub < 7) {
            if (param1->field1c_21 == 0) {
                u8 v = sub - 4;

                if (param1->field18_17 != v) {
                    param1->field18_17 = v;
                    param1->field1c_8 = v;

                    switch (param1->field1c_8) {
                        case 1:
                            param1->field08_0 = (param1->field0c_16 + 1) * 4 - (func_0200bc48(param1) >> 1);
                            param1->field08_0 = param1->field14_2 + param1->field08_0;
                            break;
                        case 2:
                            param1->field08_0 = (param1->field0c_16 + 1) * 8 - func_0200bc48(param1);
                            param1->field08_0 = param1->field08_0 - (param1->field18_22 - param1->field14_2);
                            param1->field08_0 = param1->field14_2 + param1->field08_0;
                            break;
                    }

                    param2->field00h = param1->field1c_8 + 0xff30;
                    param2->field02h = param1->field1c_8 + 0xff30;
                }
            }
        } else if (sub < 9) {
            param1->field1c_10 = (s32)(sub & 8) >> 3;
        }
    } else if (cmd < 0x50) {
        param1->field18_0 = cmd & 15;
    } else if (cmd < 0x60) {
        param1->field0c_4 = cmd & 15;
    } else if (cmd < 0x80) {
        param1->field08_0 = (cmd - 95) + param1->field08_0;
    } else if (cmd < 0xa0) {
        param1->field08_0 = param1->field08_0 - (cmd - 127);
    } else if (cmd < 0xc0) {
        param1->field08_10 = (cmd - 159) + param1->field08_10;
    } else if (cmd < 0xe0) {
        param1->field08_10 = param1->field08_10 - (cmd - 191);
    } else if (cmd < 0xe8) {
        u8 v = cmd - 0xe0;
        param1->field08_20 = v;
        param1->field1c_17 = v;
    } else if (cmd < 0xf0) {
        u8 v = cmd - 0xe8;

        if (v == 7) {
            param1->field1c_17 = param1->field08_20;
        } else {
            param1->field1c_17 = v;
        }
    }

    return ret;
}

#pragma optimization_level 2
#pragma opt_common_subs off

// MATCHED 100%
void func_0200cb30(u32 * param1, u16 param2, u16 param3, u16 param4, u32 * param5, u8 param6, u8 param7) {
    s16 col;
    s16 cnt;
    u32 * dst;
    s16 k;
    u16 lim;
    u32 shr;
    u32 c2;
    s16 col0;
    u32 c1;
    u32 shl;
    u32 inv;
    u16 w4;
    s16 k2;
    s16 col2;
    UnkStruct_0200cb30 hdr;

    hdr.field00 = param5[0];

    u32 w = (u16)((hdr.field00 & 0xf0) >> 4);
    u16 h = hdr.field00 & 15;
    param5 = param5 + 1 + ((hdr.field00 & 0xff000000) >> 24);
    u32 t = param6 * w;
    u32 t2 = h * t;
    shl = (u16)((param2 & 7) << 2);
    shr = (u16)(32 - shl);
    param5 = param5 + t2;
    dst = param1 + ((param2 & 0xfff8) + (param3 & 0xfff8) * param4);
    param3 = param3 & 7;
    lim = param3 + h * 4;
    w4 = w << 2;
    cnt = w4 & 0xfff8;

    if (cnt > 0) {
        c1 = param7;
        col0 = (s16)param3;

        do {
            col = col0;

            if (col < lim) {
                inv = 0xffffffff;

                do {
                    k = 0;

                    do {
                        u16 idx = (col & 0xfff8) * param4 + (col & 7);
                        col = col + 1;
                        u32 mask = data_0204a4cc[k];
                        u32 bits = (param5[0] & mask) >> k;
                        u32 * q = dst + idx;
                        u32 comb = bits * c1;
                        u32 m15 = bits * 16 - bits;
                        dst[idx] &= (inv ^ (m15 << shl));
                        q[8] &= (inv ^ (m15 >> shr));
                        u32 val = comb + ((param5[1] & mask) >> k);
                        dst[idx] = dst[idx] + (val << shl);
                        q[8] += (val >> shr);

                        k++;
                    } while (k < 4);

                    param5 += 2;
                } while (col < lim);
            }

            cnt = cnt - 8;
            dst += 8;
        } while (cnt > 0);
    }

    if (w4 & 4) {
        col2 = (s16)param3;

        if (col2 < lim) {
            c2 = param7;

            do {
                inv = 0xffffffff;
                k2 = 0;

                do {
                    u16 idx = (col2 & 0xfff8) * param4 + (col2 & 7);
                    col2 = col2 + 1;
                    u32 bits = (param5[0] & data_0204a4dc[k2]) >> k2;
                    u32 * q = dst + idx;
                    u32 m15 = bits * 16 - bits;
                    dst[idx] = dst[idx] & (inv ^ (m15 << shl));
                    q[8] = q[8] & (inv ^ (m15 >> shr));
                    u32 comb = bits * c2;
                    u32 val = comb + ((param5[0] & data_0204a4ec[k2]) >> (k2 + 16));
                    dst[idx] = dst[idx] + (val << shl);
                    q[8] = q[8] + (val >> shr);
                    k2 = k2 + 1;
                } while (k2 < 4);

                param5 += 1;
            } while (col2 < lim);
        }
    }
}

// MATCHED 100%
void func_0200ce04(u32 * param1, s32 param2, u16 param3, u16 param4, u32 * param5, u8 param6, u8 param7) {
    u32 * p;
    s16 k;
    u32 shr;
    u32 shl;
    s16 cnt;
    s16 col;
    u32 * dst;
    u16 lim;
    u32 c1;
    u32 rowbase2;
    s16 col0;
    u32 rowbase;
    u16 w4;
    s16 col2;
    u32 inv;
    u32 * p2;
    s16 k2;
    u32 c2;
    u32 inv2;
    UnkStruct_0200cb30 hdr;

    hdr.field00 = param5[0];

    u32 w = (u16)((hdr.field00 & 0xf0) >> 4);
    shl = (u16)((param2 & 7) << 2);
    u16 h = hdr.field00 & 15;
    param5 = param5 + 1 + ((hdr.field00 & 0xff000000) >> 24);
    u32 t = param6 * w;
    u32 t2 = h * t;
    shr = (u16)(32 - shl);
    param5 = param5 + t2;
    dst = param1 + (param2 >> 3);
    lim = param3 + h * 4;
    cnt = (u16)(w << 2) & 0xfff8;
    w4 = w << 2;

    if (cnt > 0) {
        c1 = param7;
        col0 = (s16)param3;

        do {
            col = col0;

            if (col < lim) {
                rowbase = col * param4;
                inv = 0xffffffff;

                do {
                    p = dst + rowbase;
                    k = 0;

                    do {
                        u32 mask = data_0204a4cc[k];
                        u32 bits = (param5[0] & mask) >> k;
                        u32 m15 = bits * 16 - bits;
                        u32 comb = bits * c1;

                        p[0] = p[0] & (inv ^ (m15 << shl));
                        p[1] = p[1] & (inv ^ (m15 >> shr));
                        u32 val = (param5[1] & mask) >> k;
                        val = val + comb;
                        p[0] = p[0] + (val << shl);
                        p[1] = p[1] + (val >> shr);

                        p += param4;
                        k = k + 1;
                    } while (k < 4);

                    param5 += 2;
                    rowbase += param4 * 4;
                    col = col + 4;
                } while (col < lim);
            }

            cnt = cnt - 8;
            dst += 1;
        } while (cnt > 0);
    }

    if (w4 & 4) {
        col2 = (s16)param3;

        if (col2 < lim) {
            c2 = param7;
            rowbase2 = col2 * param4;

            do {
                p2 = dst + rowbase2;
                k2 = 0;
                inv2 = 0xffffffff;

                do {
                    u32 bits = (param5[0] & data_0204a4dc[k2]) >> k2;
                    u32 m15 = bits * 16 - bits;

                    p2[0] = p2[0] & (inv2 ^ (m15 << shl));
                    u32 comb = bits * c2;
                    p2[1] = p2[1] & (inv2 ^ (m15 >> shr));
                    u32 val = (param5[0] & data_0204a4ec[k2]) >> (k2 + 16);
                    val = val + comb;
                    p2[0] = p2[0] + (val << shl);
                    p2[1] = p2[1] + (val >> shr);

                    p2 += param4;
                    k2 = k2 + 1;
                } while (k2 < 4);

                param5 += 1;
                col2 = col2 + 4;
                rowbase2 += param4 * 4;
            } while (col2 < lim);
        }
    }
}

// MATCHED 100%
void func_0200d0d8(u32 * param1, u16 param2, u16 param3, s32 param4, u16 param5, u32 * param6, u8 param7, u8 param8) {
    u32 * dst;
    s16 x;
    u32 mask;
    u8 sh;
    u32 inv;
    s16 cnt;
    s16 col;
    u32 hdr;
    s16 j;
    u16 idx2;
    s16 col0;
    s32 m1b;
    s32 m1;
    u32 c1;
    u32 shr;
    u32 m2;
    s32 xbound;
    s32 vstep;
    u32 stride;
    u32 shl;
    u32 w4;
    s16 j2;
    u16 idxb;
    u16 lim;
    u32 c2;
    u32 cnt0;
    u32 m2b;
    const u32 * tbl;
    u32 * q;
    s16 col2;

    shl = (u16)((param2 & 7) << 2);
    hdr = param6[0];
    stride = param5;
    u16 h = hdr & 15;
    u32 w = (u16)((hdr & 0xf0) >> 4);
    u32 t = param7 * w;
    shr = (u16)(32 - shl);
    u32 t2 = h * t;
    param6 = param6 + 1 + ((hdr & 0xff000000) >> 24) + t2;
    param1 = param1 + ((param2 & 0xfff8) + (param3 & 0xfff8) * stride);
    param3 = param3 & 7;
    vstep = param4 >> 1;
    dst = param1;
    lim = param3 + ((h * 4) << vstep);
    cnt0 = (u16)(w * 4) & 0xfff8;
    cnt = cnt0;
    w4 = (u16)(w * 4);

    if (cnt > 0) {
        c1 = param8;
        col0 = (s16)param3;
        m1 = param4 & 1;
        m2 = param4 & 2;
        xbound = m1 << 3;

        do {
            col = col0;

            if (col < lim) {
                inv = 0xffffffff;

                do {
                    tbl = m1 ? data_0204a4dc : data_0204a4cc;
                    j = 0;

                    do {
                        u16 idx = (col & 0xfff8) * stride + (col & 7);
                        col = col + 1;
                        mask = tbl[j];
                        sh = j;
                        x = 0;

                        if (x <= xbound) {
                            idx2 = (col & 0xfff8) * stride + (col & 7);

                            do {
                                u32 bits = (param6[0] & mask) >> sh;
                                u32 comb = bits * c1 + ((param6[1] & mask) >> sh);

                                if (m1) {
                                    bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                                    bits |= (bits << 4);

                                    comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                                    comb |= (comb << 4);
                                }

                                u32 m15 = bits * 16 - bits;
                                u32 cm0 = inv ^ (m15 << shl);
                                u32 cm1 = inv ^ (m15 >> shr);
                                u32 * r = dst + idx + x;

                                (dst + idx)[x] = (dst + idx)[x] & cm0;
                                r[8] = r[8] & cm1;
                                (dst + idx)[x] = (dst + idx)[x] + (comb << shl);
                                r[8] = r[8] + (comb >> shr);

                                if (m2) {
                                    u32 k = x + idx2;
                                    u32 * r2 = dst + k;

                                    dst[k] = dst[k] & cm0;
                                    r2[8] = r2[8] & cm1;
                                    dst[k] = dst[k] + (comb << shl);
                                    r2[8] = r2[8] + (comb >> shr);
                                }

                                mask = mask << 16;
                                sh = sh + 16;
                                x = x + 8;
                            } while (x <= xbound);
                        }

                        col = col + vstep;
                        j = j + 1;
                    } while (j < 4);

                    param6 += 2;
                } while (col < lim);
            }

            cnt = cnt - 8;
            dst += 8;
        } while (cnt > 0);
    }

    if (w4 & 4) {
        m1b = param4 & 1;
        q = param1 + (cnt0 << m1b);
        col2 = (s16)param3;

        if (col2 < lim) {
            c2 = param8;
            m2b = param4 & 2;

            do {
                j2 = 0;
                u32 inv2 = 0xffffffff;

                do {
                    idxb = (col2 & 0xfff8) * stride + (col2 & 7);
                    col2 = col2 + 1;
                    u32 v = param6[0];
                    u32 bits = (v & data_0204a4dc[j2]) >> j2;
                    u32 comb = bits * c2 + ((v & data_0204a4ec[j2]) >> (j2 + 16));

                    if (m1b) {
                        bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                        bits |= (bits << 4);

                        comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                        comb |= (comb << 4);
                    }

                    u32 m15 = bits * 16 - bits;
                    u32 cm0 = inv2 ^ (m15 << shl);
                    u32 cm1 = inv2 ^ (m15 >> shr);
                    u32 * r = q + idxb;

                    q[idxb] = q[idxb] & cm0;
                    r[8] = r[8] & cm1;
                    q[idxb] = q[idxb] + (comb << shl);
                    r[8] = r[8] + (comb >> shr);

                    if (m2b) {
                        u16 idx3 = (col2 & 0xfff8) * stride + (col2 & 7);
                        col2 = col2 + 1;
                        u32 * r3 = q + idx3;

                        q[idx3] = q[idx3] & cm0;
                        r3[8] = r3[8] & cm1;
                        q[idx3] = q[idx3] + (comb << shl);
                        r3[8] = r3[8] + (comb >> shr);
                    }

                    j2 = j2 + 1;
                } while (j2 < 4);

                param6 += 1;
            } while (col2 < lim);
        }
    }
}

// MATCHED 100%
void func_0200d63c(u32 * param1, s32 param2, u16 param3, s32 param4, u16 param5, u32 * param6, u8 param7, u8 param8) {
    u32 hdr;
    u32 * p;
    s16 x;
    u32 mask;
    u32 shl;
    u32 local40;
    u8 sh;
    s32 local3c;
    s16 local38;
    u32 bits;
    u32 shr;
    const u32 * local00;
    u32 * local34;
    s16 local30;
    u32 local2c;
    u32 local28;
    u32 inv2;
    u32 inv;
    s32 m1;
    u32 c;
    u32 m2;
    u32 local18;
    u32 stride;
    s32 local14;
    s32 local10;
    u32 * p2;
    u32 local0c;
    s16 j;
    u32 * local08;
    u32 c2;
    u32 bitsb;
    s16 local04;
    u32 m2b;
    s32 m1b;

    local40 = param5;
    hdr = param6[0];
    u32 off = param3 * local40;
    u32 w = (u16)((hdr & 0xf0) >> 4);
    shl = (u16)((param2 & 7) << 2);
    local0c = local40 << (param4 >> 1);
    u32 t = param7 * w;
    shr = (u16)(32 - shl);
    u16 h = hdr & 15;
    u32 t2 = h * t;
    local28 = (u16)local0c;
    param6 = param6 + 1 + ((hdr & 0xff000000) >> 24) + t2;
    u32 off2 = off + (param2 >> 3);
    param1 = param1 + off2;
    local34 = param1;
    local10 = (u16)(h * 4);
    local14 = (u16)(w * 4);
    local3c = (s16)(local14 & 0xfff8);

    if (local3c > 0) {
        c = param8;
        m1 = param4 & 1;
        m2 = param4 & 2;

        do {
            local38 = 0;

            if (local38 < local10) {
                local18 = local38;
                inv = 0xffffffff;

                do {
                    p = local34 + local18;
                    local00 = m1 ? data_0204a4dc : data_0204a4cc;
                    local30 = 0;

                    do {
                        mask = local00[local30];
                        sh = local30;
                        x = 0;

                        if (x <= m1) {
                            u32 * q = p + local40;

                            do {
                                bits = (param6[0] & mask) >> sh;
                                u32 comb = bits * c + ((param6[1] & mask) >> sh);

                                if (m1) {
                                    bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                                    bits |= (bits << 4);

                                    comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                                    comb |= (comb << 4);
                                }

                                u32 m15 = bits * 16 - bits;
                                u32 cm0 = inv ^ (m15 << shl);
                                u32 cm1 = inv ^ (m15 >> shr);
                                u32 * r = p + x;

                                p[x] = p[x] & cm0;
                                r[1] = r[1] & cm1;
                                p[x] = p[x] + (comb << shl);
                                r[1] = r[1] + (comb >> shr);

                                if (m2) {
                                    u32 * r2 = q + x;

                                    q[x] = q[x] & cm0;
                                    r2[1] = r2[1] & cm1;
                                    q[x] = q[x] + (comb << shl);
                                    r2[1] = r2[1] + (comb >> shr);
                                }

                                mask = mask << 16;
                                sh = sh + 16;
                                x = x + 1;
                            } while (x <= m1);
                        }

                        p += local0c;
                        local30 = local30 + 1;
                    } while (local30 < 4);

                    param6 += 2;
                    local18 += local28 * 4;
                    local38 = local38 + 4;
                } while (local38 < local10);
            }

            local34 += 1;
            local3c = (s16)(local3c - 8);
        } while (local3c > 0);
    }

    if (local14 & 4) {
        m1b = param4 & 1;
        local08 = param1 + ((local14 >> 3) << m1b);
        local04 = 0;

        if (local04 < local10) {
            local2c = local04;
            m2b = param4 & 2;
            c2 = param8;
            stride = param5;

            do {
                inv2 = 0xffffffff;
                j = 0;
                p2 = local08 + local2c;

                do {
                    u32 v = param6[0];
                    bitsb = (v & data_0204a4dc[j]) >> j;
                    u32 comb = bitsb * c2 + ((v & data_0204a4ec[j]) >> (j + 16));

                    if (m1b) {
                        bitsb = (bitsb & 15) + ((bitsb & 0xf0) << 4) + ((bitsb & 0xf00) << 8) + ((bitsb & 0xf000) << 12);
                        bitsb |= (bitsb << 4);

                        comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                        comb |= (comb << 4);
                    }

                    u32 m15 = bitsb * 16 - bitsb;
                    u32 cm0 = inv2 ^ (m15 << shl);
                    u32 cm1 = inv2 ^ (m15 >> shr);

                    p2[0] = p2[0] & cm0;
                    p2[1] = p2[1] & cm1;
                    p2[0] = p2[0] + (comb << shl);
                    p2[1] = p2[1] + (comb >> shr);

                    if (m2b) {
                        u32 * r2 = p2 + stride;

                        p2[stride] = p2[stride] & cm0;
                        r2[1] = r2[1] & cm1;
                        p2[stride] = p2[stride] + (comb << shl);
                        r2[1] = r2[1] + (comb >> shr);
                    }

                    p2 += local0c;
                    j = j + 1;
                } while (j < 4);

                local04 = local04 + 4;
                param6 += 1;
                local2c += local28 * 4;
            } while (local04 < local10);
        }
    }
}

#pragma opt_common_subs reset
#pragma optimization_level reset
