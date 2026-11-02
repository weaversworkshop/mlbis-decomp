#ifndef MAIN_PAL_ANIM_H
#define MAIN_PAL_ANIM_H

#include "types.h"

typedef u16 (*BlendFunc)(u16, u16, s32);

u16 colorLerp(u16, u16, s32);  // 0x0201dcb0
u16 colorAddSaturation(u16, u16, s32);  // 0x0201dcec
u16 colorSubSaturation(u16, u16, s32);  // 0x0201dd2c
u16 colorLuminanceBlend(u16, u16, s32);  // 0x0201dd74
u16 colorInvertLerp(u16, s32);  // 0x0201ddd0

#endif
