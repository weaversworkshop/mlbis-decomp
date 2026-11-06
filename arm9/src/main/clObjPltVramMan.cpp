// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clOamMan.h"
#include "main/clObjPltVramMan.h"

extern "C" {

#include <nitro.h>

#include "main/heap.h"
#include "main/system.h"

// --- Extern Data Variables --- //

extern GXOamAttr data_01ffa168[];  // 0x01ffa168

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);
extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

// --- Variable Definitions --- //

nsObjSys::clOamMan * oamManTable[2];  // 0x0205c260

// --- Function Declarations --- //

void sortRunsUnsigned(OamRun *, OamRun *);  // 0x0202a7cc
void sortRunsSigned(OamRun *, OamRun *);  // 0x0202a858
void scatterAffine(OamAffine *, OamAffine *, void *);  // 0x0202a8e4
s32 findOrAddAffine(OamAffine *, OamAffine *, s16 pa, s16 pb, s16 pc, s16 pd);  // 0x0202ab10

} // extern "C"

// --- Function Definitions --- //

namespace nsObjSys {

// MATCHED 100%
clObjPltVramMan::clObjPltVramMan(u8 bank, u32 size, u32 base, u32 index) : clVramManBase(bank, 1, index) {
    this->init(size, base);
}

// MATCHED 100%
void clObjPltVramMan::init(u32 size, u32 base) {
    this->head[1] = NULL;
    this->head[0] = NULL;

    MI_CpuFill16(&this->extMap, 0, 0x22);

    u32 extSize = (this->manBank == 0) ? GX_GetSizeOfOBJExtPltt() : GX_GetSizeOfSubOBJExtPltt();
    if (extSize == 0 && base == 0) {
        if (size == 0 || size == 0x110) {
            base = 0x100;
            size = 0x10;
        }
    }

    if (size == 0) {
        size = 0x110 - base;
    }

    this->base = base;
    this->size = size;

    u16 * word = this->extMap;
    u32 pos = 0;
    s32 n;
    u32 bit = 0;
    
    while (pos < base) {
        s32 lim = 0x10 - bit;
        n = base - pos;

        if (n > lim) {
            n = lim;
        }

        *word = ((1 << n) - 1) << bit;
        bit += n;

        if (bit == 0x10) {
            bit = 0;
            word++;
        }

        pos += n;
    }

    base += size;

    bit = (base & 0xf);
    u16 * tailWord = &this->extMap[base / 16];

    while (base < 0x110) {
        s32 lim = 0x10 - bit;
        s32 tailN = 0x110 - base;

        if (tailN > lim) {
            tailN = lim;
        }

        *tailWord = ((1 << tailN) - 1) << bit;
        bit += tailN;

        if (bit == 0x10) {
            bit = 0;
            tailWord++;
        }

        base += tailN;
    }
}


inline VramHandle * clObjPltVramMan::insert(VramHandle * next, VramHandle * prev, u32 offset, u32 size, u32 id, u16 attr) {
    VramHandle * handle = handleMan->alloc();
    handle->next = next;
    handle->prev = prev;
    handle->offset = offset;
    handle->size = size;
    handle->id = id;
    handle->refCount = 1;
    handle->attr = attr;

    if (prev == NULL) {
        this->head[0] = handle;
    } else {
        prev->next = handle;
    }

    if (next == NULL) {
        this->head[1] = handle;
    } else {
        next->prev = handle;
    }

    return handle;
}

// MATCHED 100%
VramHandle * clObjPltVramMan::linkFromBack(u32 offset, u32 count, u32 id, u16 attr) {
    VramHandle * cur = this->head[1];
    VramHandle * last = NULL;
    u32 end = offset + count;

    if (cur == NULL) {
        return this->insert(NULL, NULL, offset, count, id, attr);
    }

    u32 limit = this->base + this->size;

    while (TRUE) {
        u32 curOffset = cur->offset;
        u32 curEnd = curOffset + cur->size;

        if (curEnd <= offset && end <= limit) {
            return this->insert(last, cur, offset, count, id, attr);
        }

        if (cur->prev == NULL) {
            return this->insert(cur, NULL, offset, count, id, attr);
        }

        last = cur;
        limit = curOffset;
        cur = cur->prev;
    }
}

// MATCHED 100%
VramHandle * clObjPltVramMan::linkFromFront(u32 offset, u32 count, u32 id, u16 attr) {
    VramHandle * cur = this->head[0];
    VramHandle * last = NULL;
    u32 end = offset + count;

    if (cur == NULL) {
        return this->insert(NULL, NULL, offset, count, id, attr);
    }

    u32 limit = this->base;

    while (TRUE) {
        u32 curOffset = cur->offset;

        if (limit <= offset && end <= cur->offset) {
            return this->insert(cur, last, offset, count, id, attr);
        }

        if (cur->next == NULL) {
            return this->insert(NULL, cur, offset, count, id, attr);
        }

        limit = curOffset + cur->size;
        last = cur;
        cur = cur->next;
    }
}


static inline BOOL markRange(u16 * map, u32 start, u32 end, BOOL testOnly) {
    u32 bit;
    s32 n;
    u32 i;
    i = start;
    bit = start & 0xf;
    map += start / 16;

    while (i < end) {
        s32 lim = 0x10 - bit;
        n = end - i;
        if (n > lim) {
            n = lim;
        }

        u32 mask = ((1 << n) - 1) << bit;
        if (testOnly) {
            if (*map & mask) {
                return TRUE;
            }
        } else {
            *map |= mask;
        }

        i += n;
        bit += n;

        if (bit == 0x10) {
            bit = 0;
            map++;
        }
    }

    return FALSE;
}

// MATCHED 100%
VramHandle * clObjPltVramMan::allocStd(u32 count, u32 id, u32 attr) {
    u32 manBits = (this->manBank << 2) | 8 | (this->manIndex << 6);
    u32 fromBack = (attr & 1);
    u32 shared = attr & 4;
    attr = (attr & ~4) | manBits;

    if (shared) {
        VramHandle * found = this->findShared(id, attr, 1, 0, 1);
        if (found != NULL) {
            return found;
        }
    }

    u32 mask = (1 << count) - 1;
    u32 map = this->stdMap;
    s32 slot;
    s32 lim;
    BOOL flag = FALSE;

    if (fromBack == 0) {
        lim = 0x10 - count;
        for (slot = 0; slot <= lim; slot++) {
            if ((map & mask) == 0) {
                flag = TRUE;
                break;
            }
            mask <<= 1;
        }

        if (flag == FALSE) {
            return NULL;
        }
    } else {
        slot = 0x10 - count;
        mask <<= slot;
        while (slot >= 0) {
            if ((map & mask) == 0) {
                flag = TRUE;
                break;
            }
            slot--;
            mask >>= 1;
        }
        
        if (flag == FALSE) {
            return NULL;
        }
    }

    u32 start = slot | 0x100;
    markRange(this->extMap, start, (slot + count) | 0x100, FALSE);

    return this->linkFromBack(start, count, id, attr);
}

// MATCHED 100%
VramHandle * clObjPltVramMan::allocExt(u32 count, u32 id, u32 attr) {
    u32 manBits = (this->manBank << 2) | 8 | (this->manIndex << 6);
    u32 fromBack = (attr & 1);
    u32 shared = attr & 4;
    attr = (attr & ~4) | manBits;

    if (shared) {
        VramHandle * found = this->findShared(id, attr, 1, 0, 1);
        if (found != NULL) {
            return found;
        }
    }

    u16 blockMap = 0;
    BOOL flag = FALSE;
    u32 blocks = ((count + 0xf) & ~0xf) >> 4;
    s32 slot;
    s32 lim;

    u16 * word = this->extMap;
    for (s32 i = 0; i <= 0xf; i++) {
        if (*word != 0) {
            blockMap |= 1 << i;
        }
        word++;
    }

    u32 mask = (1 << blocks) - 1;
    if (fromBack == 0) {
        lim = 0x10 - blocks;
        for (slot = 0; slot <= lim; slot++) {
            if ((blockMap & mask) == 0) {
                flag = TRUE;
                break;
            }
            mask <<= 1;
        }
    } else {
        slot = 0x10 - blocks;
        mask <<= slot;
        while (slot >= 0) {
            if ((blockMap & mask) == 0) {
                flag = TRUE;
                break;
            }
            slot--;
            mask >>= 1;
        }
    }

    if (flag == FALSE) {
        return NULL;
    }

    u32 start = slot * 16;
    markRange(this->extMap, start, count + start, FALSE);

    return this->linkFromFront(start, count, id, attr);
}

// MATCHED 100%
VramHandle * clObjPltVramMan::allocStdAt(u32 offset, u32 count, u32 id, u32 attr) {
    u32 manBits = (this->manBank << 2) | 8 | (this->manIndex << 6);
    u32 shared = attr & 4;
    attr = (attr & ~4) | manBits;

    if (shared) {
        VramHandle * found = this->findShared(id, attr, 1, offset - this->base, 1);
        if (found != NULL && found->offset == offset) {
            return found;
        }
    }

    attr |= (this->manBank << 2) | 8;

    u32 mask = ((1 << count) - 1) << (offset & 0xf);
    if ((mask & this->stdMap) == 0) {
        return NULL;
    }

    markRange(this->extMap, offset | 0x100, (offset + count) | 0x100, FALSE);

    return this->linkFromBack(offset, count, id, attr);
}

// MATCHED 100%
VramHandle * clObjPltVramMan::allocExtAt(u32 offset, u32 count, u32 id, u32 attr) {
    u32 manBits = (this->manBank << 2) | 8 | (this->manIndex << 6);
    u32 shared = attr & 4;
    attr = (attr & ~4) | manBits;

    if (shared) {
        VramHandle * found = this->findShared(id, attr, 1, offset - this->base, 1);
        if (found != NULL && found->offset == offset) {
            return found;
        }
    }

    attr |= (this->manBank << 2) | 8;

    if (markRange(this->extMap, offset, offset + count, TRUE)) {
        return NULL;
    }

    markRange(this->extMap, offset, offset + count, FALSE);

    return this->linkFromFront(offset, count, id, attr);
}

// MATCHED 100%
BOOL clObjPltVramMan::release(VramHandle * handle, s32 limit, u32 refDelta) {
    handle->refCount += refDelta;

    if (handle->refCount > limit) {
        return FALSE;
    }

    u16 * word;
    u32 pos;
    u32 end;
    s32 n;
    u32 bit;
    pos = handle->offset;
    end = pos + handle->size;
    bit = pos & 0xf;
    word = &this->extMap[pos / 16];

    while (pos < end) {
        s32 lim = 0x10 - bit;
        n = end - pos;
        if (n > lim) {
            n = lim;
        }

        *word &= ~(((1 << n) - 1) << bit);
        pos += n;
        bit += n;

        if (bit == 0x10) {
            bit = 0;
            word++;
        }
    }

    if (this->head[0] == handle) {
        this->head[0] = handle->next;
    }
    if (this->head[1] == handle) {
        this->head[1] = handle->prev;
    }

    if (handle->prev != NULL) {
        handle->prev->next = handle->next;
    }
    if (handle->next != NULL) {
        handle->next->prev = handle->prev;
    }

    handleMan->free(handle);

    return TRUE;
}

// MATCHED 100%
u32 clObjPltVramMan::compactStd(u32 dir) {
    u32 shift = 0;
    u32 curSize;
    u32 curOffset;
    VramHandle * cur;
    u32 free;

    if ((dir & 1) == 0) {
        cur = this->head[0];
        while (cur != NULL && cur->offset < 0x100) {
            cur = cur->next;
        }

        free = this->base;
        if (free <= 0x100) {
            free = 0x100;
        }

        while (cur != NULL && (cur->attr & 1) == dir) {
            curOffset = cur->offset;
            curSize = cur->size;
            shift += curOffset - free;
            cur->offset -= shift;

            if (shift != 0) {
                cur->moved = 1;
            }

            cur = cur->next;
            free = curOffset + curSize;
        }
    } else {
        cur = this->head[1];
        free = this->base + this->size;
        while (cur != NULL && cur->offset >= 0x100 && (cur->attr & 1) == dir) {
            curOffset = cur->offset;
            shift += free - (curOffset + cur->size);
            cur->offset += shift;

            if (shift != 0) {
                cur->moved = 1;
            }

            cur = cur->prev;
            free = curOffset;
        }
    }

    u16 map = 0;
    VramHandle * node = this->head[1];
    while (node != NULL && node->offset >= 0x100) {
        map |= ((1 << node->size) - 1) << (node->offset & 0xf);
        node = node->prev;
    }
    this->stdMap = map;

    return shift;
}

// MATCHED 100%
u32 clObjPltVramMan::compactExt(u32 dir) {
    u32 free;
    VramHandle * cur;
    u32 shift = 0;

    if ((dir & 1) == 0) {
        free = this->base;
        cur = this->head[0];
        while (cur != NULL && cur->offset < 0x100 && (cur->attr & 1) == dir) {
            u32 curOffset = cur->offset;
            u32 gap = curOffset - free;
            free = curOffset + cur->size;

            if (gap >= 0x10) {
                gap &= ~0xf;
            } else {
                gap = 0;
            }

            shift += gap;
            cur->offset -= shift;

            if (shift != 0) {
                cur->moved = 1;
            }

            cur = cur->next;
        }
    } else {
        cur = this->head[1];
        while (cur != NULL && cur->offset >= 0x100) {
            cur = cur->prev;
        }

        free = this->base + this->size;
        if (free > 0x100) {
            free = 0x100;
        }

        while (cur != NULL && (cur->attr & 1) == dir) {
            u32 curOffset = cur->offset;
            u32 gap = free - (curOffset + cur->size);
            free = curOffset;

            if (gap >= 0x10) {
                gap &= ~0xf;
            } else {
                gap = 0;
            }

            shift += gap;
            cur->offset += shift;

            if (shift != 0) {
                cur->moved = 1;
            }

            cur = cur->prev;
        }
    }

    u16 * map = this->extMap;
    MI_CpuFill16(map, 0, 0x20);

    cur = this->head[0];
    while (cur != NULL && cur->offset < 0x100) {
        u16 * word;
        u32 pos;
        u32 end;
        s32 n;
        u32 bit;
        pos = cur->offset;
        end = pos + cur->size;
        bit = pos & 0xf;
        word = &map[pos / 16];

        while (pos < end) {
            s32 lim = 0x10 - bit;
            n = end - pos;
            if (n > lim) {
                n = lim;
            }

            *word = ((1 << n) - 1) << bit;
            pos += n;
            bit += n;

            if (bit == 0x10) {
                bit = 0;
                word++;
            }
        }

        cur = cur->next;
    }

    return shift;
}

// MATCHED 100%
VramHandle * clObjPltVramMan::findShared(u32 id, u16 attr, u32 mask, u32 offset, s16 refDelta) {
    u32 want = attr & mask;
    u32 fromBack = want & 1;
    VramHandle * cur = this->head[fromBack];
    u32 limit = offset + this->base;

    if (fromBack == 0) {
        if (offset != 0) {
            while (cur != NULL) {
                if (cur->offset >= limit) {
                    break;
                }
                cur = cur->next;
            }
        }

        while (cur != NULL) {
            if (cur->id == id && (cur->attr & mask) == want) {
                cur->refCount += refDelta;
                return cur;
            }
            cur = cur->next;
        }
    } else {
        if (offset != 0) {
            while (cur != NULL) {
                if (cur->offset + cur->size <= limit) {
                    break;
                }
                cur = cur->next;
            }
        }

        while (cur != NULL) {
            if (cur->id == id && (cur->attr & mask) == want) {
                cur->refCount += refDelta;
                return cur;
            }
            cur = cur->prev;
        }
    }

    return NULL;
}

// MATCHED 100%
clOamMan::clOamMan(u8 screen) {
    OamPool * pool = (OamPool *)::operator new[](sizeof(OamPool), 3, NULL, FALSE);
    this->init(screen, systemState.oamBuffers[screen], pool->affine, pool->runs);

    this->ownsPool = 1;
}

// MATCHED 100%
void clOamMan::init(u8 screen, GXOamAttr * oam, OamAffine * affine, OamRun * runs) {
    this->screen = screen;
    this->oam = oam;
    this->affine = affine;
    this->runs = runs;

    this->reset(0, 0);

    oamManTable[screen] = this;

}

// MATCHED 100%
clOamMan::~clOamMan() {
    if (this->ownsPool != 0 && this->affine != NULL) {
        delete[] this->affine;
        this->affine = NULL;
    }

    oamManTable[this->screen] = NULL;
}

// MATCHED 100%
void clOamMan::reset(u32 oamStart, u32 affineStart) {
    GXOamAttr * oamPos = &this->oam[oamStart];
    this->oamEnd = oamPos;
    this->oamFixedEnd = oamPos;

    this->runEnd = this->runs;

    OamAffine * affinePos = &this->affine[affineStart];
    this->affineEnd = affinePos;
    this->affineFixedEnd = affinePos;
}

// MATCHED 100%
OamRun * clOamMan::addRun(s32 start, s32 count, s32 key) {
    OamRun * run = this->runEnd;
    run->start = start;
    run->count = count;
    run->key = key;

    return (++this->runEnd);
}

// MATCHED 100%
void clOamMan::sortRuns(u32 isSigned) {
    if (isSigned) {
        sortRunsSigned(this->runs, this->runEnd);
    } else {
        sortRunsUnsigned(this->runs, this->runEnd);
    }
}

}

