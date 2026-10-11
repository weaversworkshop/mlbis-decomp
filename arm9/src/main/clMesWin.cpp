// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clMesWin.h"

#include "main/clGameMain.h"

#include "main/heap.h"
#include "main/sound.h"

extern "C" {

#include <nitro.h>

#include "main/cpu_mem32.h"

// --- Struct Definitions --- //

typedef struct {
    struct {
        u32 field00_0 : 1;
        u32 field00_1 : 4;
        u32 field00_5 : 5;
        u32 field00_10 : 5;
        u32 field00_15 : 3;
        u32 : 14;
    };
    struct {
        u32 field04_0 : 4;
        u32 field04_4 : 1;
        u32 field04_5 : 3;
        u32 field04_8 : 1;
        u32 field04_9 : 8;
        u32 field04_17 : 11;
        u32 field04_28 : 4;
    };
    struct {
        u32 field08_0 : 1;
        u32 field08_1 : 9;
        u32 field08_10 : 9;
        u32 field08_19 : 8;
        u32 field08_27 : 1;
        u32 : 4;
    };
    u32 field0c;
    u32 field10;
    u32 field14;
    u32 field18;
    u32 field1c;
    u16 field20;
    u8 pad_22[0x2];
    u32 field24;
} UnkStruct_0200dcdc;

typedef struct {
    u8 pad_00[0x8];
    s16 field08;
    s16 field0a;
} UnkStruct_0201485c;

typedef struct {
    struct {
        u32 field00_0 : 24;
        u32 field00_24 : 8;
    };
    u16 field04;
    u16 field06;
} UnkStruct_020502c8_Entry;

typedef struct {
    u32 : 2;
    u32 field00_2 : 2;
    u32 field00_4 : 1;
    u32 field00_5 : 1;
    u32 field00_6 : 2;
    u32 field00_8 : 2;
    u32 field00_10 : 2;
    u32 : 2;
    u32 field00_14 : 4;
    u32 : 14;

    s16 field04;
    s16 field06;
} UnkStruct_020502c8_08;

typedef struct {
    u32 field00;
    u32 field04;
} UnkStruct_020502c8_Tbl;

// --- Extern Data Variables --- //

extern s16 data_0204a80c[];

extern s16 data_0204a824[];

extern u32 data_020500c4[];

extern u32 data_020500dc[];

extern u32 data_02050118;
extern UnkStruct_0205011c * data_0205011c[];

extern void * data_02050144;

extern void * data_02050204;
extern void * data_02050208;

extern YanaRes data_020502c8;

extern void * data_020506bc;

extern void * data_0205133c;

extern u8 data_0205c25c[];

extern u32 data_0205c234[];

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

extern void func_02028950(UnkStruct_0205011c *, u32, u32, u32);

extern u32 func_0202af48(u32, u32, u32, YanaRes *);

extern u32 func_0202afbc(u32, u32, u32, void *);

extern u32 func_0202b170(YanaRes *, void *);

extern void func_0202d10c(u32, u32, void *, void *, void *, u32, u32, u32, u8);

// extern u32 func_0204536c(s16, u8);

// --- Variable Definitions --- //

u32 data_0204ac60[2] = { HW_OBJ_VRAM, HW_DB_OBJ_VRAM };  // 0x0204ac60
vu32 * data_0204ac68[2] = { (vu32 *)REG_DISPCNT_ADDR, (vu32 *)REG_DB_DISPCNT_ADDR };  // 0x0204ac68

// --- Function Declarations --- //

void func_0200dcdc(u32, UnkStruct_0200dcdc *);  // 0x0200dcdc

void func_0200e238(UnkStruct_0205011c *, u32, u32, u32);  // 0x0200e238
void func_0200e35c(UnkStruct_0205011c *, u32, u32, u32);  // 0x0200e35c

void func_0200fa40(u32, UnkStruct_02013540 *, u32);  // 0x0200fa40
void func_0200fb90(void);  // 0x0200fb90
void func_0200fbf4(void);  // 0x0200fbf4
void func_0200fc58(u32, UnkStruct_02013540 *, UnkStruct_0200b8bc *, u32);  // 0x0200fc58

void func_02011320(void *, void *, s32);  // 0x02011320

void func_02013540(clMesWin *, UnkStruct_02013540 *, u8, u8, BOOL);  // 0x02013540

u32 func_020139fc(clMesWin *, UnkStruct_0200b8bc *, BOOL);  // 0x020139fc
u32 func_02013a84(clMesWin *, UnkStruct_0200b8bc *, BOOL);  // 0x02013a84

void func_02013b0c(clMesWin *, UnkStruct_0200b8bc *, UnkStruct_02013540_60 *);

void func_02014070(UnkStruct_0205011c *, u32, u32, u32);  // 0x02014070

// --- Variable Definitions --- //


} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clMesWin::func_0200db0c() {
    if (this->field40_10) {
        return;
    }

    if (this->field40_0 & 4) {
        UnkStruct_02013540 * r0 = (UnkStruct_02013540 *)this->field58.field04;
        u32 r3 = this->field90 + this->field3c_4 * 2 - 1;

        while (r0->field00.field04 != (UnkStruct_0200ec8c_00 *)-1) {
            r0->fieldb8 = r3;

            if (r0->fieldbc.field08_26) {
                r0->fieldbc.field10 = r3-1;
            }

            if (r0->fieldd0.field08_26) {
                r0->fieldd0.field10 = r3-1;
            }

            r0 = (UnkStruct_02013540 *)r0->field00.field04;
            r3 -= 2;
        }
    }

    this->func_0200ea48(this->field58.field04);
    this->func_0200ea48(this->field68.field04);

    this->field04->func_02013dc8();

    this->field98h = this->field08h;
    this->field9a = this->field0a;
    this->field9ch = this->field0ch;
    this->field9e = this->field0e;

    if (this->field3c_1) {
        DC_FlushRange(this->field1c, 0x780);
    }

    if (this->field3c_2) {
        DC_FlushRange(this->field20, 0x780);
    }

    this->field40_10 = 1;
}

// MATCHED 100%
void clMesWin::func_0200dc38() {
    this->field40_10 = 0;

    if (this->field3c_0) {
        this->func_02012f54();

        if (this->field3c_1) {
            this->field3c_1 = 0;

            if (this->field1c != NULL) {
                this->func_02012b2c();
            }
        }

        if (this->field3c_2) {
            this->field3c_2 = 0;

            if (this->field20 != NULL) {
                this->func_02012ba8();
            }
        }

        if (this->field3c_3) {
            this->func_02012c24();
        }
    }
}

