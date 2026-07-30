#ifndef MAIN_UI_UTIL_H
#define MAIN_UI_UTIL_H

#include "types.h"

typedef struct {
    u8 pad_00[0x14];
} YanaResHeader;

typedef struct {
    u8 kind;
    u8 id;
    u8 pad_02[0x2];
    u32 offset;
} YanaResEntry;

typedef struct {
    YanaResHeader header;
    YanaResEntry entries[1];
} YanaRes;

#define RES_ENTRY(res, i) ((YanaResEntry *)((u8 *)(res) + ((i) * sizeof(YanaResEntry) + sizeof(YanaResHeader))))

u32 countDigits(s32);  // 0x0201f3a0
void * getResChunk(YanaRes *, u32, s32);  // 0x0201f4f8
s32 stepMenuCursor(s32, s32, u32, u32);  // 0x0201f550

#endif
