// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clYanaTransMan.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "main/tile_convert.h"
#include "main/uiUtil.h"

// --- Extern Data Variables --- //

extern GXVRamTex data_027e00e0;  // 0x027e00e0
extern GXVRamTexPltt data_027e00e4;  // 0x027e00e4

// --- Extern Functions --- //

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

extern void func_0202d10c(u32, u32, YanaRes *, void *, void *, u16, u32, u32, u8); 

// --- Function Declarations --- //

void doCopy(YanaTransEntry *);  // 0x0201fbac
void doCopyStrided(YanaTransEntry *);  // 0x0201fbd4
void doTilesToLinear4bpp(YanaTransEntry *);  // 0x0201fc40
void doTilesToLinear8bpp(YanaTransEntry *);  // 0x0201fc98
void doLinearToTiles4bpp(YanaTransEntry *);  // 0x0201fcf0
void doResChunk(YanaTransEntry *);  // 0x0201fd48
void doResChunkTex(YanaTransEntry *);  // 0x0201fd9c
void doObjExtPltt(YanaTransEntry *);  // 0x0201fdf0

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
clYanaTransMan::~clYanaTransMan() {
    this->destroy();
}

// MATCHED 100%
void clYanaTransMan::init(u32 texMode, u16 capacity, YanaTransEntry * buf, s32 heapId) {
    this->locked = 1;
    this->head = 0;
    this->tail = 0;
    this->capacity = capacity;
    this->texMode = (texMode != 0) ? 1 : 0;
    this->ownsBuffer = (buf == NULL) ? 1 : 0;
    this->entries = (buf != NULL) ? buf : (YanaTransEntry *)heapAlloc(heapId, capacity * sizeof(YanaTransEntry), (void *)"Trans Buf", FALSE);
    this->locked = 0;
    this->flushBlocked = 0;
}

// MATCHED 100%
void clYanaTransMan::destroy() {
    this->locked = 1;
    if (this->ownsBuffer != 0 && this->entries != NULL) {
        heapFree(this->entries);
    }
    this->entries = NULL;
}

// MATCHED 100%
void clYanaTransMan::flush() {
    this->flushBlocked = this->locked;

    if (this->flushBlocked == 0) {
        u32 head = this->head;
        u32 tail = this->tail;
        if (head != tail) {
            if (this->texMode != 0) {
                data_027e00e0 = GX_ResetBankForTex();
                data_027e00e4 = GX_ResetBankForTexPltt();
            }
    
            while (head != tail) {
                YanaTransEntry * entry = &this->entries[head];
                entry->fn(entry);

                if (++head == this->capacity) {
                    head = 0;
                }
            }

            if (this->texMode != 0) {
                GX_SetBankForTex(data_027e00e0);
                GX_SetBankForTexPltt(data_027e00e4);
            }

            this->head = head;
        }
    }
}

