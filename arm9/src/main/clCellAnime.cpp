// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clCellAnime.h"

#include "main/cellObjSize.h"
#include "main/clPaf.h"

extern "C" {

#include <nitro.h>

#include "main/tile_convert.h"

// --- Struct Definitions --- //

#define TEX_PLTT_SLOT_SIZE 0x4000

typedef void (*CharCopyFunc)(u32 *, u32 *, u32, u32);

typedef struct {
    u16 * ext;  // +0x0
    u16 * std;  // +0x4
} PlttBufs;

typedef struct {
    nsObjSys::clCellAnime * head;  // +0x0
    nsObjSys::clCellAnime * tail;  // +0x4
} CellAnimeList;

typedef struct {
    union {
        struct {
            s16 x;  // +0x0
            s16 y;  // +0x2
        };
        u32 w;  // +0x0
    };
} OamPos;

typedef struct {
    OamAffine m;  // +0x0
    OamPos p;  // +0x8
} OamXform;

// --- Extern Data Variables --- //

extern const u8 texPlttBaseShift[8];  // 0x0204ac70
extern const u8 texPlttSizeShift[8];  // 0x0204ac80

extern u8 * texPlttBuf;  // 0x0205c230
extern u32 texPlttBankAddrs[6];  // 0x0205c244

extern PlttBufs objPlttBufs[2];  // 0x0205c268

// --- Extern Functions --- //

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

extern void func_02036b0c(const MtxFx44 *);
extern void func_02036b28(const MtxFx43 *);
extern void func_02036b44(const MtxFx33 *);

// --- Variable Definitions --- //

vu32 * const dispcntRegs[2] = { (vu32 *)REG_DISPCNT_ADDR, (vu32 *)REG_DB_DISPCNT_ADDR };  // 0x0204ac68
const u32 objVramBase[2] = { HW_OBJ_VRAM, HW_DB_OBJ_VRAM };  // 0x0204ac60
const u32 objPlttBase[2] = { HW_OBJ_PLTT, HW_DB_OBJ_PLTT };  // 0x0204ac78
const VecFx32 camTarget = { 0, 0, -FX32_ONE };  // 0x0204aca0
const VecFx32 camPos = { 0, 0, 0 };  // 0x0204ac88
const VecFx32 camUp = { 0, FX32_ONE, 0 };  // 0x0204ac94
const u32 texSizeParams[12] = {  // 0x0204accc
    0x00000000, 0x00900000, 0x01200000, 0x01b00000, 0x00100000, 0x00200000,
    0x00a00000, 0x01300000, 0x00800000, 0x01000000, 0x01100000, 0x01a00000,
};
const u32 objDims[12] = {  // 0x0204acfc
    0x00080008, 0x00100010, 0x00200020, 0x00400040, 0x00100008, 0x00200008,
    0x00200010, 0x00400020, 0x00080010, 0x00080020, 0x00100020, 0x00200040,
};
const u32 quadVtx[12][4] = {  // 0x0204ad2c
    { 0xff80ff80, 0x0080ff80, 0x00800080, 0xff800080 },
    { 0xff00ff00, 0x0100ff00, 0x01000100, 0xff000100 },
    { 0xfe00fe00, 0x0200fe00, 0x02000200, 0xfe000200 },
    { 0xfc00fc00, 0x0400fc00, 0x04000400, 0xfc000400 },
    { 0xff80ff00, 0x0080ff00, 0x00800100, 0xff800100 },
    { 0xff80fe00, 0x0080fe00, 0x00800200, 0xff800200 },
    { 0xff00fe00, 0x0100fe00, 0x01000200, 0xff000200 },
    { 0xfe00fc00, 0x0200fc00, 0x02000400, 0xfe000400 },
    { 0xff00ff80, 0x0100ff80, 0x01000080, 0xff000080 },
    { 0xfe00ff80, 0x0200ff80, 0x02000080, 0xfe000080 },
    { 0xfe00ff00, 0x0200ff00, 0x02000100, 0xfe000100 },
    { 0xfc00fe00, 0x0400fe00, 0x04000200, 0xfc000200 },
};
const u32 quadTexCoords[12][4][4] = {  // 0x0204adec
    { { 0x00000000, 0x00800000, 0x00800080, 0x00000080 }, { 0x0000ffff, 0x0080ffff, 0x0080007f, 0x0000007f }, { 0xffff0000, 0x007f0000, 0x007f0080, 0xffff0080 }, { 0xffffffff, 0x007fffff, 0x007f007f, 0xffff007f } },
    { { 0x00000000, 0x01000000, 0x01000100, 0x00000100 }, { 0x0000ffff, 0x0100ffff, 0x010000ff, 0x000000ff }, { 0xffff0000, 0x00ff0000, 0x00ff0100, 0xffff0100 }, { 0xffffffff, 0x00ffffff, 0x00ff00ff, 0xffff00ff } },
    { { 0x00000000, 0x02000000, 0x02000200, 0x00000200 }, { 0x0000ffff, 0x0200ffff, 0x020001ff, 0x000001ff }, { 0xffff0000, 0x01ff0000, 0x01ff0200, 0xffff0200 }, { 0xffffffff, 0x01ffffff, 0x01ff01ff, 0xffff01ff } },
    { { 0x00000000, 0x04000000, 0x04000400, 0x00000400 }, { 0x0000ffff, 0x0400ffff, 0x040003ff, 0x000003ff }, { 0xffff0000, 0x03ff0000, 0x03ff0400, 0xffff0400 }, { 0xffffffff, 0x03ffffff, 0x03ff03ff, 0xffff03ff } },
    { { 0x00000000, 0x00800000, 0x00800100, 0x00000100 }, { 0x0000ffff, 0x0080ffff, 0x008000ff, 0x000000ff }, { 0xffff0000, 0x007f0000, 0x007f0100, 0xffff0100 }, { 0xffffffff, 0x007fffff, 0x007f00ff, 0xffff00ff } },
    { { 0x00000000, 0x00800000, 0x00800200, 0x00000200 }, { 0x0000ffff, 0x0080ffff, 0x008001ff, 0x000001ff }, { 0xffff0000, 0x007f0000, 0x007f0200, 0xffff0200 }, { 0xffffffff, 0x007fffff, 0x007f01ff, 0xffff01ff } },
    { { 0x00000000, 0x01000000, 0x01000200, 0x00000200 }, { 0x0000ffff, 0x0100ffff, 0x010001ff, 0x000001ff }, { 0xffff0000, 0x00ff0000, 0x00ff0200, 0xffff0200 }, { 0xffffffff, 0x00ffffff, 0x00ff01ff, 0xffff01ff } },
    { { 0x00000000, 0x02000000, 0x02000400, 0x00000400 }, { 0x0000ffff, 0x0200ffff, 0x020003ff, 0x000003ff }, { 0xffff0000, 0x01ff0000, 0x01ff0400, 0xffff0400 }, { 0xffffffff, 0x01ffffff, 0x01ff03ff, 0xffff03ff } },
    { { 0x00000000, 0x01000000, 0x01000080, 0x00000080 }, { 0x0000ffff, 0x0100ffff, 0x0100007f, 0x0000007f }, { 0xffff0000, 0x00ff0000, 0x00ff0080, 0xffff0080 }, { 0xffffffff, 0x00ffffff, 0x00ff007f, 0xffff007f } },
    { { 0x00000000, 0x02000000, 0x02000080, 0x00000080 }, { 0x0000ffff, 0x0200ffff, 0x0200007f, 0x0000007f }, { 0xffff0000, 0x01ff0000, 0x01ff0080, 0xffff0080 }, { 0xffffffff, 0x01ffffff, 0x01ff007f, 0xffff007f } },
    { { 0x00000000, 0x02000000, 0x02000100, 0x00000100 }, { 0x0000ffff, 0x0200ffff, 0x020000ff, 0x000000ff }, { 0xffff0000, 0x01ff0000, 0x01ff0100, 0xffff0100 }, { 0xffffffff, 0x01ffffff, 0x01ff00ff, 0xffff00ff } },
    { { 0x00000000, 0x04000000, 0x04000200, 0x00000200 }, { 0x0000ffff, 0x0400ffff, 0x040001ff, 0x000001ff }, { 0xffff0000, 0x03ff0000, 0x03ff0200, 0xffff0200 }, { 0xffffffff, 0x03ffffff, 0x03ff01ff, 0xffff01ff } },
};

CellAnimeList cellAnimeList;  // 0x0205c290
nsObjSys::clCellAnime * charLoadQueue[2];  // 0x0205c280
u16 * charMapBufs[2];  // 0x0205c288
nsObjSys::clCellAnime * plttLoadQueue[2];  // 0x0205c278

// --- Function Declarations --- //

u32 calcFrameCharSize(u32, u32, YanaRes *);  // 0x0202ace8
u32 calcTotalCharSize(u32, u32, YanaRes *);  // 0x0202ae6c
BOOL usesExtPltt(u32, YanaRes *);  // 0x0202b06c
u32 getResPlttUnits(YanaRes *);  // 0x0202b0c0
u32 getResPlttSize(YanaRes *);  // 0x0202b0d0

u32 getDataUnits(u32 *);  // 0x0202b15c
u32 getDataSize(u32 *);  // 0x0202b168
u32 getTexPlttSize(YanaRes *, u32 *);  // 0x0202b1a4
void setupOrtho(u32);  // 0x0202b1d8

void copyChars(u32 *, u32 *, u32, u32);  // 0x0202d08c
void linearToTiles4(u32 *, u32 *, u32, u32);  // 0x0202d09c
void linearToTiles8(u32 *, u32 *, u32, u32);  // 0x0202d0b8
void tilesToLinear4(u32 *, u32 *, u32, u32);  // 0x0202d0d4
void tilesToLinear8(u32 *, u32 *, u32, u32);  // 0x0202d0f0

void flushCharLoads(u32);  // 0x0202d980
void flushPlttLoads(u32);  // 0x0202d9f4
void cellAnimeUpdateAll(void);  // 0x0202da68
void cellAnimeDrawAll(nsObjSys::clOamMan *, nsObjSys::clOamMan *, u32, u32);  // 0x0202daa0
void queueClear(nsObjSys::clCellAnime * &);  // 0x0202dc34

void buildCharMap(YanaRes *, u16 *, u32);  // 0x0202ddcc

void nullCallback(u32, nsObjSys::clCellAnime *);  // 0x0202dfe4
void makeMtx(MtxFx44 *, fx32, fx32, fx32, u32, s32, s32, u16);  // 0x0202dfe8


const CharCopyFunc charConvertFuncs[8] = {  // 0x0204acac
    NULL, NULL, linearToTiles4, linearToTiles8, tilesToLinear4, tilesToLinear8, NULL, NULL,
};

} // extern "C"

