// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clOfx.h"

extern "C" {

#include <nitro.h>

// --- Extern Data Variables --- //

extern RfxKeySeg keySeg;
extern RfxStack rfxStack;

extern MtxFx44 animeMtx;

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

// --- Function Declarations --- //

BOOL calcOamAffine(OfxOamParams *, RfxNode *, s16, s16, int, u8);  // 0x02026254
BOOL calcOamAffineTranspose(OfxOamParams *, RfxNode *, s16, s16, int, u8);  // 0x020264dc

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
s32 clOfx::getTexAddr(DfxTexHeader * hdr, RfxNode * node) {
    return -1;
}

// MATCHED 100%
s32 clOfx::getPlttAddr(DfxPlttHeader * pltt, RfxNode * node, BOOL bpp4) {
    return -1;
}

// MATCHED 100%
void clOfx::unk_virtual1(s32 t, u16 * cmd, u8 type) {
    switch (type) {
        case 7: {
            this->cmdSprite(t, cmd);
            break;
        }
        case 13: {
            this->cmdCellFrame(t, cmd);
            break;
        }
        case 14: {
            this->cmdDrawNumber(cmd);
            break;
        }
        case 10: {
            this->cmdNodeCallback(t, cmd);
            break;
        }
        case 11: {
            this->runColorBlendTrack(t, cmd);
            break;
        }
        case 0:
        case 1:
        case 2:
        case 8:
        case 39:
        case 43:
        case 44:
        case 45: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 3: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 4: {
            runSubTracks(t, cmd);
            break;
        }
        case 5: {
            runSubTrackSeg(t, cmd);
            break;
        }
        case 6: {
            runExtTracks(t, cmd);
            break;
        }
        case 64: {
            this->beginDraw();
            break;
        }
        case 65: {
            endDraw();
            break;
        }
    }
}

// MATCHED 100%
u32 clOfx::unk_virtual2() {
    return 0x44;
}

// MATCHED 100%
void clOfx::beginDraw() {
    RfxNode * node = rfxStack.cur;

    copy36(&this->rot, SDK_AUTOLOAD_DTCM_START);
    copy36(&this->rot, node);

    node->pos.x = 0;
    node->pos.y = 0;
    node->pos.z = 0;
    
    func_01ff8228(&node->ofx, 0, sizeof(OfxNodeParams));
}

// MATCHED 100%
void clOfx::endDraw() {}

// MATCHED 100%
void clOfx::cmdSprite(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        nsObjSys::clOamMan * oamMan = this->func_020224e4();
        if (oamMan != NULL) {
            RfxNode * node = &rfxStack.cur[1];
            copyNode(node - 1, node);
            
            s32 numRows = (cmd[0] >> 6) & 7;
            s32 numKeys = (cmd[0] >> 9) & 0x7f;

            node->pos.x = node->pos.x / 16 + this->originX;
            node->pos.y = node->pos.y / 16 + this->originY;

            for (s32 key = keySeg.key; key <= keySeg.nextKey; key++) {
                applyRows(node, cmd, numRows + 1, numKeys + 1, key, keySeg.t, keySeg.dur);

                if ((u32)(oamMan->field18 - oamMan->field04) < 0x80) {
                    if (calcOamAffine(&oamParams, node, node->ofx.sprite.x, node->ofx.sprite.y, node->ofx.sprite.shape, node->ofx.sprite.doubleSize != 0)) {
                        DfxTexHeader * tex = this->getTexHeader((u16)node->ofx.sprite.texIdx);
                        if (tex != NULL) {
                            DfxPlttHeader * pltt = this->getPlttHeader((u16)node->ofx.sprite.plttIdx);
                            if (pltt != NULL) {
                                s32 texAddr = this->getTexAddr(tex, node);
                                if (texAddr >= 0) {
                                    BOOL bpp4 = (tex->bpp == 4);
                                    s32 plttAddr = this->getPlttAddr(pltt, node, bpp4);
                                    if (plttAddr >= 0) {
                                        GXOamAttr * oam = oamMan->field18;
                                        s32 z = this->func_02023188(node->pos.z / 16 + node->ofx.sprite.zOffset);
                                        s32 baseZ = this->originZ;
                                        s32 affineIdx = oamMan->func_0202aaac(oamParams.pa, oamParams.pb, oamParams.pc, oamParams.pd);
                                        
                                        oamMan->func_0202a780(oamMan->field18 - oamMan->field04, 1, baseZ + z);
                                        G2_SetOBJAttr(oam, oamParams.x, oamParams.y, this->priority, GX_OAM_MODE_NORMAL, FALSE, oamParams.attr, oamShapes[node->ofx.sprite.shape], bpp4 ? GX_OAM_COLORMODE_16 : GX_OAM_COLORMODE_256, texAddr, plttAddr, affineIdx);
                                        oamMan->field18 = oam + 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// MATCHED 100%
BOOL calcOamAffine(OfxOamParams * out, RfxNode * node, s16 x, s16 y, int shape, u8 doubleSize) {
    s32 pa, pb, pc, pd;
    GXOamEffect effect;
    s32 w, h, cx, cy;
    s16 oamX, oamY;
    s32 m01, m00, m11, m10, det;

    m00 = node->mtx._00;
    m11 = node->mtx._11;
    m01 = node->mtx._01;
    m10 = node->mtx._10;
    det = (m00 * m11 - m01 * m10) / 4096;
    
    if (det == 0) {
        return FALSE;
    }

    pa = (m11 << 8) / det;
    pb = (-m10 << 8) / det;
    pc = (-m01 << 8) / det;
    pd = (m00 << 8) / det;

    if (pa > 0x7fff) pa = 0x7fff;
    if (pa < -0x7fff) pa = -0x7fff;

    if (pb > 0x7fff) pb = 0x7fff;
    if (pb < -0x7fff) pb = -0x7fff;

    if (pc > 0x7fff) pc = 0x7fff;
    if (pc < -0x7fff) pc = -0x7fff;

    if (pd > 0x7fff) pd = 0x7fff;
    if (pd < -0x7fff) pd = -0x7fff;

    if (pa == 0x100 && pb == 0 && pc == 0 && pd == 0x100) {
        effect = GX_OAM_EFFECT_NONE;
    } else if (pa == -0x100 && pb == 0 && pc == 0 && pd == 0x100) {
        effect = GX_OAM_EFFECT_FLIP_H;
    } else if (pa == 0x100 && pb == 0 && pc == 0 && pd == -0x100) {
        effect = GX_OAM_EFFECT_FLIP_V;
    } else if (pa == -0x100 && pb == 0 && pc == 0 && pd == -0x100) {
        effect = GX_OAM_EFFECT_FLIP_HV;
    } else {
        effect = GX_OAM_EFFECT_AFFINE;
    }

    w = oamShapeSizes[shape].w;
    h = oamShapeSizes[shape].h;

    cx = x + w / 2;
    cy = y + h / 2;

    if (effect == GX_OAM_EFFECT_AFFINE) {
        if (doubleSize) {
            w *= 2;
            h *= 2;
            effect = GX_OAM_EFFECT_AFFINE_DOUBLE;
        }
    }

    oamX = node->pos.x + (cx * m00 + cy * m01) / 4096 - w / 2;
    oamY = node->pos.y + (cx * m10 + cy * m11) / 4096 - h / 2;

    if (oamX + w < 0) {
        return FALSE;
    }
    if (oamX >= 0x100) {
        return FALSE;
    }
    if (oamY + h < 0) {
        return FALSE;
    }
    if (oamY >= 0xc0) {
        return FALSE;
    }

    out->pa = pa;
    out->pb = pb;
    out->pc = pc;
    out->pd = pd;
    out->x = oamX;
    out->y = oamY;
    out->attr = effect;

    return TRUE;
}

// MATCHED 100%
BOOL calcOamAffineTranspose(OfxOamParams * out, RfxNode * node, s16 x, s16 y, int shape, u8 doubleSize) {
    s32 pa, pb, pc, pd;
    GXOamEffect effect;
    s32 w, h, cx, cy;
    s16 oamX, oamY;
    s32 m10, m00, m11, m01, det;

    m00 = node->mtx._00;
    m11 = node->mtx._11;
    m01 = node->mtx._01;
    m10 = node->mtx._10;
    det = (m00 * m11 - m01 * m10) / 4096;
    
    if (det == 0) {
        return FALSE;
    }

    pa = (m11 << 8) / det;
    pb = (-m10 << 8) / det;
    pc = (-m01 << 8) / det;
    pd = (m00 << 8) / det;

    if (pa > 0x7fff) pa = 0x7fff;
    if (pa < -0x7fff) pa = -0x7fff;

    if (pb > 0x7fff) pb = 0x7fff;
    if (pb < -0x7fff) pb = -0x7fff;

    if (pc > 0x7fff) pc = 0x7fff;
    if (pc < -0x7fff) pc = -0x7fff;

    if (pd > 0x7fff) pd = 0x7fff;
    if (pd < -0x7fff) pd = -0x7fff;

    if (pa == 0x100 && pb == 0 && pc == 0 && pd == 0x100) {
        effect = GX_OAM_EFFECT_NONE;
    } else if (pa == -0x100 && pb == 0 && pc == 0 && pd == 0x100) {
        effect = GX_OAM_EFFECT_FLIP_H;
    } else if (pa == 0x100 && pb == 0 && pc == 0 && pd == -0x100) {
        effect = GX_OAM_EFFECT_FLIP_V;
    } else if (pa == -0x100 && pb == 0 && pc == 0 && pd == -0x100) {
        effect = GX_OAM_EFFECT_FLIP_HV;
    } else {
        effect = GX_OAM_EFFECT_AFFINE;
    }

    w = oamShapeSizes[shape].w;
    h = oamShapeSizes[shape].h;

    cx = x + w / 2;
    cy = y + h / 2;

    if (effect == GX_OAM_EFFECT_AFFINE) {
        if (doubleSize) {
            w *= 2;
            h *= 2;
            effect = GX_OAM_EFFECT_AFFINE_DOUBLE;
        }
    }

    oamX = node->pos.x + (cx * m00 + cy * m10) / 4096 - w / 2;
    oamY = node->pos.y + (cx * m01 + cy * m11) / 4096 - h / 2;

    if (oamX + w < 0) {
        return FALSE;
    }
    if (oamX >= 0x100) {
        return FALSE;
    }
    if (oamY + h < 0) {
        return FALSE;
    }
    if (oamY >= 0xc0) {
        return FALSE;
    }

    out->pa = pa;
    out->pb = pb;
    out->pc = pc;
    out->pd = pd;
    out->x = oamX;
    out->y = oamY;
    out->attr = effect;

    return TRUE;
}

// MATCHED 100%
void clOfx::cmdNodeCallback(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows(node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        node->pos.x = node->pos.x / 16 << FX32_SHIFT;
        node->pos.y = node->pos.y / 16 << FX32_SHIFT;
        node->pos.z = this->func_02023188(node->pos.z / 16);

        this->onNodePos(node);
    }
}

// MATCHED 100%
nsObjSys::clCellAnime * clOfx::beginCellAnime(s32 idx, RfxNode * node) {
    nsObjSys::clCellAnime * anime = this->func_020224ec(idx);

    if (anime == NULL) {
        return NULL;
    }

    node->pos.x = (node->pos.x / 16 + this->originX) << FX32_SHIFT;
    node->pos.y = (node->pos.y / 16 + this->originY) << FX32_SHIFT;
    node->pos.z = this->func_02023188(node->pos.z / 16) + this->originZ;

    copy64(&anime->field40, &animeMtx);

    MtxFx44 mtx;
    MTX_Copy43To44(&node->mtx, &mtx);
    MTX_Concat44(&anime->field40, &mtx, &anime->field40);

    if (anime->field82 == 0) {
        anime->field30 = -1;
    }
    anime->field82 = 1;

    return anime;
}

// MATCHED 100%
void clOfx::endCellAnime(nsObjSys::clCellAnime * anime) {
    copy64(&animeMtx, &anime->field40);
}

// MATCHED 100%
void clOfx::cmdCellFrame(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows(node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        this->setCellFrame(node->ofx.cell.cellIdx, node->ofx.cell.frame, node);
    }
}

// MATCHED 100%
void clOfx::cmdDrawNumber(u16 * cmd) {
    RfxNode * node = &rfxStack.cur[1];
    copyNode(node-1, node);
    this->drawNumber(cmd[3], cmd[5], node, cmd[7], cmd[9]);
}

// MATCHED 100%
GXRgb clOfx::getBlendColor() {
    RfxNode * node = rfxStack.cur;
    return (node->ofx.color.b << 10) | (node->ofx.color.g << 5) | node->ofx.color.r;
}

// MATCHED 100%
s32 clOfx::func_02023188(s32 z) {
    return z;
}
