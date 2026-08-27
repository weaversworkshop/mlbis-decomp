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
    u8 field00;
    u8 pad_01[0x3];
    u32 field04;
    u32 field08;
} UnkStruct_0200baf0;


typedef struct {
    union {
        u32 field00;
        struct {
            u16 field00h;
            u16 field02h;
        };
    };
    u8 * field04;
} UnkStruct_0200bce8_24;

typedef struct {
    u16 field00;
    union {
        u16 field02;
        struct {
            u8 field02b;
            u8 field03b;
        };
    };
} UnkStruct_0200bce8_18;

typedef struct {
    u16 field00;
    u16 field02;
} UnkStruct_0200bce8_1C;

typedef struct {
    union {
        u16 field00;
        struct {
            u8 field00b;
            u8 field01b;
        };
    };
    u16 field02;
} UnkStruct_0200bce8_20;

typedef struct {
    UnkStruct_0200bce8_18 struct18;  // +0x0
    UnkStruct_0200bce8_1C struct1c;  // +0x4
    UnkStruct_0200bce8_20 struct20;  // +0x8
    UnkStruct_0200bce8_24 out;  // +0xc
} UnkStruct_0200bce8;

// --- Extern Data Variables --- //

extern u32 data_0204a4cc[];  // 0x0204a4cc
extern u32 data_0204a4dc[];  // 0x0204a4dc
extern u32 data_0204a4ec[];  // 0x0204a4ec

// --- Extern Functions --- //

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

// --- Variable Definitions --- //

// --- Function Declarations --- //

void func_0200b9f4(UnkStruct_0200b8bc *);  // 0x0200b9f4
void func_0200baf0(UnkStruct_0200baf0 *, u32, u32, u32, u32);  // 0x0200baf0
u16 func_0200bbb0(UnkStruct_0200b8bc *);  // 0x0200bbb0
u16 func_0200bc48(UnkStruct_0200b8bc *);  // 0x0200bc48
void func_0200bce8(u8 *, UnkStruct_0200b8bc *, u32, u32);  // 0x0200bce8
s16 func_0200c330(u16, u16, u8, u8, u32, u32, u32 *, u8, BOOL, u8);  // 0x0200c330
u32 func_0200c4b8(UnkStruct_0200b8bc *, UnkStruct_0200bce8_24 *, u32);  // 0x0200c4b8

void func_0200cb30(u32 *, u16, u16, u16, u32 *, u8, u8);  // 0x0200cb30
void func_0200ce04(u32 *, s32, u16, u16, u32 *, u8, u8);  // 0x0200ce04
void func_0200d0d8(u32 *, u16, u16, s32, u16, u32 *, u8, u8);  // 0x0200d0d8
void func_0200d63c(u32 *, s32, u16, s32, u16, u32 *, u8, u8);  // 0x0200d63c

// --- Function Definitions --- //

