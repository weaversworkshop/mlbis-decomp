#ifndef MAIN_CL_MES_WIN_H
#define MAIN_CL_MES_WIN_H

#include "types.h"

#ifdef __cplusplus

#include "main/clObjPltVramMan.h"

#include "main/sndEmitter.h"

extern "C" {
    #include "main/unk_tu_0200b8bc.h"
    #include "main/uiUtil.h"
}

typedef struct UnkStruct_0200ec8c_00 {
    UnkStruct_0200ec8c_00 * field00;
    UnkStruct_0200ec8c_00 * field04;
} UnkStruct_0200ec8c_00;

typedef struct {
    UnkStruct_0200ec8c_00 field00;
    struct {
        u32 field08_0 : 1;  // +0x8
        u32 field08_1 : 9;
        u32 field08_10 : 9;
        u32 field08_19 : 2;
        u32 field08_21 : 2;
        u32 : 2;
        u32 field08_25 : 1;
        u32 field08_26 : 1;
        u32 : 5;
    };
    u8 field0c;
    u8 field0d;
    u8 field0e;
    u8 pad_0f[0x1];
    u32 field10;
} UnkStruct_0200ec8c;

typedef struct {
    struct {
        u32 field00_0 : 1;  // +0x0
        u32 field00_1 : 4;
        u32 field00_5 : 5;
        u32 field00_10 : 5;
        u32 field00_15 : 3;
        u32 : 14;
    };
    struct {
        u32 field04_0 : 4;  // +0x4
        u32 : 5;
        u32 field04_9 : 8;
        u32 field04_17 : 11;
        u32 : 4;
    };
    struct {
        u32 field08_0 : 1;  // +0x8
        u32 field08_1 : 9;
        u32 field08_10 : 9;
        u32 field08_19 : 8;
        u32 field08_27 : 1;
        u32 : 4;
    };
    s32 * field0c;
    UnkStruct_0200b8bc_10 ** field10;
    // u8 pad_10[0x4];
    u32 * field14;
    u8 pad_18[0x4];
    s32 field1c;
    s16 field20;
    u8 pad_22[0x6];
} UnkStruct_0201008c;

typedef struct UnkStruct_02013540_60 {
    char field00[0x8];
    char * field08;
    bool field0c;
    u8 pad_0d[0x3];
} UnkStruct_02013540_60;

typedef struct {
    UnkStruct_0200ec8c_00 field00;
    UnkStruct_0201008c field08;
    u8 pad_30[0x30];
    UnkStruct_02013540_60 field60[4];
    struct {
        u32 : 8;
        u32 fielda0_8 : 8;
        u32 : 16;
    };
    struct {
        u32 fielda4_0 : 1;  // +0xa4
        u32 fielda4_1 : 1;
        u32 fielda4_2 : 1;
        u32 fielda4_3 : 3;
        u32 fielda4_6 : 15;
        u32 fielda4_21 : 5;
        u32 fielda4_26 : 1;
        u32 fielda4_27 : 1;
        u32 fielda4_28 : 2;
        u32 : 2;
    };
    struct {
        u32 fielda8_0 : 16;  // +0xa8
        u32 fielda8_16 : 3;
        u32 fielda8_19 : 1;
        u32 fielda8_20 : 1;
        u32 fielda8_21 : 5;
        u32 fielda8_26 : 1;
        u32 fielda8_27 : 4;
        u32 : 1;
    };
    u8 fieldac;
    u8 fieldad;
    u8 pad_ae[0x2];
    void ** fieldb0;
    void ** fieldb4;
    u32 fieldb8;
    UnkStruct_0200ec8c fieldbc;
    UnkStruct_0200ec8c fieldd0;
    u16 ** fielde4;
    u8 pad_e8[0x14];
    struct {
        u32 fieldfc_0 : 16;
        u32 fieldfc_16 : 16;
    };
    SndEmitter field100;
    u8 pad_146[0xc];
} UnkStruct_02013540;

typedef struct {
    u16 field00;
    u16 field02;
    u16 field04;
    u8 pad_06[0x16];
    u16 * field1c;
    u16 * field20;
} UnkStruct_0205011c_00;

typedef struct {
    UnkStruct_0205011c_00 * field00;
    u8 field04;
    u8 pad_05[0x3];
} UnkStruct_0205011c;

class UnkClass_0201450c {
public:
    UnkStruct_02029b08 * field00;
    UnkStruct_02029b08 * field04;
    UnkStruct_02029b08 * field08;
    UnkStruct_02029b08 * field0c;
    nsObjSys::clObjPltVramMan * field10[3];
    nsObjSys::clObjPltVramMan * field1c[3];
    nsObjSys::clObjPltVramMan * field28[2];
    YanaResEntry * field30[2];
    u32 field38;
    u8 field3c[2];
    u8 pad_3e[0x2];
    s32 field40;
    u32 field44[2]; // unclear boundary
    u8 pad_4c[0x8];
    BOOL field54;
    UnkStruct_0200ec8c_00 field58;
    UnkStruct_0200ec8c_00 field60;

    UnkClass_0201450c(BOOL);  // C1 0x0201465c C2 0x0201470c
    ~UnkClass_0201450c();  // D1 0x020147bc D2 0x0201480c

    void func_02013dc8();

    void func_02013ec4(BOOL, nsObjSys::clObjPltVramMan *, nsObjSys::clObjPltVramMan *, nsObjSys::clObjPltVramMan *);  // 0x02013ec4
    void func_02013f00(nsObjSys::clObjPltVramMan *, nsObjSys::clObjPltVramMan *);  // 0x02013f00

    void func_02013f3c(u32, u32);  // 0x02013f3c
    