// MATCHED 100%
void sortRunsUnsigned(OamRun * begin, OamRun * end) {
    if (end - begin > 1) {
        end--;

        while (end >= begin) {
            OamRun * lastSwap = NULL;

            for (OamRun * cur = begin+1; cur <= end; cur++) {
                OamRun * prev = cur-1;
                u32 prevKey = prev->key;
                u32 curKey = cur->key;

                if (prevKey > curKey) {
                    u32 tmp = cur->posw;
                    cur->key = prevKey;
                    cur->posw = prev->posw;
                    prev->key = curKey;
                    prev->posw = tmp;
                    lastSwap = prev;
                }
            }

            end = lastSwap;
        }
    }
}

// MATCHED 100%
void sortRunsSigned(OamRun * begin, OamRun * end) {
    if (end - begin > 1) {
        end--;

        while (end >= begin) {
            OamRun * lastSwap = NULL;

            for (OamRun * cur = begin+1; cur <= end; cur++) {
                OamRun * prev = cur-1;
                s32 prevKey = prev->key;
                s32 curKey = cur->key;

                if (prevKey > curKey) {
                    u32 tmp = cur->posw;
                    cur->key = prevKey;
                    cur->posw = prev->posw;
                    prev->key = curKey;
                    prev->posw = tmp;
                    lastSwap = prev;
                }
            }

            end = lastSwap;
        }
    }
}

