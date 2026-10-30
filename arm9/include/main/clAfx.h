#ifndef MAIN_CL_AFX_H
#define MAIN_CL_AFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clCellAnime.h"
#include "main/clRfx.h"

typedef struct {
    u8 wBlocks;  // +0x0
    u8 hBlocks;  // +0x1
    u8 field02;
    u8 fmt;  // +0x3
} DfxTexHeader;

typedef struct {
    u8 pad_00[0x4];
    u8 field04[];
} UnkStruct_020224c4;

class clAfx : public clRfx {
public:

    // ~clAfx();  // implicit; D1 0x020228d0 D0 0x020228a0 Thn12_D1 0x02022908 Thn12_D0 0x020228f8
    virtual DfxTexHeader * getTexHeader(u32);  // 0x020224bc
    virtual UnkStruct_020224c4 * getPlttHeader(u32);  // 0x020224c4
    virtual void onNodePos(RfxNode *);  // 0x020224cc
    virtual u32 func_020224d0(void);  // 0x020224d0
    virtual void func_020224d8(void (*)(void), GXRgb, s32, s32);  // 0x020224d8
    virtual GXRgb getBlendColor(void);  // 0x020224dc
    virtual u32 func_020224e4();  // 0x020224e4
    virtual nsObjSys::clCellAnime * func_020224ec(s32);  // 0x020224ec
    virtual nsObjSys::clCellAnime * beginCellAnime(s32, RfxNode *);  // 0x020224f4
    virtual void endCellAnime(nsObjSys::clCellAnime *);  // 0x020224fc
    virtual void setCellFrame(s32, s32, RfxNode *);  // 0x02022500
    virtual void drawNumber(s32, s32, RfxNode *, s32, u32);  // 0x020225e4

    void runColorBlendTrack(s32, u16 *);  // 0x0202273c
};

#endif // __cplusplus

#endif
