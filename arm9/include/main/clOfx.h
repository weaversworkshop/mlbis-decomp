#ifndef MAIN_CL_OFX_H
#define MAIN_CL_OFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clAfx.h"

extern "C" {

typedef struct {
    s16 pa;  // +0x0
    s16 pb;  // +0x2
    s16 pc;  // +0x4
    s16 pd;  // +0x6
    s16 x;  // +0x8
    s16 y;  // +0xa
    GXOamEffect attr;  // +0xc
} OfxOamParams;

typedef struct {
    u8 w;  // +0x0
    u8 h;  // +0x1
} OamShapeSize;

extern OamShapeSize oamShapeSizes[12];  // 0x027e0034
extern GXOamShape oamShapes[12];  // 0x027e004c
extern OfxOamParams oamParams;  // 0x027e1180

}

class clOfx : public clAfx {
public:
    MtxFx33 rot;  // +0x20
    s16 originX;  // +0x44
    s16 originY;  // +0x46
    s32 originZ;  // +0x48
    s16 priority;  // +0x4c

    // ~clOfx();  // implicit; D1 0x02026abc D0 0x02026a8c Thn12_D1 0x02026af4 Thn12_D0 0x02026ae4
    virtual s32 getTexAddr(RfxTexHeader *, RfxNode *);  // 0x02025dac
    virtual s32 getPlttAddr(RfxPlttHeader *, RfxNode *, BOOL);  // 0x02025db4
    virtual void unk_virtual1(s32, u16 *, u8);  // 0x02025dbc
    virtual u32 unk_virtual2(void);  // 0x02025ecc
    virtual void beginDraw(void);  // 0x02025ed4
    virtual GXRgb getBlendColor(void);  // 0x02026a54
    virtual nsObjSys::clCellAnime * beginCellAnime(s32, RfxNode *);  // 0x02026850
    virtual void endCellAnime(nsObjSys::clCellAnime *);  // 0x02026930
    virtual s32 func_02023188(s32);  // 0x02026a84

    void endDraw(void);  // 0x02025f24

    void cmdSprite(s32, u16 *);  // 0x02025f28

    void cmdNodeCallback(s32, u16 *);  // 0x02026758

    void cmdCellFrame(s32, u16 *);  // 0x02026948
    void cmdDrawNumber(u16 *);  // 0x020269f4
};

#endif // __cplusplus

#endif