// MATCHED 100%
void func_0200b8bc(UnkStruct_0200b8bc * param1, u32 param2, u32 param3, const char * param4, u8 param5, u8 param6, u8 param7, u8 param8, u8 param9, u8 param10, u8 param11, u8 param12, u8 param13, u8 param14, UnkStruct_0200b8bc_2c * param15) {
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
void func_0200baf0(UnkStruct_0200baf0 * param1, u32 param2, u32 param3, u32 param4, u32 param5) {
    u16 idx = param2-1;
    param1[idx].field00 = param3;
    param1[idx].field04 = param4;
    param1[idx].field08 = param5;
}

// MATCHED 100%
void func_0200bb1c(UnkStruct_0200b8bc * param1, UnkStruct_0200b8bc_24 * param2) {
    u8 scratch[8];
    UnkStruct_0200b8bc local;

    if (param1 < &local) {
        func_01ff8380(param1, &local, sizeof(local));
    } else {
        func_01ff82ac(param1, &local, sizeof(local));
    }

    while (local.field04[0] != '\0') {
        func_0200bce8(scratch, &local, 1, 1);
    }

    if (&local.unkStruct24 < param2) {
        func_01ff8380(&local.unkStruct24, param2, sizeof(UnkStruct_0200b8bc_24));
    } else {
        func_01ff82ac(&local.unkStruct24, param2, sizeof(UnkStruct_0200b8bc_24));
    }
}

// MATCHED 100%
u16 func_0200bbb0(UnkStruct_0200b8bc * param1) {
    u8 scratch[8];
    UnkStruct_0200b8bc local;

    if (param1 < &local) {
        func_01ff8380(param1, &local, sizeof(local));
    } else {
        func_01ff82ac(param1, &local, sizeof(local));
    }

    local.field1c_21 = 1;

    while (local.field04[0] != 0) {
        func_0200bce8(scratch, &local, 1, 1);
    }

    return local.unkStruct24.field28_16 + (local.unkStruct24.field24_0 << 8);
}

// MATCHED 100%
u16 func_0200bc48(UnkStruct_0200b8bc * param1) {
    u8 scratch[8];
    UnkStruct_0200b8bc local;

    if (param1 < &local) {
        func_01ff8380(param1, &local, sizeof(local));
    } else {
        func_01ff82ac(param1, &local, sizeof(local));
    }

    local.field1c_21 = 1;

    while (TRUE) {
        if ((local.field04[0] == 0xff && local.field04[1] == 0) || local.field04[0] == 0) {
            break;
        }

        func_0200bce8(scratch, &local, 1, 1);
    }

    return local.field08_0 - param1->field08_0;
}


void func_0200bce8(u8 * param1, UnkStruct_0200b8bc * param2, u32 param3, u32 param4) {
    UnkStruct_0200bce8_24 outv = {0};
    UnkStruct_0200bce8_20 s20;
    struct {
        UnkStruct_0200bce8_18 struct18;
        UnkStruct_0200bce8_1C struct1c;
    } w;

    switch (param2->field04[0]) {
        case 0:
            break;
        case 0x20: {
            u8 val = param2->field0c_4 - 1;

            param2->field20_0 = param2->field08_0;
            param2->field20_10 = param2->field08_10;

            if (param2->field1c_22 == 0x3ff) {
                param2->field1c_22 = param2->field08_0;
            }

            param2->field08_0 = val+1 + param2->field1c_17 + param2->field08_0;
            param2->field20_20 = val;

            param2->field04++;

            outv.field00h = 0xff20;
            outv.field02h = 0xff20;

            UnkStruct_0200b8bc_10 * r3 = param2->field10[(u16)param2->field18_0 * 2];
            u32 r2 = WORD_10(r3, r3->field08[0]/4);

            val = (r2 & 0xf) * 4;

            if (val > param2->field1c_11) {
                param2->field1c_11 = val;
            }
            break;
        }
        case 0xff: {
            param2->field04 += func_0200c4b8(param2, &outv, param4) + 2;
            outv.field02h = outv.field00h;
            break;
        }
        default: {
            outv.field04 = param2->field04;

            param2->field20_10 = param2->field08_10;

            UnkStruct_0200b8bc_10 ** tbl = param2->field10;
            u16 j = param2->field18_0;
            u16 r0 = param2->field04[0];
            UnkStruct_0200b8bc_10 * r12;

            if (r0 < 0x100) {
                r12 = tbl[j*2];
            } else {
                r12 = tbl[j*2+1];
                r0 -= 0x100;
            }

            s32 val = ((u16 *)PTR_10(r12, r12->field04))[r0];

            outv.field00h = val;
            outv.field02h = param2->field04[0];

            if (param4 == 0 && param3 == 0) {
                BOOL flag = (param2->field1c_10) != 0;
                u16 idx = param2->field18_0;

                UnkStruct_0200b8bc_10 ** tbl = param2->field10;
                UnkStruct_0200b8bc_10 * r2;

                if (outv.field02h < 0x100) {
                    r2 = tbl[idx*2];
                } else {
                    r2 = tbl[idx*2+1];
                }

                u32 * ptr = PTR_10(r2, WORD_10(r2, (u8)val + 2));

                w.struct18.field00 = func_0200c330(param2->field08_0, param2->field08_10, param2->field14_0, val >> 8, (u16)param2->field0c_9, param2->field00, ptr, param2->field0c_0, flag, param2->field14_10);
                w.struct1c.field02 = w.struct18.field00;
                s20.field00 = w.struct18.field00;
            } else {
                BOOL flag = (param2->field1c_10) != 0;
                u16 idx = param2->field18_0;
                
                UnkStruct_0200b8bc_10 ** tbl = param2->field10;
                UnkStruct_0200b8bc_10 * r12;

                if (outv.field02h < 0x100) {
                    r12 = tbl[idx*2];
                } else {
                    r12 = tbl[idx*2+1];
                }

                u32 * sec = PTR_10(r12, WORD_10(r12, (u8)val + 2));
                u8 mode = param2->field14_0;

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

                w.struct18.field02b = a;
                w.struct18.field03b = (hdr & 0xf) * 4 - 1;

                if (mode & 1) {
                    w.struct18.field02b = (w.struct18.field02b + 1) * 2 - 1;
                }

                if (mode & 2) {
                    w.struct18.field03b = (w.struct18.field03b + 1) * 2 - 1;
                }

                w.struct1c.field00 = w.struct18.field02;
                s20.field00 = w.struct18.field02;
            }

            param2->field20_0 = param2->field08_0;
            param2->field20_10 = param2->field08_10;

            if (param2->field1c_22 == 0x3ff) {
                param2->field1c_22 = param2->field08_0;
            }

            param2->field08_0 = s20.field00b+1 + param2->field1c_17 + param2->field08_0;
            param2->field20_20 = s20.field00b;
            param2->field20_26 = s20.field01b;

            s20.field00 += 0x10001;

            if (s20.field01b > param2->field1c_11) {
                param2->field1c_11 = s20.field01b;
            }

            param2->field04++;
            
            if (param2->unkStruct24.field24_8 > param2->field1c_22) {
                param2->unkStruct24.field24_8 = param2->field1c_22;
            }

            if (param2->field08_0 - param2->unkStruct24.field24_8 > param2->unkStruct24.field28_0) {
                param2->unkStruct24.field28_0 = param2->field08_0 - param2->unkStruct24.field24_8;
            }

            param2->unkStruct24.field28_16 = param2->field08_10 - param2->unkStruct24.field24_18 + param2->field1c_11;
            break;
        }
    }

    *(UnkStruct_0200bce8_24 *)param1 = outv;
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
u32 func_0200c4b8(UnkStruct_0200b8bc * param1, UnkStruct_0200bce8_24 * param2, u32 param3) {
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

        if (param3 == 0) {
            param2->field00h = (cmd & 31) | 0xff00;
            param2->field04 = param1->field04 + 2;
        }
    } else if (cmd < 0x30) {
        if (param3 == 0) {
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



void func_0200cb30(u32 * param1, u16 param2, u16 param3, u16 param4, u32 * param5, u8 param6, u8 param7) {
    u32 inv;
    s16 cnt;
    u32 c2;
    u32 c1;
    s16 col0;
    u16 w4;
    u16 lim;
    u32 * src;
    UnkStruct_0200cb30 hdr;

    hdr.field00 = param5[0];

    u32 w = ((hdr.field00 & 0xf0) << 12) >> 16;
    src = param5+1 + ((hdr.field00 & 0xff000000) >> 24);
    u32 * dst = param1 + ((param2 & 0xfff8) + (param3 & 0xfff8) * param4);
    u32 shl = (u16)((param2 & 7) << 2);
    param3 = param3 & 7;
    u32 t = param6 * w;
    u16 h = hdr.field00 & 15;
    lim = param3 + h * 4;
    u32 t2 = h * t;
    src = src + t2;
    u32 shr = (u16)(32 - shl);
    cnt = (u16)(w << 2) & 0xfff8;

    w4 = w << 2;

    if (cnt > 0) {
        c1 = param7;
        col0 = (s16)param3;

        do {
            s16 col = col0;

            if (col < lim) {
                inv = 0xffffffff;

                do {
                    s16 k = 0;

                    do {
                        u16 idx = (col & 0xfff8) * param4 + (col & 7);
                        u32 * d = &dst[idx];

                        col = col + 1;

                        u32 mask = data_0204a4cc[k];
                        u32 bits = (src[0] & mask) >> k;
                        u32 comb = bits * c1;
                        u32 m15 = bits * 16 - bits;

                        dst[idx] = dst[idx] & (inv ^ (m15 << shl));
                        d[8] = d[8] & (inv ^ (m15 >> shr));
                        u32 val = comb + ((src[1] & mask) >> k);
                        dst[idx] = dst[idx] + (val << shl);
                        d[8] = d[8] + (val >> shr);

                        k = k + 1;
                    } while (k < 4);

                    src += 2;
                } while (col < lim);
            }

            cnt = cnt - 8;
            dst += 8;
        } while (cnt > 0);
    }

    if (w4 & 4) {
        s16 col = (s16)param3;

        if (col < lim) {
            c2 = param7;

            do {
                s16 k = 0;
                inv = 0xffffffff;

                do {
                    u16 idx = (col & 0xfff8) * param4 + (col & 7);
                    u32 bits = (src[0] & data_0204a4dc[k]) >> k;
                    u32 m15 = bits * 16 - bits;

                    dst[idx] = dst[idx] & (inv ^ (m15 << shl));
                    u32 comb = bits * c2;
                    dst[idx + 8] = dst[idx + 8] & (inv ^ (m15 >> shr));
                    col = col + 1;
                    u32 val = comb + ((src[0] & data_0204a4ec[k]) >> (k + 16));
                    dst[idx] = dst[idx] + (val << shl);
                    dst[idx + 8] = dst[idx + 8] + (val >> shr);

                    k = k + 1;
                } while (k < 4);

                src += 1;
            } while (col < lim);
        }
    }
}


void func_0200ce04(u32 * param1, s32 param2, u16 param3, u16 param4, u32 * param5, u8 param6, u8 param7) {
    u16 w4;
    u16 lim;
    u32 * src;
    UnkStruct_0200cb30 hdr;

    hdr.field00 = param5[0];

    u32 w = ((hdr.field00 & 0xf0) << 12) >> 16;
    u32 shl = (u16)((param2 & 7) << 2);
    u32 * dst = param1 + (param2 >> 3);
    u32 h = hdr.field00 & 15;
    src = param5 + 1 + ((hdr.field00 & 0xff000000) >> 24);
    lim = param3 + h * 4;
    u32 shr = (u16)(32 - shl);
    u32 t = param6 * w;
    u32 t2 = h * t;
    src = src + t2;
    s16 cnt = (u16)(w << 2) & 0xfff8;
    w4 = w << 2;

    if (cnt > 0) {
        u32 c1 = param7;
        s16 col0 = (s16)param3;

        do {
            s16 col = col0;

            if (col < lim) {
                volatile u32 rowbase = col * param4;
                u32 inv = 0xffffffff;

                do {
                    u32 * p = dst + rowbase;
                    s16 k = 0;

                    do {
                        u32 mask = data_0204a4cc[k];
                        u32 bits = (src[0] & mask) >> k;
                        u32 m15 = bits * 16 - bits;
                        u32 comb = bits * c1;

                        p[0] = p[0] & (inv ^ (m15 << shl));
                        p[1] = p[1] & (inv ^ (m15 >> shr));
                        u32 val = comb + ((src[1] & mask) >> k);
                        p[0] = p[0] + (val << shl);
                        p[1] = p[1] + (val >> shr);

                        p += param4;
                        k = k + 1;
                    } while (k < 4);

                    src += 2;
                    rowbase += param4 * 4;
                    col = col + 4;
                } while (col < lim);
            }

            cnt = cnt - 8;
            dst += 1;
        } while (cnt > 0);
    }

    if (w4 & 4) {
        s16 col = (s16)param3;

        if (col < lim) {
            u32 c2 = param7;
            volatile u32 rowbase = col * param4;

            do {
                u32 * p = dst + rowbase;
                s16 k = 0;
                u32 inv = 0xffffffff;

                do {
                    u32 bits = (src[0] & data_0204a4dc[k]) >> k;
                    u32 m15 = bits * 16 - bits;

                    p[0] = p[0] & (inv ^ (m15 << shl));
                    u32 comb = bits * c2;
                    p[1] = p[1] & (inv ^ (m15 >> shr));
                    u32 val = comb + ((src[0] & data_0204a4ec[k]) >> (k + 16));
                    p[0] = p[0] + (val << shl);
                    p[1] = p[1] + (val >> shr);

                    p += param4;
                    k = k + 1;
                } while (k < 4);

                src += 1;
                col = col + 4;
                rowbase += param4 * 4;
            } while (col < lim);
        }
    }
}


void func_0200d0d8(u32 * param1, u16 param2, u16 param3, s32 param4, u16 param5, u32 * param6, u8 param7, u8 param8) {
    u16 w4;
    u16 lim;
    UnkStruct_0200cb30 hdr;

    hdr.field00 = param6[0];

    u32 w = ((hdr.field00 & 0xf0) << 12) >> 16;
    u32 shl = (u16)((param2 & 7) << 2);
    u32 t = param7 * w;
    u16 h = hdr.field00 & 15;
    u16 cnt0 = (u16)(w << 2) & 0xfff8;
    u32 t2 = h * t;
    u32 shr = (u16)(32 - shl);
    u32 * dst;
    s32 vstep = param4 >> 1;
    s16 cnt;

    param1 = param1 + ((param2 & 0xfff8) + (param3 & 0xfff8) * param5);
    param3 = param3 & 7;
    w4 = w << 2;
    param6 = param6 + 1 + ((hdr.field00 & 0xff000000) >> 24) + t2;
    lim = param3 + ((h * 4) << vstep);
    dst = param1;
    cnt = cnt0;

    if (cnt > 0) {
        u32 c1 = param8;
        s16 col0 = (s16)param3;
        u32 m1 = param4 & 1;
        u32 m2 = param4 & 2;
        s32 xbound = m1 << 3;

        do {
            s16 col = col0;

            if (col < lim) {
                do {
                    u32 inv = 0xffffffff;
                    u32 * tbl = m1 ? data_0204a4dc : data_0204a4cc;
                    s16 j = 0;

                    do {
                    u16 idx = (col & 0xfff8) * param5 + (col & 7);
                    u32 * p = dst + idx;

                    col = col + 1;

                    u32 mask = tbl[j];
                    u8 sh = j;
                    s16 x;

                    for (x = 0; x <= xbound; x += 8) {
                        u16 idx2 = (col & 0xfff8) * param5 + (col & 7);
                        u32 bits = (param6[0] & mask) >> sh;
                        u32 comb = bits * c1 + ((param6[1] & mask) >> sh);

                        if (m1) {
                            bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                            bits = bits | (bits << 4);
                            comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                            comb = comb | (comb << 4);
                        }

                        u32 m15 = bits * 16 - bits;
                        u32 cm0 = inv ^ (m15 << shl);
                        u32 cm1 = inv ^ (m15 >> shr);

                        p[x] = p[x] & cm0;
                        p[x + 8] = p[x + 8] & cm1;
                        p[x] = p[x] + (comb << shl);
                        p[x + 8] = p[x + 8] + (comb >> shr);

                        if (m2) {
                            dst[idx2 + x] = dst[idx2 + x] & cm0;
                            dst[idx2 + x + 8] = dst[idx2 + x + 8] & cm1;
                            dst[idx2 + x] = dst[idx2 + x] + (comb << shl);
                            dst[idx2 + x + 8] = dst[idx2 + x + 8] + (comb >> shr);
                        }

                        sh = sh + 16;
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
        u32 m1 = param4 & 1;
        u32 * q = param1 + (cnt0 << m1);
        s16 col = (s16)param3;

        if (col < lim) {
            u32 c2 = param8;
            u32 m2 = param4 & 2;
            u32 inv = 0xffffffff;

            do {
                s16 j = 0;

                do {
                u16 idx = (col & 0xfff8) * param5 + (col & 7);

                col = col + 1;

                u32 bits = (param6[0] & data_0204a4dc[j]) >> j;
                u32 comb = bits * c2;

                    comb = comb + ((param6[0] & data_0204a4ec[j]) >> (j + 16));

                if (m1) {
                    bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                    bits = bits | (bits << 4);
                    comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                    comb = comb | (comb << 4);
                }

                u32 m15 = bits * 16 - bits;
                u32 cm0 = inv ^ (m15 << shl);
                u32 cm1 = inv ^ (m15 >> shr);

                q[idx] = q[idx] & cm0;
                q[idx + 8] = q[idx + 8] & cm1;
                q[idx] = q[idx] + (comb << shl);
                q[idx + 8] = q[idx + 8] + (comb >> shr);

                if (m2) {
                    u16 idx3 = (col & 0xfff8) * param5 + (col & 7);

                    col = col + 1;

                    q[idx3] = q[idx3] & cm0;
                    q[idx3 + 8] = q[idx3 + 8] & cm1;
                    q[idx3] = q[idx3] + (comb << shl);
                    q[idx3 + 8] = q[idx3 + 8] + (comb >> shr);
                }

                    j = j + 1;
                } while (j < 4);

                param6 += 1;
            } while (col < lim);
        }
    }
}


void func_0200d63c(u32 * param1, s32 param2, u16 param3, s32 param4, u16 param5, u32 * param6, u8 param7, u8 param8) {
    u16 local14;    // sp+0x14
    s16 local3c;    // sp+0x3c
    u32 local0c;    // sp+0x0c
    u32 * local34;  // sp+0x34
    s16 local30;    // sp+0x30
    u16 local28;    // sp+0x28
    u16 local10;    // sp+0x10
    
    u32 shl = (u16)((param2 & 7) * 4);

    local0c = param5 << (param4 >> 1);

    local28 = local0c;

    u32 shr = (u16)(32 - shl);

    u32 w = ((param6[0] & 0xf0) << 12) >> 16;

    s16 x;

    u32 off = (param3 * param5);

    off += param2 >> 3;

    local10 = (u16)((param6[0] & 0xf) * 4);

    param1 += off;

    param6 = &param6[1] + (param6[0] & 0xf) * (param7 * w) + ((param6[0] & 0xff000000) >> 24);

    local34 = param1;

    local14 = (u16)(w * 4);

    local3c = (u16)(w * 4) & 0xfff8;

    if (local3c > 0) {
        s16 local38;
        do {
            for (local38 = 0; local38 < (s32)local10; local38 += 4) {
                u32 * p = local34 + local28 * local38;
                u32 * local00 = (param4 & 1) ? data_0204a4dc : data_0204a4cc;

                for (local30 = 0; local30 < 4; p += local0c, local30++) {
                    u32 mask = local00[local30];
                    u8 sh = local30;

                    for (x = 0; x <= (param4 & 1); x++) {
                        u32 bits = (param6[0] & mask) >> sh;
                        u32 comb = bits * param8 + ((param6[1] & mask) >> sh);

                        if (param4 & 1) {
                            bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                            bits |= (bits << 4);

                            comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                            comb |= (comb << 4);
                        }

                        u32 cm0 = 0xffffffff ^ ((bits * 16 - bits) << shl);
                        u32 cm1 = 0xffffffff ^ ((bits * 16 - bits) >> shr);

                        p[x] &= cm0;
                        p[x + 1] &= cm1;

                        p[x] += (comb << shl);
                        p[x + 1] += (comb >> shr);

                        if (param4 & 2) {
                            p[param5 + x] &= cm0;
                            p[param5 + x+1] &= cm1;

                            p[param5 + x] += (comb << shl);
                            p[param5 + x+1] += (comb >> shr);
                        }
                        
                        sh = sh + 16;
                        mask <<= 16;
                    }
                }

                param6 += 2;
            }

            local34++;
            local3c -= 8;
        } while (local3c > 0);
    }

    if (local14 & 4) {
        u32 * local08 = &param1[(local14 >> 3) << (param4 & 1)];
        s16 local04;
        w = param5;

        for (local04 = 0; local04 < (s32)local10; param6++, local04 += 4) {
            u32 * p = &local08[local28 * local04];

            s16 j;

            for (j = 0; j < 4; j++) {
                u32 bits = (param6[0] & data_0204a4dc[j]) >> j;
                u32 comb = bits * param8 + ((param6[0] & data_0204a4ec[j]) >> (j + 16));

                if (param4 & 1) {
                    bits = (bits & 15) + ((bits & 0xf0) << 4) + ((bits & 0xf00) << 8) + ((bits & 0xf000) << 12);
                    bits |= (bits << 4);

                    comb = (comb & 15) + ((comb & 0xf0) << 4) + ((comb & 0xf00) << 8) + ((comb & 0xf000) << 12);
                    comb |= (comb << 4);
                }

                u32 cm0 = 0xffffffff ^ ((bits * 16 - bits) << shl);
                u32 cm1 = 0xffffffff ^ ((bits * 16 - bits) >> shr);

                p[0] &= cm0;
                p[1] &= cm1;

                p[0] += (comb << shl);
                p[1] += (comb >> shr);

                if (param4 & 2) {

                    w = param5;

                    p[w] &= cm0;
                    p[w+1] &= cm1;

                    p[w] += (comb << shl);
                    p[w+1] += (comb >> shr);
                }

                p += local0c;
            }
        }
    }
}
