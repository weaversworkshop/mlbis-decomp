// objdiff-target: build/delinks/_dsd_gap@main_7.o
// objdiff-target: build/delinks/_dsd_gap@dtcm_1.o

#include "globals.h"

#include "main/clDfx.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"
#include "main/vecmath.h"
#include "main/vecUtil.h"

// --- Variable Definitions --- //

static MtxFx33 lightMtx;  // 0x0205bb24
static MtxFx43 texMtx;  // 0x0205bb48

#include <nitro/dtcm_begin.h>
s32 polygonIdCounter = 1;  // 0x027e0024
VecFx32 lightDir = { -1, -2, -4 };  // 0x027e0028
s32 stripVtxCount;  // 0x027e1160
DfxTexInfo texInfo;  // 0x027e1164
#include <nitro/dtcm_end.h>

// --- Function Declarations --- //

void flushStrip(void);  // 0x02022bfc

s32 nextPolygonId(s32);  // 0x02022c64

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
s32 clDfx::getTexAddr(RfxTexHeader * hdr, DfxNode * node) {
    return -1;
}

// MATCHED 100%
s32 clDfx::getPlttAddr(RfxPlttHeader * pltt, DfxNode * node) {
    return -1;
}

// MATCHED 100%
void clDfx::unk_virtual1(s32 t, u16 * cmd, u8 type) {
    switch (type) {
        case 0: {
            this->cmdVertexStrip(t, cmd);
            break;
        }
        case 1:
        case 2:
        case 3:
        case 4: {
            this->cmdCircle(t, cmd);
            break;
        }
        case 5: {
            cmdTexQuads(cmd);
            break;
        }
        case 13: {
            this->cmdCellFrame(t, cmd);
            break;
        }
        case 14: {
            cmdDrawNumber(cmd);
            break;
        }
        case 6: {
            cmdSetTexParams(cmd);
            break;
        }
        case 7: {
            this->cmdTexMatrix(t, cmd);
            break;
        }
        case 8: {
            this->cmdTexStrip(cmd);
            break;
        }
        case 9: {
            this->cmdTexStrips(cmd);
            break;
        }
        case 12: {
            this->cmdLitMesh(cmd);
            break;
        }
        case 10: {
            this->cmdNodeCallback(t, cmd);
            break;
        }
        case 15: {
            this->func_020230e4(t, cmd);
            break;
        }
        case 11: {
            this->runColorBlendTrack(t, cmd);
            break;
        }
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 43:
        case 44:
        case 45: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 42: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 52: {
            this->runSubTracks(t, cmd);
            break;
        }
        case 54: {
            this->runSubTrackSeg(t, cmd);
            break;
        }
        case 55: {
            this->runExtTracks(t, cmd);
            break;
        }
        case 64: {
            this->beginDraw();
            break;
        }
        case 65: {
            this->endDraw();
            break;
        }
    }
}

// MATCHED 100%
u32 clDfx::unk_virtual2() {
    return sizeof(DfxNode);
}

// MATCHED 100%
void clDfx::beginDraw() {
    DfxNode * node = (DfxNode *)rfxStack.cur;

    copy36(&this->rot, &identityMtx);
    copy36(&this->rot, node);

    node->pos.x = this->originX * 16;
    node->pos.y = this->originY * 16;
    node->pos.z = this->originZ * 16;
    
    cpu_fill32(&node->mtx + 1, 0, sizeof(DfxNode) - sizeof(MtxFx43));

    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_Scale(0x10000, 0x10000, 0x10000);
    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_PushMtx();
    G3_Scale(0x2000, 0x2000, 0x100);
}

// MATCHED 100%
void clDfx::endDraw() {
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_PopMtx(1);
    G3_Identity();
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_PopMtx(1);

    flushStrip();
}

// MATCHED 100%
void flushStrip() {
    if (stripVtxCount != 0) {
        while (stripVtxCount < 3) {
            G3_Vtx(0, 0, 0);
            stripVtxCount++;
        }

        stripVtxCount = 0;

        G3_End();
        G3_PopMtx(1);
    }
}

// MATCHED 100%
s32 nextPolygonId(s32 alpha) {
    if (alpha == 0) {
        return 0;
    }

    if (++polygonIdCounter >= 32) {
        polygonIdCounter = 1;
    }
    return polygonIdCounter;
}

