#ifndef MAIN_CL_OFX_SYS_H
#define MAIN_CL_OFX_SYS_H

#include "types.h"

#ifdef __cplusplus

#include "main/clOfx.h"
#include "main/clYanaPileMan.h"
#include "main/clYanaTransMan.h"

class clOfxVramMan {
public:
    clYanaPileMan texPile;  // +0x4
    clYanaPileMan plttPile;  // +0x14
    clYanaPileMan extPlttPile;  // +0x24
    clYanaTransMan * transMan;  // +0x34
    u32 texOffset;  // +0x38
    u16 plttOffset;  // +0x3c
    u16 extPlttOffset;  // +0x3e
    u16 engine;  // +0x40

    virtual u8 * getTexBase(u32);  // 0x02026dc8
    virtual u8 * getPlttBase(u32, u32);  // 0x02026dd8
};

class clOfxSh : public virtual clOfx {
public:
    s32 shiftX;  // +0x4
    s32 shiftY;  // +0x8

    // ~clOfxSh();  // implicit; D1 0x02026ff8 D0 0x02026fc8 Tv0_n48_D1 0x020271d0 Tv0_n48_D0 0x020271c0 Tvn12_n48_D1 0x020271a4 Tvn12_n48_D0 0x02027188
    virtual void onNodePos(RfxNode *);  // 0x02026efc Thn12 0x020272b0
    virtual void exec(void);  // 0x02026f20 Thn12 0x02027290
};

class clOfxSys : public clYanaUnitSys, public clRfxFileMan, public clTexFileMan {
public:
    nsObjSys::clOamMan * oamMan;  // +0x848

    virtual ~clOfxSys();  // D1 0x02027108 D0 0x020270d0 Thn28_D1 0x02027350 Thn28_D0 0x02027340
    virtual void func_02026fc4();  // 0x02026fc4
};

class clOfxVr : public virtual clOfx {
public:
    clOfxSys * sys;  // +0x4
    clOfxVramMan * vramMan;  // +0x8

    // ~clOfxVr();  // implicit; D1 0x02027050 D0 0x02027020 Tv0_n48_D1 0x02027228 Tv0_n48_D0 0x02027218 Tvn12_n48_D1 0x020271fc Tvn12_n48_D0 0x020271e0
    virtual nsObjSys::clOamMan * getOamMan(void);  // 0x02026e08 Thn12 0x02027158 Thn24 0x020272a0
    virtual RfxTexHeader * getTexHeader(u32);  // 0x02026e1c Thn12 0x02027178 Thn24 0x020272d0
    virtual RfxPlttHeader * getPlttHeader(u32);  // 0x02026e3c Thn12 0x02027168 Thn24 0x020272c0
    virtual s32 getTexAddr(RfxTexHeader *, RfxNode *);  // 0x02026e5c Tv0_n12 0x02027310
    virtual s32 getPlttAddr(RfxPlttHeader *, RfxNode *, BOOL);  // 0x02026e78 Tv0_n16 0x02027300
    virtual u32 extScriptCount(void);  // 0x02026e94 Thn12 0x02027138 Thn24 0x02027270
    virtual u16 * extScript(s32);  // 0x02026ec4 Thn12 0x02027148 Thn24 0x02027280
};

class clOfxEx : public clOfxVr, public clOfxSh {
public:

    virtual ~clOfxEx();  // D1 0x020270a8 D0 0x02027078 Thn12_D1 0x02027330 Thn12_D0 0x02027320 Tv0_n48_D1 0x020272f0 Tv0_n48_D0 0x020272e0 Tvn12_n48_D1 0x02027254 Tvn12_n48_D0 0x02027238
};

#endif // __cplusplus

#endif
