#ifndef MAIN_UNK_TU_0200B8BC_H
#define MAIN_UNK_TU_0200B8BC_H

#include "types.h"

#define WORD_10(p, i)   ((&(p)->field00)[(i)])
#define PTR_10(p, off)  (&(&(p)->field00)[(off) / 4])

typedef struct UnkStruct_0200b8bc UnkStruct_0200b8bc;

typedef struct {
    u32 field00;
    u32 field04;
    u32 field08[1];
} UnkStruct_0200b8bc_10;

typedef struct {
    union {
        u32 field24;
        struct {
            u32 field24_0 : 8;  // +0x0
            u32 field24_8 : 10;
            u32 field24_18 : 10;
            u32 : 4;
        };
    };
    union {
        u32 field28;
        struct {
            u32 field28_0 : 16;  // +0x4
            u32 field28_16 : 16;
        };
    };
} UnkStruct_0200b8bc_24;

typedef u32 (* UnkCallback_0200b8bc_2c)(void *, UnkStruct_0200b8bc *, BOOL);

typedef struct {
    s8 field00;
    u8 field01;
    UnkCallback_0200b8bc_2c field04;
    void * field08;
} UnkStruct_0200b8bc_2c;

struct UnkStruct_0200b8bc {
    u32 field00;
    u8 * field04;
    union {
        u32 field08;
        struct {
            u32 field08_0 : 10;  // +0x8
            u32 field08_10 : 10;
            u32 field08_20 : 4;
            u32 field08_24 : 4;
            u32 field08_28 : 4;
        };
    };
    union {
        u32 field0c;
        struct {
            u32 field0c_0 : 4;  // +0xc
            u32 field0c_4 : 5;
            u32 field0c_9 : 7;
            u32 field0c_16 : 7;
            u32 : 9;
        };
    };
    UnkStruct_0200b8bc_10 ** field10;
    union {
        u32 field14;
        struct {
            u32 field14_0 : 2;  // +0x14
            u32 field14_2 : 8;
            u32 field14_10 : 2;
            u32 : 20;
        };
        struct {
            u32 field14_0a : 1;  // +0x14
            u32 field14_0b : 1;
            u32 : 30;
        };
    };
    union {
        u32 field18;
        struct {
            u32 field18_0 : 16;  // +0x18
            u32 field18_16 : 1;
            u32 field18_17 : 2;
            u32 field18_19 : 3;
            u32 field18_22 : 10;
        };
    };
    union {
        u32 field1c;
        struct {
            u32 field1c_0 : 8;  // +0x1c
            u32 field1c_8 : 2;
            u32 field1c_10 : 1;
            u32 field1c_11 : 6;
            u32 field1c_17 : 4;
            u32 field1c_21 : 1;
            u32 field1c_22 : 10;
        };
    };
    union {
        u32 field20;
        struct {
            u32 field20_0 : 10;  // +0x20
            u32 field20_10 : 10;
            u32 field20_20 : 6;
            u32 field20_26 : 6;
        };
    };
    UnkStruct_0200b8bc_24 unkStruct24;  // +0x24
    UnkStruct_0200b8bc_2c * field2c;
};

typedef struct {
    u8 field00;
    u8 pad_01[0x3];
    u32 field04;
    u32 field08;
} UnkStruct_0200baf0;

void func_0200b8bc(UnkStruct_0200b8bc *, UnkStruct_0200b8bc_10 **, u32, const char *, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, UnkStruct_0200b8bc_2c *);  // 0x0200b8bc
void func_0200baf0(UnkStruct_0200b8bc_2c *, u32, u8, UnkCallback_0200b8bc_2c, void *);  // 0x0200baf0
void func_0200bb1c(UnkStruct_0200b8bc *, UnkStruct_0200b8bc_24 *);  // 0x0200bb1c

#endif