// MATCHED 100%
void func_0200dcdc(u32 param1, UnkStruct_0200dcdc * param2) {
    param2->field00_0 = 0;
    param2->field00_1 = 1;
    param2->field00_5 = 0;
    param2->field00_10 = 0;
    param2->field00_15 = 2;

    param2->field1c = 0;

    param2->field04_0 = 0;
    param2->field04_4 = 0;
    param2->field04_5 = 0;
    param2->field04_8 = 0;
    param2->field04_9 = 0;
    param2->field04_17 = 0;
    param2->field04_28 = 0;

    param2->field08_1 = 0;
    param2->field08_10 = 0;
    param2->field08_19 = 0;
    param2->field08_0 = 0;

    param2->field0c = 0;
    param2->field10 = 0;
    param2->field14 = 0;
    param2->field18 = 0;

    param2->field24 = 0;

    param2->field08_27 = 0;

    param2->field20 = 0x7fff;
}

// MATCHED 100%
void clMesWin::func_0200dd9c(UnkStruct_0201008c * param1, s16 param2) {
    this->func_0200db0c();

    if (param2 != -1 && this->field4c[param2].fieldb0 != NULL) {
        this->func_020107a0(param2);
        this->func_02010930(this->field4c[param2].field08.field00_0);

        if (this->field4c[param2].field08.field00_0 == 0) {
            this->field3c_1 = 0;
        } else {
            this->field3c_2 = 0;
        }
    }

    this->func_0201008c(param1, param2);
}

// MATCHED 100%
void clMesWin::func_0200de80(u32 param1, u16 param2, u32 param3, u32 param4, u8 param5, u8 param6) {
    u32 nBytes = this->func_0200dee0(param2, param3, param4, param5, param6);

    UnkStruct_02013540 * arr = (UnkStruct_02013540 *)::operator new[](nBytes, param1, NULL, FALSE);

    this->func_0200dfac(arr);

    this->field40_11 = 1;
}

// MATCHED 100%
u32 clMesWin::func_0200dee0(u16 param1, u32 param2, u32 param3, u8 param4, u8 param5) {
    this->field2c = 0;
    this->field30 = 0;

    u32 r12 = param1 * 0x154;

    if (this->field40_0 & 5) {
        r12 += param2;
        this->field2c = param2;
        
        if (param4 == 0) {
            r12 += param2;
        }
    }

    if (this->field40_0 & 2) {
        r12 += param3;
        this->field30 = param3;

        if (param5 == 0) {
            r12 += param3;
        }
    }

    this->field34 = 0;
    this->field36 = 0;

    if (this->field40_0 & 1) {
        this->field34 = 0x780;
        r12 += 0x780;
    }

    if (this->field40_0 & 2) {
        this->field36 = 0x780;
        r12 += 0x780;
    }

    this->field38[0] = param4;
    this->field38[1] = param5;

    this->field50 = param1;
    this->field54 = r12;

    return r12;
}

// MATCHED 100%
void clMesWin::func_0200dfac(UnkStruct_02013540 * param1) {
    func_01ff8228(param1, 0, this->field54);

    u8 * r0 = (u8 *)&param1[this->field50];

    this->field4c = param1;

    if (this->field40_0 == 3 || this->field40_0 == 6) {
        this->field24 = r0;
        this->field28 = r0 + this->field2c;
    } else if (this->field40_0 & 5) {
        this->field24 = r0;
        this->field28 = NULL;
    } else {
        this->field28 = r0;
        this->field24 = NULL;
    }

    r0 += this->field2c + this->field30;

    this->field1c = NULL;
    this->field20 = NULL;

    if (this->field40_0 == 3) {
        this->field1c = r0;
        this->field20 = r0 + this->field34;
    } else {
        if (this->field40_0 & 1) {
            this->field1c = r0;
        }

         if (this->field40_0 & 2) {
            this->field20 = r0;
         }
    }

    r0 += this->field34 + this->field36;

    this->field14 = NULL;
    this->field18 = NULL;

    if (this->field38[0] == 0) {
        this->field14 = r0;
        r0 += this->field2c;
    }

    if (this->field38[1] == 0) {
        this->field18 = r0;
    }
}

// MATCHED 100%
u32 clMesWin::func_0200e12c(const char * param1, UnkStruct_0200b8bc_10 ** param2) {
    UnkStruct_0200b8bc_24 local04;

    this->func_020106fc(param1, param2, &local04, 1);

    u16 r1 = local04.field28_0 + 24;
    if (r1 & 1) {
        r1 += 8;
    }
    r1 = (r1 >> 3) - 1;

    u16 r0 = local04.field28_16 + 16;
    if (r0 & 1) {
        r0 += 8;
    }
    r0 = (r0 >> 3) - 1;

    return (r0 << 16) + r1;
}

// MATCHED 100%
u16 clMesWin::func_0200e1bc(u32 param1, u32 param2) {
    UnkStruct_0205011c * ret = this->func_02010bd4(param1, param2);

    return ret->field00->field00;
}

// MATCHED 100%
void clMesWin::func_0200e1d0(nsObjSys::clObjPltVramMan * param1) {
    if (this->field40_0 & 4 && this->field80 == NULL) {
        this->field78 = param1;

        this->field04->func_02013f00(param1, NULL);

        this->field80 = this->field78->func_02014380(this->field2c, 0x19720329 | 0x80000000, 4);
        this->field88 = this->field80->field08;
    }
}

// MATCHED 100%
void func_0200e238(UnkStruct_0205011c * param1, u32 param2, u32 param3, u32 param4) {
    func_02028950(param1, MATH_ROUNDUP(param2, 8), param3, param4);
}

// MATCHED 100%
void clMesWin::func_0200e24c(BOOL param1, u32 param2, void * param3) {
    if (!param1) {
        this->field3c_22 = param2;

        if (param3 == NULL) {
            GX_LoadBGPltt(this->field48, param2 * 32, 0x20);
        } else {
            GX_LoadBGPltt(param3, param2 * 32, 0x20);
        }
    } else {
        this->field3c_26 = param2;

        if (param3 == NULL) {
            GXS_LoadBGPltt(this->field48, param2 * 32, 0x20);
        } else {
            GXS_LoadBGPltt(param3, param2 * 32, 0x20);
        }
    }
}

// MATCHED 100%
void clMesWin::func_0200e2d8(nsObjSys::clObjPltVramMan * param1, u32 param2) {
    this->field7c = param1;

    this->field04->func_02013f00(NULL, param1);

    u32 r4 = data_02050118;

    this->field84 = this->field7c->func_02014380(r4, 0x19720329, param2);
    this->field8c = this->field84->field08;

    GX_BeginLoadTexPltt();

    GX_LoadTexPltt(this->field48, this->field8c, r4);
    GX_EndLoadTexPltt();

    this->field04->func_02014220(0, param2);
}