#ifndef IDE_PARSE
// MATCHED 100%
asm void scatterAffine(register OamAffine * affine, register OamAffine * affineEnd, register void * oam) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9}
    add r2, r2, #0x6
@loop
    ldmia r0!, {r3, r4, r5, r6, r7, r8, r9, ip}
    strh r3, [r2], #0x8
    mov r3, r3, lsr #0x10
    strh r3, [r2], #0x8
    strh r4, [r2], #0x8
    mov r4, r4, lsr #0x10
    strh r4, [r2], #0x8
    strh r5, [r2], #0x8
    mov r5, r5, lsr #0x10
    strh r5, [r2], #0x8
    strh r6, [r2], #0x8
    mov r6, r6, lsr #0x10
    strh r6, [r2], #0x8
    strh r7, [r2], #0x8
    mov r7, r7, lsr #0x10
    strh r7, [r2], #0x8
    strh r8, [r2], #0x8
    mov r8, r8, lsr #0x10
    strh r8, [r2], #0x8
    strh r9, [r2], #0x8
    mov r9, r9, lsr #0x10
    strh r9, [r2], #0x8
    strh ip, [r2], #0x8
    mov ip, ip, lsr #0x10
    strh ip, [r2], #0x8
    cmp r0, r1
    blt @loop
    ldmia sp!, {r4, r5, r6, r7, r8, r9}
    bx lr
}
#endif

