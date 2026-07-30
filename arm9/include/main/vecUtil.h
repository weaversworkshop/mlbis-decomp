#ifndef MAIN_VEC_UTIL_H
#define MAIN_VEC_UTIL_H

#include "types.h"

#include <nitro.h>

#define VECTOR_COS_INVALID 0xffff

void vector_set_length(VecFx32 *, fx32);  // 0x0201f218
fx32 vector_cosine(VecFx32 *, VecFx32 *);  // 0x0201f26c
fx32 vector_normal_alignment(VecFx32 *, VecFx32 *, VecFx32 *);  // 0x0201f354

#endif
