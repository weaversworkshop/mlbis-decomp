#ifndef MAIN_CL_DFX_SYS_H
#define MAIN_CL_DFX_SYS_H

#include "types.h"

#ifdef __cplusplus

#include "main/clDfx.h"
#include "main/clRfxFileMan.h"
#include "main/clYanaPileMan.h"
#include "main/clYanaTransMan.h"

class clDfxVramMan {
public:
    clYanaPileMan texPile;  // +0x4
    clYanaPileMan plttPile;  // +0x14
    clYanaTransMan * transMan;  // +0x24
    u32 texOffset;  // +0x28
    u32 plttOffset;  // +0x2c
    
    virtual u8 * getTexBase(void);  // 0x02025870
    virtual u8 * getPlttBase(void);  // 0x02025880
};

class clTexFileMan {
public:
    RfxGenPools pools;  // +0x0
};

class clDfxSh : public virtual clDfx {
public:
    s32 shiftX;  // +0x4
    s32 shiftY;  // +0x8

    // ~clDfxSh();  // implicit; D1 0x02025a64 D0 0x02025a34 Tv0_n48_D1 0x02025c2c Tv0_n48_D0 0x02025c1c Tvn12_n48_D1 0x02025c00 Tvn12_n48_D0 0x02025be4
    virtual void onNodePos(RfxNode *);  // 0x02025970 Thn12 0x02025cfc
    virtual void exec(void);  // 0x02025994 Thn12 0x02025cec
};

class clDfxSys : public clYanaUnitSys, public clRfxFileMan, public clTexFileMan {
public:

    virtual ~clDfxSys();  // D1 0x02025b74 D0 0x02025b3c Thn28_D1 0x02025d9c Thn28_D0 0x02025d8c
    virtual void func_02025a30();  // 0x02025a30
};

class clDfxVr : public virtual clDfx {
public:
    clDfxSys * sys;  // +0x4
    clDfxVramMan * vramMan;  // +0x8

    // ~clDfxVr();  // implicit; D1 0x02025abc D0 0x02025a8c Tv0_n48_D1 0x02025c84 Tv0_n48_D0 0x02025c74 Tvn12_n48_D1 0x02025c58 Tvn12_n48_D0 0x02025c3c
    virtual DfxTexHeader * getTexHeader(u32);  // 0x02025890 Thn12 0x02025bd4 Thn24 0x02025d1c
    virtual UnkStruct_020224c4 * getPlttHeader(u32);  // 0x020258b0 Thn12 0x02025bc4 Thn24 0x02025d0c
    virtual s32 getTexAddr(DfxTexHeader *, RfxNode *);  // 0x020258d0 Tv0_n12 0x02025d5c
    virtual s32 getPlttAddr(UnkStruct_020224c4 *, RfxNode *);  // 0x020258ec Tv0_n16 0x02025d4c
    virtual u32 extScriptCount(void);  // 0x02025908 Thn12 0x02025ba4 Thn24 0x02025ccc
    virtual u16 * extScript(s32);  // 0x02025938 Thn12 0x02025bb4 Thn24 0x02025cdc
};

class clDfxEx : public clDfxVr, public clDfxSh {
public:

    virtual ~clDfxEx();  // D1 0x02025b14 D0 0x02025ae4 Thn12_D1 0x02025d7c Thn12_D0 0x02025d6c Tv0_n48_D1 0x02025d3c Tv0_n48_D0 0x02025d2c Tvn12_n48_D1 0x02025cb0 Tvn12_n48_D0 0x02025c94
};

#endif // __cplusplus

#endif