// MATCHED 100%
void func_0200e35c(UnkStruct_0205011c * param1, u32 param2, u32 param3, u32 param4) {
    func_02028950(param1, MATH_ROUNDUP(param2, 16), param3, param4);
}

// MATCHED 100%
clMesWin::clMesWin(u32 param1, u32 param2, u32 param3, u8 param4, u8 param5, u32 param6, UnkClass_0201450c * param7) {
    this->field3c = 0;
    this->field40 = 0;

    this->field08 = 0;
    this->field0c = 0;

    this->field98 = 0;
    this->field9c = 0;

    this->field4c = NULL;

    this->field24 = NULL;
    this->field28 = NULL;

    this->field50 = 0;
    this->field54 = 0;

    this->field88 = 0;
    this->field8c = 0;
    this->field90 = 0;
    this->field94 = 1;

    this->field78 = NULL;
    this->field7c = NULL;
    this->field80 = NULL;
    this->field84 = NULL;

    this->field40_0 = param1;

    this->field44 = &data_020506bc;
    this->field48 = &data_02050144;

    this->field3c_14 = param2;
    this->field3c_16 = param3;
    this->field3c_18 = param4;
    this->field3c_20 = param5;

    if (param6 == 1) {
        if (param1 & 1) {
            this->func_0201291c(9, 0);
        }

        if (param1 & 2) {
            this->func_02012a24(9, 0);
        }
    }

    u32 heap = GET_HEADER(this)->heapId;
    this->field04 = param7;

    if (param7 == NULL) {
        this->field04 = new(heap, NULL, FALSE) UnkClass_0201450c((param1 & 4) != 0);
        this->field40_12 = 1;
    }

    this->field3c_0 = 1;

    this->field40_3 = 0;
    this->field40_4 = 0;

    this->field10 = 0x401;
    this->field12 = 2;

    this->field3c_22 = 0xf;
    this->field3c_26 = 0xf;

    this->field58.field00 = (UnkStruct_0200ec8c_00 *)0x11111111;
    this->field58.field04 = &this->field60;
    this->field60.field00 = &this->field58;
    this->field60.field04 = (UnkStruct_0200ec8c_00 *)-1;

    this->field68.field00 = (UnkStruct_0200ec8c_00 *)0x11111111;
    this->field68.field04 = &this->field70;
    this->field70.field00 = &this->field68;
    this->field70.field04 = (UnkStruct_0200ec8c_00 *)-1;

    MI_CpuFill8(this->fielda0, 0, sizeof(this->fielda0));

    func_0200baf0(this->fielda0, 11, 1, NULL, NULL);
    func_0200baf0(this->fielda0, 12, 1, NULL, NULL);
    func_0200baf0(this->fielda0, 15, 2, (UnkCallback_0200b8bc_2c)func_020139fc, this);
    func_0200baf0(this->fielda0, 16, 2, (UnkCallback_0200b8bc_2c)func_02013a84, this);
    func_0200baf0(this->fielda0, 17, 1, NULL, NULL);
}

// MATCHED 100%
void clMesWin::func_0200ec8c(UnkStruct_0200ec8c * param2) {
    if (param2->field08_26) {
        if (param2->field08_25) {
            param2->field08_25 = 0;
        } else {
            s16 r2 = 0, r3 = 0;

            if (!param2->field08_0) {
                if (this->field08h != this->field98h) {
                    r2 = this->field98h - this->field08h;
                }

                if (this->field0a != this->field9a) {
                    r3 = this->field9a - this->field0a;
                }
            } else {
                if (this->field0ch != this->field9ch) {
                    r2 = this->field9ch - this->field0ch;
                }

                if (this->field0e != this->field9e) {
                    r3 = this->field9e - this->field0e;
                }
            }

            param2->field08_1 += r2;
            param2->field08_10 += r3;
        }
    }
}

// MATCHED 100%
BOOL clMesWin::func_0200ed74(UnkStruct_02013540 * param1) {
    if (param1->fielda4_2) {
        param1->fielda4_26 = 1;

        if (param1->fielda8_16 != 0 && param1->fielda8_16 != 3) {
            param1->fielda8_16 = 0;

            if (param1->field08.field1c != 0) {
                param1->field100.stop(2);
            }
        }

        if (param1->field08.field08_0 == 0) {
            param1->fielda4_2 = 0;

            this->func_0200de40(param1->fielda4_21);
        }

        return TRUE;
    }

    return FALSE;
}

// MATCHED 100%
void func_0200fa40(u32 param1, UnkStruct_02013540 * param2, u32 param3) {
    if (param2->field08.field1c == 0) {
        return;
    }

    BOOL flag = (param2->field100.next != NULL || param2->field100.prev != NULL);

    if (flag) {
        param2->field100.stop(0);
        param2->fielda4_28 = (param3 == 1) ? 2 : 1;
        return;
    }

    param2->fielda4_28 = 0;

    s16 r0 = 0x7f;
    if (param2->field08.field04_0) {
        r0 = param2->field08.field20;
        if (param2->field08.field20 == 0x7fff) {
            if (param2->field08.field04_9 == 0) {
                r0 = (param2->field08.field00_5 + 1) * 8 / 2 + param2->field08.field08_1;
            } else {
                r0 = param2->field08.field04_9 + param2->field08.field08_1;
            }
        }
    }

    s32 r1 = param2->field08.field1c;
    if (param3 == 0) {
        param2->fielda8_16 = 1;
    } else {
        param2->fielda8_16 = 3;
        r1++;
    }

    param2->field100.start(r1, 0, 0, 0, r0, 0x60, 0x7f, 0);
}

// MATCHED 100%
void func_0200fb90() {
    if (gameMain->mode != 2) {
        sndPlayById(0x200201d8, 0, 0x7f, 0x60, 0x7f, 0);
    } else {
        sndPlayById(0x2003000a, 0, 0x7f, 0x60, 0x7f, 0);
    }
}

// MATCHED 100%
void func_0200fbf4() {
    if (gameMain->mode != 2) {
        sndPlayById(0x201d9, 0, 0x7f, 0x60, 0x7f, 0);
    } else {
        sndPlayById(0x3000b, 0, 0x7f, 0x60, 0x7f, 0);
    }
}

// MATCHED 100%
void func_0200fc58(u32 param1, UnkStruct_02013540 * param2, UnkStruct_0200b8bc * param3, u32 param4) {
    if (param2->fieldfc_16 == 0) {
        param2->fieldfc_16 = func_0200bbb0(param3);
    }

    u16 r1 = param2->field08.field00_10;

    switch (param4) {
        case 0: {
            param3->field08_10 = param2->fielda0_8;
            break;
        }
        case 1: {
            param3->field08_10 = (u32)((r1+1) * 8 - (u8)param2->fieldfc_16) / 2;

            if (param2->field08.field00_15 == 0) {
                param3->field08_10 += param2->field08.field08_10 & 7;
            }
            break;
        }
        case 2: {
            param3->field08_10 = (r1 + 2 - param2->fielde4[0][2]) * 8;
            param3->field08_10 -= param3->field08_24 * (param2->fieldfc_16 >> 8) + (u8)param2->fieldfc_16 + 1;

            if (param2->field08.field00_15 == 0) {
                param3->field08_10 += param2->field08.field08_10 & 7;
            }
        }
    }

    if (param2->fielda8_21 == 0) {
        param3->unkStruct24.field24_18 = param3->field08_10;
    }
}

