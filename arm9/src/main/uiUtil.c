// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/system.h"
#include "main/uiUtil.h"

// --- Function Definitions --- //

// MATCHED 100% -- Note: preexisting bug for negative powers of 10 having off-by-1 result
u32 countDigits(s32 value) {
    if (value < 0) {
        if (value >= -10) {
            return 2;
        }
        if (value >= -100) {
            return 3;
        }
        if (value >= -1000) {
            return 4;
        }
        if (value >= -10000) {
            return 5;
        }
        if (value >= -100000) {
            return 6;
        }
        if (value >= -1000000) {
            return 7;
        }
        if (value >= -10000000) {
            return 8;
        }
        if (value >= -100000000) {
            return 9;
        }
        if (value >= -1000000000)  {
            return 10;
        }
        return 11;
    } else {
        if (value < 10) {
            return 1;
        }
        if (value < 100) {
            return 2;
        }
        if (value < 1000) {
            return 3;
        }
        if (value < 10000) {
            return 4;
        }
        if (value < 100000) {
            return 5;
        }
        if (value < 1000000) {
            return 6;
        }
        if (value < 10000000) {
            return 7;
        }
        if (value < 100000000) {
            return 8;
        }
        if (value < 1000000000) {
            return 9;
        }
        return 10;
    }
}

// MATCHED 100%
void * getResChunk(YanaRes * res, u32 id, s32 index) {
    YanaResEntry * curEntry = res->entries;
    u32 kind = (index >= 0) ? 1 : 0;
    u32 i = 0;
    while (TRUE) {
        if (curEntry->id == id && curEntry->kind == kind) {
            break;
        }

        i++;
        curEntry++;
    }

    YanaResEntry * entry = RES_ENTRY(res, i);
    if (kind == 0) {
        index = 0;
    }

    return (u8 *)res + entry->offset + index;
}

// MATCHED 100%
s32 stepMenuCursor(s32 cursor, s32 count, u32 keyPrev, u32 keyNext) {
    if (inputState.repeat & keyPrev) {
        cursor--;
    }
    if (inputState.repeat & keyNext) {
        cursor++;
    }

    if (cursor < 0) {
        if (inputState.press & keyPrev) {
            return count-1;
        }
        return 0;
    } else if (count <= cursor) {
        if (inputState.press & keyNext) {
            return 0;
        }
        return count-1;
    }
    return cursor;
}
