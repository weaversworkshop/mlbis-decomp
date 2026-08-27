#ifndef MAIN_CL_YANA_ANIM_H
#define MAIN_CL_YANA_ANIM_H

#include "types.h"

#ifdef __cplusplus

class clYanaAnim {
public:
    s32 pos : 23;  // +0x4
    u32 unk04_23 : 1;
    u32 needsRewind : 1;
    u32 loopsRemaining : 7;
    s16 speed;  // +0x8
    u16 length : 14;  // +0xa
    u16 atEnd : 1;
    u16 stopped : 1;

    virtual ~clYanaAnim();  // D1 0x02020744 D0 0x02020754 D2 0x02020774
    virtual void onEnd(void);  // 0x02020930
    virtual void seekEnd(void);  // 0x02020940

    void init(u16);  // 0x020206e0
    void beginUpdate(void);  // 0x02020784
    void advance(void);  // 0x020207ec
    void updateLoop(void);  // 0x020208c0
};

#endif
#endif
