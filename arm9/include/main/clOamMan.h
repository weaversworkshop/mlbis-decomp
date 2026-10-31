#ifndef MAIN_CL_OAM_MAN_H
#define MAIN_CL_OAM_MAN_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
    #include <nitro.h>
}

namespace nsObjSys {

class clOamMan {
public:
    GXOamAttr * field04;
    u8 pad_08[0x4];
    u8 pad_0c[0x4]; // void * field0c;
    u8 pad_10[0x4]; // void * field10;
    u8 pad_14[0x4];
    GXOamAttr * field18;
    u8 pad_1c[0x4];
    u8 pad_20[0x4]; // void * field20;

    virtual void func_0202a6ac();
    virtual void func_0202a6fc();
    virtual void func_0202a754();
    virtual void * func_0202a780(s32, s32, s32);
    virtual void func_0202a7a4();
    virtual void func_0202a960();
    virtual s32 func_0202aaac(s16, s16, s16, s16);
};

}

#endif // __cplusplus

#endif