// --- Function Definitions --- //

namespace nsObjSys {

// MATCHED 100%
clCellAnime::clCellAnime() {
    this->state = 0;
}

// MATCHED 100%
clCellAnime::~clCellAnime() {
    this->release();
}

}

// MATCHED 100%
u32 calcFrameCharSize(u32 screen, u32 mode, YanaRes * res) {
    u32 maxSize = 0;

    u32 unitShift;
    if (mode == 0) {
        u16 mapShift = (*dispcntRegs[screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT;
        unitShift = mapShift + 5;
    } else {
        unitShift = 3;
    }

    
    u32 unitSize = (1 << unitShift);
    s32 animIdx = 0;
    s32 numAnims = res->numAnims;

    u32 tileShift = tileSizeShift[res->texFmt];

    while (animIdx < numAnims) {
        s32 numFrames = ANIM_REC(res, animIdx)->count;

        for (s32 frameIdx = 0; frameIdx < numFrames; frameIdx++) {
            YanaResFrame * frame = ANIM_ENTRY(res, ANIM_REC(res, animIdx), frameIdx);
            YanaResObj * objs = ANIM_ITEM(res, frame->cell);
            u32 size = 0;
            s32 i;
            s32 numObj = frame->numObj;

            for (i = 0; i < numObj; i++) {
                if (objs[i].newInFrame) {
                    u32 tiles = objs[i].field00;
                    tiles = (((tiles >> 6) & 3) << 2) | ((tiles >> 10) & 3);
                    tiles = objTileCounts[tiles];
                    size += ~(unitSize - 1) & ((unitSize - 1) + (tiles << tileShift));
                }
            }

            if (size > maxSize) {
                maxSize = size;
            }
        }

        animIdx++;
    }

    return maxSize;
}

// MATCHED 100%
u32 calcTotalCharSize(u32 screen, u32 mode, YanaRes * res) {
    u32 size = 0;
    u32 unitShift;

    if (mode == 0) {
        u16 mapShift = (*dispcntRegs[screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT;
        unitShift = mapShift + 5;
    } else {
        unitShift = 3;
    }

    YanaResObj * objs = ANIM_ITEMS(res);
    u16 * objsEnd = ANIM_TBL_DATA(res, ANIM_TBL(res, 0));
    u32 unitSize = (1 << unitShift);
    u32 tileShift = tileSizeShift[res->texFmt];
    s32 numObj = ((u32)objsEnd - (u32)objs) / sizeof(YanaResObj);

    for (s32 i = 0; i < numObj; i++) {
        if (objs[i].newInFile) {
            u32 tiles = objs[i].field00;
            tiles = (((tiles >> 6) & 3) << 2) | ((tiles >> 10) & 3);
            tiles = objTileCounts[tiles];
            size += ~(unitSize - 1) & ((unitSize - 1) + (tiles << tileShift));
        }
    }

    return size;
}

// MATCHED 100%
s32 findCharEntry(u32 screen, u32 mode, u32 kind, YanaRes * res) {
    YanaResEntry * entry = ANIM_TBL(res, 0);
    u32 unitShift;

    if (mode == 0) {
        u16 mapShift = (*dispcntRegs[screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT;
        unitShift = mapShift + 5;
    } else {
        unitShift = 3;
    }

    u8 numEntries = res->numEntries;
    for (s32 i = 0; i < numEntries; i++) {
        if (entry->unitShift == unitShift && entry->kind == kind) {
            return i;
        }

        entry++;
    }

    return -1;
}

// MATCHED 100%
u32 getResCharSize(u32 screen, u32 mode, u32 kind, YanaRes * res) {
    s32 entryIdx = findCharEntry(screen, mode, kind, res);

    if (entryIdx < 0) {
        if (kind != 0) {
            calcFrameCharSize(screen, mode, res);
        } else {
            u32 unitShift;
            if (mode == 0) {
                unitShift = (u16)((*dispcntRegs[screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT) + 5;
            } else {
                unitShift = 3;
            }

            YanaResEntry * entry = ANIM_TBL(res, 0);
            if (entry->unitShift > unitShift) {
                return (entry->numUnits << entry->unitShift);
            } else {
                return calcTotalCharSize(screen, mode, res);
            }
        }
    }

    YanaResEntry * entry = ANIM_TBL(res, entryIdx);
    return (entry->numUnits << entry->unitShift);
}

// MATCHED 100%
BOOL usesExtPltt(u32 screen, YanaRes * res) {
    u32 texFmt = res->texFmt;

    if (texFmt != 3 && texFmt != 4) {
        return FALSE;
    }

    u32 bank = (screen == 0) ? GX_GetBankForOBJExtPltt() : GX_GetBankForSubOBJExtPltt();

    if (bank != 0) {
        return (texFmt != 3);
    }

    return FALSE;
}

// MATCHED 100%
u32 getResPlttUnits(YanaRes * res) {
    return getResPlttSize(res) >> 5;
}

// MATCHED 100%
u32 getResPlttSize(YanaRes * res) {
    u32 numPltt = res->plttLast + 1;

    switch (res->texFmt) {
        case 0: 
            return 0;
        case 1:
            return numPltt << 6;
        case 2:
            return numPltt << 3;
        case 3:
            return numPltt << 5;
        case 4:
            return numPltt << 9;
        case 5:
            return 0;
        case 6:
            return numPltt << 4;
        case 7:
            return 0;
        default:
            return 0;
    }
}

// MATCHED 100%
u32 getDataUnits(u32 * data) {
    return *data / 32;
}

// MATCHED 100%
u32 getDataSize(u32 * data) {
    return *data;
}

// MATCHED 100%
u32 getObjPlttUnits(YanaRes * res, u32 * plttData) {
    u32 units = getResPlttUnits(res);
    u32 dataUnits = (plttData != NULL) ? getDataUnits(plttData) : 0;
    if (units < dataUnits) {
        units = dataUnits;
    }
    return units;
}

// MATCHED 100%
u32 getTexPlttSize(YanaRes * res, u32 * plttData) {
    u32 size = getResPlttSize(res);
    u32 dataSize = (plttData != NULL) ? getDataSize(plttData) : 0;
    if (size < dataSize) {
        size = dataSize;
    }
    return size;
}

// MATCHED 100%
void setupOrtho(u32 scale) {
    u32 width = scale << 0xd;
    u32 depth = scale << 0xd;
    G3_OrthoW(0, scale * 0x1800, 0, width, 0, -depth, 0x400000, NULL);
    G3_LookAt(&camPos, &camUp, &camTarget, NULL);
}

namespace nsObjSys {

// MATCHED 100%
void clCellAnime::initWithHandles(VramHandle * charHandle, VramHandle * plttHandle, u32 perFrame, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    this->setup(charHandle->manBank, (charHandle->manType == 0) ? VRAM_MAN_OBJ : VRAM_MAN_TEX, perFrame, charHandle, charHandle->offset, plttHandle, plttHandle->offset, res, charData, plttData, pafData);

    this->ownsChar = 0;
    this->ownsPltt = 0;

    this->state = 1;
}

// MATCHED 100%
void clCellAnime::initAt(u32 screen, u32 mode, u32 perFrame, u32 charOffset, u32 plttOffset, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    this->setup(screen, mode, perFrame, NULL, charOffset, NULL, plttOffset, res, charData, plttData, pafData);

    this->state = 1;
}

// MATCHED 100%
void clCellAnime::initAlloc(u32 mode, u32 perFrame, clVramManBase * charMan, clVramManBase * plttMan, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    u32 charAttr = (perFrame != 0) ? 2 : 5;
    u32 charSize = getResCharSize(charMan->manBank, mode, perFrame, res);
    VramHandle * charHandle = charMan->alloc(charSize, (u32)charData, charAttr);

    u16 plttAttr = 4;
    u32 plttSize;
    if (mode == 0) {
        plttSize = getObjPlttUnits(res, plttData);
        plttAttr |= (usesExtPltt(plttMan->manBank, res) ? 0x100 : 0);
    } else {
        plttSize = getTexPlttSize(res, plttData);
    }

    VramHandle * plttHandle = plttMan->alloc(plttSize, (u32)plttData, plttAttr);

    this->setup(charHandle->manBank, mode, perFrame, charHandle, charHandle->offset, plttHandle, plttHandle->offset, res, charData, plttData, pafData);

    this->ownsChar = 1;
    this->ownsPltt = 1;

    this->state = 1;
}

// MATCHED 100%
void clCellAnime::setup(u32 screen, u32 mode, u32 perFrame, VramHandle * charHandle, u32 charOffset, VramHandle * plttHandle, u32 plttOffset, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    this->state = 0;

    this->res = res;
    this->charData = charData;
    this->plttData = plttData;
    this->pafData = pafData;

    this->charHandle = charHandle;
    this->charOffset = charOffset;
    this->plttHandle = plttHandle;
    this->plttOffset = plttOffset;

    this->field80w = (mode << 8) | 0x10000 | screen;
    this->field8cw = 0x101;

    s32 entryIdx = findCharEntry(screen, mode, perFrame, res);
    if (entryIdx < 0) {
        this->charEntry = 0xff;

        if (perFrame == 0) {
            u32 unitShift;
            if (mode == 0) {
                u16 mapShift = (*dispcntRegs[screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT;
                unitShift = mapShift + 5;
            } else {
                unitShift = 3;
            }

            buildCharMap(res, this->getCharMap(), unitShift);
        }
    } else {
        this->charEntry = entryIdx;
    }

    this->anim = 0;
    this->frame = 0;
    this->loadedCell = -1;

    this->cell = ANIM_ENTRY(this->res, ANIM_REC(this->res, this->anim), this->frame)->cell;

    this->speed = 0x100;
    this->ended = 0;

    this->perFrame = perFrame;

    MTX_Identity44(&this->mtx);

    this->onEnd = nullCallback;
    this->onEndArg = 0;

    this->endMode = 0;
    this->playCount = -1;

    this->field88w = 0x3020100;

    switch (this->mode) {
        case 0: {
            this->field98h = 0x100;
            break;
        }
        case 1: {
            this->field98w = 0x7fff001f;
            this->field9cw = 0x10010;
            break;
        }
    }

    this->nextCharLoad = NULL;
    this->nextPlttLoad = NULL;

    this->link();

    this->frameTimer = ANIM_ENTRIES(this->res, ANIM_REC(this->res, 0))->time << 8;;
    this->animTime = 0;
}

// MATCHED 100%
void clCellAnime::setRes(YanaRes * res, u32 * charData, u32 * plttData) {
    if (this->state == 0) {
        return;
    }
    this->state = -1;

    this->res = res;
    this->charData = charData;
    this->plttData = plttData;

    s32 entryIdx = findCharEntry(this->screen, this->mode, this->perFrame, res);
    if (entryIdx < 0) {
        this->charEntry = 0xff;
    } else {
        this->charEntry = entryIdx;
    }

    this->anim = 0;
    this->frame = 0;

    this->loadedCell = -1;
    this->cell = ANIM_ENTRY(this->res, ANIM_REC(this->res, this->anim), this->frame)->cell;

    this->speed = 0x100;

    this->frameTimer = ANIM_ENTRIES(this->res, ANIM_REC(this->res, 0))->time << 8;

    this->playCount = -1;

    this->animTime = 0;

    this->state = 1;
}

// MATCHED 100%
void clCellAnime::release() {
    if (this->state) {
        this->state = 0;

        if (this->charHandle != NULL && this->ownsChar) {
            clVramManBase * man = getVramManOfHandle(this->charHandle);
            man->release(this->charHandle, 0, -1);
        }

        if (this->plttHandle != NULL && this->ownsPltt) {
            clVramManBase * man = getVramManOfHandle(this->plttHandle);
            man->release(this->plttHandle, 0, -1);
        }

        this->unlink();
    }
}

// MATCHED 100%
void clCellAnime::update() {
    if (this->state != 0) {
        if (this->charHandle != NULL && this->charHandle->movedBit != 0) {
            this->loadedCell = -1;
            this->charOffset = this->charHandle->offset;
        }

        if (this->plttHandle != NULL && this->plttHandle->movedBit != 0) {
            this->plttDirty = 1;
            this->plttOffset = this->plttHandle->offset;
        }

        this->cell = ANIM_ENTRY(this->res, ANIM_REC(this->res, this->anim), this->frame)->cell;

        if (this->paused == 0) {
            this->ended = 0;

            s32 step = this->speed;
            s32 dir;
            if (step < 0) {
                dir = -1;
            } else {
                step = -step;
                dir = 1;
            }

            s32 timer = this->frameTimer + step;

            YanaResAnim * animRec = ANIM_REC(this->res, this->getAnim());
            YanaResFrame * frames = ANIM_ENTRIES(this->res, ANIM_REC(this->res, this->getAnim()));

            s32 numFrames = animRec->count;

            s32 frame = this->getFrame();

            while (timer <= 0) {
                frame += dir;
    
                if (frame < 0 || frame >= numFrames) {
                    if (this->playCount != -1 && --this->playCount == 0) {
                        this->onEnd(this->onEndArg, this);

                        this->setPaused(1);

                        frame = 0;

                        this->frameTimer = 0;
                        this->ended = 1;

                        if (dir == 1) {
                            frame = numFrames - 1;
                        }

                        switch (this->endMode) {
                            case 0:
                            case 2: {
                                break;
                            }
                            case 1:
                            case 3: {
                                this->visible = 0;
                            }
                        }
                        break;
                    }

                    switch (this->endMode) {
                        case 0:
                        case 1: {
                            if (dir == 1) {
                                frame = 0;
                            } else {
                                frame = numFrames-1;
                            }
                            break;
                        }
                        case 2:
                        case 3: {
                            this->speed = -this->speed;

                            dir *= -1;
                            if (dir == 1) {
                                frame = 0;
                            } else {
                                frame = numFrames-1;
                            }
                            break;
                        }
                    }
                }

                s32 duration;
                if (frame <= 0) {
                    duration = frames->time;
                } else {
                    duration = frames[frame].time - frames[frame-1].time;
                }

                timer += duration * 0x100;
            }

            this->frame = frame;
            this->frameTimer = timer;
        }

        if (this->paused == 0) {
            YanaResAnim * animRec = ANIM_REC(this->res, this->getAnim());
            YanaResFrame * frames = ANIM_ENTRIES(this->res, ANIM_REC(this->res, this->getAnim()));

            s32 elapsed = this->animTime + this->speed;
            u16 length = frames[animRec->count - 1].time;
            while (elapsed < 0) {
                elapsed += length * 0x100;
            }
            while (elapsed >= length * 0x100) {
                elapsed -= length * 0x100;
            }

            this->animTime = elapsed;
        }

        this->updatePaf();
    }
}

// MATCHED 100%
s16 clCellAnime::getFrame() {
    return this->frame;
}

// MATCHED 100%
void clCellAnime::setPaused(u8 paused) {
    this->paused = paused;
}

// MATCHED 100%
void clCellAnime::updatePaf() {
    if (this->pafData != NULL && this->plttData != NULL) {
        u16 * buf = this->getPlttBuf();
        if (buf != NULL) {
            s32 speed = (this->paused != 0) ? 0 : this->speed;
            pafUpdate(this->pafData, buf, this->getAnim(), this->animTime, &this->pafState, speed);
        }
    }
}

// MATCHED 100%
u32 clCellAnime::draw(clOamMan * oamMan) {
    if (this->state == 0) {
        return 0;
    }

    if (this->visible == 0) {
        this->plttBuffered = 0;
        return 0;
    }

    GXOamAttr * oamEnd = NULL;
    OamAffine * affineEnd = NULL;

    u32 size;
    if (oamMan != NULL) {
        GXOamAttr * oamStart = oamMan->oamEnd;
        oamEnd = oamMan->oamEnd;
        affineEnd = oamMan->affineEnd;

        size = this->drawImpl(oamMan->oam, &oamEnd, oamMan->affine, &affineEnd);

        oamMan->oamEnd = oamEnd;
        oamMan->affineEnd = affineEnd;

        if (oamStart != oamEnd) {
            GXOamAttr * oam = oamMan->oam;
            u32 depth = this->getDepth();
            oamMan->addRun(oamStart - oam, oamEnd - oamStart, depth);
        }
    } else {
        size = this->drawImpl(NULL, NULL, NULL, NULL);
    }

    return size;
}

// MATCHED 100%
u32 clCellAnime::getDepth() {
    return this->mtx._32;
}

// MATCHED 100%
u32 clCellAnime::drawImpl(GXOamAttr * oam, GXOamAttr ** oamEnd, OamAffine * affine, OamAffine ** affineEnd) {
    if (this->state == 0) {
        return 0;
    }

    if (this->visible == 0) {
        this->plttBuffered = 0;
        return 0;
    }

    if (this->mode == 0) {
        return this->drawOam(oam, oamEnd, affine, affineEnd);
    }

    return this->draw3d();
}

// MATCHED 100%
void clCellAnime::mtxToAffine(YanaResXform * xform, MtxFx44 * mtx) {
    xform->m00 = mtx->_00 >> 4;
    xform->m01 = mtx->_01 >> 4;
    xform->m10 = mtx->_10 >> 4;
    xform->m11 = mtx->_11 >> 4;
    xform->x = (mtx->_30 + mtx->_03) >> 12;
    xform->y = (mtx->_31 + mtx->_13) >> 12;
}


static inline u32 findAffine(OamAffine * begin, OamAffine * end, u32 w0, u32 w1) {
    OamAffine * cur = end - 1;
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

// MATCHED 100%
u32 clCellAnime::drawOam(GXOamAttr * oam, GXOamAttr ** oamEnd, OamAffine * affine, OamAffine ** affineEnd) {
    OamXform xform[2];
    OamXform composed;
    OamXform objXform;
    OamAffine inverse;
    u32 charOffset;
    u32 unitShift;
    u32 attrMask;
    u32 attrBase;
    u32 doubleSize;
    u32 tileShift;
    u8 * priority;
    u32 unitMask;
    u32 alignMask;
    u32 mapMode;
    YanaResObj * objEnd;
    YanaResMtx * mtxTable;
    GXOamAttr * oamCur;
    s32 baseX;
    s32 baseY;
    u32 charSize;
    u32 attr;
    u32 shape;
    s32 halfW;
    s32 objX;
    YanaResFrame * frame;
    s32 det;
    YanaRes * res;
    s16 tmp;
    BOOL cellChanged;
    BOOL hasXform;
    u32 cell;
    u16 * map;
    u16 * charMap;
    YanaResObj * obj;
    OamAffine * affineCur;
    u32 bits;
    u32 tile;

    if (this->state == 0) {
        return 0;
    }

    if (this->visible == 0) {
        this->plttBuffered = 0;
        return 0;
    }

    attrBase = this->plttOffset / 16;
    if (attrBase == 16) {
        attrBase = this->plttOffset % 16;
    }
    attrBase <<= 14;

    if (this->alpha != 0) {
        attrBase |= this->alpha << 2;
        attrMask = 0x8003ccf0;
    } else {
        attrMask = 0x8003ccfc;
    }

    doubleSize = this->polygonId * 2;
    unitShift = (u16)((*dispcntRegs[this->screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT) + 5;
    charOffset = this->charOffset;
    u32 maskShift = (u16)((*dispcntRegs[this->screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT) + 5;
    unitMask = (1 << maskShift) - 1;
    priority = this->priority;
    alignMask = ~unitMask;

    this->mtxToAffine((YanaResXform *)&xform[1], &this->mtx);

    res = this->res;
    frame = ANIM_ENTRY(res, ANIM_REC(res, this->anim), this->frame);
    this->cell = frame->cell;
    cell = frame->cell;
    tileShift = tileSizeShift[res->texFmt];
    obj = ANIM_ITEM(res, cell);
    mtxTable = (YanaResMtx *)ANIM_PTR(res, res->mtxOffset);
    objEnd = obj + frame->numObj;

    if (this->charEntry == 0xff) {
        charMap = this->getCharMap();
        map = ANIM_TBL_DATA(res, ANIM_TBL(res, 0)) + cell;

        if (this->perFrame != 0) {
            mapMode = 1;

            u32 offset = 0;
            u32 shift = (u16)((*dispcntRegs[this->screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT) + 5;
            u16 * dst = charMap;
            for (s32 i = 0; i < objEnd - obj; i++) {
                u32 bits = obj[i].field00;
                if (bits & 0x20000000) {
                    dst[0] = offset >> shift;
                    dst[1] = map[i];
                    dst += 2;

                    bits = (((bits >> 6) & 3) << 2) | ((bits >> 10) & 3);
                    offset += alignMask & (unitMask + (objTileCounts[bits] << tileShift));
                }
            }
        } else {
            mapMode = 2;
        }
    } else {
        map = ANIM_TBL_DATA(res, ANIM_TBL(res, this->charEntry)) + cell;
        mapMode = 0;
    }

    if (frame->xformIdx != 0) {
        YanaResXform * resXform = (YanaResXform *)ANIM_PTR(res, res->xformOffset) + (frame->xformIdx - 1);
        s32 a = resXform->m00;
        s32 b = resXform->m01;

        composed.m.pa = (a * xform[1].m.pa + b * xform[1].m.pc) >> 8;
        composed.m.pb = (a * xform[1].m.pb + b * xform[1].m.pd) >> 8;

        a = resXform->m10;
        b = resXform->m11;

        composed.m.pc = (a * xform[1].m.pa + b * xform[1].m.pc) >> 8;
        composed.m.pd = (a * xform[1].m.pb + b * xform[1].m.pd) >> 8;

        a = resXform->x;
        b = resXform->y;

        composed.p.x = xform[1].p.x + ((a * xform[1].m.pa + b * xform[1].m.pc) >> 8);
        composed.p.y = xform[1].p.y + ((a * xform[1].m.pb + b * xform[1].m.pd) >> 8);

        xform[1] = composed;
    }

    cellChanged = (this->loadedCell != this->cell);

    s32 ad = (xform[1].m.pa * xform[1].m.pd) >> 8;
    s32 bc = (xform[1].m.pb * xform[1].m.pc) >> 8;
    if ((MATH_IAbs(ad) <= 1 && MATH_IAbs(bc) <= 1) || ad == bc) {
        this->plttBuffered = 0;
        return 0;
    }

    oamCur = *oamEnd;
    affineCur = *affineEnd;

    baseX = xform[1].p.x;
    baseY = xform[1].p.y;
    hasXform = (xform[1].m.w0 != 0x100 || xform[1].m.w1 != 0x1000000);
    charSize = 0;
    xform[0].p = xform[1].p;

    while (obj != objEnd) {
        bits = obj->field00;
        attr = (attrBase + (bits & attrMask)) | (priority[(bits >> 12) & 3] << 12);
        xform[0].m = xform[1].m;
        shape = (((bits >> 6) & 3) << 2) | ((bits >> 10) & 3);
        halfW = objHalfWidths[shape];
        s32 halfH = objHalfHeights[shape];
        u32 xformBits;

        if (bits & 1) {
            s32 a = mtxTable[(bits >> 18) & 0x7ff].m00;
            s32 b = mtxTable[(bits >> 18) & 0x7ff].m01;

            objXform.m.pa = (a * xform[0].m.pa + b * xform[0].m.pc) >> 8;
            objXform.m.pb = (a * xform[0].m.pb + b * xform[0].m.pd) >> 8;

            a = mtxTable[(bits >> 18) & 0x7ff].m10;
            b = mtxTable[(bits >> 18) & 0x7ff].m11;

            objXform.m.pc = (a * xform[0].m.pa + b * xform[0].m.pc) >> 8;
            objXform.m.pd = (a * xform[0].m.pb + b * xform[0].m.pd) >> 8;

            xform[0].m = objXform.m;
        } else {
            if (bits & 0x100) {
                xform[0].m.pa *= -1;
                xform[0].m.pb *= -1;
            }
            if (bits & 0x200) {
                xform[0].m.pc *= -1;
                xform[0].m.pd *= -1;
            }
        }

        u32 w0 = xform[0].m.w0;
        if (w0 == 0x100 && xform[0].m.w1 == 0x1000000) {
            xformBits = 0;
        } else if (w0 == 0xff00 && xform[0].m.w1 == 0x1000000) {
            xformBits = 0x10000000;
        } else if (w0 == 0x100 && xform[0].m.w1 == 0xff000000) {
            xformBits = 0x20000000;
        } else if (w0 == 0xff00 && xform[0].m.w1 == 0xff000000) {
            xformBits = 0x30000000;
        } else {
            tmp = xform[0].m.pb;
            xform[0].m.pb = xform[0].m.pc;
            xform[0].m.pc = tmp;
            det = (xform[0].m.pa * xform[0].m.pd - xform[0].m.pb * xform[0].m.pc) >> 4;
            if (det == 0) {
                det = 0;
            } else {
                s32 inv = 0x1000000 / det;
                if (0x1000000 - inv * det > 0x300) {
                    inv++;
                }

                inverse.pa = (xform[0].m.pd * inv) >> 12;
                inverse.pb = -((xform[0].m.pb * inv) >> 12);
                inverse.pc = -((xform[0].m.pc * inv) >> 12);
                inverse.pd = (xform[0].m.pa * inv) >> 12;
            }

            if (det == 0) {
                obj++;
                map++;
                continue;
            }

            if (doubleSize != 0 && 
                (MATH_IAbs(((YanaResXform *)&xform[0])->m00) > 0x100 || MATH_IAbs(((YanaResXform *)&xform[0])->m11) > 0x100
                    || MATH_IAbs(((YanaResXform *)&xform[0])->m01) > 0x100 || MATH_IAbs(((YanaResXform *)&xform[0])->m10) > 0x100
                    || ((YanaResXform *)&xform[0])->m01 != 0 || ((YanaResXform *)&xform[0])->m10 != 0)
                ) {
                attr |= 3;
                halfH *= 2;
                halfW *= 2;
            } else {
                attr |= 1;
            }
            xformBits = 1;
        }

        s32 x = obj->x;
        s32 y = obj->y;
        if (bits & 0x20000000) {
            charSize += alignMask & (unitMask + (objTileCounts[shape] << tileShift));
        }

        if (hasXform) {
            objX = x;
            x = baseX + ((xform[1].m.pa * x + xform[1].m.pc * y) >> 8);
            y = baseY + ((xform[1].m.pb * objX + xform[1].m.pd * y) >> 8);
        } else {
            x += baseX;
            y += baseY;
        }

        y -= halfH;
        x -= halfW;
        s32 right = x + halfW * 2;
        s32 bottom = y + halfH * 2;
        if (!(-(halfW * 2) < x && right < halfW * 2 + 0x100 && -(halfH * 2) < y && bottom < halfH * 2 + 0xc0)) {
            obj++;
            map++;
            continue;
        }

        if (xformBits == 1) {
            xformBits = findAffine(affine, affineCur, inverse.w0, inverse.w1);
            if (xformBits >= (u32)(affineCur - affine)) {
                affineCur++;
            }
            xformBits <<= 25;
        }

        switch (mapMode) {
            case 0: {
                tile = *map;
                break;
            }
            case 1:
            case 2: {
                tile = *map;
                for (s32 i = 0; ; i++) {
                    if (tile == charMap[i * 2 + 1]) {
                        tile = charMap[i * 2];
                        break;
                    }
                }
                break;
            }
        }

        oamCur->attr01 = xformBits | ((y & 0xff) | ((x << 16) & 0x1ff0000) | ((attr & 0xff) << 8) | ((attr & 0xc00) << 20));
        oamCur->attr2 = (tile + (charOffset >> unitShift)) | ((attr & 0x3f000) >> 2);
        obj++;
        map++;
        oamCur++;
    }

    if (oamCur == *oamEnd) {
        this->plttBuffered = 0;
        return 0;
    }

    *oamEnd = oamCur;
    *affineEnd = affineCur;

    if (this->charDirty != 0 && cellChanged && this->charData != NULL && this->needsCharLoad()) {
        queuePush(this, charLoadQueue[0], this->nextCharLoad, 0);
        if (this->charHandle != NULL) {
            this->charHandle->queued = 1;
        }
    }

    if (this->plttDirty != 0 && this->plttData != NULL) {
        queuePush(this, plttLoadQueue[0], this->nextPlttLoad, 0);
    }

    return charSize;
}

// MATCHED 100%
u32 clCellAnime::draw3d() {
    YanaRes * res;
    u32 plttShift;
    u32 plttBase;
    BOOL multiPltt;
    u32 tileShift;
    u32 mapMode;
    YanaResObj * objEnd;
    YanaResMtx * mtxTable;
    u32 texParam;
    s32 depthStep;
    u32 charSize;
    YanaResFrame * frame;
    BOOL hasXform;
    BOOL cellChanged;
    u32 cell;
    u16 * map;
    u16 * charMap;
    YanaResObj * obj;

    if (this->state == 0) {
        return 0;
    }

    if (this->visible == 0) {
        this->plttBuffered = 0;
        return 0;
    }

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PushMtx();
    
    hasXform = FALSE;

    reg_G3_POLYGON_ATTR = GX_PACK_POLYGONATTR_PARAM(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, this->polygonId, this->alpha, 0);
    G3_Color(this->color);

    fx32 m30 = this->mtx._30;
    fx32 m31 = this->mtx._31;
    fx32 m03 = this->mtx._03;
    fx32 m13 = this->mtx._13;
    this->mtx._30 >>= 7;
    this->mtx._31 >>= 7;
    this->mtx._03 >>= 7;
    this->mtx._13 >>= 7;

    func_02036b0c(&this->mtx);

    this->mtx._30 = m30;
    this->mtx._31 = m31;
    this->mtx._03 = m03;
    this->mtx._13 = m13;

    if (this->mtx._00 != FX32_ONE || this->mtx._01 != 0) {
        hasXform = TRUE;
    }
    if ((!hasXform && this->mtx._10 != 0) || this->mtx._11 != FX32_ONE) {
        hasXform = TRUE;
    }

    res = this->res;
    plttShift = texPlttSizeShift[res->texFmt] - texPlttBaseShift[res->texFmt];
    plttBase = this->plttOffset >> texPlttBaseShift[res->texFmt];

    if (res->plttLast != 0) {
        multiPltt = TRUE;
    } else {
        multiPltt = FALSE;
        G3_Direct1(G3OP_TEXPLTT_BASE, plttBase);
    }

    frame = ANIM_ENTRY(this->res, ANIM_REC(this->res, this->anim), this->frame);
    this->cell = frame->cell;

    cell = frame->cell;
    tileShift = tileSizeShift[res->texFmt];
    obj = ANIM_ITEM(res, cell);
    objEnd = obj + frame->numObj;
    mtxTable = (YanaResMtx *)ANIM_PTR(res, res->mtxOffset);

    if (this->charEntry == 0xff) {
        charMap = this->getCharMap();
        map = ANIM_TBL_DATA(res, ANIM_TBL(res, 0)) + cell;

        if (this->perFrame != 0) {
            mapMode = 1;

            u32 offset = 0;
            u16 * dst = charMap;
            for (s32 i = 0; i < objEnd - obj; i++) {
                u32 bits = obj[i].field00;
                if (bits & 0x20000000) {
                    dst[0] = offset / 8;
                    dst[1] = map[i];
                    dst += 2;

                    bits = (((bits >> 6) & 3) << 2) | ((bits >> 10) & 3);
                    offset += ((objTileCounts[bits] << tileShift) + 7) & ~7;
                }
            }
        } else {
            mapMode = 2;
        }
    } else {
        map = ANIM_TBL_DATA(res, ANIM_TBL(res, this->charEntry)) + cell;
        mapMode = 0;
    }

    if (frame->xformIdx != 0) {
        YanaResXform * xform = (YanaResXform *)ANIM_PTR(res, res->xformOffset) + (frame->xformIdx - 1);

        MtxFx43 xformMtx;
        xformMtx._00 = xform->m00 * 16;
        xformMtx._10 = xform->m10 * 16;
        xformMtx._01 = xform->m01 * 16;
        xformMtx._11 = xform->m11 * 16;
        xformMtx._21 = 0;
        xformMtx._20 = 0;
        xformMtx._12 = 0;
        xformMtx._02 = 0;
        xformMtx._22 = FX32_ONE;
        xformMtx._30 = xform->x * 32;
        xformMtx._31 = xform->y * 32;
        xformMtx._32 = 0;
        func_02036b28(&xformMtx);

        if (!hasXform) {
            if (xformMtx._00 != FX32_ONE || xformMtx._01 != 0) {
                hasXform = TRUE;
            }
            if (xformMtx._10 != 0 || xformMtx._11 != FX32_ONE) {
                hasXform = TRUE;
            }
        }
    }

    cellChanged = (this->loadedCell != this->cell);
    texParam = (res->texFmt << 26) | (this->charOffset >> 3);
    depthStep = this->depthStep;

    s32 depthScale = this->depthScale;
    s32 depths[4];
    for (s32 i = 0; i < 4; i++) {
        depths[i] = depthScale * this->getPriority(i);
    }

    if (obj != objEnd) {
        G3_Begin(GX_BEGIN_QUADS);
    }

    charSize = 0;
    while (obj != objEnd) {
        BOOL hasMtx = FALSE;
        u32 flip = 0;

        G3_PushMtx();

        u32 bits = obj->field00;
        s16 x = obj->x;
        u32 shape = (((bits >> 6) & 3) << 2) | ((bits >> 10) & 3);
        s16 y = obj->y;

        if (bits & 0x20000000) {
            charSize += objTileCounts[shape] << tileShift;
        }

        G3_Translate(x * 32, y * 32, depths[(bits >> 12) & 3]);

        depths[(bits >> 12) & 3] += depthStep;

        if (bits & 1) {
            MtxFx33 objMtx;
            objMtx._00 = mtxTable[(bits >> 18) & 0x7ff].m00 * 16;
            objMtx._10 = mtxTable[(bits >> 18) & 0x7ff].m10 * 16;
            objMtx._01 = mtxTable[(bits >> 18) & 0x7ff].m01 * 16;
            objMtx._11 = mtxTable[(bits >> 18) & 0x7ff].m11 * 16;
            objMtx._21 = 0;
            objMtx._20 = 0;
            objMtx._12 = 0;
            objMtx._02 = 0;
            objMtx._22 = FX32_ONE;
            func_02036b44(&objMtx);

            hasMtx = TRUE;
        } else {
            switch (bits & 0x300) {
                case 0x100: {
                    G3_Scale(-FX32_ONE, FX32_ONE, FX32_ONE);
                    flip = 1;
                    break;
                }
                case 0x200: {
                    G3_Scale(FX32_ONE, -FX32_ONE, FX32_ONE);
                    flip = 2;
                    break;
                }
                case 0x300: {
                    G3_Scale(-FX32_ONE, -FX32_ONE, FX32_ONE);
                    flip = 3;
                    break;
                }
            }
        }

        if (multiPltt) {
            G3_Direct1(G3OP_TEXPLTT_BASE, plttBase + (((bits >> 14) & 0xf) << plttShift));
        }

        u32 tile;
        switch (mapMode) {
            case 0: {
                tile = *map;
                break;
            }
            case 1:
            case 2: {
                tile = *map;
                for (s32 i = 0; ; i++) {
                    if (tile == charMap[i * 2 + 1]) {
                        tile = charMap[i * 2];
                        break;
                    }
                }
                break;
            }
        }

        G3_Direct1(G3OP_TEXIMAGE_PARAM, (tile + (texParam | texSizeParams[shape])) | 0x60000000);

        if (hasXform | hasMtx) {
            VecFx16 test;
            G3_VectorTest(8, 0, 0);

            while (G3X_GetVectorTestResult(&test.x)) {}

            if (test.x < 0) {
                flip ^= 1;
            }

            G3_VectorTest(0, 8, 0);

            while (G3X_GetVectorTestResult(&test.x)) {}

            if (test.y < 0) {
                flip ^= 2;
            }
        }

        const u32 * vtx = quadVtx[shape];
        const u32 * texCoords = quadTexCoords[shape][flip];
        G3_Direct1(G3OP_TEXCOORD, texCoords[0]);
        G3_Direct2(G3OP_VTX_16, vtx[0], 0);
        G3_Direct1(G3OP_TEXCOORD, texCoords[1]);
        G3_Direct1(G3OP_VTX_XY, vtx[1]);
        G3_Direct1(G3OP_TEXCOORD, texCoords[2]);
        G3_Direct1(G3OP_VTX_XY, vtx[2]);
        G3_Direct1(G3OP_TEXCOORD, texCoords[3]);
        G3_Direct1(G3OP_VTX_XY, vtx[3]);
        G3_PopMtx(1);

        obj++;
        map++;
    }

    if (charSize != 0) {
        G3_End();
    }
    G3_PopMtx(1);

    if (this->charDirty != 0 && cellChanged && this->charData != NULL && this->needsCharLoad()) {
        queuePush(this, charLoadQueue[1], this->nextCharLoad, 0);

        if (this->charHandle != NULL) {
            this->charHandle->queued = 1;
        }
    }

    if (this->plttDirty != 0 && this->plttData != NULL) {
        queuePush(this, plttLoadQueue[1], this->nextPlttLoad, 0);
    }

    return charSize;
}

// MATCHED 100%
u8 clCellAnime::getPriority(u32 idx) {
    return this->priority[idx];
}

// MATCHED 100%
u32 clCellAnime::func_0202d084() {
    return 0;
}

}

#ifndef IDE_PARSE
// MATCHED 100%
asm void copyChars(u32 *, u32 *, u32, u32) {
    stmdb sp!, {lr}
    bl func_01ff8380
    ldmia sp!, {lr}
    bx lr
}

// MATCHED 100%
asm void linearToTiles4(u32 * src, u32 * dst, u32 size, u32 dims) {
    stmdb sp!, {lr}
    mov r2, r3, lsr #0x11
    mov r3, r3, lsl #0x10
    mov r3, r3, lsr #0x10
    bl LinearToTiles4bpp
    ldmia sp!, {lr}
    bx lr
}

// MATCHED 100%
asm void linearToTiles8(u32 * src, u32 * dst, u32 size, u32 dims) {
    stmdb sp!, {lr}
    mov r2, r3, lsr #0x10
    mov r3, r3, lsl #0x10
    mov r3, r3, lsr #0x10
    bl LinearToTiles8bpp
    ldmia sp!, {lr}
    bx lr
}

// MATCHED 100%
asm void tilesToLinear4(u32 * src, u32 * dst, u32 size, u32 dims) {
    stmdb sp!, {lr}
    mov r2, r3, lsr #0x11
    mov r3, r3, lsl #0x10
    mov r3, r3, lsr #0x10
    bl TilesToLinear4bpp
    ldmia sp!, {lr}
    bx lr
}

// MATCHED 100%
asm void tilesToLinear8(u32 * src, u32 * dst, u32 size, u32 dims) {
    stmdb sp!, {lr}
    mov r2, r3, lsr #0x10
    mov r3, r3, lsl #0x10
    mov r3, r3, lsr #0x10
    bl TilesToLinear8bpp
    ldmia sp!, {lr}
    bx lr
}
#endif

// MATCHED 100%
void transferChars(u32 dst, u32 linear, YanaRes * res, u32 * charData, u16 * map, u32 unitShift, u32 firstObj, u32 numObj, u8 remap) {
    YanaResObj * objEnd;
    u32 srcShift;
    YanaResObj * allObjEnd;
    u8 * srcBase;

    u32 ownedStep = (remap != 0) ? 2 : 1;
    u32 sharedStep = (remap == 0) ? 1 : 0;

    if (numObj != 0) {
        YanaResObj * obj = ANIM_ITEM(res, firstObj);
        u16 * srcMap = ANIM_TBL_DATA(res, ANIM_TBL(res, 0)) + firstObj;
        objEnd = obj + numObj;
        srcShift = ANIM_TBL(res, 0)->unitShift;
        u32 tileShift = tileSizeShift[res->texFmt];

        CharCopyFunc copy;
        if (linear == res->linear) {
            copy = copyChars;
        } else {
            copy = charConvertFuncs[(linear << 2) | (res->linear << 1) | ((tileShift == 6) ? 1 : 0)];
        }

        u8 * src = (u8 *)(charData + 1);
        while (obj != objEnd) {
            if (obj->field00 & 0x20000000) {
                u32 shape = (obj->shape << 2) | obj->size;
                copy((u32 *)(src + (*srcMap << srcShift)), (u32 *)((u8 *)dst + (*map << unitShift)), objTileCounts[shape] << tileShift, objDims[shape]);
                map += ownedStep;
            } else {
                map += sharedStep;
            }

            obj++;
            srcMap++;
        }
    } else {
        srcShift = ANIM_TBL(res, 0)->unitShift;

        if (unitShift == srcShift && linear == res->linear) {
            u32 size = *charData;
            DC_FlushRange(charData + 1, size);
            MI_DmaCopy32(GXi_DmaId, charData + 1, (u8 *)dst, size);
        } else {
            u16 * srcMap = ANIM_TBL_DATA(res, ANIM_TBL(res, 0));
            YanaResObj * obj = ANIM_ITEMS(res);
            allObjEnd = (YanaResObj *)srcMap;
            u32 tileShift = tileSizeShift[res->texFmt];
            CharCopyFunc copy;

            if (linear == res->linear) {
                copy = copyChars;
            } else {
                copy = charConvertFuncs[(linear << 2) | (res->linear << 1) | ((tileShift == 6) ? 1 : 0)];
            }

            srcBase = (u8 *)(charData + 1);
            while (obj != allObjEnd) {
                if (obj->field00 & 0x40000000) {
                    u32 shape = (obj->shape << 2) | obj->size;
                    copy((u32 *)(srcBase + (*srcMap << srcShift)), (u32 *)((u8 *)dst + (*map << unitShift)), objTileCounts[shape] << tileShift, objDims[shape]);
                    map += ownedStep;
                } else {
                    map += sharedStep;
                }

                obj++;
                srcMap++;
            }
        }
    }
}

namespace nsObjSys {

// MATCHED 100%
void clCellAnime::loadChars() {
    if (this->state != 0) {
        if (this->perFrame == 0) {
            this->charDirty = 0;
        }

        if (this->charHandle != NULL) {
            this->charHandle->queued = 0;
        }

        u32 dst;
        if (this->charData != NULL) {
            GXVRamTex savedBank = GX_VRAM_TEX_NONE;
            switch (this->mode) {
                case 0: {
                    dst = objVramBase[this->screen] + this->charOffset;
                    break;
                }
                case 1: {
                    GXVRamTex bank = GX_GetBankForTex();
                    u32 * addrs = texBankAddrs;

                    if (bank & GX_VRAM_A) {
                        *addrs++ = HW_LCDC_VRAM_A;
                    }
                    if (bank & GX_VRAM_B) {
                        *addrs++ = HW_LCDC_VRAM_B;
                    }
                    if (bank & GX_VRAM_C) {
                        *addrs++ = HW_LCDC_VRAM_C;
                    }
                    if (bank & GX_VRAM_D) {
                        *addrs++ = HW_LCDC_VRAM_D;
                    }

                    dst = texBankAddrs[this->charOffset >> 0x11] + (this->charOffset & 0x1ffff);
                    savedBank = GX_ResetBankForTex();
                    break;
                }
            }

            YanaRes * res = this->res;
            YanaResFrame * frame = ANIM_ENTRY(res, ANIM_REC(res, this->anim), this->frame);
            this->cell = frame->cell;

            u16 * map;
            if (this->charEntry == 0xff) {
                map = this->getCharMap();
            } else {
                map = ANIM_TBL_DATA(this->res, ANIM_TBL(this->res, this->charEntry));
                if (this->perFrame != 0) {
                    map += frame->cell;
                }
            }

            u32 unitShift = this->screen;
            if (this->mode == 0) {
                u16 mapShift = (*dispcntRegs[unitShift] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT;
                unitShift = mapShift + 5;
            } else {
                unitShift = 3;
            }

            u32 linear = (this->mode == 1);
            u32 numObj = (this->perFrame != 0) ? frame->numObj : 0;
            u8 remap = (this->charEntry == 0xff);
            transferChars(dst, linear, this->res, this->charData, map, unitShift, frame->cell, numObj, remap);

            if (savedBank != GX_VRAM_TEX_NONE) {
                GX_SetBankForTex(savedBank);
            }

            this->loadedCell = this->cell;
        }
    }
}

// MATCHED 100%
void clCellAnime::loadPltt() {
    if (this->state != 0) {
        u16 * src;
        if (this->plttBuffered == 0) {
            src = (u16 *)(this->plttData+1);
        } else {
            src = this->getPlttBuf();
        }

        this->plttBuffered = 0;

        if (this->plttHandle != NULL) {
            this->plttHandle->queued = 0;
        }

        if (this->plttData != NULL) {
            u32 size = this->getPlttSize();
            u16 savedBank = 0;
            u32 restoreKind = 0;
            u32 addr;

            switch (this->mode) {
                case 0: {
                    if (this->plttOffset / 16 == 16) {
                        addr = (this->plttOffset % 16) * 32 + objPlttBase[this->screen];
                    } else {
                        addr = this->plttOffset * 32;

                        u32 extBank = (this->screen == 0) ? GX_GetBankForOBJExtPltt() : GX_GetBankForSubOBJExtPltt();
                        switch (extBank) {
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
                            }
                        }

                        restoreKind = this->screen + 2;

                        if (this->screen == 0) {
                            savedBank = GX_ResetBankForOBJExtPltt();
                        } else {
                            savedBank = GX_ResetBankForSubOBJExtPltt();
                        }
                    }
                    break;
                }
                case 1: {
                    GXVRamTexPltt bank = GX_GetBankForTexPltt();
                    u32 * addrs = texPlttBankAddrs;
                    if (bank & GX_VRAM_E) {
                        addrs[0] = HW_LCDC_VRAM_E;
                        addrs[1] = HW_LCDC_VRAM_E + TEX_PLTT_SLOT_SIZE;
                        addrs[2] = HW_LCDC_VRAM_E + 2 * TEX_PLTT_SLOT_SIZE;
                        addrs[3] = HW_LCDC_VRAM_E + 3 * TEX_PLTT_SLOT_SIZE;
                        addrs += 4;
                    }
                    if (bank & GX_VRAM_F) {
                        *addrs++ = HW_LCDC_VRAM_F;
                    }
                    if (bank & GX_VRAM_G) {
                        *addrs++ = HW_LCDC_VRAM_G;
                    }

                    addr = texPlttBankAddrs[this->plttOffset / TEX_PLTT_SLOT_SIZE] + (this->plttOffset  % TEX_PLTT_SLOT_SIZE);
                    restoreKind = 1;
                    savedBank = GX_ResetBankForTexPltt();
                    break;
                }
            }

            u16 * dst = (u16 *)addr;
            if (src < dst) {
                func_01ff8380(src, dst, size);
            } else {
                func_01ff82ac(src, dst, size);
            }

            switch (restoreKind) {
                case 1: {
                    GX_SetBankForTexPltt((GXVRamTexPltt)savedBank);
                    break;
                }
                case 2: {
                    GX_SetBankForOBJExtPltt((GXVRamOBJExtPltt)savedBank);
                    break;
                }
                case 3: {
                    GX_SetBankForSubOBJExtPltt((GXVRamSubOBJExtPltt)savedBank);
                    break;
                }
            }

            this->plttDirty = 0;
        }
    }
}

// MATCHED 100%
u32 clCellAnime::getPlttSize() {
    VramHandle * handle = this->plttHandle;
    if (handle != NULL) {
        return (handle->manType == 1) ? handle->size * 32 : handle->size;
    } else {
        return (this->plttData != NULL) ? this->plttData[0] : getResPlttSize(this->res);
    }
}

// MATCHED 100%
u16 * clCellAnime::getPlttBuf() {
    u32 mode = this->mode;
    u16 * buf;
    u32 offset;

    if (mode == 0) {
        offset = this->plttOffset;
        u32 idx = offset;

        u16 * base;
        if ((offset >> 4) == 0x10) {
            base = objPlttBufs[this->screen].std;
            idx = offset & 0xf;
        } else {
            base = objPlttBufs[this->screen].ext;
        }

        if (base == NULL) {
            buf = NULL;
        } else {
            buf = base + idx * 16;
        }
    } else {
        offset = this->plttOffset;

        if (texPlttBuf == NULL) {
            buf = NULL;
        } else {
            buf = (u16 *)(texPlttBuf + offset);
        }
    }

    if (buf == NULL) {
        return NULL;
    }

    if (this->plttBuffered == 0) {
        BOOL needCopy = FALSE;
        VramHandle * handle = this->plttHandle;
        if (handle != NULL) {
            if (!handle->queued) {
                handle->queued = 1;
                needCopy = TRUE;
            }
        } else {
            clCellAnime * other = plttLoadQueue[mode];
            while (other != NULL) {
                if (other->plttOffset == offset) {
                    break;
                }
                other = other->nextPlttLoad;
            }

            if (other == NULL) {
                needCopy = TRUE;
            }
        }

        if (needCopy) {
            u32 size = this->getPlttSize();
            u16 * src = (u16 *)(this->plttData+1);
            if (src < buf) {
                func_01ff8380(src, buf, size);
            } else {
                func_01ff82ac(src, buf, size);
            }
            this->plttDirty = 1;
        }

        this->plttBuffered = 1;
    }

    return buf;
}

}

// MATCHED 100%
void flushCharLoads(u32 modeMask) {
    for (s32 mode = 0; mode < 2; mode++) {
        if (modeMask & (1 << mode)) {
            nsObjSys::clCellAnime * anime = charLoadQueue[mode];

            while (anime != NULL) {
                anime->loadChars();

                nsObjSys::clCellAnime * next = anime->nextCharLoad;
                anime->nextCharLoad = NULL;
                anime = next;
            }

            queueClear(charLoadQueue[mode]);
        }
    }
}

// MATCHED 100%
void flushPlttLoads(u32 modeMask) {
    for (s32 mode = 0; mode < 2; mode++) {
        if (modeMask & (1 << mode)) {
            nsObjSys::clCellAnime * anime = plttLoadQueue[mode];
            while (anime != NULL) {
                anime->loadPltt();

                nsObjSys::clCellAnime * next = anime->nextPlttLoad;
                anime->nextPlttLoad = NULL;
                anime = next;
            }

            queueClear(plttLoadQueue[mode]);
        }
    }
}

// MATCHED 100%
void cellAnimeUpdateAll() {
    nsObjSys::clCellAnime * anime = cellAnimeList.head;
    while (anime != NULL) {
        anime->update();
        anime = anime->next;
    }
}

// MATCHED 100%
void cellAnimeDrawAll(nsObjSys::clOamMan * oamMain, nsObjSys::clOamMan * oamSub, u32 packChars, u32 screenMask) {
    if (packChars == 0) {
        for (s32 screen = 0; screen <= 1; screen++) {
            if (screenMask & (1 << screen)) {
                nsObjSys::clCellAnime * anime = cellAnimeList.head;
                nsObjSys::clOamMan * oamMan = (screen == 0) ? oamMain : oamSub;

                while (anime != NULL) {
                    if (anime->screen == screen) {
                        anime->draw(oamMan);
                    }

                    anime = anime->next;
                }
            }
        }
    } else {
        for (s32 screen = 0; screen <= 1; screen++) {
            if (screenMask & (1 << screen)) {
                u32 offset = 0;
                nsObjSys::clCellAnime * anime = cellAnimeList.head;
                nsObjSys::clOamMan * oamMan = (screen == 0) ? oamMain : oamSub;
                
                while (anime != NULL) {
                    if (anime->screen == screen) {
                        if (anime->perFrame != 0) {
                            if (anime->charOffset != offset || anime->skipped) {
                                anime->loadedCell = -1;
                            }
                            anime->charOffset = offset;

                            u32 size = anime->draw(oamMan);
                            if (size != 0) {
                                offset += size;
                            } else {
                                anime->skipped = 1;
                            }
                        } else {
                            anime->draw(oamMan);
                        }
                    }

                    anime = anime->next;
                }
            }
        }
    }
}

// MATCHED 100%
void queuePush(nsObjSys::clCellAnime * anime, nsObjSys::clCellAnime * &head, nsObjSys::clCellAnime * &next, u32 unused) {
    if (next == NULL && head != anime) {
        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);
        next = head;
        head = anime;
        OS_EnableIrqMask(mask);
    }
}

// MATCHED 100%
void queueClear(nsObjSys::clCellAnime * &head) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);
    head = NULL;
    OS_EnableIrqMask(mask);
}

namespace nsObjSys {

// MATCHED 100%
void clCellAnime::link() {
    CellAnimeList * list = &cellAnimeList;
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);

    if (cellAnimeList.head == NULL) {
        this->next = NULL;
        this->prev = NULL;

        list->tail = this;
        list->head = this;

        OS_EnableIrqMask(mask);
    } else {
        if (this->mode == 1) {
            list->head->prev = this;

            this->next = list->head;
            this->prev = NULL;

            list->head = this;
        } else {
            list->tail->next = this;

            this->prev = list->tail;
            this->next = NULL;

            list->tail = this;
        }

        OS_EnableIrqMask(mask);
    }
}

// MATCHED 100%
void clCellAnime::unlink() {
    CellAnimeList * list = &cellAnimeList;
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);

    if (cellAnimeList.head == this) {
        list->head = this->next;
    }
    if (list->tail == this) {
        list->tail = this->prev;
    }

    if (this->next != NULL) {
        this->next->prev = this->prev;
    }
    if (this->prev != NULL) {
        this->prev->next = this->next;
    }

    this->prev = NULL;
    this->next = NULL;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
u16 * clCellAnime::getCharMap() {
    if (this->charEntry == 0xff) {
        u32 unitShift;
        u8 screen = this->screen;

        if (this->mode == 0) {
            u16 mapShift = (*dispcntRegs[screen] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT;
            unitShift = mapShift + 5;
        } else {
            unitShift = 3;
        }

        if (unitShift == 3) {
            unitShift = 4;
        }

        return charMapBufs[screen] + (this->charOffset >> unitShift) * 2;
    }

    return NULL;
}

}

// MATCHED 100%
void buildCharMap(YanaRes * res, u16 * map, u32 unitShift) {
    u16 * srcMap = ANIM_TBL_DATA(res, ANIM_TBL(res, 0));
    YanaResObj * obj = ANIM_ITEMS(res);
    YanaResObj * objEnd = (YanaResObj *)srcMap;
    u32 offset = 0;
    u32 unitMask = (1 << unitShift) - 1;
    u8 tileShift = tileSizeShift[res->texFmt];
    u32 alignMask = ~unitMask;

    while (obj != objEnd) {
        u32 bits = obj->field00;
        if (bits & 0x40000000) {
            map[0] = offset >> unitShift;
            map[1] = *srcMap;

            bits = (((bits >> 6) & 3) << 2) | ((bits >> 10) & 3);
            bits = objTileCounts[bits];
            offset += alignMask & (unitMask + (bits << tileShift));

            map += 2;
        }

        obj++;
        srcMap++;
    }
}

namespace nsObjSys {

// MATCHED 100%
BOOL clCellAnime::needsCharLoad() {
    if (this->charHandle != NULL) {
        return (this->charHandle->queued == 0);
    }

    if (this->perFrame != 0) {
        return TRUE;
    }

    clCellAnime * other = charLoadQueue[this->mode];
    u32 offset = this->charOffset;

    while (other != 0) {
        if (other->charOffset == offset) {
            return FALSE;
        }

        other = other->nextCharLoad;
    }

    return TRUE;
}

// MATCHED 100%
void clCellAnime::setAnim(s32 anim, s32 frame) {
    YanaRes * res;
    YanaResAnim * animRec;
    YanaResFrame * frames;

    this->anim = anim;

    if (frame == -1) {
        if (this->speed < 0) {
            res = this->res;
            animRec = ANIM_REC(res, anim);
            frame = animRec->count - 1;
        } else {
            frame = 0;
        }
    }

    this->frame = frame;

    res = this->res;
    animRec = ANIM_REC(res, anim);
    frames = ANIM_ENTRIES(res, animRec);

    s32 duration;
    if (frame > 0) {
        duration = frames[frame].time - frames[(u32)(frame-1)].time;
    } else {
        duration = frames[0].time;
    }

    this->frameTimer = duration << 8;

    if (this->speed < 0) {
        this->animTime = (frames[frame].time << 8) - 0x100;
    } else if (frame > 0) {
        this->animTime = frames[(u32)(frame-1)].time << 8;
    } else {
        this->animTime = 0;
    }

    res = this->res;
    animRec = ANIM_REC(res, this->anim);
    this->cell = ANIM_ENTRY(res, animRec, this->frame)->cell;
    this->ended = 0;
}

}

// MATCHED 100%
void nullCallback(u32 arg, nsObjSys::clCellAnime * anime) {}

// MATCHED 100%
void makeMtx(MtxFx44 * mtx, fx32 x, fx32 y, fx32 z, u32 flip, s32 scaleX, s32 scaleY, u16 angle) {
    MTX_Identity44(mtx);

    if (flip & 1) {
        scaleX *= -1;
    }

    if (flip & 2) {
        scaleY *= -1;
    }

    fx16 cos = FX_CosIdx(angle);
    fx16 sin = FX_SinIdx(angle);

    mtx->_00 = (scaleX * cos) >> FX32_SHIFT;
    mtx->_01 = (scaleX * sin) >> FX32_SHIFT;
    mtx->_10 = (-scaleY * sin) >> FX32_SHIFT;
    mtx->_11 = (scaleY * cos) >> FX32_SHIFT;
    mtx->_30 = x;
    mtx->_31 = y;
    mtx->_32 = z;
}

namespace nsObjSys {

// MATCHED 100%
void clCellAnime::setAllPriority(u32 priority) {
    for (s32 i = 0; i < 4; i++) {
        this->priority[i] = priority;
    }
}

// MATCHED 100%
void clCellAnime::setPriority(u32 idx, u8 priority) {
    this->priority[idx] = priority;
}

// MATCHED 100%
void clCellAnime::setDepth(fx32 depth) {
    this->mtx._32 = depth;
}

// MATCHED 100%
u16 clCellAnime::getNumFrames(s16 anim) {
    if (anim == -1) {
        anim = this->anim;
    }

    return ANIM_REC(this->res, anim)->count;
}

}
