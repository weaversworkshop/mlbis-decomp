#ifndef MAIN_UI_UTIL_H
#define MAIN_UI_UTIL_H

#include "types.h"

typedef struct {
    u8 kind;  // +0x0
    u8 unitShift;  // +0x1
    u16 numUnits;  // +0x2
    u32 offset;  // +0x4
} YanaResEntry;

typedef struct {
    u8 numEntries;  // +0x0
    u8 numAnims;  // +0x1
    struct {
        u8 texFmt : 3;  // +0x2
        u8 linear : 1;
        u8 plttLast : 4;
    };
    u8 pad_03[0x1];
    u32 animOffset;  // +0x4
    u32 objOffset;  // +0x8
    u32 mtxOffset;  // +0xc
    u32 xformOffset;  // +0x10
    YanaResEntry entries[1];  // +0x14
} YanaRes;

typedef struct {
    u32 frames;  // +0x0
    u16 count;  // +0x4
    u8 pad_06[0x2];
} YanaResAnim;

typedef struct {
    union {
        u32 field00;
        struct {
            u32 cell : 24;  // +0x0
            u32 numObj : 8;
        };
    };
    u16 time;  // +0x4
    u16 xformIdx;  // +0x6
} YanaResFrame;

typedef struct {
    union {
        u32 field00;
        struct {
            u32 useMtx : 1;  // +0x0
            u32 : 1;
            u32 mode : 2;
            u32 mosaic : 1;
            u32 color : 1;
            u32 shape : 2;
            u32 flip : 2;
            u32 size : 2;
            u32 prioIdx : 2;
            u32 pltt : 4;
            u32 mtxIdx : 11;
            u32 newInFrame : 1;
            u32 newInFile : 1;
            u32 : 1;
        };
    };
    s16 x;  // +0x4
    s16 y;  // +0x6
} YanaResObj;

typedef struct {
    s16 m00;  // +0x0
    s16 m01;  // +0x2
    s16 m10;  // +0x4
    s16 m11;  // +0x6
    s16 x;  // +0x8
    s16 y;  // +0xa
} YanaResXform;

#define RES_HDR_SIZE (sizeof(YanaRes) - sizeof(YanaResEntry))
#define RES_PTR(res, off) ((u8 *)(res) + (off))
#define RES_ENTRY(res, i) ((YanaResEntry *)((u8 *)(res) + ((i) * sizeof(YanaResEntry) + RES_HDR_SIZE)))

u32 countDigits(s32);  // 0x0201f3a0
void * getResChunk(YanaRes *, u32, s32);  // 0x0201f4f8
s32 btnSelect(s32, s32, u32, u32);  // 0x0201f550

#endif