    void func_020140a4(u32);  // 0x020140a4
    u32 func_02014200(u32);  // 0x02014200
    void func_02014220(u32, u32);
    void func_02014264(u32, u32);  // 0x02014264
    void func_020143ac(u32);  // 0x020143ad
    void func_02014410(UnkStruct_0200ec8c *, u8, s8, u16, u16, u8, u8, s32);  // 0x02014410
    void func_0201450c(BOOL);  // 0x0201450c
    void func_020145b4(BOOL);  // 0x020145b4

    void func_0201485c(UnkStruct_0200ec8c *);  // 0x0201485c
};

class clMesWin {
public:
    UnkClass_0201450c * field04;
    union {
        u32 field08;
        struct {
            s16 field08h;
            s16 field0a;
        };
    };
    union {
        u32 field0c;
        struct {
            s16 field0ch;
            s16 field0e;
        };
    };
    u16 field10;
    u16 field12;
    void * field14;
    void * field18;
    void * field1c;
    void * field20;
    void * field24;
    void * field28;
    u32 field2c;
    u32 field30;
    u16 field34;
    u16 field36;
    u8 field38[2];
    u8 pad_40[0x2];
    union {
        u32 field3c;
        struct {
            u32 field3c_0 : 1;  // +0x3c
            u32 field3c_1 : 1;
            u32 field3c_2 : 1;
            u32 field3c_3 : 1;
            u32 field3c_4 : 5;
            u32 field3c_9 : 5;
            u32 field3c_14 : 2;
            u32 field3c_16 : 2;
            u32 field3c_18 : 2;
            u32 field3c_20 : 2;
            u32 field3c_22 : 4;
            u32 field3c_26 : 4;
            u32 : 2;
        };
    };
    union {
        u32 field40;
        struct {
            u32 field40_0 : 3;  // +0x40
            u32 field40_3 : 1;
            u32 field40_4 : 1;
            u32 field40_5 : 5;
            u32 field40_10 : 1;
            u32 field40_11 : 1;
            u32 field40_12 : 1;
            u32 : 19;
        };
    };
    void * field44;
    void * field48;
    UnkStruct_02013540 * field4c;
    u16 field50;
    u8 pad_52[0x2];
    u32 field54;
    UnkStruct_0200ec8c_00 field58;
    UnkStruct_0200ec8c_00 field60;
    UnkStruct_0200ec8c_00 field68;
    UnkStruct_0200ec8c_00 field70;
    nsObjSys::clObjPltVramMan * field78;
    nsObjSys::clObjPltVramMan * field7c;
    UnkStruct_02029b08 * field80;
    UnkStruct_02029b08 * field84;
    u32 field88;
    u32 field8c;
    u32 field90;
    u32 field94;
    union {
        u32 field98;
        struct {
            s16 field98h;
            s16 field9a;
        };
    };
    union {
        u32 field9c;
        struct {
            s16 field9ch;
            s16 field9e;
        };
    };
    UnkStruct_0200b8bc_2c fielda0[31];

    clMesWin(u32, u32, u32, u8, u8, u32, UnkClass_0201450c *);  // C1 0x0200e370 C2 0x0200e5f8
    ~clMesWin();

    virtual void func_0200dd9c(UnkStruct_0201008c *, s16);  // 0x0200dd9c
    virtual void func_0200de40(u32);
    virtual void func_0200ea48(UnkStruct_0200ec8c_00 *);
    virtual void func_02010c00();
    virtual void func_020126d8();
    virtual void func_02012c24();

    void func_0200db0c(void);  // 0x0200db0c
    void func_0200dc38(void);  // 0x0200dc38

    void func_0200de80(u32, u16, u32, u32, u8, u8);  // 0x0200de80
    u32 func_0200dee0(u16, u32, u32, u8, u8);  // 0x0200dee0
    void func_0200dfac(UnkStruct_02013540 *);  // 0x0200dfac

    u32 func_0200e12c(const char *, UnkStruct_0200b8bc_10 **);  // 0x0200e12c
    u16 func_0200e1bc(u32, u32);  // 0x0200e1bc
    void func_0200e1d0(nsObjSys::clObjPltVramMan *);  // 0x0200e1d0
    void func_0200e24c(BOOL, u32, void *);  // 0x0200e24c
    void func_0200e2d8(nsObjSys::clObjPltVramMan *, u32);  // 0x0200e2d8

    void func_0200ec8c(UnkStruct_0200ec8c *);  // 0x0200ec8c

    BOOL func_0200ed74(UnkStruct_02013540 *);  // 0x0200ed74

    void func_0200fe38(UnkStruct_02013540 *, s16, s16, u16, u8, UnkStruct_0200ec8c *);  // 0x0200fe38
    void func_0200ff3c(UnkStruct_02013540 *, s16, s16, u8, u8);  // 0x0200ff3c
    void func_0201008c(UnkStruct_0201008c *, s16);  // 0x0201008c

    void func_020106fc(const char *, UnkStruct_0200b8bc_10 **, UnkStruct_0200b8bc_24 *, u8);  // 0x020106fc
    bool func_020107a0(s16);  // 0x020107a0

    void func_02010930(u8);  // 0x02010930

    UnkStruct_0205011c * func_02010bd4(u32, u32);  // 0x02010bd4

    void func_020125c0(UnkStruct_02013540 * param1);  // 0x020125c0

    void func_0201291c(u32, u32);  // 0x0201291c
    void func_02012a24(u32, u32);  // 0x02012a24
    void func_02012b2c(void);  // 0x02012b2c
    void func_02012ba8(void);  // 0x02012ba8

    void func_02012f54(void);  // 0x02012f54

    u32 func_02013164(u8, u8, u8, u8 *, u8);  // 0x02013164
};

// extern "C" {
#endif // __cplusplus

// #ifdef __cplusplus
// } // extern "C"
// #endif

#endif
