// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clDfxSys.h"

extern "C" {

#include <nitro.h>

#include "main/vramBank.h"

// --- Extern Data Variables --- //

extern DfxTexInfo texInfo;

// --- Variable Definitions --- //

u8 plttBlockCount[8] = {0, 2, 1, 1, 16, 0, 1, 0};  // 0x02055118

// --- Function Declarations --- //

s32 dfxLoadTexture(clDfxVramMan *, DfxTexHeader *, RfxNode *);  // 0x02025654
s32 dfxLoadPalette(clDfxVramMan *, DfxPlttHeader *, RfxNode *);  // 0x020257ac

void dfxVrStart(clDfxSys *, clDfxVr *, u32, inYanaUnit **, clDfxVramMan *);  // 0x020259b8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
s32 dfxLoadTexture(clDfxVramMan * vramMan, DfxTexHeader * hdr, RfxNode * node) {
    u8 * vram = vramMan->getTexBase();

    if (vramMan->transMan == NULL || vram == NULL) {
        return 0;
    }

    s32 blockX = node->dfx.field0c / 8;
    s32 blockY = node->dfx.field0e / 8;
    s32 blockIdx = blockY * hdr->wBlocks + blockX;

    u8 * tiles = (u8 *)hdr + 8 + blockIdx * hdr->bpp * 8;
    u32 key = ((u32)tiles >> 5) | (texInfo.sizeS << 26) | (texInfo.sizeT << 29);

    u32 w = (1 << texInfo.sizeS);
    u32 h = (1 << texInfo.sizeT);

    YanaPileEntry * entry = vramMan->texPile.acquire(key, hdr->bpp * (w * h) * 8, 0);

    if (entry->needsLoad) {
        vram = vram + vramMan->texOffset + entry->offset;

        if (hdr->bpp == 4) {
            vramMan->transMan->postTilesToLinear4bpp(tiles, vram, w, h, hdr->wBlocks, w);
        }

        if (hdr->bpp == 8) {
            vramMan->transMan->postTilesToLinear8bpp(tiles, vram, w, h, hdr->wBlocks, w);
        }
    }

    return vramMan->texOffset + entry->offset;
}

// MATCHED 100%
s32 dfxLoadPalette(clDfxVramMan * vramMan, DfxPlttHeader * pltt, RfxNode * node) {
    u8 * vram = vramMan->getPlttBase();

    if (vramMan->transMan == NULL || vram == NULL) {
        return 0;
    }

    u8 blocks = plttBlockCount[texInfo.fmt];
    u8 * src = pltt->data + node->dfx.field14 * 32;
    u32 key = ((u32)src >> 5) | (blocks << 27);

    YanaPileEntry * entry = vramMan->plttPile.acquire(key, (blocks << 5), 0);

    if (entry->needsLoad) {
        u32 offset = entry->offset;
        u8 * dst = vramMan->getPlttBase() + vramMan->plttOffset + offset;
        vramMan->transMan->postCopy(src, dst, (blocks << 5));
    }

    return vramMan->plttOffset + entry->offset;
}

// MATCHED 100%
u8 * clDfxVramMan::getTexBase() {
    return getTexVramAddr(GX_GetBankForTex());
}

// MATCHED 100%
u8 * clDfxVramMan::getPlttBase() {
    return getTexPlttVramAddr(GX_GetBankForTexPltt());
}

// MATCHED 100%
DfxTexHeader * clDfxVr::getTexHeader(u32 id) {
    if (this->sys == NULL) {
        return NULL;
    }

    return (DfxTexHeader *)func_020222a4(&this->sys->pools, id);
}

// MATCHED 100%
DfxPlttHeader * clDfxVr::getPlttHeader(u32 id) {
    if (this->sys == NULL) {
        return NULL;
    }

    return (DfxPlttHeader *)func_020223b0(&this->sys->pools, id);
}

// MATCHED 100%
s32 clDfxVr::getTexAddr(DfxTexHeader * hdr, RfxNode * node) {
    if (this->vramMan == NULL) {
        return -1;
    }

    return dfxLoadTexture(this->vramMan, hdr, node);
}

// MATCHED 100%
s32 clDfxVr::getPlttAddr(DfxPlttHeader * pltt, RfxNode * node) {
    if (this->vramMan == NULL) {
        return -1;
    }

    return dfxLoadPalette(this->vramMan, pltt, node);
}

// MATCHED 100%
u32 clDfxVr::extScriptCount() {
    if (this->sys != NULL) {
        if (this->sys->dataTable != NULL) {
            return fsChunkCount(this->sys->dataTable);
        }

        return 0;
    }

    return 0;
}

// MATCHED 100%
u16 * clDfxVr::extScript(s32 idx) {
    if (this->sys != NULL) {
        if (this->sys->dataTable == NULL) {
            return NULL;
        }

        return (u16 *)fsChunkPtr(this->sys->dataTable, idx);
    }

    return NULL;
}

// MATCHED 100%
void clDfxSh::onNodePos(RfxNode * node) {
    this->shiftX += node->pos.x;
    this->shiftY += node->pos.y;
}

// MATCHED 100%
void clDfxSh::exec() {
    this->shiftX = 0;
    this->shiftY = 0;

    this->clRfx::exec();
}

// MATCHED 100%
void dfxVrStart(clDfxSys * sys, clDfxVr * vr, u32 scriptId, inYanaUnit ** list, clDfxVramMan * vramMan) {
    u16 * script = (sys->scriptTable != NULL) ? (u16 *)fsChunkPtr(sys->scriptTable, scriptId) : NULL;

    clDfx & dfx = *vr;

    MTX_Identity33(&dfx.rot);

    dfx.originX = 128;
    dfx.originY = 128;
    dfx.originZ = 4096;

    dfx.start(list, script);

    vr->vramMan = vramMan;
    vr->sys = sys;
}

// MATCHED 100%
void clDfxSys::func_02025a30() {}

// MATCHED 100%
clDfxEx::~clDfxEx() {}

// MATCHED 100%
clDfxSys::~clDfxSys() {}
