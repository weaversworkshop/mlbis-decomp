#ifndef MAIN_MES_WRITER_H
#define MAIN_MES_WRITER_H

#include "types.h"

#define MES_FONT_WORD(p, i)   ((&(p)->field00)[(i)])
#define MES_FONT_PTR(p, off)  (&(&(p)->field00)[(off) / 4])

typedef struct MesWriter MesWriter;

typedef struct {
    u32 field00;
    u32 charMapOffset;  // +0x4
    u32 pageOffsets[1];  // +0x8
} MesFont;

typedef union {
    u16 raw;  // +0x0
    struct {
        u8 width;  // +0x0
        u8 height;  // +0x1
    };
} MesGlyphSize;

typedef struct {
    union {
        u16 raw;  // +0x0
        struct {
            u8 width;  // +0x0
            u8 height;  // +0x1
        };
    };
    u16 field02;
} MesGlyphSizeSlot;

typedef struct {
    MesGlyphSize field00;
    MesGlyphSize field02;
    MesGlyphSizeSlot s20;  // +0x4
} UnkStruct_02013640;

typedef struct {
    union {
        u32 field24;
        struct {
            u32 lines : 8;  // +0x0
            u32 left : 10;
            u32 top : 10;
            u32 : 4;
        };
    };
    union {
        u32 field28;
        struct {
            u32 width : 16;  // +0x4
            u32 height : 16;
        };
    };
} MesExtents;

typedef u32 (* MesCodeCallback)(void *, MesWriter *, BOOL);

typedef struct {
    s8 argLen;  // +0x0
    u8 disabled;  // +0x1
    MesCodeCallback func;  // +0x4
    void * arg;  // +0x8
} MesCodeHandler;

typedef struct {
    union {
        u32 field00;
        struct {
            u16 glyph;  // +0x0
            u16 code;  // +0x2
        };
    };
    u8 * text;  // +0x4
} MesStepResult;

typedef struct MesWriter {
    u32 * dst;  // +0x0
    u8 * text;  // +0x4
    union {
        u32 field08;
        struct {
            u32 x : 10;  // +0x8
            u32 y : 10;
            u32 baseSpacing : 4;
            u32 lineSpacing : 4;
            u32 baseColor : 4;
        };
    };
    union {
        u32 field0c;
        struct {
            u32 color : 4;  // +0xc
            u32 spaceWidth : 5;
            u32 cols : 7;
            u32 width : 7;
            u32 : 9;
        };
    };
    MesFont ** fonts;  // +0x10
    union {
        u32 field14;
        struct {
            u32 scale : 2;  // +0x14
            u32 xOffset : 8;
            u32 mode : 2;
            u32 : 20;
        };
        struct {
            u32 scaleWide : 1;  // +0x14
            u32 scaleTall : 1;
            u32 : 30;
        };
    };
    union {
        u32 field18;
        struct {
            u32 font : 16;  // +0x18
            u32 field18_16 : 1;
            u32 lastAlign : 2;
            u32 field18_19 : 3;
            u32 lineX : 10;
        };
    };
    union {
        u32 field1c;
        struct {
            u32 field1c_0 : 8;  // +0x1c
            u32 align : 2;
            u32 fixedWidth : 1;
            u32 lineHeight : 6;
            u32 spacing : 4;
            u32 noAlign : 1;
            u32 lineLeft : 10;
        };
    };
    union {
        u32 field20;
        struct {
            u32 lastX : 10;  // +0x20
            u32 lastY : 10;
            u32 lastWidth : 6;
            u32 lastHeight : 6;
        };
    };
    MesExtents extents;  // +0x24
    MesCodeHandler * callbacks;  // +0x2c
} MesWriter;

void mesWriterInit(MesWriter *, MesFont **, u32 *, const char *, u8, u8, u8, u8, u8, u8, u8, u8, u8, u8, MesCodeHandler *);  // 0x0200b8bc
void mesWriterReset(MesWriter *);  // 0x0200b9f4
void mesWriterSetCallback(MesCodeHandler *, u32, u8, MesCodeCallback, void *);  // 0x0200baf0
void mesWriterMeasure(MesWriter *, MesExtents *);  // 0x0200bb1c
u16 mesWriterMeasureHeight(MesWriter *);  // 0x0200bbb0
s16 mesWriterDrawGlyph(u16, u16, u8, u8, u32, u32 *, u32 *, u8, BOOL, u8);  // 0x0200c330
MesStepResult mesWriterStep(MesWriter *, BOOL, BOOL);  // 0x0200bce8

#endif
