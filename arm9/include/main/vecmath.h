#ifndef MAIN_VECMATH_H
#define MAIN_VECMATH_H

#include "types.h"

#include <nitro/fs.h>

s32 idiv(s32 dividend, s32 divisor);  // 0x0201ea90
u32 isqrt(u32 val);  // 0x0201ec50
u32 func_0201ed3c();  // 0x0201ed3c
void vec_cross_product(s32 * outVec, s32 * vecA, s32 * vecB);  // 0x0201ed48
s32 vector_magnitude(fx32 * vec);  // 0x0201ee14

#endif
