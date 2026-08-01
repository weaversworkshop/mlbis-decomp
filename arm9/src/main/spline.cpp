// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/spline.h"

#include "main/heap.h"

extern "C" {

// --- Function Declarations --- //

void solveSplineCoeffs(s32 nPoints, s32 * values, fx32 * knots, fx32 * dknots, fx32 * coeffs, fx32 * slopes);  // 0x0201e11c
fx32 evalSplineAxis(fx32 t, u8 nPoints, fx32 * values, fx32 * knots, fx32 * coefs);  // 0x0201e294

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
Spline::Spline() {
    this->bufCount = 0;
    this->nPoints = 0;
    this->heapId = -1;
    this->totalLength = 0;
    this->xs = NULL;
    this->ys = NULL;
    this->zs = NULL;
    this->knots = NULL;
    this->coefsX = NULL;
    this->coefsY = NULL;
    this->coefsZ = NULL;
}

// MATCHED 100%s
Spline::~Spline() {
    if (this->heapId != (u8)-1) {
        delete[](this->knots);
    }
}

// MATCHED 100%
void Spline::build(fx32 * xs, fx32 * ys, fx32 * zs, u8 nPoints, fx32 * workBuf) {
    u8 i;
    fx32 * buf = workBuf;

    if (buf == NULL) {
        buf = new(this->heapId, (void *)"SplineTempWork", FALSE) fx32[nPoints * 2];
    }

    this->xs = xs;
    this->ys = ys;
    this->zs = zs;
    this->nPoints = nPoints;
    this->totalLength = 0;
    this->knots[0] = 0;

    fx32 * ptr = &buf[nPoints];

    for (i = 1; i < nPoints; i++) {
        this->totalLength += FX_Sqrt(FX_MUL(xs[i] - xs[i-1], xs[i] - xs[i-1])
                                 + FX_MUL(ys[i] - ys[i-1], ys[i] - ys[i-1])
                                 + FX_MUL(zs[i] - zs[i-1], zs[i] - zs[i-1]));

        this->knots[i] = this->totalLength;
    }

    for (i = 1; i < nPoints; i++) {
        this->knots[i] = FX_Div(this->knots[i], this->totalLength);
        buf[i-1] = this->knots[i] - this->knots[i-1];
    }

    solveSplineCoeffs(this->nPoints, xs, this->knots, buf, this->coefsX, ptr);
    solveSplineCoeffs(this->nPoints, ys, this->knots, buf, this->coefsY, ptr);
    solveSplineCoeffs(this->nPoints, zs, this->knots, buf, this->coefsZ, ptr);

    if (workBuf == NULL) {
        delete[](buf);
    }
}

// MATCHED 100%
fx32 Spline::getTotalLength() {
    return this->totalLength;
}

// MATCHED 100%
void Spline::evalPoint(fx32 t, fx32& outX, fx32& outY, fx32& outZ) {
    outX = evalSplineAxis(t, this->nPoints, this->xs, this->knots, this->coefsX);
    outY = evalSplineAxis(t, this->nPoints, this->ys, this->knots, this->coefsY);
    outZ = evalSplineAxis(t, this->nPoints, this->zs, this->knots, this->coefsZ);
}

// MATCHED 100%
void Spline::setBuffer(u8 count, fx32 * buf) {
    this->bufCount = count;
    this->knots = buf;
    this->coefsX = &buf[count];
    this->coefsY = &this->knots[count * 2];
    this->coefsZ = &this->knots[count * 3];
}

// MATCHED 100%
void solveSplineCoeffs(s32 nPoints, s32 * values, fx32 * knots, fx32 * dknots, fx32 * coeffs, fx32 * slopes) {
    s32 count = nPoints;
    s32 n;
    u8 i;

    n = count-1;
    coeffs[0] = 0;
    coeffs[n] = 0;

    for (i = 0; i < n; i++) {
        slopes[i+1] = FX_Div(values[i+1] - values[i], dknots[i]);
    }

    n = count-2;
    coeffs[1] = slopes[2] - slopes[1];
    slopes[1] = (knots[2] - knots[0]) * 2;

    for (i = 1; i < n; i++) {
        fx32 div = FX_Div(dknots[i], slopes[i]);
        fx32 num = coeffs[i];
        coeffs[i+1] = (slopes[i+2] - slopes[i+1]) - FX_MUL(num, div);
        num = dknots[i];
        slopes[i+1] = (knots[i+2] - knots[i]) * 2 - FX_MUL(num, div);
    }

    for (i = count-2; i != 0; i--) {
        coeffs[i] = FX_Div(coeffs[i] - FX_MUL(dknots[i], coeffs[i+1]), slopes[i]);
    }
}

// MATCHED 100%
fx32 evalSplineAxis(fx32 t, u8 nPoints, fx32 * values, fx32 * knots, fx32 * coefs) {
    s32 right = nPoints-1;
    s32 left = 0;

    while (left < right) {
        s32 mid = (left + right) / 2;
        if (knots[mid] < t) {
            left = mid+1;
        } else {
            right = mid;
        }
    }

    if (left > 0) {
        left--;
    }

    s32 next = left+1;
    fx32 dt = t - knots[left];
    fx32 h = knots[next] - knots[left];
    fx32 c0 = coefs[left];
    fx32 c1 = coefs[next];
    fx32 x0 = values[left];
    fx32 x1 = values[next];
    fx32 tmp = c1 - c0;
    fx32 acc = c0 * 3 + FX_Div(FX_MUL(tmp, dt), h);
    fx32 slope = FX_Div(x1 - x0, h);
    tmp = c1 + c0 * 2;
    slope -= FX_MUL(tmp, h);
    tmp = FX_MUL(acc, dt);
    acc = tmp + slope;
    return x0 + FX_MUL(acc, dt);
}

