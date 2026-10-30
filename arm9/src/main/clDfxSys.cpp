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
s32 dfxLoadPalette(clDfxVramMan *, UnkStruct_020224c4 *, RfxNode *);  // 0x020257ac

void dfxVrStart(clDfxSys *, clDfxVr *, u32, inYanaUnit **, clDfxVramMan *);  // 0x020259b8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
s32 dfxLoadTexture(clDfxVramMan * param1, DfxTexHeader * param2, RfxNode * param3) {
    u8 * addr = param1->getTexBase();

    if (param1->transMan == NULL || addr == NULL) {
        return 0;
    }

    s32 x = param3->field30.field0c / 8;
    s32 y = param3->field30.field0e / 8;
    s32 idx = y * param2->wBlocks + x;

    u8 * src = (u8 *)param2 + 8 + idx * param2->field02 * 8;
    u32 key = ((u32)src >> 5) | (texInfo.sizeS << 26) | (texInfo.sizeT << 29);

    u32 w = (1 << texInfo.sizeS);
    u32 h = (1 << texInfo.sizeT);

    YanaPileEntry * entry = param1->texPile.acquire(key, param2->field02 * (w * h) * 8, 0);

    if (entry->needsLoad) {
        addr = addr + param1->texOffset + entry->offset;

        if (param2->field02 == 4) {
            param1->transMan->postTilesToLinear4bpp(src, addr, w, h, param2->wBlocks, w);
        }

        if (param2->field02 == 8) {
            param1->transMan->postTilesToLinear8bpp(src, addr, w, h, param2->wBlocks, w);
        }
    }

    return param1->texOffset + entry->offset;
}

// MATCHED 100%
s32 dfxLoadPalette(clDfxVramMan * param1, UnkStruct_020224c4 * param2, RfxNode * param3) {
    u8 * addr = param1->getPlttBase();

    if (param1->transMan == NULL || addr == NULL) {
        return 0;
    }

    u8 r0 = plttBlockCount[texInfo.fmt];
    u8 * src = param2->field04 + param3->field30.field14 * 32;
    u32 key = ((u32)src >> 5) | (r0 << 27);

    YanaPileEntry * entry = param1->plttPile.acquire(key, (r0 << 5), 0);

    if (entry->needsLoad) {
        u32 offset = entry->offset;
        u8 * r2 = param1->getPlttBase() + param1->plttOffset + offset;
        param1->transMan->postCopy(src, r2, (r0 << 5));
    }

    return param1->plttOffset + entry->offset;
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
DfxTexHeader * clDfxVr::getTexHeader(u32 param1) {
    if (this->sys == NULL) {
        return NULL;
    }

    return (DfxTexHeader *)func_020222a4(&this->sys->pools, param1);
}

// MATCHED 100%
UnkStruct_020224c4 * clDfxVr::getPlttHeader(u32 param1) {
    if (this->sys == NULL) {
        return NULL;
    }

    return (UnkStruct_020224c4 *)func_020223b0(&this->sys->pools, param1);
}

// MATCHED 100%
s32 clDfxVr::getTexAddr(DfxTexHeader * param1, RfxNode * param2) {
    if (this->vramMan == NULL) {
        return -1;
    }

    return dfxLoadTexture(this->vramMan, param1, param2);
}

// MATCHED 100%
s32 clDfxVr::getPlttAddr(UnkStruct_020224c4 * param1, RfxNode * param2) {
    if (this->vramMan == NULL) {
        return -1;
    }

    return dfxLoadPalette(this->vramMan, param1, param2);
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
u16 * clDfxVr::extScript(s32 param1) {
    if (this->sys != NULL) {
        if (this->sys->dataTable == NULL) {
            return NULL;
        }

        return (u16 *)fsChunkPtr(this->sys->dataTable, param1);
    }

    return NULL;
}

// MATCHED 100%
void clDfxSh::onNodePos(RfxNode * param1) {
    this->shiftX += param1->pos.x;
    this->shiftY += param1->pos.y;
}

// MATCHED 100%
void clDfxSh::exec() {
    this->shiftX = 0;
    this->shiftY = 0;

    this->clRfx::exec();
}

// MATCHED 100%
void dfxVrStart(clDfxSys * param1, clDfxVr * param2, u32 param3, inYanaUnit ** param4, clDfxVramMan * param5) {
    u16 * r4 = (param1->scriptTable != NULL) ? (u16 *)fsChunkPtr(param1->scriptTable, param3) : NULL;

    clDfx & dfx = *param2;

    MTX_Identity33(&dfx.rot);

    dfx.originX = 128;
    dfx.originY = 128;
    dfx.originZ = 4096;

    dfx.start(param4, r4);

    param2->vramMan = param5;
    param2->sys = param1;
}

// MATCHED 100%
void clDfxSys::func_02025a30() {}

// MATCHED 100%
clDfxEx::~clDfxEx() {}

// MATCHED 100%
clDfxSys::~clDfxSys() {}
