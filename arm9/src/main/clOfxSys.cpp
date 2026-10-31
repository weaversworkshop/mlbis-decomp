// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clOfxSys.h"

extern "C" {

#include <nitro.h>

#include "main/vramBank.h"

// --- Function Declarations --- //

s32 ofxLoadTexture(clOfxVramMan *, RfxTexHeader *, RfxNode *);  // 0x02026b04
s32 ofxLoadPalette(clOfxVramMan *, RfxPlttHeader *, RfxNode *, BOOL);  // 0x02026ca4
void ofxVrStart(clOfxSys *, clOfxVr *, u32, inYanaUnit **, clOfxVramMan *);  // 0x02026f44

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
s32 ofxLoadTexture(clOfxVramMan * vramMan, RfxTexHeader * hdr, RfxNode * node) {
    u8 * vram = vramMan->getTexBase(vramMan->engine);

    if (vramMan->transMan == NULL || vram == NULL) {
        return 0;
    }

    s32 blockIdx = node->ofx.sprite.tileY * hdr->wBlocks + node->ofx.sprite.tileX;

    u8 * tiles = (u8 *)hdr + 8 + hdr->bpp * blockIdx * 8;
    u32 key = ((u32)tiles >> 5) | (node->ofx.sprite.shape << 26);

    u32 mask;
    GXOBJVRamModeChar mode = (vramMan->engine == 0) ? GX_GetOBJVRamModeChar() : GXS_GetOBJVRamModeChar();
    switch (mode) {
        case GX_OBJVRAMMODE_CHAR_1D_32K: {
            mask = 0x1f;
            break;
        }
        case GX_OBJVRAMMODE_CHAR_1D_64K: {
            mask = 0x3f;
            break;
        }
        case GX_OBJVRAMMODE_CHAR_1D_128K: {
            mask = 0x7f;
            break;
        }
        case GX_OBJVRAMMODE_CHAR_1D_256K: {
            mask = 0xff;
            break;
        }
    }

    s32 wTiles = oamShapeSizes[node->ofx.sprite.shape].w / 8;
    s32 hTiles = oamShapeSizes[node->ofx.sprite.shape].h / 8;

    u32 size = hdr->bpp * (wTiles * hTiles) * 8;
    size += mask;
    size &= ~mask;

    YanaPileEntry * entry = vramMan->texPile.acquire(key, size, 0);
    if (entry->needsLoad) {
        u16 rowBytes = wTiles * hdr->bpp * 8;
        u16 srcStride = hdr->wBlocks * hdr->bpp * 8;

        vram = vram + vramMan->texOffset + entry->offset;
        vramMan->transMan->postCopyStrided(tiles, vram, rowBytes, hTiles, srcStride, rowBytes);
    }

    return (vramMan->texOffset + entry->offset) / (mask + 1);
}

// MATCHED 100%
s32 ofxLoadPalette(clOfxVramMan * vramMan, RfxPlttHeader * pltt, RfxNode * node, BOOL bpp4) {
    u8 * vram = (u8 *)vramMan->getPlttBase(vramMan->engine, bpp4);

    if (vramMan->transMan == NULL || vram == NULL) {
        return -1;
    }

    if (bpp4 == TRUE) {
        u8 * src = pltt->data + node->ofx.sprite.plttBlock * 32;
        YanaPileEntry * entry = vramMan->plttPile.acquire((u32)src >> 5, 0x20, 0);
        s32 offset;

        if (entry->needsLoad) {
            offset = vramMan->plttOffset + entry->offset;
            vramMan->transMan->postCopy(src, vram + offset, 0x20);
        } else {
            offset = vramMan->plttOffset + entry->offset;
        }

        return offset / 32;
    } else {
        u8 * src = pltt->data;
        YanaPileEntry * entry = vramMan->extPlttPile.acquire((u32)src >> 5, 0x200, 0);
        s32 offset;

        if (entry->needsLoad) {
            offset = vramMan->extPlttOffset + entry->offset;
            vramMan->transMan->postObjExtPltt(src, vram + offset, 0x200, vramMan->engine);
        } else {
            offset = vramMan->extPlttOffset + entry->offset;
        }

        return offset / 512;
    }
}

// MATCHED 100%
u8 * clOfxVramMan::getTexBase(u32 engine) {
    return (engine == 0) ? (u8 *)HW_OBJ_VRAM : (u8 *)HW_DB_OBJ_VRAM;
}

// MATCHED 100%
u8 * clOfxVramMan::getPlttBase(u32 engine, u32 bpp4) {
    if (bpp4) {
        return (engine == 0) ? (u8 *)HW_OBJ_PLTT : (u8 *)HW_DB_OBJ_PLTT;
    } else {
        return getObjExtPlttVramAddr(this->engine);
    }
}

// MATCHED 100%
nsObjSys::clOamMan * clOfxVr::getOamMan() {
    return (this->sys != NULL) ? this->sys->oamMan : NULL;
}

// MATCHED 100%
RfxTexHeader * clOfxVr::getTexHeader(u32 id) {
    return (this->sys != NULL) ? func_020222a4(&this->sys->pools, id) : NULL;
}

// MATCHED 100%
RfxPlttHeader * clOfxVr::getPlttHeader(u32 id) {
    return (this->sys != NULL) ? func_020223b0(&this->sys->pools, id) : NULL;
}

// MATCHED 100%
s32 clOfxVr::getTexAddr(RfxTexHeader * hdr, RfxNode * node) {
    return (this->vramMan != NULL) ? ofxLoadTexture(this->vramMan, hdr, node) : -1;
}

// MATCHED 100%
s32 clOfxVr::getPlttAddr(RfxPlttHeader * pltt, RfxNode * node, BOOL bpp4) {
    return (this->vramMan != NULL) ? ofxLoadPalette(this->vramMan, pltt, node, bpp4) : -1;
}

// MATCHED 100%
u32 clOfxVr::extScriptCount() {
    if (this->sys != NULL) {
        if (this->sys->dataTable != NULL) {
            return fsChunkCount(this->sys->dataTable);
        }
        return 0;

    }
    return 0;
}

// MATCHED 100%
u16 * clOfxVr::extScript(s32 idx) {
    if (this->sys != NULL) {
        if (this->sys->dataTable != NULL) {
            return fsChunkPtr(this->sys->dataTable, idx);
        }
        return NULL;
    }
    return NULL;
}

// MATCHED 100%
void clOfxSh::onNodePos(RfxNode * node) {
    this->shiftX += node->pos.x;
    this->shiftY += node->pos.y;
}

// MATCHED 100%
void clOfxSh::exec() {
    this->shiftX = 0;
    this->shiftY = 0;

    clRfx::exec();
}

// MATCHED 100%
void ofxVrStart(clOfxSys * sys, clOfxVr * vr, u32 scriptId, inYanaUnit ** list, clOfxVramMan * vramMan) {
    u16 * script = (sys->scriptTable != NULL) ? fsChunkPtr(sys->scriptTable, scriptId) : NULL;

    clOfx & ofx = *vr;

    MTX_Identity33(&ofx.rot);

    ofx.originX = 128;
    ofx.originY = 128;
    ofx.originZ = 4096;

    ofx.start(list, script);

    ofx.priority = 0;

    vr->vramMan = vramMan;
    vr->sys = sys;
}

// MATCHED 100%
void clOfxSys::func_02026fc4() {}

// MATCHED 100%
clOfxEx::~clOfxEx() {}

// MATCHED 100%
clOfxSys::~clOfxSys() {}
