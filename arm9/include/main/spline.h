#ifndef MAIN_SPLINE_H
#define MAIN_SPLINE_H

#include "types.h"

#include <nitro/fx/fx_cp.h>

#ifdef __cplusplus

class Spline {
public:
    u8 bufCount;  // +0x0
    u8 nPoints;  // +0x1
    u8 heapId;  // +0x2
    u8 pad_03[0x1];
    fx32 totalLength;  // +0x4
    fx32 * xs;  // +0x8
    fx32 * ys;  // +0xc
    fx32 * zs;  // +0x10
    fx32 * knots;  // +0x14
    fx32 * coefsX;  // +0x18
    fx32 * coefsY;  // +0x1c
    fx32 * coefsZ;  // +0x20

    Spline();  // C1 0x0201de08
    ~Spline();  // D1 0x0201de40

    void build(fx32 * xs, fx32 * ys, fx32 * zs, u8 nPoints, fx32 * workBuf); // 0x0201de64
    fx32 getTotalLength(void);  // 0x0201e068
    void evalPoint(fx32 t, fx32& outX, fx32& outY, fx32& outZ);  // 0x0201e070
    void setBuffer(u8 count, fx32 * buf);  // 0x0201e0ec
};

#endif // __cplusplus

#endif
