#ifndef MAIN_CL_OBJ_PLT_VRAM_MAN_H
#define MAIN_CL_OBJ_PLT_VRAM_MAN_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
    #include <nitro.h>
}

typedef struct {
    u8 pad_00[0x8];
    u32 field08;
    u32 field0c;
} UnkStruct_02029b08;

namespace nsObjSys {

class clObjPltVramMan {
public:

    GXOamAttr * field04;
    u8 pad_08[0x10];
    GXOamAttr * field18;

    virtual void func_0202ac34();
    virtual void func_0202ab84();
    virtual void func_0202abd0(u32);
    virtual UnkStruct_02029b08 * func_02014380(u32, u32, u32);  // 0x02014380
    virtual void func_0202a134(void *, s32, s32);
    virtual void func_0202ab98(u32);
    virtual UnkStruct_02029b08 * func_02029b08(u32, u32, u32);
    virtual UnkStruct_02029b08 * func_02029c9c(u32, u32, u32);
    virtual void func_02029e54(void *, u32, u32, u32);
    virtual void func_02029f9c(void *, u32, u32, u32);
    virtual void func_0202a224(u32);
    virtual void func_0202a35c(u32);
    virtual void func_0202a508(void *, u32, u32, u32);
};

}

// extern "C" {
#endif // __cplusplus

// #ifdef __cplusplus
// } // extern "C"
// #endif

#endif
