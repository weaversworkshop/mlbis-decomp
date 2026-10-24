// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clAfx.h"

extern "C" {

#include <nitro.h>

#include "main/uiUtil.h"

// --- Extern Data Variables --- //

typedef void (* ColorBlendFunc)(void);
extern ColorBlendFunc colorBlendFuncs[4];  // 0x02054ff8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
u32 clAfx::func_020224bc() {
    return 0;
}

// MATCHED 100%
u32 clAfx::func_020224c4() {
    return 0;
}

// MATCHED 100%
void clAfx::func_020224cc() {}

// MATCHED 100%
u32 clAfx::func_020224d0() {
    return 0x7b;
}

// MATCHED 100%
void clAfx::func_020224d8(void (* param1)(void), u32 param2, s32 param3, s32 param4) {}

// MATCHED 100%
u16 clAfx::getBlendColor() {
    return 0;
}

// MATCHED 100%
u32 clAfx::func_020224e4() {
    return 0;
}

// MATCHED 100%
u32 clAfx::func_020224ec() {
    return 0;
}

// MATCHED 100%
nsObjSys::clCellAnime * clAfx::func_020224f4(s32 param1, u32 param2) {
    return NULL;
}

// MATCHED 100%
void clAfx::func_020224fc(nsObjSys::clCellAnime * param1) {}

// MATCHED 100%
void clAfx::setCellFrame(s32 idx, s32 frame, u32 param3) {
    nsObjSys::clCellAnime * ret = this->func_020224f4(idx, param3);

    if (ret != NULL) {
        if (frame >= 0 && frame < ret->func_020225d0()) {
            if (idx >= 0 && frame != ret->func_020225dc()) {
                ret->func_0202dee8(frame, -1);
            }

            if (ret->field81 != 1 || (s32)ret->field98 > 0) {
                ret->func_0202bb44(this->func_020224e4());
            }
        }

        this->func_020224fc(ret);
    }
}

// MATCHED 100%
void clAfx::drawNumber(s32 idx, s32 digitFrame, u32 param3, s32 spacing, u32 align) {
    nsObjSys::clCellAnime * ret = this->func_020224f4(idx, param3);

    if (ret != NULL) {
        s32 val = this->func_020224d0();
        s32 digits = countDigits(val);
        spacing = (spacing << 20) / 16;
        s32 off = 0;

        if (align == 1) {
            off = (digits-1) * spacing / 2;
        } else if (align == 2) {
            off = (digits-1) * spacing;
        }

        if (off != 0) {
            vecMulAdd16(off, &ret->field40, &ret->field70);
        }

        do {
            s32 digit = val % 10 + digitFrame;
            if (digit >= 0 && digit < ret->func_020225d0()) {
                ret->func_0202dee8(digit, -1);
                ret->func_0202bb44(this->func_020224e4());
            }

            vecMulAdd16(-spacing, &ret->field40, &ret->field70);

            val /= 10;
        } while (val != 0);

        this->func_020224fc(ret);
    }
}

// MATCHED 100%
void clAfx::runColorBlendTrack(s32 t, u16 * cmd) {
    u32 rowStride;
    u32 key;
    s32 numRows;
    u32 segDur;
    s32 row;
    s32 rowHdr;
    s32 rowOff;

    s32 elapsed = t - ((s16)cmd[1] << 8);
    if (elapsed < 0) {
        return;
    }

    numRows = ((cmd[0] >> 6) & 7) + 1;
    s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;

    s32 nextKey = 1;
    segDur = 0;
    while (TRUE) {
        nextKey++;

        if (nextKey > numKeys) {
            if (segDur != 0) {
                return;
            }
            break;
        }

        segDur = cmd[nextKey];
        if (segDur == 0) {
            continue;
        }
        segDur <<= 8;

        if (elapsed <= (s32)segDur) {
            break;
        }
        elapsed -= segDur;
    }

    row = 1;

    if (numRows < 1) {
        return;
    }

    key = nextKey - 1;
    rowStride = numKeys + 1;
    rowOff = rowStride;
    u32 keyTime = elapsed;
    do {
        s32 val = evalRow((s16 *)cmd, row, key, keyTime, segDur);

        rowHdr = cmd[rowOff];

        s32 weight = ((val << 5) + 0x3200) / 25600;

        if (weight < 0) {
            weight = 0;
        }

        if (weight > 32) {
            weight = 32;
        }

        BOOL flag = (rowHdr & 0x10) ? TRUE : FALSE;
        this->func_020224d8(colorBlendFuncs[rowHdr & 3], this->getBlendColor(), weight, flag);

        rowOff += rowStride;
        row++;
    } while (row <= numRows);
}