namespace nsObjSys {

// MATCHED 100%
void clOamMan::flush(GXOamAttr * dst, u32 clear) {
    if (this->runEnd != this->runs) {
        GXOamAttr * out = data_01ffa168;
        GXOamAttr * src = this->oam;

        if (this->oamFixedEnd != src) {
            func_01ff82ac(src, out, (u8*)this->oamFixedEnd - (u8*)src);
            out += (this->oamFixedEnd - src);
        }

        OamRun * run = this->runs;
        OamRun * runEnd = this->runEnd;
        while (run != runEnd) {
            func_01ff82ac(src + run->start, out, run->count * sizeof(GXOamAttr));

            out += run->count;
            run++;
        }

        s32 filled = out - data_01ffa168;
        if (filled < 0x80) {
            func_01ff82ac(src + filled, out, (0x80 - filled) * sizeof(GXOamAttr));
        }

        scatterAffine(this->affine, this->affineEnd, data_01ffa168);

        if (dst == NULL) {
            dst = this->oam;
        }

        if (data_01ffa168 < dst) {
            func_01ff8380(data_01ffa168, dst, 0x400);
        } else {
            func_01ff82ac(data_01ffa168, dst, 0x400);
        }
    } else if (dst != NULL && dst != this->oam) {
        if (this->oam < dst) {
            func_01ff8380(this->oam, dst, 0x400);
        } else {
            func_01ff82ac(this->oam, dst, 0x400);
        }
    }

    if (clear != 0) {
        func_01ff8228(this->oam, 0x200, 0x400);
    }
}

// MATCHED 100%
s32 clOamMan::getAffineIndex(s32 pa, s32 pb, s32 pc, s32 pd) {
    s32 index = findOrAddAffine(this->affine, this->affineEnd, pa, pb, pc, pd);
    
    if (this->affine + index == this->affineEnd) {
        this->affineEnd++;
    }

    return index;
}

}

