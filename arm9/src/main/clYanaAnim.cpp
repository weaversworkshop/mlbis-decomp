// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clYanaAnim.h"

extern "C" {

// --- Extern Data --- //

extern u8 data_0205bb20[];

// --- Extern Functions --- //

extern s64 func_0204536c(s32 param1, s32 param2);

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clYanaAnim::func_020206e0(u16 param1) {
    this->field04_00 = 0;
    this->field04_18 = 1;
    this->field04_19 = 1;
    this->field08 = 0x100;
    this->field0a_00 = param1;
    this->field0a_0e = 0;
    this->field0a_0f = 0;
}

// MATCHED 100%
clYanaAnim::~clYanaAnim() {
    field0a_0e = 1;
}

// MATCHED 100%
void clYanaAnim::func_02020784() {
    data_0205bb20[0] = 0;

    if (this->field04_18 != 0) {
        this->field04_18 = 0;
        this->field04_00 = (this->field08 < 0) ? (this->field0a_00 << 8) : 0;
    }
}

// MATCHED 100%
void clYanaAnim::func_020207ec() {
    if (this->field0a_0f == 0) {
        s32 r4 = this->field0a_00 << 8;

        if (r4 == 0) {
            this->field0a_0e = 1;
            this->field04_00 = 0;
        } else {
            s32 val = this->field04_00 + this->field08;
            if (val > 0 && val < r4) {
                this->field0a_0e = 0;
            } else {
                this->field0a_0e = 1;

                if (this->field04_19 == 1) {
                    val = (val < 0) ? 0 : r4;
                } else {
                    val = (s32)(func_0204536c(val, r4) >> 32);
                    if (val < 0) {
                        val += r4;
                    }
                }
            }

            this->field04_00 = val;
        }
    }
}

// MATCHED 100%
void clYanaAnim::func_020208c0() {
    if (this->field0a_0f == 0 && this->field0a_0e != 0 && this->field04_19 != 0) {
        if (--this->field04_19 == 0) {
            this->field0a_0f = 1;
            data_0205bb20[0] = 1;

            this->m08();
        }
    }
}

// MATCHED 100%
void clYanaAnim::m08() {
    this->field0a_0e = 1;
}

// MATCHED 100%
void clYanaAnim::m0c() {
    this->field04_00 = this->field0a_00 << 8;

    if (this->field04_19 == 0) {
        this->field0a_0f = 0;
    } else {
        this->field04_19 = 1;
    }
}