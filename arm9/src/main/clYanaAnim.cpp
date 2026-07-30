// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clYanaAnim.h"

extern "C" {

// --- Extern Functions --- //

extern s64 func_0204536c(s32, s32);

// --- Variable Definitions --- //

u8 animEnded;  // 0x0205bb20

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clYanaAnim::init(u16 length) {
    this->pos = 0;
    this->needsRewind = 1;
    this->loopsRemaining = 1;
    this->speed = 256;
    this->length = length;
    this->atEnd = 0;
    this->stopped = 0;
}

// MATCHED 100%
clYanaAnim::~clYanaAnim() {
    atEnd = 1;
}

// MATCHED 100%
void clYanaAnim::beginUpdate() {
    animEnded = 0;

    if (this->needsRewind != 0) {
        this->needsRewind = 0;
        this->pos = (this->speed < 0) ? (this->length << 8) : 0;
    }
}

// MATCHED 100%
void clYanaAnim::advance() {
    if (this->stopped == 0) {
        s32 end = this->length << 8;

        if (end == 0) {
            this->atEnd = 1;
            this->pos = 0;
        } else {
            s32 next = this->pos + this->speed;
            if (next > 0 && next < end) {
                this->atEnd = 0;
            } else {
                this->atEnd = 1;

                if (this->loopsRemaining == 1) {
                    next = (next < 0) ? 0 : end;
                } else {
                    next = (s32)(func_0204536c(next, end) >> 32);
                    if (next < 0) {
                        next += end;
                    }
                }
            }

            this->pos = next;
        }
    }
}

// MATCHED 100%
void clYanaAnim::updateLoop() {
    if (this->stopped == 0 && this->atEnd != 0 && this->loopsRemaining != 0) {
        if (--this->loopsRemaining == 0) {
            this->stopped = 1;
            animEnded = 1;

            this->onEnd();
        }
    }
}

// MATCHED 100%
void clYanaAnim::onEnd() {
    this->atEnd = 1;
}

// MATCHED 100%
void clYanaAnim::seekEnd() {
    this->pos = this->length << 8;

    if (this->loopsRemaining == 0) {
        this->stopped = 0;
    } else {
        this->loopsRemaining = 1;
    }
}