// MATCHED 100%
s32 findOrAddAffine(OamAffine * begin, OamAffine * end, s16 pa, s16 pb, s16 pc, s16 pd) {
    u32 w0 = (pb << 0x10) | (u16)pa;
    u32 w1 = (pd << 0x10) | (u16)pc;

    OamAffine * cur = end-1;
    while (cur >= begin) {
        if (cur->w0 == w0 && cur->w1 == w1) {
            return cur - begin;
        }

        cur--;
    }

    end->w0 = w0;
    end->w1 = w1;

    return end - begin;
}

namespace nsObjSys {

// MATCHED 100%
u32 clObjPltVramMan::compact(u32 dir) {
    return this->compactStd(dir) + this->compactExt(dir);
}

// MATCHED 100%
u32 clObjPltVramMan::getVramAddr(VramHandle * handle) {
    u32 addr = handle->offset * 32;
    u32 bank = (handle->manBank == 0) ? GX_GetBankForOBJExtPltt() : GX_GetBankForSubOBJExtPltt();

    switch (bank) {
        case GX_VRAM_F: {
            addr += HW_LCDC_VRAM_F;
            break;
        }
        case GX_VRAM_G: {
            addr += HW_LCDC_VRAM_G;
            break;
        }
        case GX_VRAM_I: {
            addr += HW_LCDC_VRAM_I;
            break;
        }
    }

    return addr;
}

}