// MATCHED 100%
void clDfx::cmdNodeCallback(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        DfxNode * node = (DfxNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows((RfxNode *)node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        node->axisX.z /= 32;
        node->axisY.z /= 32;
        node->axisZ.z /= 32;

        node->pos.x = (node->pos.x / 16) << FX32_SHIFT;
        node->pos.y = (node->pos.y / 16) << FX32_SHIFT;
        node->pos.z /= 16;

        node->pos.x -= this->originX << FX32_SHIFT;
        node->pos.y -= this->originY << FX32_SHIFT;
        node->pos.z -= this->originZ;

        this->onNodePos((RfxNode *)node);
    }
}

// MATCHED 100%
nsObjSys::clCellAnime * clDfx::beginCellAnime(s32 idx, RfxNode * node) {
    s32 alpha = 31 - ((DfxNode *)node)->color.transparency;
    if (alpha < 0) {
        alpha = 0;
    }

    nsObjSys::clCellAnime * anime = this->func_020224ec(idx);
    if (anime == NULL) {
        return NULL;
    }

    flushStrip();

    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_PushMtx();
    G3_Scale(256, 256, 256);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_PushMtx();
    G3_Scale(2048, 2048, 2048);

    node->pos.x = (node->pos.x / 16) << FX32_SHIFT;
    node->pos.y = (node->pos.y / 16) << FX32_SHIFT;
    node->pos.z <<= 1;

    s32 polygonId = nextPolygonId(alpha);
    GXRgb color = (((DfxNode *)node)->color.b << 10) | (((DfxNode *)node)->color.g << 5) | ((DfxNode *)node)->color.r;

    anime->setAlpha(alpha);

    anime->polygonId = polygonId;
    anime->color = color;

    copy64(&anime->mtx, &animeMtx);

    MtxFx44 nodeMtx;
    MTX_Copy43To44((MtxFx43 *)node, &nodeMtx);
    MTX_Concat44((MtxFx44 *)&anime->mtx, &nodeMtx, (MtxFx44 *)&anime->mtx);

    if (anime->visible == 0) {
        anime->loadedCell = -1;
    }

    anime->visible = 1;

    return anime;
}

// MATCHED 100%
void clDfx::endCellAnime(nsObjSys::clCellAnime * anime) {
    anime->setAlpha(31);
    anime->polygonId = 0;
    anime->color = 0x7fff;

    copy64(&animeMtx, &anime->mtx);

    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_PopMtx(1);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_PopMtx(1);
}

// MATCHED 100%
void clDfx::cmdCellFrame(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        DfxNode * node = (DfxNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows((RfxNode *)node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        this->setCellFrame(node->cell.cellIdx, node->cell.frame, (RfxNode *)node);
    }
}

// MATCHED 100%
void clDfx::cmdDrawNumber(u16 * cmd) {
    DfxNode * node = (DfxNode *)&rfxStack.cur[1];
    copyNode((RfxNode *)node-1, (RfxNode *)node);
    this->drawNumber(cmd[3], cmd[5], (RfxNode *)node, cmd[7], cmd[9]);
}

// MATCHED 100%
void clDfx::func_020230e4(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        DfxNode * node = (DfxNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows((RfxNode *)node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        this->func_02023188(node->color.transparency);
    }
}

// MATCHED 100%
void clDfx::func_02023188(s16 transparency) {}

// MATCHED 100%
void clDfx::cmdVertexStrip(s32 t, u16 * cmd) {
    s32 numKeys;
    s32 numRows;

    if (findKeySeg(&keySeg, t, cmd)) {
        DfxNode * node = (DfxNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        numRows = (cmd[0] >> 6) & 7;
        numKeys = (cmd[0] >> 9) & 0x7f;

        s32 alpha = 31 - node->color.transparency;
        if (alpha >= 1) {
            s32 polygonId = nextPolygonId(node->color.transparency);
            s32 vtxCount = stripVtxCount;
            s32 key = keySeg.key;
            while (key <= keySeg.nextKey) {
                applyRows((RfxNode *)node, cmd, numRows+1, numKeys+1, key, keySeg.t, keySeg.dur);

                if (vtxCount == 0) {
                    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, 0);
                    G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
                    G3_PushMtx();

                    G3_MultMtx43(&node->mtx);

                    G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
                }

                GXRgb color = (node->color.b << 10) | (node->color.g << 5) | (node->color.r);
                G3_Color(color);
                G3_Vtx(node->vtx.x << 4, node->vtx.y << 4, node->vtx.z << 4);

                vtxCount++;
                if (node->vtx.breakStrip) {
                    while (vtxCount < 3) {
                        G3_Vtx(0, 0, 0);
                        vtxCount++;
                    }
                    G3_End();
                    vtxCount = 0;
                    G3_PopMtx(1);
                }

                key++;
            }

            stripVtxCount = vtxCount;
        }
    }
}

// MATCHED 100%
void clDfx::cmdCircle(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        DfxNode * node = (DfxNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        u16 shape = cmd[0] & 0x3f;
        applyRows((RfxNode *)node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        s32 alpha = 31 - node->color.transparency;
        if (alpha < 1) {
            return;
        }

        s32 polygonId = nextPolygonId(node->color.transparency);

        flushStrip();

        G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, 0);
        G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
        G3_PushMtx();

        G3_MultMtx43(&node->mtx);

        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);

        GXRgb outerColor = (node->color.b << 10) | (node->color.g << 5) | node->color.r;
        GXRgb innerColor = (node->circle.innerB << 10) | (node->circle.innerG << 5) | node->circle.innerR;

        s32 segments = node->circle.segments;
        if (segments < 3) {
            segments = 3;
        }

        s32 radiusA = node->circle.radiusA;
        s32 radiusB = node->circle.radiusB;
        s32 innerRadiusA = node->circle.innerRadiusA;
        s32 innerRadiusB = node->circle.innerRadiusB;

        switch (shape) {
            case 1: {
                for (s32 i = 0; i <= segments; i++) {
                    G3_Color(outerColor);

                    u16 angle = (i << 16) / segments;
                    G3_Vtx(radiusA * FX_CosIdx(angle) * 16 / 4096, radiusA * FX_SinIdx(angle) * 16 / 4096, 0);
                    G3_Color(innerColor);
                    G3_Vtx(0, 0, 0);
                }
                break;
            }
            case 2: {
                for (s32 i = 0; i <= segments; i += 2) {
                    G3_Color(outerColor);

                    u16 angle = (i << 16) / segments;
                    G3_Vtx(radiusA * FX_CosIdx(angle) * 16 / 4096, radiusA * FX_SinIdx(angle) * 16 / 4096, 0);
                    G3_Color(innerColor);
                    G3_Vtx(0, 0, 0);

                    if (segments < i + 1) {
                        break;
                    }

                    G3_Color(outerColor);

                    angle = ((i + 1) << 16) / segments;
                    G3_Vtx(radiusB * FX_CosIdx(angle) * 16 / 4096, radiusB * FX_SinIdx(angle) * 16 / 4096, 0);
                    G3_Color(innerColor);
                    G3_Vtx(0, 0, 0);
                }
                break;
            }
            case 3: {
                for (s32 i = 0; i <= segments; i++) {
                    G3_Color(outerColor);

                    u16 angle = (i << 16) / segments;
                    G3_Vtx(radiusA * FX_CosIdx(angle) * 16 / 4096, radiusA * FX_SinIdx(angle) * 16 / 4096, 0);
                    G3_Color(innerColor);
                    G3_Vtx(innerRadiusA * FX_CosIdx(angle) * 16 / 4096, innerRadiusA * FX_SinIdx(angle) * 16 / 4096, 0);
                }
                break;
            }
            case 4: {
                for (s32 i = 0; i <= segments; i += 2) {
                    G3_Color(outerColor);

                    u16 angle = (i << 16) / segments;
                    G3_Vtx(radiusA * FX_CosIdx(angle) * 16 / 4096, radiusA * FX_SinIdx(angle) * 16 / 4096, 0);
                    G3_Color(innerColor);
                    G3_Vtx(innerRadiusA * FX_CosIdx(angle) * 16 / 4096, innerRadiusA * FX_SinIdx(angle) * 16 / 4096, 0);

                    if (segments < i + 1) {
                        break;
                    }

                    G3_Color(outerColor);

                    angle = ((i + 1) << 16) / segments;
                    G3_Vtx(radiusB * FX_CosIdx(angle) * 16 / 4096, radiusB * FX_SinIdx(angle) * 16 / 4096, 0);
                    G3_Color(innerColor);
                    G3_Vtx(innerRadiusB * FX_CosIdx(angle) * 16 / 4096, innerRadiusB * FX_SinIdx(angle) * 16 / 4096, 0);
                }
                break;
            }
        }

        G3_End();
        G3_PopMtx(1);
    }
}

// MATCHED 100%
void clDfx::cmdTexQuads(u16 * cmd) {
    flushStrip();

    DfxNode * node = (DfxNode *)&rfxStack.cur[1];

    s32 keyBits = (cmd[0] >> 9);
    s32 rowBits = (cmd[0] >> 6);
    s32 numKeys = (keyBits & 0x7f);
    s32 numRows = (rowBits & 0x7);

    s32 count = numKeys+1;
    for (s32 key = 1; key <= count; key++) {
        copyNode((RfxNode *)node-1, (RfxNode *)node);
        applyRows((RfxNode *)node, cmd, numRows+1, count, key, 0, 0);

        this->drawTexQuad(cmd[key], node);
    }
}

// MATCHED 100%
BOOL clDfx::drawTexQuad(u16 texIdx, DfxNode * node) {
    s32 alpha = 31 - node->color.transparency;
    if (alpha < 1) {
        return TRUE;
    }

    DfxTexInfo * tex = this->resolveTexture(texIdx, node);
    if (tex == NULL) {
        return TRUE;
    }

    s32 polygonId = nextPolygonId(node->color.transparency);
    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, 0);
    G3_TexImageParam(tex->fmt, GX_TEXGEN_TEXCOORD, tex->sizeS, tex->sizeT, GX_TEXREPEAT_ST, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_TRNS, tex->texAddr);
    G3_TexPlttBase(tex->plttAddr, tex->fmt);

    node->pos.z += node->vtx.z * 16;

    G3_PushMtx();

    G3_MultMtx43(&node->mtx);

    G3_Begin(GX_BEGIN_QUADS);

    GXRgb color = (node->color.b << 10) | (node->color.g << 5) | (node->color.r);
    G3_Color(color);

    s32 s0 = tex->offsetU * 256;
    s32 t0 = tex->offsetV * 256;
    s32 x0 = node->vtx.x * 16;
    s32 y0 = node->vtx.y * 16;
    s32 t1 = t0 + (tex->height * 256);
    s32 s1 = s0 + (tex->width * 256);
    s32 x1 = x0 + (tex->width * 16);
    s32 y1 = y0 + (tex->height * 16);

    G3_TexCoord(s0, t0);
    G3_Vtx(x0, y0, 0);
    G3_TexCoord(s0, t1);
    G3_Vtx(x0, y1, 0);
    G3_TexCoord(s1, t1);
    G3_Vtx(x1, y1, 0);
    G3_TexCoord(s1, t0);
    G3_Vtx(x1, y0, 0);
    G3_End();
    G3_PopMtx(1);
    
    return FALSE;
}

// MATCHED 100%
void clDfx::cmdSetTexParams(u16 * cmd) {
    DfxNode * node = (DfxNode *)rfxStack.cur;
    node->tex.texIdx = cmd[1];
    node->tex.offsetU = cmd[3] * 8;
    node->tex.offsetV = cmd[5] * 8;
    node->tex.tileW = 8 << cmd[7];
    node->tex.tileH = 8 << cmd[9];
    node->tex.plttBlock = cmd[11];
    node->tex.cull = cmd[13];
    node->tex.triStrip = cmd[15];
    node->tex.cols = cmd[17];
}

// MATCHED 100%
void clDfx::cmdTexMatrix(s32 t, u16 * cmd) {
    flushStrip();
    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_PopMtx(1);
    G3_PushMtx();

    s32 elapsed = t - (s16)cmd[1] * 256;
    if (elapsed < 0) {
        G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
        return;
    }

    MTX_Identity43(&texMtx);

    s32 numRows = (cmd[0] >> 6) & 7;
    s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;

    s32 key = 1;
    s32 segDur = 0;

    while (TRUE) {
        if (++key > numKeys) {
            if (segDur != 0) {
                G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
                return;
            }

            applyRows((RfxNode *)&texMtx, cmd, numRows+1, numKeys, key-1, elapsed, segDur);
            break;
        }

        segDur = cmd[key];
        if (segDur != 0) {
            segDur *= 256;

            if (elapsed <= segDur) {
                applyRows((RfxNode *)&texMtx, cmd, numRows+1, numKeys, key-1, elapsed, segDur);
                break;
            }

            elapsed -= segDur;
        }
    }

    texMtx._30 *= 256;
    texMtx._31 *= 256;
    texMtx._32 *= 256;
    G3_MultMtx43(&texMtx);

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}

// MATCHED 100%
void clDfx::cmdTexStrip(u16 * cmd) {
    flushStrip();

    s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;
    if (numKeys >= 4) {
        DfxNode * node = (DfxNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        if (!this->setupTexture(node->tex.texIdx, node)) {
            node->pos.z += node->tex.texIdx * 16;

            G3_PushMtx();

            G3_MultMtx43(&node->mtx);

            GXRgb color = (node->color.b << 10) | (node->color.g << 5) | node->color.r;
            G3_Color(color);

            G3_Begin(node->tex.triStrip ? GX_BEGIN_TRIANGLE_STRIP : GX_BEGIN_QUAD_STRIP);

            s32 stride = numKeys + 1;
            s16 * px = (s16 *)&cmd[(s32)(stride * 1)] + 1;
            s16 * py = (s16 *)&cmd[(s32)(stride * 2)] + 1;
            s16 * pz = (s16 *)&cmd[(s32)(stride * 3)] + 1;
            s16 * ps = (s16 *)&cmd[(s32)(stride * 4)] + 1;
            s16 * pt = (s16 *)&cmd[(s32)(stride * 5)] + 1;

            s32 u0 = 0;
            s32 v0 = 0;
            s32 u1 = 0;
            s32 v1 = node->tex.tileH;

            for (s32 i = 0; i < numKeys / 2; i++) {
                s32 x0 = *px++;
                s32 y0 = *py++;
                s32 z0 = *pz++;

                u0 += *ps++;
                v0 += *pt++;

                G3_TexCoord(u0 * 256, v0 * 256);
                G3_Vtx(x0 * 16, y0 * 16, z0 * 16);

                s32 x1 = *px++;
                s32 y1 = *py++;
                s32 z1 = *pz++;

                u1 += *ps++;
                v1 += *pt++;

                G3_TexCoord(u1 * 256, v1 * 256);
                G3_Vtx(x1 * 16, y1 * 16, z1 * 16);

                u0 += node->tex.tileW;
                u1 += node->tex.tileW;
            }

            G3_End();
            G3_PopMtx(1);
        }
    }
}


// MATCHED 100%
void clDfx::cmdTexStrips(u16 * cmd) {
    flushStrip();

    s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;
    if (numKeys < 4) {
        return;
    }

    DfxNode * node = (DfxNode *)&rfxStack.cur[1];
    copyNode((RfxNode *)node-1, (RfxNode *)node);

    if (this->setupTexture(node->tex.texIdx, node)) {
        return;
    }

    node->pos.z += node->tex.texIdx * 16;

    G3_PushMtx();

    G3_MultMtx43(&node->mtx);

    GXRgb color = (node->color.b << 10) | (node->color.g << 5) | node->color.r;
    G3_Color(color);

    s32 stride = numKeys + 1;
    s16 * pBreak = (s16 *)cmd + 1;
    s16 * px = (s16 *)&cmd[(s32)(stride * 1)] + 1;
    s16 * py = (s16 *)&cmd[(s32)(stride * 2)] + 1;
    s16 * pz = (s16 *)&cmd[(s32)(stride * 3)] + 1;
    s16 * ps = (s16 *)&cmd[(s32)(stride * 4)] + 1;
    s16 * pt = (s16 *)&cmd[(s32)(stride * 5)] + 1;

    s32 segLen = 0;
    s32 pos = 0;
    s32 segIdx = 0;
    s32 vtxCount = 0;
    for (s32 i = 0; i < numKeys; i++) {
        if (*pBreak++ != 0) {
            if (vtxCount != 0) {
                if (vtxCount == 1) {
                    G3_Vtx(0, 0, 0);
                    G3_Vtx(0, 0, 0);
                }
                G3_End();
                vtxCount = 0;
            }
            segLen = pos;
            pos = 0;
            segIdx++;
        }

        pos++;

        if (segLen > 1 && pos <= segLen) {
            s32 x0 = px[-segLen];
            s32 y0 = py[-segLen];
            s32 z0 = pz[-segLen];
            s32 s0 = node->tex.tileW * pos + ps[-segLen];
            s32 t0 = node->tex.tileH * (segIdx - 1) + pt[-segLen];

            if (pos == 1) {
                G3_Begin(node->tex.triStrip ? GX_BEGIN_TRIANGLE_STRIP : GX_BEGIN_QUAD_STRIP);
            }
            G3_TexCoord(s0 * 256, t0 * 256);
            G3_Vtx(x0 * 16, y0 * 16, z0 * 16);

            vtxCount++;

            s32 x1 = *px++;
            s32 y1 = *py++;
            s32 z1 = *pz++;
            s32 s1 = node->tex.tileW * pos + *ps++;
            s32 t1 = node->tex.tileH * segIdx + *pt++;
            G3_TexCoord(s1 * 256, t1 * 256);
            G3_Vtx(x1 * 16, y1 * 16, z1 * 16);
        } else {
            px++;
            py++;
            pz++;
            ps++;
            pt++;
        }
    }

    if (vtxCount != 0) {
        if (vtxCount == 1) {
            G3_Vtx(0, 0, 0);
            G3_Vtx(0, 0, 0);
        }
        G3_End();
    }
    G3_PopMtx(1);
}


typedef struct {
    s32 t;  // +0x0
    s32 s;  // +0x4
} TexCoord;

// MATCHED 100%
void clDfx::cmdLitMesh(u16 * cmd) {
    flushStrip();

    s32 cr;
    DfxNode * node;
    s32 cols;
    s32 rows;
    s32 row;
    s32 col;
    s32 cg;
    s32 cb;
    s32 sA;
    s32 tA;
    s32 sB;
    s32 tB;
    s32 nxSq;
    s32 nySq;

    node = (DfxNode *)&rfxStack.cur[1];

    s32 numKeys = (cmd[0] >> 9) & 0x7f;
    copyNode((RfxNode *)node-1, (RfxNode *)node);
    if (this->setupTexture(node->tex.texIdx, node)) {
        return;
    }

    cols = node->tex.cols;
    if (cols > 11) {
        cols = 11;
    }

    rows = (numKeys + 1) / (cols + 1) - 1;
    if (rows <= 0) {
        return;
    }
    if (rows > 11) {
        rows = 11;
    }

    node->pos.z += node->tex.texIdx * 16;

    G3_PushMtx();

    G3_MultMtx43(&node->mtx);

    VecFx32 localLight;
    if (MTX_Inverse33((MtxFx33 *)node, &lightMtx)) {
        localLight.x = 0;
        localLight.y = 0;
        localLight.z = 0;
    } else {
        localLight.x = lightDir.x * lightMtx._00 + lightDir.y * lightMtx._10 + lightDir.z * lightMtx._20;
        localLight.y = lightDir.x * lightMtx._01 + lightDir.y * lightMtx._11 + lightDir.z * lightMtx._21;
        localLight.z = lightDir.x * lightMtx._02 + lightDir.y * lightMtx._12 + lightDir.z * lightMtx._22;
        vector_set_length(&localLight, FX32_ONE);
    }

    s32 stride = numKeys + 2;
    s16 * px = (s16 *)&cmd[(s32)(stride * 1)] + 1;
    s16 * py = (s16 *)&cmd[(s32)(stride * 2)] + 1;
    s16 * pz = (s16 *)&cmd[(s32)(stride * 3)] + 1;
    s16 * ps = (s16 *)&cmd[(s32)(stride * 4)] + 1;
    s16 * pt = (s16 *)&cmd[(s32)(stride * 5)] + 1;

    G3_Begin(GX_BEGIN_TRIANGLES);

    MtxFx43 tri;
    if (rows > 0) {
        int rowVtx = cols + 1;
        row = 0;
        do {
            s16 x10 = px[rowVtx];
            s16 y10 = py[rowVtx];
            s16 z10 = pz[rowVtx];
            s16 x00 = *px++;
            s16 y00 = *py++;
            s16 z00 = *pz++;

            TexCoord uv10;
            uv10.s = ps[rowVtx];
            uv10.t = pt[rowVtx];

            TexCoord uv00;
            uv00.s = *ps++;
            uv00.t = *pt++;
            
            for (col = 0; col < cols; col++) {
                s16 x11 = px[rowVtx];
                s16 y11 = py[rowVtx];
                s16 z11 = pz[rowVtx];
                s16 x01 = *px++;
                s16 y01 = *py++;
                s16 z01 = *pz++;

                TexCoord uv11;
                uv11.s = ps[rowVtx];
                uv11.t = pt[rowVtx];

                TexCoord uv01;
                uv01.s = *ps++;
                uv01.t = *pt++;

                tri._30 = x10 - (s32)x00;
                tri._31 = y10 - (s32)y00;
                tri._32 = z10 - z00;
                tri._20 = x01 - (s32)x10;
                tri._21 = y01 - (s32)y10;
                tri._22 = z01 - z10;
                tri._10 = x01 - (s32)x11;
                tri._11 = y01 - (s32)y11;
                tri._12 = z01 - z11;


                vec_cross_product((VecFx32 *)&tri._00, (VecFx32 *)&tri._30, (VecFx32 *)&tri._20);
                vector_set_length((VecFx32 *)&tri._00, FX32_ONE);
                fx32 cosine = vector_cosine((VecFx32 *)&tri._00, &localLight);
                if (cosine != VECTOR_COS_INVALID) {
                    s32 shade = (cosine + FX32_ONE) / 2;
                    cr = node->color.r * shade / 4096;
                    cg = node->color.g * shade / 4096;
                    cb = node->color.b * shade / 4096;

                    s32 sC, tC;
                    if (!node->tex.triStrip) {
                        sA = uv00.s + node->tex.tileW * col;
                        tA = uv00.t + node->tex.tileH * row;
                        sB = uv10.s + node->tex.tileW * col;
                        tB = uv10.t + node->tex.tileH * (row + 1);
                        sC = uv01.s + node->tex.tileW * (col + 1);
                        tC = uv01.t + node->tex.tileH * row;
                    } else {
                        nxSq = tri._00 * tri._00 / 4;
                        nySq = tri._01 * tri._01 / 4;

                        if (nxSq + nySq > 0) {
                            s32 nxy = tri._00 * tri._01 / 4;
                            sA = uv00.s + ((x00 * (nySq - (nxSq / 16) * tri._02 / 256) - y00 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z00 * tri._00 / 4096);
                            tA = uv00.t + ((y00 * (nxSq - (nySq / 16) * tri._02 / 256) - x00 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z00 * tri._01 / 4096);
                            sB = uv10.s + ((x10 * (nySq - (nxSq / 16) * tri._02 / 256) - y10 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z10 * tri._00 / 4096);
                            tB = uv10.t + ((y10 * (nxSq - (nySq / 16) * tri._02 / 256) - x10 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z10 * tri._01 / 4096);
                            sC = uv01.s + ((x01 * (nySq - (nxSq / 16) * tri._02 / 256) - y01 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z01 * tri._00 / 4096);
                            tC = uv01.t + ((y01 * (nxSq - (nySq / 16) * tri._02 / 256) - x01 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z01 * tri._01 / 4096);
                        } else {
                            sA = uv00.s + x00;
                            tA = uv00.t + y00;
                            sB = uv10.s + x10;
                            tB = uv10.t + y10;
                            sC = uv01.s + x01;
                            tC = uv01.t + y01;
                        }
                    }

                    GXRgb color = (cb << 10) | (cg << 5) | cr;
                    G3_Color(color);
                    G3_TexCoord(sA * 256, tA * 256);
                    G3_Vtx((s32)x00 * 16, (s32)y00 * 16, z00 * 16);
                    G3_TexCoord(sB * 256, tB * 256);
                    G3_Vtx((s32)x10 * 16, (s32)y10 * 16, z10 * 16);
                    G3_TexCoord(sC * 256, tC * 256);
                    G3_Vtx((s32)x01 * 16, (s32)y01 * 16, z01 * 16);
                }

                vec_cross_product((VecFx32 *)&tri._00, (VecFx32 *)&tri._20, (VecFx32 *)&tri._10);
                vector_set_length((VecFx32 *)&tri._00, FX32_ONE);
                cosine = vector_cosine((VecFx32 *)&tri._00, &localLight);
                if (cosine != VECTOR_COS_INVALID) {
                    s32 cg;
                    s32 cb;
                    s32 sA;
                    s32 tA;
                    s32 sB;
                    s32 tB;
                    s32 nxSq;
                    s32 nySq;
                    s32 shade = (cosine + FX32_ONE) / 2;
                    cr = node->color.r * shade / 4096;
                    cg = node->color.g * shade / 4096;
                    cb = node->color.b * shade / 4096;

                    s32 sC, tC;
                    if (!node->tex.triStrip) {
                        sA = uv01.s + node->tex.tileW * (col + 1);
                        tA = uv01.t + node->tex.tileH * row;
                        sB = uv10.s + node->tex.tileW * col;
                        tB = uv10.t + node->tex.tileH * (row + 1);
                        sC = uv11.s + node->tex.tileW * (col + 1);
                        tC = uv11.t + node->tex.tileH * (row + 1);
                    } else {
                        nxSq = tri._00 * tri._00 / 4;
                        nySq = tri._01 * tri._01 / 4;

                        if (nxSq + nySq > 0) {
                            s32 nxy = tri._00 * tri._01 / 4;
                            sA = uv01.s + ((x01 * (nySq - (nxSq / 16) * tri._02 / 256) - y01 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z01 * tri._00 / 4096);
                            tA = uv01.t + ((y01 * (nxSq - (nySq / 16) * tri._02 / 256) - x01 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z01 * tri._01 / 4096);
                            sB = uv10.s + ((x10 * (nySq - (nxSq / 16) * tri._02 / 256) - y10 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z10 * tri._00 / 4096);
                            tB = uv10.t + ((y10 * (nxSq - (nySq / 16) * tri._02 / 256) - x10 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z10 * tri._01 / 4096);
                            sC = uv11.s + ((x11 * (nySq - (nxSq / 16) * tri._02 / 256) - y11 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z11 * tri._00 / 4096);
                            tC = uv11.t + ((y11 * (nxSq - (nySq / 16) * tri._02 / 256) - x11 * (nxy + (nxy / 16) * tri._02 / 256)) / (nxSq + nySq) + z11 * tri._01 / 4096);
                        } else {
                            sA = uv01.s + x01;
                            tA = uv01.t + y01;
                            sB = uv10.s + x10;
                            tB = uv10.t + y10;
                            sC = uv11.s + x11;
                            tC = uv11.t + y11;
                        }
                    }

                    GXRgb color = (cb << 10) | (cg << 5) | cr;
                    G3_Color(color);
                    G3_TexCoord(sA * 256, tA * 256);
                    G3_Vtx((s32)x01 * 16, (s32)y01 * 16, z01 * 16);
                    G3_TexCoord(sB * 256, tB * 256);
                    G3_Vtx((s32)x10 * 16, (s32)y10 * 16, z10 * 16);
                    G3_TexCoord(sC * 256, tC * 256);
                    G3_Vtx((s32)x11 * 16, (s32)y11 * 16, z11 * 16);
                }

                x10 = x11;
                y10 = y11;
                z10 = z11;
                x00 = x01;
                y00 = y01;
                z00 = z01;
                uv10.s = uv11.s;
                uv10.t = uv11.t;
                uv00.s = uv01.s;
                uv00.t = uv01.t;
            }
        } while (++row < rows);
    }

    G3_End();
    G3_PopMtx(1);
}

// MATCHED 100%
BOOL clDfx::setupTexture(u16 texIdx, DfxNode * node) {
    s32 alpha = 31 - node->color.transparency;
    if (alpha < 1) {
        return TRUE;
    }

    DfxTexInfo * tex = this->resolveTexture(texIdx, node);
    if (tex == NULL) {
        return TRUE;
    }

    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, (GXCull)(3 - node->tex.cull), nextPolygonId(node->color.transparency), alpha, 0);
    G3_TexImageParam(tex->fmt, GX_TEXGEN_TEXCOORD, tex->sizeS, tex->sizeT, GX_TEXREPEAT_ST, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_TRNS, tex->texAddr);
    G3_TexPlttBase(tex->plttAddr, tex->fmt);

    return FALSE;
}

// MATCHED 100%
DfxTexInfo * clDfx::resolveTexture(u16 texIdx, DfxNode * node) {
    RfxTexHeader * hdr = this->getTexHeader(texIdx);
    if (hdr == NULL) {
        return NULL;
    }

    RfxPlttHeader * pltt = this->getPlttHeader(texIdx);
    if (pltt == NULL) {
        return NULL;
    }

    s16 offsetU = node->tex.offsetU;
    s16 offsetV = node->tex.offsetV;
    s32 width = hdr->wBlocks * 8 - offsetU;
    s32 height = hdr->hBlocks * 8 - offsetV;

    if (width <= 0) {
        return NULL;
    }
    if (height <= 0) {
        return NULL;
    }

    if (node->tex.tileW < width) {
        width = node->tex.tileW;
    }
    if (node->tex.tileH < height) {
        height = node->tex.tileH;
    }

    DfxTexInfo * info = &texInfo;
    info->fmt = (GXTexFmt)hdr->fmt;
    info->offsetU = offsetU & 7;
    info->offsetV = offsetV & 7;
    info->sizeS = (GXTexSizeS)(32 - MATH_CLZ((width + info->offsetU - 1) / 8));
    info->sizeT = (GXTexSizeT)(32 - MATH_CLZ((height + info->offsetV - 1) / 8));
    info->width = width;
    info->height = height;

    s32 texAddr = this->getTexAddr(hdr, node);
    if (texAddr < 0) {
        return NULL;
    }

    s32 plttAddr = this->getPlttAddr(pltt, node);
    if (plttAddr < 0) {
        return NULL;
    }

    info->plttAddr = plttAddr;
    info->texAddr = texAddr;
    return info;
}

// MATCHED 100%
GXRgb clDfx::getBlendColor() {
    DfxNode * node = (DfxNode *)rfxStack.cur;
    return (node->color.b << 10) | (node->color.g << 5) | (node->color.r);
}