// MATCHED 100%
void clMesWin::func_0200fe38(UnkStruct_02013540 * param1, s16 param2, s16 param3, u16 param4, u8 param5, UnkStruct_0200ec8c * param6) {
    if (param1->field08.field00_15 == 0) {
        if (param1->field08.field00_0 == 0) {
            param2 -= this->field08h;
            param3 -= this->field0a;
        } else {
            param2 -= this->field0ch;
            param3 -= this->field0e;
        }
    }

    u8 r12;
    if (param1->field08.field00_15 != 0) {
        if (param1->field08.field00_15 == 1) {
            r12 = 0;
        }
    } else {
        r12 = (param1->field08.field00_0 != 0) ? this->field3c_20 : this->field3c_18;
    }

    this->field04->func_02014410(param6, (u8)param1->field08.field00_0, (s8)param4, param2, param3, param5, r12, param1->fieldb8-1);

    param6->field08_25 = 1;
}

// MATCHED 100%
#pragma optimization_level 3
void clMesWin::func_0200ff3c(UnkStruct_02013540 * param1, s16 param2, s16 param3, u8 param4, u8 param5) {
    if (this->field38[param1->field08.field00_0] != 0) {
        return;
    }

    switch (param1->field08.field00_15) {
        case 0: {
            u16 r9 = (param1->fieldac + 1) * 8;

            u32 r5 = (param3 >> 3) * r9 + (param2 & 0xfff8);
            s32 r8 = (((param4 + (param2 & 7)) >> 3) + 1) << 5;

            u32 p5 = param5;
            s32 p6 = (param3 & 7);
            p6 = p5 + p6;

            s16 r6 = p6 >> 3;

            for (; r6 >= 0; r6--) {
                void * r1 = &param1->fieldb0[r5];
                void * r0 = &param1->fieldb4[r5];

                if (r0 < r1) {
                    func_01ff8380(r0, r1, r8);
                } else {
                    func_01ff82ac(r0, r1, r8);
                }

                r5 += r9;
            }
            break;
        }
        case 1: {
            u32 r5;
            u32 r6;

            u16 r1 = param1->fieldac + 1;
            u32 r7 = param3 * r1 + (param2 >> 3);
            s32 r6_sub = param4 + (param2 & 0x7);

            s16 r0 = param5;
            if (r0 >= 0) {
                r6 = r7 + (r6_sub >> 3);
                do {
                    for (r5 = r6; r5 >= r7; r5--) {
                        param1->fieldb0[r5] = param1->fieldb4[r5];
                    }

                    r6 += r1;
                    r7 += r1;
                } while (--r0 >= 0);
            }
        }
    }
}
#pragma optimization_level reset


void clMesWin::func_0201008c(UnkStruct_0201008c * param1, s16 param2) {
    u8 * r7;
    if (0x3ff == param1->field04_17) {
        r7 = (u8*)param1->field14;
    } else {
        if (param1->field14) {
            r7 = (u8*)param1->field14 + param1->field14[param1->field04_17];
        } else {
            r7 = NULL;
        }
    }
    u16 r4;
    u16 r10;
    if (r7 != NULL) {
        if (0x3ff == param1->field04_17) {
            s32 r0;
            if (param1->field00_5 == 0 || param1->field00_10 == 0) {
                r0 = (u16)this->func_0200e12c((const char *)r7, param1->field10);
            }
            r4 = (param1->field00_5) ? param1->field00_5 : r0;
            r10 = (param1->field00_10) ? param1->field00_10 : r0;
        } else {
            r4 = (param1->field00_5) ? param1->field00_5 : r7[0]+1;
            r10 = (param1->field00_10) ? param1->field00_10 : r7[1]+1;
        }
    } else {
        r4 = param1->field00_5;
        r10 = param1->field00_10;
    }
    if (param1->field00_0 == 0) {
        if (this->field40_0 & 4) {
            param1->field00_15 = 1;
        } else {
            param1->field00_15 = 0;
        }
    } else {
        param1->field00_15 = 0;
    }
    this->field40_5 = param2;
    if (param1->field08_27 == 0) {
        BOOL flag = false;
        if (param1->field04_17 && r7 != NULL) {
            flag = TRUE;
        }
        u8 * local00;
        if (flag) {
            local00 = &r7[2];
        } else {
            local00 = r7;
        }
        // this->func_02013164(param1->field00_1, r4, r10, local00, param1->field08_19);
    }
}

// MATCHED 100%
void clMesWin::func_020106fc(const char * param1, UnkStruct_0200b8bc_10 ** param2, UnkStruct_0200b8bc_24 * param3, u8 param4) {
    UnkStruct_0200b8bc local;
    func_0200b8bc(&local, param2, 0, param1, 0, 0, (param4 == 0) ? 2 : 1, (param4 == 0) ? 3 : 1, 1, 6, 0x40, param4, 0, 0, this->fielda0);
    func_0200bb1c(&local, param3);
}

// MATCHED 100%
bool clMesWin::func_020107a0(s16 param1) {
    UnkStruct_02013540 * r4 = &this->field4c[param1];

    if (this->field3c_4 + this->field3c_9 == 0) {
        return false;
    }

    if (r4->fieldb0 == NULL) {
        return false;
    }

    func_01ff8228(r4->fieldb0, 0, r4->fielda8_0);

    if (this->field38[r4->field08.field00_0] == 0) {
        func_01ff8228(r4->fieldb4, 0, r4->fielda8_0);
    }

    r4->fieldb0 = NULL;

    r4->fielda4_26 = 1;

    if (r4->fielda8_16) {
        r4->fielda8_16 = 0;
        if (r4->field08.field1c != 0) {
            r4->field100.stop(2);
        }
    }

    if (r4->fieldbc.field08_26) {
        r4->fieldbc.field08_26 = 0;
        r4->fieldbc.field00.field00->field04 = r4->fieldbc.field00.field04;
        r4->fieldbc.field00.field04->field00 = r4->fieldbc.field00.field00;
    }

    if (r4->fieldd0.field08_26) {
        r4->fieldd0.field08_26 = 0;
        r4->fieldd0.field00.field00->field04 = r4->fieldd0.field00.field04;
        r4->fieldd0.field00.field04->field00 = r4->fieldd0.field00.field00;
    }

    r4->field00.field00->field04 = r4->field00.field04;
    r4->field00.field04->field00 = r4->field00.field00;

    if (r4->field08.field00_0 == 0) {
        this->field3c_4--;
    } else {
        this->field3c_9--;
    }

    return true;
}

