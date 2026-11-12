// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clCellAnimeML3.h"

#include "main/cellObjSize.h"

extern "C" {

#include <nitro.h>

// --- Extern Data Variables --- //

extern const u8 texPlttBaseShift[8];
extern const u8 texPlttSizeShift[8];

} // extern "C"

// --- Function Definitions --- //

namespace nsObjSys {

// MATCHED 100%
u32 clCellAnimeML3::draw3d() {
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
    BOOL cellChanged;
    u32 cell;
    u16 * map;
    u16 * charMap;
    YanaResObj * obj;
    MtxFx43 mtx;

    if (this->state == 0) {
        return 0;
    }

    if (this->visible == 0 || this->alpha == 0) {
        this->plttBuffered = 0;
        return 0;
    }

    G3_MtxMode(GX_MTXMODE_POSITION);

    u32 polygonId = this->polygonId;
    u32 alpha = this->alpha;
    reg_G3_POLYGON_ATTR = GX_PACK_POLYGONATTR_PARAM(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, this->getPolygonAttrMisc());
    G3_Color(this->color);

    this->getDrawMtx(&mtx);

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
    mtxTable = (YanaResMtx *)ANIM_PTR(res, res->mtxOffset);
    objEnd = obj + frame->numObj;

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
        MTX_Concat43(&xformMtx, &mtx, &mtx);
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
        u32 flip = 0;
        MtxFx43 objMtx;

        G3_PushMtx();

        u32 bits = obj->field00;
        s16 x = obj->x;
        u32 shape = (((bits >> 6) & 3) << 2) | ((bits >> 10) & 3);
        s16 y = obj->y;

        if (bits & 0x20000000) {
            charSize += objTileCounts[shape] << tileShift;
        }

        MTX_TransApply43(&mtx, &objMtx, x * 32, y * 32, 0);
        objMtx._30 &= ~0x1f;
        objMtx._31 &= ~0x1f;
        objMtx._32 += depths[(bits >> 12) & 3];

        depths[(bits >> 12) & 3] += depthStep;

        if (bits & 1) {
            MtxFx33 rot;
            rot._00 = mtxTable[(bits >> 18) & 0x7ff].m00 * 16;
            rot._10 = mtxTable[(bits >> 18) & 0x7ff].m10 * 16;
            rot._01 = mtxTable[(bits >> 18) & 0x7ff].m01 * 16;
            rot._11 = mtxTable[(bits >> 18) & 0x7ff].m11 * 16;
            rot._21 = 0;
            rot._20 = 0;
            rot._12 = 0;
            rot._02 = 0;
            rot._22 = FX32_ONE;
            MTX_Concat33(&rot, (MtxFx33 *)&objMtx, (MtxFx33 *)&objMtx);
        } else {
            switch (bits & 0x300) {
                case 0x100: {
                    objMtx._00 = -objMtx._00;
                    objMtx._01 = -objMtx._01;
                    objMtx._02 = -objMtx._02;
                    break;
                }
                case 0x200: {
                    objMtx._10 = -objMtx._10;
                    objMtx._11 = -objMtx._11;
                    objMtx._12 = -objMtx._12;
                    break;
                }
                case 0x300: {
                    objMtx._00 = -objMtx._00;
                    objMtx._01 = -objMtx._01;
                    objMtx._02 = -objMtx._02;
                    objMtx._10 = -objMtx._10;
                    objMtx._11 = -objMtx._11;
                    objMtx._12 = -objMtx._12;
                    break;
                }
            }
        }

        if (objMtx._00 < 0 || objMtx._01 < 0) {
            flip ^= 1;
        }
        if (objMtx._10 < 0 || objMtx._11 < 0) {
            flip ^= 2;
        }

        G3_MultMtx43(&objMtx);

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
u32 clCellAnimeML3::getPolygonAttrMisc() {
    return 0;
}

// MATCHED 100%
void clCellAnimeML3::getDrawMtx(MtxFx43 * dst) {
    MTX_Copy44To33(&this->mtx, (MtxFx33 *)dst);
    dst->_30 = this->mtx._30 >> 7;
    dst->_31 = this->mtx._31 >> 7;
    dst->_32 = this->mtx._32;
}

// MATCHED 100%
void clCellAnimeML3::update() {
    this->prevEnded = this->ended;
    this->clCellAnime::update();
}

// MATCHED 100%
clCellAnimeML3::~clCellAnimeML3() {}

}
