#ifndef MAIN_RFX_NODE_H
#define MAIN_RFX_NODE_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
    #include <nitro.h>
}

typedef struct {
    s16 field00;
    union {
        s16 field02;
        struct {
            u16 cull : 2;  // +0x2
            u16 triStrip : 1;
            s16 cols : 4;
            u16 : 9;
        };
    };
    s16 field04;
    s16 r;  // +0x6
    s16 g;  // +0x8
    s16 b;  // +0xa
    s16 field0c;
    s16 field0e;
    s16 w;  // +0x10
    s16 h;  // +0x12
} AfxNodeParams;

struct OfxNodeParams : AfxNodeParams {
};

struct DfxNodeParams : AfxNodeParams {
    s16 field14;
    s16 transparency;  // +0x16
};

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
        AfxNodeParams afx;  // +0x30
        OfxNodeParams ofx;  // +0x30
        DfxNodeParams dfx;  // +0x30
    };
    u8 pad_48[0x18];
} RfxNode;

#endif // __cplusplus

#endif
