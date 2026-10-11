#ifndef MAIN_UI_UTIL_H
#define MAIN_UI_UTIL_H

#include "types.h"

typedef struct {
    u8 field00;
    u8 field01;
    u16 field02;
    u32 field04;
    u32 field08;
    u32 field0c;
    u32 field10;
} YanaResHeader;

typedef struct {
    u8 kind;  // +0x0
    u8 id;  // +0x1
    u8 pad_02[0x2];
    u32 offset;  // +0x4
} YanaResEntry;

typedef struct {
    YanaResHeader header;  // +0x0
    YanaResEntry entries[1];  // +0x14
} YanaRes;

#define RES_PTR(res, off) ((u8 *)(res) + (off))
#define RES_ENTRY(res, i) ((YanaResEntry *)((u8 *)(res) + ((i) * sizeof(YanaResEntry) + sizeof(YanaResHeader))))

u32 countDigits(s32);  // 0x0201f3a0
void * getResChunk(YanaRes *, u32, s32);  // 0x0201f4f8
s32 stepMenuCursor(s32, s32, u32, u32);  // 0x0201f550

#endif
