#ifndef MAIN_VECMATH_H
#define MAIN_VECMATH_H

#include "types.h"

#include <nitro/fx/fx.h>

s32 idiv(s32, s32);  // 0x0201ea90
u32 isqrt(u32);  // 0x0201ec50
u32 func_0201ed3c(void);  // 0x0201ed3c
void vec_cross_product(VecFx32 *, VecFx32 *, VecFx32 *);  // 0x0201ed48
fx32 vector_magnitude(VecFx32 *);  // 0x0201ee14
s32 mul_div(s32 a, s32 b, s32 c);  // 0x0201ee64
#endif
