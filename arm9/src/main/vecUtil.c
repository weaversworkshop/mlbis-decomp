// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/vecmath.h"
#include "main/vecUtil.h"

// --- Struct Definitions --- //

typedef struct { 
    fx32 unused;  // +0x0
    VecFx32 sq2, sq1, prod, cross;  // +0x28
} VectorScratchData;

// --- Variable Definitions --- //

VectorScratchData vectorScratch;  // 0x0205baec

// --- Function Definitions --- //

// MATCHED 100%
void vector_set_length(VecFx32 * vec, fx32 length) {
    fx32 mag = vector_magnitude(vec);
    vec->x = mul_div(vec->x, length, mag);
    vec->y = mul_div(vec->y, length, mag);
    vec->z = mul_div(vec->z, length, mag);
}

// MATCHED 100%
fx32 vector_cosine(VecFx32 * a, VecFx32 * b) {
    fx32 x1 = a->x;
    vectorScratch.sq1.x = x1 * x1;
    fx32 y1 = a->y;
    vectorScratch.sq1.y = y1 * y1;
    fx32 z1 = a->z;
    vectorScratch.sq1.z = z1 * z1;

    fx32 x2 = b->x;
    vectorScratch.sq2.x = x2 * x2;
    fx32 y2 = b->y;
    vectorScratch.sq2.y = y2 * y2;
    fx32 z2 = b->z;
    vectorScratch.sq2.z = z2 * z2;

    vectorScratch.prod.x = x1 * x2;
    vectorScratch.prod.y = y1 * y2;
    vectorScratch.prod.z = z1 * z2;

    fx32 sqSum1 = vectorScratch.sq1.x + vectorScratch.sq1.y + vectorScratch.sq1.z;
    fx32 sqSum2 = vectorScratch.sq2.x + vectorScratch.sq2.y + vectorScratch.sq2.z;
    u32 root = isqrt((sqSum1 / 1024) * (sqSum2 / 1024));
    if (root == 0) {
        return 0xffff;
    }

    fx32 dot = vectorScratch.prod.x + vectorScratch.prod.y + vectorScratch.prod.z;
    fx32 result = (dot * 4) / (fx32)root;
    if (result > FX32_ONE) {
        result = FX32_ONE;
    }
    if (result < -FX32_ONE) {
        result = -FX32_ONE;
    }
    return result;
}

// MATCHED 100%
fx32 vector_normal_alignment(VecFx32 * v, VecFx32 * a, VecFx32 * b) {
    vec_cross_product(&vectorScratch.cross, a, b);
    vector_set_length(&vectorScratch.cross, FX32_ONE);

    fx32 cos = vector_cosine(&vectorScratch.cross, v);
    return (cos == VECTOR_COS_INVALID) ? VECTOR_COS_INVALID : (cos + FX32_ONE) / 2;
}
