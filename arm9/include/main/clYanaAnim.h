#ifndef MAIN_CL_YANA_ANIM_H
#define MAIN_CL_YANA_ANIM_H

#include "types.h"

#ifdef __cplusplus

// EXPORT this to the right TU when able
class clYanaAnim {
public:
    s32 field04_00 : 23;
    u32 field04_17 : 1;
    u32 field04_18 : 1;
    u32 field04_19 : 7;

    s16 field08;

    u16 field0a_00 : 14;
    u16 field0a_0e : 1;
    u16 field0a_0f : 1;

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
