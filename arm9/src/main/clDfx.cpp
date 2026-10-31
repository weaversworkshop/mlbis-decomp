// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clDfx.h"

extern "C" {

#include <nitro.h>

#include "main/vecmath.h"
#include "main/vecUtil.h"

// --- Extern Data Variables --- //

extern s32 polygonIdCounter;  // 0x027e0024
extern VecFx32 lightDir;  // 0x027e0028

extern RfxKeySeg keySeg;  // 0x027e00ec
extern RfxStack rfxStack;

extern MtxFx44 animeMtx;  // 0x027e1120

extern s32 stripVtxCount;  // 0x027e1160
extern DfxTexInfo texInfo;  // 0x027e1164

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

extern void func_02036b28(RfxNode *);

// --- Variable Definitions --- //

static MtxFx33 lightMtx;  // 0x0205bb24
static MtxFx43 texMtx;  // 0x0205bb48

// --- Function Declarations --- //

void flushStrip(void);  // 0x02022bfc

s32 nextPolygonId(s32);  // 0x02022c64

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
s32 clDfx::getTexAddr(RfxTexHeader * hdr, RfxNode * node) {
    return -1;
}

// MATCHED 100%
s32 clDfx::getPlttAddr(RfxPlttHeader * pltt, RfxNode * node) {
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
            endDraw();
            break;
        }
    }
}

// MATCHED 100%
u32 clDfx::unk_virtual2() {
    return 0x48;
}

// MATCHED 100%
void clDfx::beginDraw() {
    RfxNode * node = rfxStack.cur;

    copy36(&this->rot, SDK_AUTOLOAD_DTCM_START);
    copy36(&this->rot, node);

    node->pos.x = this->originX * 16;
    node->pos.y = this->originY * 16;
    node->pos.z = this->originZ * 16;
    
    func_01ff8228(&node->dfx, 0, sizeof(DfxNodeParams));

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
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows(node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        node->axisX.z /= 32;
        node->axisY.z /= 32;
        node->axisZ.z /= 32;

        node->pos.x = (node->pos.x / 16) << FX32_SHIFT;
        node->pos.y = (node->pos.y / 16) << FX32_SHIFT;
        node->pos.z /= 16;

        node->pos.x -= this->originX << FX32_SHIFT;
        node->pos.y -= this->originY << FX32_SHIFT;
        node->pos.z -= this->originZ;

        this->onNodePos(node);
    }
}