// MATCHED 100%
void clYanaTransMan::post(void (*fn)(YanaTransEntry *)) {
    YanaTransEntry entry;
    entry.fn = fn;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::post1(void (*fn)(YanaTransEntry *), void * src) {
    YanaTransEntry entry;
    entry.fn = fn;
    entry.src = src;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::post2(void (*fn)(YanaTransEntry *), void * src, void * dst) {
    YanaTransEntry entry;
    entry.fn = fn;
    entry.src = src;
    entry.dst = dst;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::post3(void (*fn)(YanaTransEntry *), void * src, void * dst, u32 size) {
    YanaTransEntry entry;
    entry.fn = fn;
    entry.src = src;
    entry.dst = dst;
    entry.raw.size = size;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::post4(void (*fn)(YanaTransEntry *), void * src, void * dst, u32 size, u32 arg) {
    YanaTransEntry entry;
    entry.fn = fn;
    entry.src = src;
    entry.dst = dst;
    entry.raw.size = size;
    entry.raw.arg = arg;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postCopy(void * src, void * dst, u32 size) {
    YanaTransEntry entry;
    entry.fn = doCopy;
    entry.src = src;
    entry.dst = dst;
    entry.raw.size = size;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postCopyStrided(void * src, void * dst, u16 rowBytes, u16 rows, u16 srcStride, u16 dstStride) {
    YanaTransEntry entry;
    entry.fn = doCopyStrided;
    entry.src = src;
    entry.dst = dst;
    entry.strided.rowBytes = rowBytes;
    entry.strided.rows = rows;
    entry.strided.srcStride = srcStride;
    entry.strided.dstStride = dstStride;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postTilesToLinear4bpp(void * src, void * dst, u16 widthTiles, u16 rows, u16 srcStrideTiles, u16 dstStrideTiles) {
    YanaTransEntry entry;
    entry.fn = doTilesToLinear4bpp;
    entry.src = src;
    entry.dst = dst;
    entry.tiles.widthTiles = widthTiles;
    entry.tiles.rows = rows;
    entry.tiles.srcStrideTiles = srcStrideTiles;
    entry.tiles.dstStrideTiles = dstStrideTiles;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postTilesToLinear8bpp(void * src, void * dst, u16 widthTiles, u16 rows, u16 srcStrideTiles, u16 dstStrideTiles) {
    YanaTransEntry entry;
    entry.fn = doTilesToLinear8bpp;
    entry.src = src;
    entry.dst = dst;
    entry.tiles.widthTiles = widthTiles;
    entry.tiles.rows = rows;
    entry.tiles.srcStrideTiles = srcStrideTiles;
    entry.tiles.dstStrideTiles = dstStrideTiles;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postLinearToTiles4bpp(void * src, void * dst, u16 widthTiles, u16 rows, u16 srcStrideTiles, u16 dstStrideTiles) {
    YanaTransEntry entry;
    entry.fn = doLinearToTiles4bpp;
    entry.src = src;
    entry.dst = dst;
    entry.tiles.widthTiles = widthTiles;
    entry.tiles.rows = rows;
    entry.tiles.srcStrideTiles = srcStrideTiles;
    entry.tiles.dstStrideTiles = dstStrideTiles;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postResChunk(void * src, void * dst, u32 size, u16 arg) {
    YanaTransEntry entry;
    entry.fn = (this->texMode != 0) ? doResChunkTex : doResChunk;
    entry.src = src;
    entry.dst = dst;
    entry.raw.size = size;
    entry.raw.arg = arg;

    this->submit(&entry);
}

// MATCHED 100%
void clYanaTransMan::postObjExtPltt(void * src, void * dst, u32 size, u32 sub) {
    YanaTransEntry entry;
    entry.fn = doObjExtPltt;
    entry.src = src;
    entry.dst = dst;
    entry.pltt.size = size;
    entry.pltt.sub = sub;

    this->submit(&entry);
}

// MATCHED 100%
YanaTransEntry * clYanaTransMan::beginEntry() {
    this->locked = 1;
    YanaTransEntry * entry = &this->entries[this->tail];
    this->tail = (this->tail+1 == this->capacity) ? 0 : this->tail+1;
    return entry;
}

// MATCHED 100%
void clYanaTransMan::endEntry() {
    this->locked = 0;
}

// MATCHED 100%
void clYanaTransMan::submit(YanaTransEntry * srcEntry) {
    YanaTransEntry * entry = this->beginEntry();

    entry->fn = srcEntry->fn;
    entry->src = srcEntry->src;
    entry->dst = srcEntry->dst;
    entry->raw.size = srcEntry->raw.size;
    entry->raw.arg = srcEntry->raw.arg;

    this->endEntry();
}

// MATCHED 100%
void doCopy(YanaTransEntry * entry) {
    u32 size = entry->raw.size;
    if (entry->src < entry->dst) {
        func_01ff8380((u16 *)entry->src, (u16 *)entry->dst, size);
    } else {
        func_01ff82ac((u16 *)entry->src, (u16 *)entry->dst, size);
    }
}

// MATCHED 100%
void doCopyStrided(YanaTransEntry * entry) {
    u16 * src = (u16 *)entry->src;
    u16 * dst = (u16 *)entry->dst;
    u32 rowBytes = entry->strided.rowBytes;
    s32 rows = entry->strided.rows;
    u16 srcStride = entry->strided.srcStride;
    u16 dstStride = entry->strided.dstStride;

    for (s32 i = 0; i < rows; i++) {
        if (src < dst) {
            func_01ff8380(src, dst, rowBytes);
        } else {
            func_01ff82ac(src, dst, rowBytes);
        }

        dst = (u16 *)((u8 *)dst + dstStride);
        src = (u16 *)((u8 *)src + srcStride);
    }
}

// MATCHED 100%
void doTilesToLinear4bpp(YanaTransEntry * param1) {
    u32 * src = (u32 *)param1->src;
    u32 * dst = (u32 *)param1->dst;
    u32 widthTiles = param1->tiles.widthTiles;
    s32 rows = param1->tiles.rows;
    u32 srcStrideTiles = param1->tiles.srcStrideTiles;
    u32 dstStrideTiles = param1->tiles.dstStrideTiles;

    while (rows-- > 0) {
        TilesToLinear4bpp(src, dst, widthTiles * 4, 8);
        dst = (u32 *)((u8 *)dst + dstStrideTiles * 32);
        src = (u32 *)((u8 *)src + srcStrideTiles * 32);
    }
}

// MATCHED 100%
void doTilesToLinear8bpp(YanaTransEntry * param1) {
    u32 * src = (u32 *)param1->src;
    u32 * dst = (u32 *)param1->dst;
    u32 widthTiles = param1->tiles.widthTiles;
    s32 rows = param1->tiles.rows;
    u32 srcStrideTiles = param1->tiles.srcStrideTiles;
    u32 dstStrideTiles = param1->tiles.dstStrideTiles;

    while (rows-- > 0) {
        TilesToLinear8bpp(src, dst, widthTiles * 8, 8);
        dst = (u32 *)((u8 *)dst + dstStrideTiles * 64);
        src = (u32 *)((u8 *)src + srcStrideTiles * 64);
    }
}

// MATCHED 100%
void doLinearToTiles4bpp(YanaTransEntry * param1) {
    u32 * src = (u32 *)param1->src;
    u32 * dst = (u32 *)param1->dst;
    u32 widthTiles = param1->tiles.widthTiles;
    s32 rows = param1->tiles.rows;
    u32 srcStrideTiles = param1->tiles.srcStrideTiles;
    u32 dstStrideTiles = param1->tiles.dstStrideTiles;

    while (rows-- > 0) {
        LinearToTiles4bpp(src, dst, widthTiles * 4, 8);
        dst = (u32 *)((u8 *)dst + dstStrideTiles * 32);
        src = (u32 *)((u8 *)src + srcStrideTiles * 32);
    }
}

// MATCHED 100%
void doResChunk(YanaTransEntry * entry) {
    YanaRes * res = (YanaRes *)entry->src;
    void * dst = entry->dst;
    u32 size = entry->raw.size;
    u16 arg = entry->raw.arg;

    func_0202d10c(size, 0, res, dst, getResChunk(res, arg, -1), arg, 0, 0, 0);
}

// MATCHED 100%
void doResChunkTex(YanaTransEntry * entry) {
    YanaRes * res = (YanaRes *)entry->src;
    void * dst = entry->dst;
    u32 size = entry->raw.size;

    func_0202d10c(size, 1, res, dst, getResChunk(res, 3, -1), 3, 0, 0, 0);
}

// MATCHED 100%
void doObjExtPltt(YanaTransEntry * entry) {
    if (entry->pltt.sub == 0) {
        GXVRamOBJExtPltt pltt = GX_ResetBankForOBJExtPltt();

        u16 * src = (u16 *)entry->src;
        u16 * dst = (u16 *)entry->dst;
        u32 size = entry->raw.size;
        if (src < dst) {
            func_01ff8380(src, dst, size);
        } else {
            func_01ff82ac(src, dst, size);
        }

        GX_SetBankForOBJExtPltt(pltt);
    } else {
        GXVRamSubOBJExtPltt pltt = GX_ResetBankForSubOBJExtPltt();

        u16 * src = (u16 *)entry->src;
        u16 * dst = (u16 *)entry->dst;
        u32 size = entry->raw.size;
        if (src < dst) {
            func_01ff8380(src, dst, size);
        } else {
            func_01ff82ac(src, dst, size);
        }

        GX_SetBankForSubOBJExtPltt(pltt);
    }
}
