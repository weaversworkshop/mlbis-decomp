#ifndef MAIN_MES_WIN_UTIL_H
#define MAIN_MES_WIN_UTIL_H

#include "types.h"

typedef struct MesWinRequest {
    s16 style;  // +0x0
    s16 width;  // +0x2
    s16 height;  // +0x4
    s16 indent;  // +0x6
    s16 tailKind;  // +0x8
    s16 tailAlt;  // +0xa
    s16 tailUp;  // +0xc
    s16 tailX;  // +0xe
    u32 * text;  // +0x10
    u16 textId;  // +0x14
    u8 pad_16[0x2];
} MesWinRequest;

typedef struct MesWinPlacement {
    s16 x;  // +0x0
    s16 y;  // +0x2
    s16 width;  // +0x4
    s16 height;  // +0x6
    struct {
        u16 tailKind : 4;  // +0x8
        u16 tailAlt : 1;
        u16 tailUp : 1;
        u16 tailFlipH : 1;
        u16 : 9;
    };
    s16 tailX;  // +0xa
} MesWinPlacement;

#endif
