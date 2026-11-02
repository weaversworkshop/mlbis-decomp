#ifndef MAIN_CL_DFX_H
#define MAIN_CL_DFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clAfx.h"

typedef struct {
    GXTexFmt fmt;  // +0x0
    GXTexSizeS sizeS;  // +0x4
    GXTexSizeT sizeT;  // +0x8
    s16 offsetU;  // +0xc
    s16 offsetV;  // +0xe
    u16 width;  // +0x10
    u16 height;  // +0x12
    s32 texAddr;  // +0x14
    s32 plttAddr;  // +0x18
} DfxTexInfo;

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
        struct {
            u8 pad_00[0x6];
            s16 r;  // +0x36
            s16 g;  // +0x38
            s16 b;  // +0x3a
            u8 pad_0c[0xa];
            s16 transparency;  // +0x46
        } color;
        struct {
            s16 x;  // +0x30
            s16 y;  // +0x32
            s16 z;  // +0x34
            u8 pad_06[0x6];
            s16 breakStrip;  // +0x3c
        } vtx;
        struct {
            s16 segments;  // +0x30
            s16 radiusA;  // +0x32
            s16 radiusB;  // +0x34
            u8 pad_06[0x6];
            s16 innerRadiusA;  // +0x3c
            s16 innerRadiusB;  // +0x3e
            s16 innerR;  // +0x40
            s16 innerG;  // +0x42
            s16 innerB;  // +0x44
        } circle;
        struct {
            u8 pad_00[0x2];
            union {
                s16 flags;  // +0x32
                struct {
                    u16 cull : 2;
                    u16 triStrip : 1;
                    s16 cols : 4;
                    u16 : 9;
                };
            };
            s16 texIdx;  // +0x34
            u8 pad_06[0x6];
            s16 offsetU;  // +0x3c
            s16 offsetV;  // +0x3e
            s16 tileW;  // +0x40
            s16 tileH;  // +0x42
            s16 plttBlock;  // +0x44
        } tex;
        struct {
            u8 pad_00[0xc];
            s16 cellIdx;  // +0x3c
            s16 frame;  // +0x3e
        } cell;
    };
} DfxNode;

class clDfx : public clAfx {
public:
    MtxFx33 rot;  // +0x20
    s16 originX;  // +0x44
    s16 originY;  // +0x46
    s32 originZ;  // +0x48

    // ~clDfx();  // implicit; D1 0x0202560c D0 0x020255dc Thn12_D1 0x02025644 Thn12_D0 0x02025634
    virtual s32 getTexAddr(RfxTexHeader *, DfxNode *);  // 0x02022918
    virtual s32 getPlttAddr(RfxPlttHeader *, DfxNode *);  // 0x02022920
    virtual void unk_virtual1(s32, u16 *, u8);  // 0x02022928
    virtual u32 unk_virtual2(void);  // 0x02022b10
    virtual void beginDraw(void);  // 0x02022b18
    virtual GXRgb getBlendColor(void);  // 0x020255ac
    virtual nsObjSys::clCellAnime * beginCellAnime(s32, RfxNode *);  // 0x02022df0
    virtual void endCellAnime(nsObjSys::clCellAnime *);  // 0x02022f74
    virtual void func_02023188(s16);  // 0x02023188

    void endDraw(void);  // 0x02022bc8

    void cmdNodeCallback(s32, u16 *);  // 0x02022c9c

    void cmdCellFrame(s32, u16 *);  // 0x02022fd8
    void cmdDrawNumber(u16 *);  // 0x02023084
    void func_020230e4(s32, u16 *);  // 0x020230e4

    void cmdVertexStrip(s32, u16 *);  // 0x0202318c
    void cmdCircle(s32, u16 *);  // 0x02023368

    void cmdTexQuads(u16 *);  // 0x02023a24
    BOOL drawTexQuad(u16, DfxNode *);  // 0x02023abc
    void cmdSetTexParams(u16 *);  // 0x02023cc8
    void cmdTexMatrix(s32, u16 *);  // 0x02023d64

    void cmdTexStrip(u16 *);  // 0x02023eb0
    void cmdTexStrips(u16 *);  // 0x02024184
    void cmdLitMesh(u16 *);  // 0x02024528

    BOOL setupTexture(u16, DfxNode *);  // 0x02025378
    DfxTexInfo * resolveTexture(u16, DfxNode *);  // 0x02025430    
};

#endif // __cplusplus

#endif
