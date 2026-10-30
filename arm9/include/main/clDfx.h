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

class clDfx : public clAfx {
public:
    MtxFx33 rot;  // +0x20
    s16 originX;  // +0x44
    s16 originY;  // +0x46
    s32 originZ;  // +0x48

    // ~clDfx();  // implicit; D1 0x0202560c D0 0x020255dc Thn12_D1 0x02025644 Thn12_D0 0x02025634
    virtual s32 getTexAddr(DfxTexHeader *, RfxNode *);  // 0x02022918
    virtual s32 getPlttAddr(DfxPlttHeader *, RfxNode *);  // 0x02022920
    virtual void unk_virtual1(s32, u16 *, u8);  // 0x02022928
    virtual u32 unk_virtual2(void);  // 0x02022b10
    virtual void beginDraw(void);  // 0x02022b18
    virtual GXRgb getBlendColor(void);  // 0x020255ac
    virtual nsObjSys::clCellAnime * beginCellAnime(s32, RfxNode *);  // 0x02022df0
    virtual void endCellAnime(nsObjSys::clCellAnime *);  // 0x02022f74
    virtual void func_02023188(s16);  // 0x02023188

    void cmdNodeCallback(s32, u16 *);  // 0x02022c9c

    void cmdCellFrame(s32, u16 *);  // 0x02022fd8
    void cmdDrawNumber(u16 *);  // 0x02023084
    void func_020230e4(s32, u16 *);  // 0x020230e4

    void cmdVertexStrip(s32, u16 *);  // 0x0202318c
    void cmdCircle(s32, u16 *);  // 0x02023368

    void cmdTexQuads(u16 *);  // 0x02023a24
    BOOL drawTexQuad(u16, RfxNode *);  // 0x02023abc
    void cmdSetTexParams(u16 *);  // 0x02023cc8
    void cmdTexMatrix(s32, u16 *);  // 0x02023d64

    void cmdTexStrip(u16 *);  // 0x02023eb0
    void cmdTexStrips(u16 *);  // 0x02024184
    void cmdLitMesh(u16 *);  // 0x02024528

    BOOL setupTexture(u16, RfxNode *);  // 0x02025378
    DfxTexInfo * resolveTexture(u16, RfxNode *);  // 0x02025430    
};

#endif // __cplusplus

#endif
