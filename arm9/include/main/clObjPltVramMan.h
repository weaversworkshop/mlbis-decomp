#ifndef MAIN_CL_OBJ_PLT_VRAM_MAN_H
#define MAIN_CL_OBJ_PLT_VRAM_MAN_H

#include "types.h"

#ifdef __cplusplus

#include "main/clHandleMan.h"

extern "C" {
    #include <nitro.h>
}

namespace nsObjSys {

class clObjPltVramMan : public clVramManBase {
public:
    VramHandle * head[2];  // +0x8
    u16 extMap[16];  // +0x10
    u16 stdMap;  // +0x30
    u16 base;  // +0x32
    u16 size;  // +0x34

    clObjPltVramMan(u8, u32, u32, u32);  // C1 0x020296c4
    virtual ~clObjPltVramMan() {};  // D1 0x0202ac34 D0 0x0202ab84
    virtual u32 getVramAddr(VramHandle *);  // 0x0202abd0
    virtual VramHandle * alloc(u32, u32, u32);  // 0x02014380
    virtual BOOL release(VramHandle *, s32, u32);  // 0x0202a134
    virtual u32 compact(u32);  // 0x0202ab98
    virtual VramHandle * allocStd(u32, u32, u32);  // 0x02029b08
    virtual VramHandle * allocExt(u32, u32, u32);  // 0x02029c9c
    virtual VramHandle * allocStdAt(u32, u32, u32, u32);  // 0x02029e54
    virtual VramHandle * allocExtAt(u32, u32, u32, u32);  // 0x02029f9c
    virtual u32 compactStd(u32);  // 0x0202a224
    virtual u32 compactExt(u32);  // 0x0202a35c
    virtual VramHandle * findShared(u32, u16, u32, u32, s16);  // 0x0202a508

    void init(u32, u32);  // 0x02029724
    VramHandle * linkFromBack(u32, u32, u32, u16);  // 0x0202984c
    VramHandle * linkFromFront(u32, u32, u32, u16);  // 0x020299b0

    VramHandle * insert(VramHandle *, VramHandle *, u32, u32, u32, u16);
};

}

#endif // __cplusplus

#endif
