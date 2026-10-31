#ifndef MAIN_RFX_NODE_H
#define MAIN_RFX_NODE_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
    #include <nitro.h>
}

typedef struct {
    union {
        struct {
            u8 pad_00[0x6];
            s16 r;  // +0x6
            s16 g;  // +0x8
            s16 b;  // +0xa
            u8 pad_0c[0xa];
            s16 transparency;  // +0x16
        } color;
        struct {
            s16 x;  // +0x0
            s16 y;  // +0x2
            s16 z;  // +0x4
            u8 pad_06[0x6];
            s16 breakStrip;  // +0xc
        } vtx;
        struct {
            s16 segments;  // +0x0
            s16 radiusA;  // +0x2
            s16 radiusB;  // +0x4
            u8 pad_06[0x6];
            s16 innerRadiusA;  // +0xc
            s16 innerRadiusB;  // +0xe
            s16 innerR;  // +0x10
            s16 innerG;  // +0x12
            s16 innerB;  // +0x14
        } circle;
        struct {
            u8 pad_00[0x2];
            union {
                s16 flags;  // +0x2
                struct {
                    u16 cull : 2;
                    u16 triStrip : 1;
                    s16 cols : 4;
                    u16 : 9;
                };
            };
            s16 texIdx;  // +0x4
            u8 pad_06[0x6];
            s16 offsetU;  // +0xc
            s16 offsetV;  // +0xe
            s16 tileW;  // +0x10
            s16 tileH;  // +0x12
            s16 plttBlock;  // +0x14
        } tex;
        struct {
            u8 pad_00[0xc];
            s16 cellIdx;  // +0xc
            s16 frame;  // +0xe
        } cell;
    };
} DfxNodeParams;

typedef struct {
    union {
        struct {
            s16 x;  // +0x0
            s16 y;  // +0x2
            s16 zOffset;  // +0x4
            s16 shape;  // +0x6
            u8 pad_08[0x6];
            s16 texIdx;  // +0xe
            s16 plttIdx;  // +0x10
            s16 doubleSize;  // +0x12
        } sprite;
        struct {
            s16 cellIdx;  // +0x0
            s16 frame;  // +0x2
        } cell;
        struct {
            s16 r;  // +0x0
            s16 g;  // +0x2
            s16 b;  // +0x4
        } color;
    };
} OfxNodeParams;

typedef struct {
    union {
        MtxFx43 mtx;  // +0x0
        struct {
            VecFx32 axisX;  // +0x0
            VecFx32 axisY;  // +0xc
            VecFx32 axisZ;  // +0x18
            VecFx32 pos;  // +0x24
        };
    };
    union {
        DfxNodeParams dfx;  // +0x30
        OfxNodeParams ofx;  // +0x30
    };
    u8 pad_48[0x18];
} RfxNode;

#endif // __cplusplus

#endif