// MATCHED 100%
void clMesWin::func_02010930(u8 param1) {
    UnkStruct_02013540 * r4;

    if (param1 == 0) {
        if (this->field1c != NULL) {
            func_01ff8228(this->field1c, 0, 0x780);
        }

        r4 = (UnkStruct_02013540 *)this->field58.field04;
    } else {
        if (this->field20 != NULL) {
            func_01ff8228(this->field20, 0, 0x780);
        }

        r4 = (UnkStruct_02013540 *)this->field68.field04;
    }

    while (r4->field00.field04 != (UnkStruct_0200ec8c_00 *)-1) {
        if (r4->field08.field00_15 == 0) {
            this->func_020125c0(r4);
        }

        r4 = (UnkStruct_02013540 *)r4->field00.field04;
    }

    u32 r0 = this->field3c;
    if (param1 == 0) {
        r0 |= 2;
    } else {
        r0 |= 4;
    }

    this->field3c = r0;
}

// MATCHED 100%
UnkStruct_0205011c * clMesWin::func_02010bd4(u32 param1, u32 param2) {
    UnkStruct_0205011c * ptr = data_0205011c[param1];

    while (ptr->field04 < param2) {
        ptr++;
    }

    return ptr;
}

// MATCHED 100%
#ifndef IDE_PARSE
asm void func_02011320(register void * src, register void * dst, register s32 stride) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ldmia r0!, {r4, r5, r6, r7, r8, r9, r10, r11}
    mov r2, r2, lsl #0x2
    str r4, [r1, #0x0]
    add r1, r1, r2
    str r5, [r1, #0x0]
    add r1, r1, r2
    str r6, [r1, #0x0]
    add r1, r1, r2
    str r7, [r1, #0x0]
    add r1, r1, r2
    str r8, [r1, #0x0]
    add r1, r1, r2
    str r9, [r1, #0x0]
    add r1, r1, r2
    str r10, [r1, #0x0]
    add r1, r1, r2
    str r11, [r1, #0x0]
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}
#endif

// MATCHED 100%
void clMesWin::func_020125c0(UnkStruct_02013540 * param1) {
    if (param1->field08.field00_15 != 0) {
        return;
    }

    s16 r2_idx;
    s16 r3;

    u16 r6 = param1->field08.field08_1 >> 3;
    u16 r7 = param1->field08.field08_10 >> 3;

    u16 * r12;
    u16 r14 = param1->fielda4_6;
    if (param1->field08.field00_0 == 0) {
        r12 = (u16 *)this->field1c;
        r14 += this->field3c_22 * 4096;
    } else {
        r12 = (u16 *)this->field20;
        r14 += this->field3c_26 * 4096;
    }

    for (r3 = r7; r3 < r7 + param1->fieldad + 1; r3++) {
        for (r2_idx = r6; r2_idx < r6 + param1->fieldac + 1; r2_idx++) {
            r12[r3 * 32 + r2_idx] = r14;
            r14++;
        }
    }
}

// MATCHED 100%
void clMesWin::func_0201291c(u32 param1, u32 param2) {
    switch (this->field3c_14) {
        case 0:
            reg_G2_BG0CNT = reg_G2_BG0CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2_BG0CNT = (reg_G2_BG0CNT & ~3) | this->field3c_18;
            break;
        case 1:
            reg_G2_BG1CNT = reg_G2_BG1CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2_BG1CNT = (reg_G2_BG1CNT & ~3) | this->field3c_18;
            break;
        case 2:
            reg_G2_BG2CNT = reg_G2_BG2CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2_BG2CNT = (reg_G2_BG2CNT & ~3) | this->field3c_18;
            break;
        case 3:
            reg_G2_BG3CNT = reg_G2_BG3CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2_BG3CNT = (reg_G2_BG3CNT & ~3) | this->field3c_18;
    }
}

// MATCHED 100%
void clMesWin::func_02012a24(u32 param1, u32 param2) {
    switch (this->field3c_16) {
        case 0:
            reg_G2S_DB_BG0CNT = reg_G2S_DB_BG0CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2S_DB_BG0CNT = (reg_G2S_DB_BG0CNT & ~3) | this->field3c_20;
            break;
        case 1:
            reg_G2S_DB_BG1CNT = reg_G2S_DB_BG1CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2S_DB_BG1CNT = (reg_G2S_DB_BG1CNT & ~3) | this->field3c_20;
            break;
        case 2:
            reg_G2S_DB_BG2CNT = reg_G2S_DB_BG2CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2S_DB_BG2CNT = (reg_G2S_DB_BG2CNT & ~3) | this->field3c_20;
            break;
        case 3:
            reg_G2S_DB_BG3CNT = reg_G2S_DB_BG3CNT & 0x43 | (param1 << 0x8) | (param2 << 0x2);
            reg_G2S_DB_BG3CNT = (reg_G2S_DB_BG3CNT & ~3) | this->field3c_20;
    }
}

// MATCHED 100%
void clMesWin::func_02012b2c() {
    switch (this->field3c_14) {
        case 0:
            GX_LoadBG0Scr(this->field1c, 0x80, this->field34);
            break;
        case 1:
            GX_LoadBG1Scr(this->field1c, 0x80, this->field34);
            break;
        case 2:
            GX_LoadBG2Scr(this->field1c, 0x80, this->field34);
            break;
        case 3:
            GX_LoadBG3Scr(this->field1c, 0x80, this->field34);
    }
}

// MATCHED 100%
void clMesWin::func_02012ba8() {
    switch (this->field3c_16) {
        case 0:
            GXS_LoadBG0Scr(this->field20, 0x80, this->field36);
            break;
        case 1:
            GXS_LoadBG1Scr(this->field20, 0x80, this->field36);
            break;
        case 2:
            GXS_LoadBG2Scr(this->field20, 0x80, this->field36);
            break;
        case 3:
            GXS_LoadBG3Scr(this->field20, 0x80, this->field36);
    }
}

// MATCHED 100%
void clMesWin::func_02012f54() {
    if (this->field40_3 == 0 && (this->field40_0 & 1)) {
        switch (this->field3c_14) {
            case 0: {
                G2_SetBG0Offset(this->field98h, this->field9a + 0x10);
                break;
            }
            case 1: {
                G2_SetBG1Offset(this->field98h, this->field9a + 0x10);
                break;
            }
            case 2: {
                G2_SetBG2Offset(this->field98h, this->field9a + 0x10);
                break;
            }
            case 3: {
                G2_SetBG3Offset(this->field98h, this->field9a + 0x10);
            }
        }
    }

    if (this->field40_4 == 0 && (this->field40_0 & 2)) {
        switch (this->field3c_16) {
            case 0: {
                G2S_SetBG0Offset(this->field9ch, this->field9e + 0x10);
                break;
            }
            case 1: {
                G2S_SetBG1Offset(this->field9ch, this->field9e + 0x10);
                break;
            }
            case 2: {
                G2S_SetBG2Offset(this->field9ch, this->field9e + 0x10);
                break;
            }
            case 3: {
                G2S_SetBG3Offset(this->field9ch, this->field9e + 0x10);
            }
        }
    }
}

// NONMATCHING -- 92.73%
u32 clMesWin::func_02013164(u8 param1, u8 param2, u8 param3, u8 * param4, u8 param5) {
    u32 local00 = param1;
    u8 r10 = param2;
    u8 r9 = param3;

    u8 r4;
    u8 r5;
    u8 r6;
    u8 r7;
    u8 r11;

    if (param4 != NULL && local00 == 2) {
        u32 r2 = 0;
        u8 r3 = 1;

        do {
            u16 r6 = (param4[0] << 8) | param4[1];

            if (r6 == 0xff0a) {
                break;
            }
            
            if (r6 == 0xff01) {
                r3 = 1;
            } else if (r6 == 0xff11) {
                if (param4[2] == 0) {
                    r3 = 1;
                }
            } else if (r6 == 0xff00) {
                r3++;

                if (r2 < r3) {
                    r2 = r3;
                }
            }
            param4++;
        } while (TRUE);

        if (r2 >= 2) {
            r9++;
        }
    }

    UnkStruct_0205011c * ret = this->func_02010bd4(local00, r9);
    UnkStruct_0205011c_00 * unkstruct = ret->field00;
    r11 = ret->field04;
    u32 sum = unkstruct->field02 + unkstruct->field04;
    r7 = unkstruct->field00 * 2;
    r6 = unkstruct->field1c[0];
    r5 = (unkstruct->field20 != NULL) ? unkstruct->field20[0] : 0;
    r10 += param5 >> 3;
    if (param5 & 1) {
        r10++;
    }
    if (r10 < r7 + r6) {
        r10 = r7 + r6;
    }
    r4 = sum;
    if (r11 == 0xff) {
        if (r9 < r4 + r5) {
            r9 = r4 + r5;
        }
    } else {
        r9 = r11;
    }

    if (local00 == 2) {
        if (param4 != NULL) {
            r10++;
        }

        s16 val = r10-r7;
        if (val % r6 != 0) {
            r10 = r6 * (val / r6 + 1) + r7;
        }

        if (r11 == 0xff) {
            val = r9 - r4;
            if (val % r5 != 0) {
                r9 = r5 * (val / r5 + 1) + r4;
            }
        }
    } else {
        s16 val = (((r10 - r7) << 8) / r6 + 0x80) >> 8;
        r10 = val * r6 + r7;

        if (r11 == 0xff) {
            val = (((r9 - r4) << 8) / r5 + 0x80) >> 8;
            r9 = val * r5 + r4;
        }
    }

    if (r10 > 0x20) {
        r10 -= r6;
    }

    if (r9 > 0x18) {
        r9 -= r5;
    }

    return r10 | (r9 << 16);
}

// MATCHED 100%
void func_02013540(clMesWin * param1, UnkStruct_02013540 * param2, u8 param3, u8 param4, BOOL param5) {
    UnkStruct_02013540_60 * r0 = &param2->field60[param3];
    r0->field08 = r0->field00;

    if (!r0->field0c) {
        r0->field0c = true;

        s32 r4 = param2->field08.field0c[param3];
        bool r5 = true;
        if (r4 < 0) {
            r4 = r4 * (r5-2);
        } else {
            r5 = false;
        }

        if (r4 > data_020500c4[param4]) {
            r4 = data_020500c4[param4];
        }

        u32 * r7 = &data_020500dc[6 - (param4+1)];
        char * r8 = r0->field08;
        bool r6 = false;

        while (*r7 != 0) {
            u8 r2 = r4 / (*r7);
            r4 -= r2 * (*r7);

            if (*r7++ == 1 || r2 != 0) {
                r6 = true;
                if (r5) {
                    r5 = false;
                    *r8++ = 0x2d;
                }
            } else if (r6 == false) {
                if (param5) {
                    *r8++ = 0x20;
                }
                continue;
            }

            *r8++ = r2 + 0x30;
        }

        *r8 = 0;
    }
}

// MATCHED 100%
u32 func_020139fc(clMesWin * param1, UnkStruct_0200b8bc * param2, BOOL param3) {
    if (!param3) {
        return 0;
    }

    UnkStruct_02013540 * r1 = &param1->field4c[param1->field40_5];

    u8 r2 = param2->field04[2];

    UnkStruct_02013540_60 * r4 = &r1->field60[r2];

    if (!r4->field0c) {
        func_02013540(param1, r1, r2, param2->field04[3]-1, FALSE);
    }

    func_02013b0c(param1, param2, r4);

    return 0;
}

// MATCHED 100%
u32 func_02013a84(clMesWin * param1, UnkStruct_0200b8bc * param2, BOOL param3) {
    if (!param3) {
        return 0;
    }

    UnkStruct_02013540 * r1 = &param1->field4c[param1->field40_5];

    u8 r2 = param2->field04[2];

    UnkStruct_02013540_60 * r4 = &r1->field60[r2];

    if (!r4->field0c) {
        func_02013540(param1, r1, r2, param2->field04[3]-1, TRUE);
    }

    func_02013b0c(param1, param2, r4);

    return 0;
}

// MATCHED 100%
void UnkClass_0201450c::func_02013ec4(BOOL param1, nsObjSys::clObjPltVramMan * param2, nsObjSys::clObjPltVramMan * param3, nsObjSys::clObjPltVramMan * param4) {
    if (!param1) {
        this->field10[0] = param2;
        if (param3 != NULL) {
            this->field10[1] = param3;
        }
        if (param4 != NULL) {
            this->field10[2] = param4;
        }
    } else {
        this->field1c[0] = param2;
        if (param3 != NULL) {
            this->field1c[1] = param3;
        }
        if (param4 != NULL) {
            this->field1c[2] = param4;
        }
    }
}

// MATCHED 100%
void UnkClass_0201450c::func_02013f00(nsObjSys::clObjPltVramMan * param1, nsObjSys::clObjPltVramMan * param2) {
    if (param1 != NULL) {
        this->field28[0] = param1;
    }

    if (param2 != NULL) {
        this->field28[1] = param2;
    }
}

// NONMATCHING 91.14 % -- r0 param is lifted above sp params in target asm
void UnkClass_0201450c::func_02013f3c(u32 param1, u32 param2) {
    UnkStruct_02029b08 ** ref = (param1 == 0) ? &this->field00 : &this->field04;

    if (param2 == -1) {
        u32 ret = this->func_02014200(param1);

        if (param1 == 0) {
            *ref = this->field10[1]->func_02014380(ret, (u32)&data_0205133c | 0x80000000, 5);
        } else {
            *ref = this->field1c[1]->func_02014380(ret, (u32)&data_0205133c | 0x40000000, 5);
        }
    }

    u32 idx = func_0202af48(param1, 0, 0, &data_020502c8);
    this->field30[param1] = RES_ENTRY(&data_020502c8, idx);

    if (param2 == -1) {
        param2 = (*ref)->field08;
    }

    u16 r4 = (*data_0204ac68[param1] & 0x300000) >> 0x14;

    u32 r0 = param2 + data_0204ac60[param1];

    func_0202d10c(r0, 0, &data_020502c8, &data_0205133c, (u8*)&data_020502c8 + this->field30[param1]->offset, r4+5, 0, 0, 0);
}

// MATCHED 100%
void func_02014070(UnkStruct_0205011c * param1, u32 param2, u32 param3, u32 param4) {
    u32 base = (1 << data_0205c25c[param1->field04]);
    func_02028950(param1, MATH_ROUNDUP(param2, base), param3, param4);
}

// MATCHED 100%
void UnkClass_0201450c::func_020140a4(u32 param1) {
    if (param1 == -1) {
        u32 ret = func_0202afbc(0, 1, 0, &data_020502c8);
        this->field00 = this->field28[0]->func_02014380(ret, (u32)&data_0205133c, 5);
    }

    u32 idx = func_0202af48(0, 1, 0, &data_020502c8);
    this->field30[0] = RES_ENTRY(&data_020502c8, idx);

    GXVRamTex bank = GX_GetBankForTex();
    u32 * ptr = data_0205c234;
    if (bank & GX_VRAM_TEX_0_A) {
        *ptr++ = HW_LCDC_VRAM_A;
    }
    if (bank & GX_VRAM_TEX_0_B) {
        *ptr++ = HW_LCDC_VRAM_B;
    }
    if (bank & GX_VRAM_TEX_0_C) {
        *ptr++ = HW_LCDC_VRAM_C;
    }
    if (bank & GX_VRAM_TEX_0_D) {
        *ptr = HW_LCDC_VRAM_D;
    }

    GXVRamTex saved = GX_ResetBankForTex();

    u32 r0;
    if (param1 == -1) {
        r0 = (this->field00->field08 & 0x1FFFF) + data_0205c234[this->field00->field08 >> 17];
    } else {
        r0 = (param1 & 0x1FFFF) + data_0205c234[param1 >> 17];
    }
    func_0202d10c(r0, 1, &data_020502c8, &data_0205133c, (u8*)&data_020502c8 + this->field30[0]->offset, 3, 0, 0, 0);

    GX_SetBankForTex(saved);
}

// MATCHED 100%
u32 UnkClass_0201450c::func_02014200(u32 param1) {
    return func_0202afbc(param1, 0, 0, &data_020502c8);
}

// MATCHED 100%
void UnkClass_0201450c::func_02014264(u32 param1, u32 param2) {
    UnkStruct_02029b08 ** ref = (param1 == 0) ? &this->field08 : &this->field0c;

    if (*ref == NULL) {
        if (param2 == 0) {
            param2 = 0x104;
        }

        u32 p1 = func_0202b170(&data_020502c8, &data_02050204);
        if (param1 == 0) {
            *ref = this->field10[2]->func_02014380(p1, (u32)&data_02050204, param2);
        } else {
            *ref = this->field1c[2]->func_02014380(p1, (u32)&data_02050204, param2);
        }

        UnkStruct_02029b08 * ptr = *ref;

        void * r4 = &data_02050208;
        u32 r5 = ptr->field0c * 32;
        u32 r6 = (ptr->field08 & 0xff) * 32;

        if (param1 == 0) {
            if ((ptr->field08 & 0x100) == 0) {
                GX_BeginLoadOBJExtPltt();
                GX_LoadOBJExtPltt(r4, r6, r5);
                GX_EndLoadOBJExtPltt();
            } else {
                GX_LoadOBJPltt(r4, r6, r5);
            }
        } else {
            if ((ptr->field08 & 0x100) == 0) {
                GXS_BeginLoadOBJExtPltt();
                GXS_LoadOBJExtPltt(r4, r6, r5);
                GXS_EndLoadOBJExtPltt();
            } else {
                GXS_LoadOBJPltt(r4, r6, r5);
            }
        }
    }
}

// MATCHED 100%
UnkStruct_02029b08 * nsObjSys::clObjPltVramMan::func_02014380(u32 param1, u32 param2, u32 param3) {
    u32 p3 = param3 & ~0x100;

    if (param3 & 0x100) {
        return this->func_02029c9c(param1, param2, p3);
    } else {
        return this->func_02029b08(param1, param2, p3);
    }
}

// MATCHED 100%
void UnkClass_0201450c::func_020143ac(u32 param1) {
    if (this->field08 == NULL) {
        if (param1 == 0) { // TODO: Check for a Enum or macro value replacement
            param1 = 0;
        }

        UnkStruct_02029b08 * ptr = this->field28[1]->func_02014380(0x20, NULL, param1);
        this->field08 = ptr;

        u32 p1 = ptr->field08;
        u32 p2 = ptr->field0c;

        GX_BeginLoadTexPltt();
        GX_LoadTexPltt(&data_02050208, p1, p2);
        GX_EndLoadTexPltt();
    }
}

// MATCHED 100%
void UnkClass_0201450c::func_02014410(UnkStruct_0200ec8c * param1, u8 param2, s8 param3, u16 param4, u16 param5, u8 param6, u8 param7, s32 param8) {
    cpu_clear32(0, param1, sizeof(UnkStruct_0200ec8c));

    param1->field08_26 = 1;
    param1->field08_0 = param2;
    param1->field08_1 = param4;
    param1->field08_10 = param5;
    param1->field08_19 = param6;
    param1->field08_21 = param7;

    param1->field0c = param3;

    u32 idx = param1->field0d;

    u8 * base = (u8 *)&data_020502c8;
    UnkStruct_020502c8_Tbl * tbl = (UnkStruct_020502c8_Tbl *)(base + data_020502c8.header.field04);
    UnkStruct_020502c8_Entry * rec = (UnkStruct_020502c8_Entry *)(base + tbl[param1->field0c].field00);

    u16 r1;
    if (idx == 0) {
        r1 = rec[0].field04;
    } else {
        r1 = rec[idx].field04 - rec[idx-1].field04;
    }

    param1->field0e = r1;
    param1->field10 = param8;

    this->field60.field00->field04 = &param1->field00;
    param1->field00.field00 = this->field60.field00;
    param1->field00.field04 = &this->field60;
    this->field60.field00 = &param1->field00;
}

// MATCHED 100%
void UnkClass_0201450c::func_0201450c(BOOL param1) {
    if (!param1) {
        if (!this->field54) {
            if (this->field00 != NULL) {
                this->field10[1]->func_0202a134(this->field00, 0, -1);
                this->field00 = NULL;
            }
        } else {
            if (this->field00 != NULL) {
                this->field28[0]->func_0202a134(this->field00, 0, -1);
                this->field00 = NULL;
            }
        }
    } else {
        if (this->field04 != NULL) {
            this->field1c[1]->func_0202a134(this->field04, 0, -1);
            this->field04 = NULL;
        }
    }
}

// MATCHED 100%
void UnkClass_0201450c::func_020145b4(BOOL param1) {
    if (!param1) {
        if (!this->field54) {
            if (this->field08 != NULL) {
                this->field10[2]->func_0202a134(this->field08, 0, -1);
                this->field08 = NULL;
            }
        } else {
            if (this->field08 != NULL) {
                this->field28[1]->func_0202a134(this->field08, 0, -1);
                this->field08 = NULL;
            }
        }
    } else {
        if (this->field0c != NULL) {
            this->field1c[2]->func_0202a134(this->field0c, 0, -1);
            this->field0c = NULL;
        }
    }
}

// MATCHED 100%
UnkClass_0201450c::UnkClass_0201450c(BOOL param1) {
    func_01ff8228(this->field10, 0, sizeof(this->field10));
    func_01ff8228(this->field1c, 0, sizeof(this->field1c));
    func_01ff8228(this->field28, 0, sizeof(this->field28));

    this->field54 = FALSE;

    if (param1) {
        this->field54 = TRUE;
    }

    this->field00 = NULL;
    this->field04 = NULL;
    this->field08 = NULL;
    this->field0c = NULL;

    this->field30[0] = NULL;
    this->field30[1] = NULL;

    this->field58.field00 = (UnkStruct_0200ec8c_00 *)0x11111111;
    this->field58.field04 = &this->field60;
    this->field60.field00 = &this->field58;
    this->field60.field04 = (UnkStruct_0200ec8c_00 *)-1;

    this->field38 = 1;

    this->field3c[0] = 0xff;
    this->field3c[1] = 0xff;

    this->field40 = -1;
}

// MATCHED 100%
UnkClass_0201450c::~UnkClass_0201450c() {
    this->func_0201450c(FALSE);
    this->func_0201450c(TRUE);

    this->func_020145b4(FALSE);
    this->func_020145b4(TRUE);

    this->field00 = NULL;
    this->field04 = NULL;
    this->field08 = NULL;
    this->field0c = NULL;
}

// NONMATCHING 98.11 % -- register rotation with r1/r2/r3
void UnkClass_0201450c::func_0201485c(UnkStruct_0200ec8c * param1) {
    u8 * base = (u8*)&data_020502c8;

    UnkStruct_020502c8_Tbl * tbl = (UnkStruct_020502c8_Tbl *)RES_PTR(base, data_020502c8.header.field04);
    UnkStruct_020502c8_Entry * local10 = (UnkStruct_020502c8_Entry *)RES_PTR(base, tbl[param1->field0c].field00) + param1->field0d;

    UnkStruct_020502c8_08 * r5 = (UnkStruct_020502c8_08 *)RES_PTR(base, data_020502c8.header.field08) + local10->field00_0;

    UnkStruct_0201485c * tbl2 = (UnkStruct_0201485c *)RES_PTR(base, data_020502c8.header.field10);
    UnkStruct_0201485c * local0c = &tbl2[local10->field06-1];

    UnkStruct_02029b08 * r6;
    UnkStruct_02029b08 * r7;
    nsObjSys::clObjPltVramMan * local08;

    if (param1->field08_0 == 0) {
        r6 = this->field00;
        r7 = this->field08;
        local08 = this->field10[0];
    } else {
        r6 = this->field04;
        r7 = this->field0c;
        local08 = this->field1c[0];
    }

    if (this->field30[param1->field08_0] == NULL) {
        this->field30[param1->field08_0] = RES_ENTRY(&data_020502c8, func_0202af48(param1->field08_0, 0, 0, &data_020502c8));
    }

    u32 idx0 = param1->field08_0;
    u16 * r1 = (u16 *)RES_PTR(base, this->field30[idx0]->offset) + local10->field00_0;
    GXOamAttr * r0 = local08->field18;

    u32 local04;
    if (r6 != NULL) {
        u16 r2 = (*data_0204ac68[idx0] & 0x300000) >> 0x14;
        local04 = r6->field08 >> (r2 + 5);
    } else {
        u16 r2 = (*data_0204ac68[idx0] & 0x300000) >> 0x14;
        local04 = this->field44[idx0] >> (r2 + 5);
    }

    u16 local00;
    if (r7 != NULL) {
        if ((u16)r7->field08 & 0x100) {
            local00 = (u8)r7->field08;
        } else {
            local00 = r7->field08 >> 4;
        }
    } else {
        local00 = this->field3c[idx0];
    }

    s32 r14;
    s16 x;
    s16 y;
    for (r14 = local10->field00_24; r14 > 0; r14--) {
        u32 r9 = r5->field00_10;
        u32 r8 = r5->field00_6;
        u8 idx = r9 + r8 * 4;
        s32 w = data_0204a824[idx];
        s32 h = data_0204a80c[idx];
        x = r5->field04 - (w >> 1);
        y = r5->field06 - (h >> 1);

        u8 r12 = r5->field00_8;
        if (local10->field06 != 0) {
            x += local0c->field08;
            y += local0c->field0a;
        }
        if (param1->field08_19) {
            if (param1->field08_19 & 1) {
                x = -(x + w);
                r12 ^= 0x8;
            }
            if (param1->field08_19 & 2) {
                y = -(y + h);
                r12 ^= 0x10;
            }
        }
        x += param1->field08_1;
        y += param1->field08_10;
        
        r0->attr01 = (y & 0xff) | (r5->field00_2 << 8) | (r5->field00_4 << 12) | (r5->field00_5 << 13) | (r8 << 14) | ((x << 16) & 0x1ff0000) | (r12 << 25) | (r9 << 30);
        r0->attr2 = (*r1++ + local04) | (param1->field08_21 << 10) | ((local00 + r5->field00_14) << 12);
        r5++;
        r0++;
    }
    local08->func_02014380(local08->field18 - local08->field04, local10->field00_24, 0x7fffffff);
    local08->field18 += local10->field00_24;
}