// MATCHED 100%
nsObjSys::clCellAnime * clDfx::beginCellAnime(s32 idx, RfxNode * node) {
    s32 alpha = 31 - node->dfx.color.transparency;
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
    GXRgb color = (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | node->dfx.color.r;

    anime->func_02022f38(alpha);

    anime->field99 = polygonId;
    anime->field9a = color;

    copy64(&anime->field40, &animeMtx);

    MtxFx44 nodeMtx;
    MTX_Copy43To44((MtxFx43 *)node, &nodeMtx);
    MTX_Concat44((MtxFx44 *)&anime->field40, &nodeMtx, (MtxFx44 *)&anime->field40);

    if (anime->field82 == 0) {
        anime->field30 = -1;
    }

    anime->field82 = 1;

    return anime;
}

// MATCHED 100%
void clDfx::endCellAnime(nsObjSys::clCellAnime * anime) {
    anime->func_02022f38(31);
    anime->field99 = 0;
    anime->field9a = 0x7fff;

    copy64(&animeMtx, &anime->field40);

    G3_MtxMode(GX_MTXMODE_TEXTURE);
    G3_PopMtx(1);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_PopMtx(1);
}

// MATCHED 100%
void clDfx::cmdCellFrame(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows(node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        this->setCellFrame(node->dfx.cell.cellIdx, node->dfx.cell.frame, node);
    }
}

// MATCHED 100%
void clDfx::cmdDrawNumber(u16 * cmd) {
    RfxNode * node = &rfxStack.cur[1];
    copyNode(node-1, node);
    this->drawNumber(cmd[3], cmd[5], node, cmd[7], cmd[9]);
}

// MATCHED 100%
void clDfx::func_020230e4(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows(node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        this->func_02023188(node->dfx.color.transparency);
    }
}

// MATCHED 100%
void clDfx::func_02023188(s16 transparency) {}

// MATCHED 100%
void clDfx::cmdVertexStrip(s32 t, u16 * cmd) {
    s32 numKeys;
    s32 numRows;

    if (findKeySeg(&keySeg, t, cmd)) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        numRows = (cmd[0] >> 6) & 7;
        numKeys = (cmd[0] >> 9) & 0x7f;

        s32 alpha = 31 - node->dfx.color.transparency;
        if (alpha >= 1) {
            s32 polygonId = nextPolygonId(node->dfx.color.transparency);
            s32 vtxCount = stripVtxCount;
            s32 key = keySeg.key;
            while (key <= keySeg.nextKey) {
                applyRows(node, cmd, numRows+1, numKeys+1, key, keySeg.t, keySeg.dur);

                if (vtxCount == 0) {
                    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, 0);
                    G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
                    G3_PushMtx();

                    func_02036b28(node);

                    G3_Begin(GX_BEGIN_TRIANGLE_STRIP);
                }

                GXRgb color = (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | (node->dfx.color.r);
                G3_Color(color);
                G3_Vtx(node->dfx.vtx.x << 4, node->dfx.vtx.y << 4, node->dfx.vtx.z << 4);

                vtxCount++;
                if (node->dfx.vtx.breakStrip) {
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
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        u16 shape = cmd[0] & 0x3f;
        applyRows(node, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        s32 alpha = 31 - node->dfx.color.transparency;
        if (alpha < 1) {
            return;
        }

        s32 polygonId = nextPolygonId(node->dfx.color.transparency);

        flushStrip();

        G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, 0);
        G3_TexImageParam(GX_TEXFMT_NONE, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, 0);
        G3_PushMtx();

        func_02036b28(node);

        G3_Begin(GX_BEGIN_TRIANGLE_STRIP);

        GXRgb outerColor = (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | node->dfx.color.r;
        GXRgb innerColor = (node->dfx.circle.innerB << 10) | (node->dfx.circle.innerG << 5) | node->dfx.circle.innerR;

        s32 segments = node->dfx.circle.segments;
        if (segments < 3) {
            segments = 3;
        }

        s32 radiusA = node->dfx.circle.radiusA;
        s32 radiusB = node->dfx.circle.radiusB;
        s32 innerRadiusA = node->dfx.circle.innerRadiusA;
        s32 innerRadiusB = node->dfx.circle.innerRadiusB;

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

    RfxNode * node = &rfxStack.cur[1];

    s32 keyBits = (cmd[0] >> 9);
    s32 rowBits = (cmd[0] >> 6);
    s32 numKeys = (keyBits & 0x7f);
    s32 numRows = (rowBits & 0x7);

    s32 count = numKeys+1;
    for (s32 key = 1; key <= count; key++) {
        copyNode(node-1, node);
        applyRows(node, cmd, numRows+1, count, key, 0, 0);

        this->drawTexQuad(cmd[key], node);
    }
}

// MATCHED 100%
BOOL clDfx::drawTexQuad(u16 texIdx, RfxNode * node) {
    s32 alpha = 31 - node->dfx.color.transparency;
    if (alpha < 1) {
        return TRUE;
    }

    DfxTexInfo * tex = this->resolveTexture(texIdx, node);
    if (tex == NULL) {
        return TRUE;
    }

    s32 polygonId = nextPolygonId(node->dfx.color.transparency);
    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, polygonId, alpha, 0);
    G3_TexImageParam(tex->fmt, GX_TEXGEN_TEXCOORD, tex->sizeS, tex->sizeT, GX_TEXREPEAT_ST, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_TRNS, tex->texAddr);
    G3_TexPlttBase(tex->plttAddr, tex->fmt);

    node->pos.z += node->dfx.vtx.z * 16;

    G3_PushMtx();

    func_02036b28(node);

    G3_Begin(GX_BEGIN_QUADS);

    GXRgb color = (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | (node->dfx.color.r);
    G3_Color(color);

    s32 s0 = tex->offsetU * 256;
    s32 t0 = tex->offsetV * 256;
    s32 x0 = node->dfx.vtx.x * 16;
    s32 y0 = node->dfx.vtx.y * 16;
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
    RfxNode * node = rfxStack.cur;
    node->dfx.tex.texIdx = cmd[1];
    node->dfx.tex.offsetU = cmd[3] * 8;
    node->dfx.tex.offsetV = cmd[5] * 8;
    node->dfx.tex.tileW = 8 << cmd[7];
    node->dfx.tex.tileH = 8 << cmd[9];
    node->dfx.tex.plttBlock = cmd[11];
    node->dfx.tex.cull = cmd[13];
    node->dfx.tex.triStrip = cmd[15];
    node->dfx.tex.cols = cmd[17];
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
    func_02036b28((RfxNode *)&texMtx);

    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
}

// MATCHED 100%
void clDfx::cmdTexStrip(u16 * cmd) {
    flushStrip();

    s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;
    if (numKeys >= 4) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        if (!this->setupTexture(node->dfx.tex.texIdx, node)) {
            node->pos.z += node->dfx.tex.texIdx * 16;

            G3_PushMtx();

            func_02036b28(node);

            GXRgb color = (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | node->dfx.color.r;
            G3_Color(color);

            G3_Begin(node->dfx.tex.triStrip ? GX_BEGIN_TRIANGLE_STRIP : GX_BEGIN_QUAD_STRIP);

            s32 stride = numKeys + 1;
            s16 * px = (s16 *)&cmd[(s32)(stride * 1)] + 1;
            s16 * py = (s16 *)&cmd[(s32)(stride * 2)] + 1;
            s16 * pz = (s16 *)&cmd[(s32)(stride * 3)] + 1;
            s16 * ps = (s16 *)&cmd[(s32)(stride * 4)] + 1;
            s16 * pt = (s16 *)&cmd[(s32)(stride * 5)] + 1;

            s32 u0 = 0;
            s32 v0 = 0;
            s32 u1 = 0;
            s32 v1 = node->dfx.tex.tileH;

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

                u0 += node->dfx.tex.tileW;
                u1 += node->dfx.tex.tileW;
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

    RfxNode * node = &rfxStack.cur[1];
    copyNode(node-1, node);

    if (this->setupTexture(node->dfx.tex.texIdx, node)) {
        return;
    }

    node->pos.z += node->dfx.tex.texIdx * 16;

    G3_PushMtx();

    func_02036b28(node);

    GXRgb color = (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | node->dfx.color.r;
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
            s32 s0 = node->dfx.tex.tileW * pos + ps[-segLen];
            s32 t0 = node->dfx.tex.tileH * (segIdx - 1) + pt[-segLen];

            if (pos == 1) {
                G3_Begin(node->dfx.tex.triStrip ? GX_BEGIN_TRIANGLE_STRIP : GX_BEGIN_QUAD_STRIP);
            }
            G3_TexCoord(s0 * 256, t0 * 256);
            G3_Vtx(x0 * 16, y0 * 16, z0 * 16);

            vtxCount++;

            s32 x1 = *px++;
            s32 y1 = *py++;
            s32 z1 = *pz++;
            s32 s1 = node->dfx.tex.tileW * pos + *ps++;
            s32 t1 = node->dfx.tex.tileH * segIdx + *pt++;
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

// NONMATCHING 99.73% -- spill stack order is scrambled
void clDfx::cmdLitMesh(u16 * cmd) {
    flushStrip();

    s32 cr;
    RfxNode * node;
    s32 cols;
    s32 rows;
    s32 r;
    s32 c;
    s32 cg;
    s32 cb;
    s32 sA;
    s32 tA;
    s32 sB;
    s32 tB;
    s32 nx2;
    s32 ny2;

    node = &rfxStack.cur[1];

    s32 numKeys = (cmd[0] >> 9) & 0x7f;
    copyNode(node-1, node);
    if (this->setupTexture(node->dfx.tex.texIdx, node)) {
        return;
    }

    cols = node->dfx.tex.cols;
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

    node->pos.z += node->dfx.tex.texIdx * 16;

    G3_PushMtx();

    func_02036b28(node);

    VecFx32 light;
    if (MTX_Inverse33((MtxFx33 *)node, &lightMtx)) {
        light.x = 0;
        light.y = 0;
        light.z = 0;
    } else {
        light.x = lightDir.x * lightMtx._00 + lightDir.y * lightMtx._10 + lightDir.z * lightMtx._20;
        light.y = lightDir.x * lightMtx._01 + lightDir.y * lightMtx._11 + lightDir.z * lightMtx._21;
        light.z = lightDir.x * lightMtx._02 + lightDir.y * lightMtx._12 + lightDir.z * lightMtx._22;
        vector_set_length(&light, FX32_ONE);
    }

    s32 stride = numKeys + 2;
    s16 * px = (s16 *)&cmd[(s32)(stride * 1)] + 1;
    s16 * py = (s16 *)&cmd[(s32)(stride * 2)] + 1;
    s16 * pz = (s16 *)&cmd[(s32)(stride * 3)] + 1;
    s16 * ps = (s16 *)&cmd[(s32)(stride * 4)] + 1;
    s16 * pt = (s16 *)&cmd[(s32)(stride * 5)] + 1;

    G3_Begin(GX_BEGIN_TRIANGLES);

    MtxFx43 m;
    if (rows > 0) {
        s32 w = cols + 1;
        r = 0;
        do {
            s16 x10 = px[w];
            s16 y10 = py[w];
            s32 z10 = pz[w];
            s16 x00 = *px++;
            s16 y00 = *py++;
            s32 z00 = *pz++;

            TexCoord c10;
            c10.s = ps[w];
            c10.t = pt[w];

            TexCoord c00;
            c00.s = *ps++;
            c00.t = *pt++;
            
            for (c = 0; c < cols; c++) {
                s16 x11 = px[w];
                s16 y11 = py[w];
                s32 z11 = pz[w];
                s16 x01 = *px++;
                s16 y01 = *py++;
                s32 z01 = *pz++;

                TexCoord c11;
                c11.s = ps[w];
                c11.t = pt[w];

                TexCoord c01;
                c01.s = *ps++;
                c01.t = *pt++;

                m._30 = x10 - x00;
                m._31 = y10 - y00;
                m._32 = z10 - z00;
                m._21 = y01 - y10;
                m._20 = x01 - x10;
                m._22 = z01 - z10;
                m._11 = y01 - y11;
                m._10 = x01 - x11;
                m._12 = z01 - z11;

    #define PROJ_S(SC, X, Y, Z) ((SC) + (((X) * (ny2 - (nx2 / 16) * m._02 / 256) - (Y) * k) / (nx2 + ny2) + (Z) * m._00 / 4096))
    #define PROJ_T(TC, X, Y, Z) ((TC) + (((Y) * (nx2 - (ny2 / 16) * m._02 / 256) - (X) * k) / (nx2 + ny2) + (Z) * m._01 / 4096))

                vec_cross_product((VecFx32 *)&m._00, (VecFx32 *)&m._30, (VecFx32 *)&m._20);
                vector_set_length((VecFx32 *)&m._00, FX32_ONE);
                fx32 cos = vector_cosine((VecFx32 *)&m._00, &light);
                if (cos != VECTOR_COS_INVALID) {
                    s32 shade = (cos + FX32_ONE) / 2;
                    cr = node->dfx.color.r * shade / 4096;
                    cg = node->dfx.color.g * shade / 4096;
                    cb = node->dfx.color.b * shade / 4096;

                    s32 sC, tC;
                    if (!node->dfx.tex.triStrip) {
                        sA = c00.s + node->dfx.tex.tileW * c;
                        tA = c00.t + node->dfx.tex.tileH * r;
                        sB = c10.s + node->dfx.tex.tileW * c;
                        tB = c10.t + node->dfx.tex.tileH * (r + 1);
                        sC = c01.s + node->dfx.tex.tileW * (c + 1);
                        tC = c01.t + node->dfx.tex.tileH * r;
                    } else {
                        nx2 = m._00 * m._00 / 4;
                        ny2 = m._01 * m._01 / 4;

                        if (nx2 + ny2 > 0) {
                            s32 k0 = m._00 * m._01 / 4;
                            s32 k = k0 + (k0 / 16) * m._02 / 256;
                            sA = PROJ_S(c00.s, x00, y00, z00);
                            tA = PROJ_T(c00.t, x00, y00, z00);
                            sB = PROJ_S(c10.s, x10, y10, z10);
                            tB = PROJ_T(c10.t, x10, y10, z10);
                            sC = PROJ_S(c01.s, x01, y01, z01);
                            tC = PROJ_T(c01.t, x01, y01, z01);
                        } else {
                            sA = c00.s + x00;
                            tA = c00.t + y00;
                            sB = c10.s + x10;
                            tB = c10.t + y10;
                            sC = c01.s + x01;
                            tC = c01.t + y01;
                        }
                    }

                    GXRgb color = (cb << 10) | (cg << 5) | cr;
                    G3_Color(color);
                    G3_TexCoord(sA * 256, tA * 256);
                    G3_Vtx(x00 * 16, y00 * 16, z00 * 16);
                    G3_TexCoord(sB * 256, tB * 256);
                    G3_Vtx(x10 * 16, y10 * 16, z10 * 16);
                    G3_TexCoord(sC * 256, tC * 256);
                    G3_Vtx(x01 * 16, y01 * 16, z01 * 16);
                }

                vec_cross_product((VecFx32 *)&m._00, (VecFx32 *)&m._20, (VecFx32 *)&m._10);
                vector_set_length((VecFx32 *)&m._00, FX32_ONE);
                cos = vector_cosine((VecFx32 *)&m._00, &light);
                if (cos != VECTOR_COS_INVALID) {
                    s32 shade = (cos + FX32_ONE) / 2;
                    cr = node->dfx.color.r * shade / 4096;
                    cg = node->dfx.color.g * shade / 4096;
                    cb = node->dfx.color.b * shade / 4096;

                    s32 sC, tC;
                    if (!node->dfx.tex.triStrip) {
                        sA = c01.s + node->dfx.tex.tileW * (c + 1);
                        tA = c01.t + node->dfx.tex.tileH * r;
                        sB = c10.s + node->dfx.tex.tileW * c;
                        tB = c10.t + node->dfx.tex.tileH * (r + 1);
                        sC = c11.s + node->dfx.tex.tileW * (c + 1);
                        tC = c11.t + node->dfx.tex.tileH * (r + 1);
                    } else {
                        nx2 = m._00 * m._00 / 4;
                        ny2 = m._01 * m._01 / 4;

                        if (nx2 + ny2 > 0) {
                            s32 k0 = m._00 * m._01 / 4;
                            s32 k = k0 + (k0 / 16) * m._02 / 256;
                            sA = PROJ_S(c01.s, x01, y01, z01);
                            tA = PROJ_T(c01.t, x01, y01, z01);
                            sB = PROJ_S(c10.s, x10, y10, z10);
                            tB = PROJ_T(c10.t, x10, y10, z10);
                            sC = PROJ_S(c11.s, x11, y11, z11);
                            tC = PROJ_T(c11.t, x11, y11, z11);
                        } else {
                            sA = c01.s + x01;
                            tA = c01.t + y01;
                            sB = c10.s + x10;
                            tB = c10.t + y10;
                            sC = c11.s + x11;
                            tC = c11.t + y11;
                        }
                    }

                    GXRgb color = (cb << 10) | (cg << 5) | cr;
                    G3_Color(color);
                    G3_TexCoord(sA * 256, tA * 256);
                    G3_Vtx(x01 * 16, y01 * 16, z01 * 16);
                    G3_TexCoord(sB * 256, tB * 256);
                    G3_Vtx(x10 * 16, y10 * 16, z10 * 16);
                    G3_TexCoord(sC * 256, tC * 256);
                    G3_Vtx(x11 * 16, y11 * 16, z11 * 16);
                }
    #undef PROJ_S
    #undef PROJ_T

                x10 = x11;
                y10 = y11;
                z10 = z11;
                x00 = x01;
                y00 = y01;
                z00 = z01;
                c10 = c11;
                c00 = c01;
            }
        } while (++r < rows);
    }
    G3_End();
    G3_PopMtx(1);
}

// MATCHED 100%
BOOL clDfx::setupTexture(u16 texIdx, RfxNode * node) {
    s32 alpha = 31 - node->dfx.color.transparency;
    if (alpha < 1) {
        return TRUE;
    }

    DfxTexInfo * tex = this->resolveTexture(texIdx, node);
    if (tex == NULL) {
        return TRUE;
    }

    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, (GXCull)(3 - node->dfx.tex.cull), nextPolygonId(node->dfx.color.transparency), alpha, 0);
    G3_TexImageParam(tex->fmt, GX_TEXGEN_TEXCOORD, tex->sizeS, tex->sizeT, GX_TEXREPEAT_ST, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_TRNS, tex->texAddr);
    G3_TexPlttBase(tex->plttAddr, tex->fmt);

    return FALSE;
}

// MATCHED 100%
DfxTexInfo * clDfx::resolveTexture(u16 texIdx, RfxNode * node) {
    RfxTexHeader * hdr = this->getTexHeader(texIdx);
    if (hdr == NULL) {
        return NULL;
    }

    RfxPlttHeader * pltt = this->getPlttHeader(texIdx);
    if (pltt == NULL) {
        return NULL;
    }

    s16 offsetU = node->dfx.tex.offsetU;
    s16 offsetV = node->dfx.tex.offsetV;
    s32 width = hdr->wBlocks * 8 - offsetU;
    s32 height = hdr->hBlocks * 8 - offsetV;

    if (width <= 0) {
        return NULL;
    }
    if (height <= 0) {
        return NULL;
    }

    if (node->dfx.tex.tileW < width) {
        width = node->dfx.tex.tileW;
    }
    if (node->dfx.tex.tileH < height) {
        height = node->dfx.tex.tileH;
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
    RfxNode * node = rfxStack.cur;
    return (node->dfx.color.b << 10) | (node->dfx.color.g << 5) | (node->dfx.color.r);
}
