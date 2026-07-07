#ifndef MAIN_CL_YANA_ANIM_H
#define MAIN_CL_YANA_ANIM_H

#include "types.h"

#ifdef __cplusplus

class clYanaAnim {
public:
    s32 field04_00 : 23;
    u32 field04_23 : 1;
    u32 field04_24 : 1;
    u32 field04_25 : 7;

    s16 field08;

    u16 field0a_00 : 14;
    u16 field0a_14 : 1;
    u16 field0a_15 : 1;

    virtual ~clYanaAnim();
    virtual void m08();
    virtual void m0c();

    void func_020206e0(u16 param1);
    void func_02020784();
    void func_020207ec();
    void func_020208c0();
};

#endif
#endif
