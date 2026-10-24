#ifndef MAIN_CL_AFX_H
#define MAIN_CL_AFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clCellAnime.h"
#include "main/clRfx.h"

class clAfx : public clRfx {
public:

    // ~clAfx();  // implicit; D1 0x020228d0 D0 0x020228a0 Thn12_D1 0x02022908 Thn12_D0 0x020228f8
    virtual u32 func_020224bc(void);  // 0x020224bc
    virtual u32 func_020224c4(void);  // 0x020224c4
    virtual void func_020224cc(void);  // 0x020224cc
    virtual u32 func_020224d0(void);  // 0x020224d0
    virtual void func_020224d8(void (*)(void), u32, s32, s32);  // 0x020224d8
    virtual u16 getBlendColor();  // 0x020224dc
    virtual u32 func_020224e4();  // 0x020224e4
    virtual u32 func_020224ec();  // 0x020224ec
    virtual nsObjSys::clCellAnime * func_020224f4(s32, u32);  // 0x020224f4
    virtual void func_020224fc(nsObjSys::clCellAnime *);  // 0x020224fc
    virtual void setCellFrame(s32, s32, u32);  // 0x02022500
    virtual void drawNumber(s32, s32, u32, s32, u32);  // 0x020225e4

    void runColorBlendTrack(s32, u16 *);  // 0x0202273c
};

#endif // __cplusplus

#endif
