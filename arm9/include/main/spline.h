#ifndef MAIN_SPLINE_H
#define MAIN_SPLINE_H

#include "types.h"

#include <nitro/fx/fx_cp.h>

#ifdef __cplusplus

class Spline {
public:
    u8 bufCount;
    u8 nPoints;
    u8 heapId;
    u8 pad_03[0x1];
    fx32 totalLength;
    fx32 * xs;
    fx32 * ys;
    fx32 * zs;
    fx32 * knots;
    fx32 * coefsX;
    fx32 * coefsY;
    fx32 * coefsZ;

    Spline();  // C1 0x0201de08
    ~Spline();  // D1 0x0201de40

    void build(fx32 * xs, fx32 * ys, fx32 * zs, u8 nPoints, fx32 * workBuf); // 0x0201de64
    fx32 getTotalLength();  // 0x0201e068
    void evalPoint(fx32 t, fx32& outX, fx32& outY, fx32& outZ);  // 0x0201e070
    void setBuffer(u8 count, fx32 * buf);  // 0x0201e0ec
};

#endif // __cplusplus

#endif
