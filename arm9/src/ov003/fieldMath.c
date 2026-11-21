// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/fieldMath.h"

#include <nitro.h>

// --- Variable Definitions --- //

const s16 dir8Vecs[8][2] = {  // 0x0207e518
    { 0, -FX16_ONE },
    { FX16_CONST(0.75), -FX16_CONST(0.75) },
    { FX16_ONE, 0 },
    { FX16_CONST(0.75), FX16_CONST(0.75) },
    { 0, FX16_ONE },
    { -FX16_CONST(0.75), FX16_CONST(0.75) },
    { -FX16_ONE, 0 },
    { -FX16_CONST(0.75), -FX16_CONST(0.75) },
};

// --- Function Declarations --- //

fx32 ellipseCircumference(fx32, fx32);  // 0x0206b9bc
fx32 vecLength2D(fx32, fx32);  // 0x0206ba54
void dir8ToVec(u32, fx32, fx32 *, fx32 *);  // 0x0206bae4
void clampVecLength3D(fx32, fx32, fx32, fx32, fx32 *, fx32 *, fx32 *);  // 0x0206bb50
void clampVecLength2D(fx32, fx32, fx32, fx32 *, fx32 *);  // 0x0206bc98

// --- Function Definitions --- //

// MATCHED 100% -- Note: ellipse path seems to be computed wrong. should be (a² + b²)/2, has a² + b²/2
fx32 ellipseCircumference(fx32 radiusA, fx32 radiusB) {
    if (radiusB == 0 || radiusA == radiusB) {
        return FX_Mul32x64c(radiusA * 2, FX64C_PI);
    } else {
        return FX_Mul32x64c(FX_Sqrt(FX_Mul(radiusA, radiusA) + FX_Mul(radiusB, radiusB) / 2), FX64C_PI * 2);
    }
}

// MATCHED 100%
fx32 vecLength2D(fx32 x, fx32 y) {
    if (x < 0) {
        x = -x;
    }
    if (y < 0) {
        y = -y;
    }

    s32 bits = 32;
    s32 shift = MATH_CountLeadingZeros(x);
    if (bits > shift) {
        bits = shift;
    }

    shift = MATH_CountLeadingZeros(y);
    if (bits > shift) {
        bits = shift;
    }

    if (bits <= 16) {
        shift = 17 - bits;
        y >>= shift;
        x >>= shift;
    }

    fx32 len = FX_Sqrt(FX_Mul(x, x) + FX_Mul(y, y));
    if (bits <= 16) {
        len <<= shift;
    }

    return len;
}

// MATCHED 100%
void dir8ToVec(u32 dir, fx32 len, fx32 * outX, fx32 * outY) {
    if (dir > 7) {
        *outY = 0;
        *outX = 0;
    } else {
        *outX = FX_Mul(len, dir8Vecs[dir][0]);
        *outY = FX_Mul(len, dir8Vecs[dir][1]);
    }
}

// MATCHED 100%
void clampVecLength3D(fx32 x, fx32 y, fx32 z, fx32 maxLen, fx32 * outX, fx32 * outY, fx32 * outZ) {
    s32 shift;
    if (x >= FX32_CONST(4096) || x < -FX32_CONST(4096) || y >= FX32_CONST(4096) || y < -FX32_CONST(4096) || z >= FX32_CONST(4096) || z < -FX32_CONST(4096)) {
        shift = 8;
    } else if (x >= FX32_CONST(256) || x < -FX32_CONST(256) || y >= FX32_CONST(256) || y < -FX32_CONST(256) || z >= FX32_CONST(256) || z < -FX32_CONST(256)) {
        shift = 4;
    } else {
        shift = 0;
    }

    fx32 ratio = FX_Sqrt(FX_Mul(x >> shift, x >> shift) + FX_Mul(y >> shift, y >> shift) + FX_Mul(z >> shift, z >> shift)) << shift;
    ratio = FX_Div(ratio, maxLen);
    if (ratio > FX32_ONE) {
        *outX = FX_Div(x, ratio);
        *outY = FX_Div(y, ratio);
        *outZ = FX_Div(z, ratio);
    } else {
        *outX = x;
        *outY = y;
        *outZ = z;
    }
}

// MATCHED 100%
void clampVecLength2D(fx32 x, fx32 y, fx32 maxLen, fx32 * outX, fx32 * outY) {
    s32 shift;
    if (x >= FX32_CONST(4096) || x < -FX32_CONST(4096) || y >= FX32_CONST(4096) || y < -FX32_CONST(4096)) {
        shift = 8;
    } else if (x >= FX32_CONST(256) || x < -FX32_CONST(256) || y >= FX32_CONST(256) || y < -FX32_CONST(256)) {
        shift = 4;
    } else {
        shift = 0;
    }

    fx32 ratio = FX_Sqrt(FX_Mul(x >> shift, x >> shift) + FX_Mul(y >> shift, y >> shift)) << shift;
    ratio = FX_Div(ratio, maxLen);
    if (ratio > FX32_ONE) {
        *outX = FX_Div(x, ratio);
        *outY = FX_Div(y, ratio);
    } else {
        *outX = x;
        *outY = y;
    }
}
