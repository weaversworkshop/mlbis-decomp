#ifndef MAIN_CL_LFX_H
#define MAIN_CL_LFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clAfx.h"

typedef struct {
    u32 * bitmap;  // +0x0
    u16 wTiles;  // +0x4
    u16 hTiles;  // +0x6
    u16 * pltt;  // +0x8
    u16 colorStart;  // +0xc
    u16 colorEnd;  // +0xe
    u16 colorMid;  // +0x10
    s16 midIdx;  // +0x12
    u16 decaySpeed;  // +0x14
    u16 decayCount;  // +0x16
    s16 scrollCount;  // +0x18
    s16 scrollVelocity;  // +0x1a
    s16 framesRemaining;  // +0x1c
    u8 ev;  // +0x1e
    u8 evb;  // +0x1f
    struct {
        u8 dirty : 1;  // +0x20
        u8 : 7;
    };
} LfxLayer;

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
    u16 field30;
    u16 field32;
    u16 field34;
    u16 field36;
    u16 field38;
    u16 field3a;
    u16 field3c;
    u16 field3e;
    u16 field40;
    u16 field42;
    s16 field44;
    s16 field46;
    u16 field48;
    u16 field4a;
    u16 field4c;
    u16 field4e;
    u16 field50;
} LfxNode;

class clLfx : public clAfx {
public:
    u8 pad_20[0x24];
    s16 originX;  // +0x44
    s16 originY;  // +0x46

    // ~clLfx();  // implicit; D1 0x02028798 D0 0x02028768 Thn12_D1 0x020287d0 Thn12_D0 0x020287c0
    virtual void unk_virtual1(s32, u16 *, u8);  // 0x02027b5c
    virtual u32 unk_virtual2(void);  // 0x02027c34
    virtual void beginDraw(void);  // 0x02027c3c
    virtual GXRgb getBlendColor(void);  // 0x02028464
    virtual LfxLayer * func_02027b54();  // 0x02027b54

    void endDraw(void);  // 0x02027d2c
    void cmdDrawTex(s32, u16 *);  // 0x02027d30
    void cmdLayerColor(s32, u16 *);  // 0x02027ed0
    void cmdLayerDecay(s32, u16 *);  // 0x02027fd0

    void cmdLayerRows(s32, u16 *);  // 0x020283c4
};

#endif // __cplusplus

#endif
