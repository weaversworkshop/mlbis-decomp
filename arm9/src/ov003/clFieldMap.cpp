// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clFieldMap.h"

#include "main/clGameMain.h"
#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"

#include "main/palAnim.h"
#include "main/vramBank.h"

// --- Extern Functions --- //

void func_ov006_02121fc4(s32, u16 *, u8 *, BOOL);
void func_ov006_02122040(s32, u16 *, u8 *, BOOL, u16 **, s32, TileAnim *, u8, s32);
void func_ov006_02123500(UnkStruct_F808 *, u16, void *);
void func_ov006_02123534(UnkStruct_F808 *);
void func_ov006_02123570(UnkStruct_F808 *, TexLayer *);
void func_ov006_02123598(UnkStruct_F808 *, TexLayer *);
void func_ov006_021235c0(UnkStruct_F808 *, TexLayer *);
void func_ov006_021235fc(UnkStruct_F808 *, TexLayer *, s32, s32, s32, s32);
void func_ov006_02123640(UnkStruct_F808 *, s32);

// --- Variable Definitions --- //

#include "data/clFieldMap.h"

}

// --- Function Definitions --- //

// MATCHED 100%
clFieldMap::clFieldMap(u8 priority, u32 param2, u32 param3, u8 isSub, u16 mapId, u32 heapId, u32 tmpHeapId, clDPP * dpp, u16 slot, s32 scrollDX, s32 scrollDY, u8 async)
    : clTask(priority, param2, param3) {
        this->init(isSub, 0, mapId, heapId, tmpHeapId, dpp, slot, scrollDX, scrollDY, async);
}

// MATCHED 100%
clFieldMap::clFieldMap(u8 priority, u32 param2, u32 param3, u16 mapId, u32 heapId, u32 tmpHeapId, clDPP * dpp, u32 texTileOfs, u32 texPalOfs, u16 slot, s32 scrollDX, s32 scrollDY, u8 async)
    : clTask(priority, param2, param3) {
        this->init(0, 1, mapId, heapId, tmpHeapId, dpp, slot, scrollDX, scrollDY, async);

        this->work.texTileBase[0] = texTileOfs / 32;
        this->work.texPalBase[0] = texPalOfs / 32;
}

// MATCHED 100%
clFieldMap::~clFieldMap() {
    void * ptr;

    deleteVCountTask(this->work.vblank);

    for (s32 layer = 0; layer < 3; layer++) {
        ptr = this->work.texLayers[layer].zTbl;
        this->work.texLayers[layer].zTbl = NULL;

        if (ptr != NULL) {
            heapFree(ptr);
        }
    }

    for (s32 i = 0; i < 4; i++) {
        if ((this->work.extPafMask >> i) & 1) {
            clFPaf ** paf = this->work.extPaf[i];
            delete *paf;

            *this->work.extPaf[i] = NULL;
        }
    }

    if (this->work.field2600 != NULL) {
        delete[] this->work.field2600;
    }

    for (s32 i = 15; i >= 0; i--) {
        ptr = this->work.tileAnimGfx[i];
        if (ptr != NULL) {
            heapFree(ptr);

            for (s32 j = 0; j < i; j++) {
                if (ptr == this->work.tileAnimGfx[j]) {
                    this->work.tileAnimGfx[j] = NULL;
                }
            }
        }
    }

    if (this->work.field718 != NULL) {
        delete[] this->work.field718;
    }

    if (this->work.field714 != NULL) {
        delete[] this->work.field714;
    }

    if (this->work.mapPatches != NULL) {
        delete[] this->work.mapPatches;
    }

    delete[] this->work.basePalSrc;

    if (this->work.layerPal[2] != NULL) {
        delete[] this->work.layerPal[2];
    }

    if (this->work.layerPal[1] != NULL) {
        delete[] this->work.layerPal[1];
    }

    if (this->work.layerPal[0] != NULL) {
        delete[] this->work.layerPal[0];
    }

    delete[] this->work.basePal;

    heapFree(this->work.mapData);

    delete[] this->work.screenBuf[0];

    if (this->work.gfxToRam) {
        if (((this->work.layerGfxOwned >> 2) & 1) && this->work.layerGfxBuf[2] != NULL) {
            delete[] this->work.layerGfxBuf[2];
        }

        if (((this->work.layerGfxOwned >> 1) & 1) && this->work.layerGfxBuf[1] != NULL) {
            delete[] this->work.layerGfxBuf[1];
        }

        if ((this->work.layerGfxOwned & 1) && this->work.layerGfxBuf[0] != NULL) {
            delete[] this->work.layerGfxBuf[0];
        }
    }
}

// MATCHED 100%
void clFieldMap::run() {
    u32 * tbl;
    u32 * rec;
    u32 bytes;
    s32 i;
    s32 j;
    MapFileReq * req;

    switch (this->work.state) {
        case 0: {
            this->work.screenDirty[2] = 0;
            this->work.screenDirty[1] = 0;
            this->work.screenDirty[0] = 0;

            u16 * screens = new (this->work.heapId, NULL, FALSE) u16[0x1800];
            this->work.screenBuf[0] = screens;
            this->work.screenBuf[1] = this->work.screenBuf[0] + 0x800;
            this->work.screenBuf[2] = this->work.screenBuf[1] + 0x800;

            DC_FlushRange(this->work.screenBuf[0], 0x3000);
            MI_DmaFill32(GX_DEFAULT_DMAID, this->work.screenBuf[0], 0, 0x3000);

            this->work.mapReq.chunkIdx = mapChunkIdx[this->work.mapId].idx[3];

            if (this->work.async) {
                this->work.mapReq.bufSize = 0;

                u32 size = this->work.dpp->readStreamHeader(&this->work.mapReq, this->work.fsSlot);
                this->work.mapData = (u32 *)new (this->work.heapId, NULL, FALSE) u8[size];

                this->work.dpp->queueStream(&this->work.mapReq, this->work.mapData, NULL, 0);
            } else {
                clDAP * dap = new (this->work.tmpHeapId, NULL, TRUE) clDAP(8, 0, this->field04);

                this->work.mapReq.bufSize = this->work.dpp->getChunkSize(this->work.fsSlot, this->work.mapReq.chunkIdx);

                u8 * packed = new (this->work.tmpHeapId, NULL, TRUE) u8[this->work.mapReq.bufSize];
                this->work.mapReq.buf = packed;

                this->work.dpp->queueRead(&this->work.mapReq, 0, this->work.fsSlot, NULL, 0, 0x8000);
                this->work.dpp->waitReq(&this->work.mapReq);

                this->work.mapData = (u32 *)heapAlloc(this->work.heapId, dap->beginJob(&this->work.mapReq), NULL, FALSE);

                dap->queueJob(&this->work.mapReq, this->work.mapData);
                dap->waitJob(&this->work.mapReq);

                delete[] packed;
                delete dap;
            }

            this->work.state++;
        }
        case 1: {
            if (this->work.mapReq.streamState != 0) {
                return;
            }

            u32 * offs = this->work.mapData;

            this->work.sections[0].data = (u8 *)offs + offs[0];
            this->work.sections[1].data = (u8 *)offs + offs[1];
            this->work.sections[2].data = (u8 *)offs + offs[2];
            this->work.sections[3].data = (u8 *)offs + offs[3];
            this->work.sections[4].data = (u8 *)offs + offs[4];
            this->work.sections[5].data = (u8 *)offs + offs[5];
            this->work.sections[6].data = (u8 *)offs + offs[6];
            this->work.sections[7].data = (u8 *)offs + offs[7];
            this->work.sections[11].data = (u8 *)offs + offs[8];
            this->work.sections[14].data = (u8 *)offs + offs[9];
            this->work.sections[15].data = (u8 *)offs + offs[10];
            this->work.sections[8].data = (u8 *)offs + offs[11];
            this->work.sections[9].data = (u8 *)offs + offs[12];
            this->work.sections[10].data = (u8 *)offs + offs[13];
            this->work.sections[12].data = (u8 *)offs + offs[14];
            this->work.sections[13].data = (u8 *)offs + offs[15];
            this->work.sections[16].data = (u8 *)offs + offs[16];

            this->work.sections[0].size = offs[1] - offs[0];
            this->work.sections[1].size = offs[2] - offs[1];
            this->work.sections[2].size = offs[3] - offs[2];
            this->work.sections[3].size = offs[4] - offs[3];
            this->work.sections[4].size = offs[5] - offs[4];
            this->work.sections[5].size = offs[6] - offs[5];
            this->work.sections[6].size = offs[7] - offs[6];
            this->work.sections[7].size = offs[8] - offs[7];
            this->work.sections[11].size = offs[9] - offs[8];
            this->work.sections[14].size = offs[10] - offs[9];
            this->work.sections[15].size = offs[11] - offs[10];
            this->work.sections[8].size = offs[12] - offs[11];
            this->work.sections[9].size = offs[13] - offs[12];
            this->work.sections[10].size = offs[14] - offs[13];
            this->work.sections[12].size = offs[15] - offs[14];
            this->work.sections[13].size = offs[16] - offs[15];
            this->work.sections[16].size = offs[17] - offs[16];

            MapInfo * info = this->work.infoSection.data;

            if (this->work.texMode == 1) {
                s32 last = -1;

                for (j = 0; j < 3; j++) {
                    if (((info->texLayerMask >> j) & 1) == 0) {
                        last = j;
                    }
                }

                this->work.lastBgLayer = last;
            }

            this->work.color256Mask = info->color256Mask;
            this->work.clipMask = info->clipMask;
            this->work.width = info->width;
            this->work.height = info->height;

            this->resetDirtyRect();

            this->work.widthPx = this->work.width * 8;
            this->work.heightPx = this->work.height * 8;
            this->work.wrapW = this->calcWrapSize(this->work.width);
            this->work.wrapH = this->calcWrapSize(this->work.height);

            this->allocPalettes();

            if (this->work.sections[14].size != 0) {
                tbl = (u32 *)this->work.sections[14].data;
                rec = tbl + 1;
                bytes = tbl[0];

                this->work.mapPatchCount = (bytes - 8) / 12;
                this->work.mapPatches = new (this->work.heapId, NULL, FALSE) MapPatch[this->work.mapPatchCount];

                MapPatch * dst = this->work.mapPatches;
                for (i = 0; i < this->work.mapPatchCount; i++) {
                    dst->setFlag = ((MapPatchHdr *)((u8 *)tbl + bytes))[(u32)i].setFlag;
                    dst->clearFlag = ((MapPatchHdr *)((u8 *)tbl + bytes))[(u32)i].clearFlag;

                    if (((MapPatchHdr *)((u8 *)tbl + bytes))[(u32)i].hasTiles) {
                        u16 * src = (u16 *)((u8 *)tbl + rec[0]);

                        dst->x = src[0];
                        dst->y = src[1];
                        dst->w = src[2];
                        dst->h = src[3];

                        dst->tiles = src + 4;
                    } else {
                        dst->x = 0;
                        dst->y = 0;
                        dst->w = 0;
                        dst->h = 0;

                        dst->tiles = NULL;
                    }

                    if (((MapPatchHdr *)((u8 *)tbl + bytes))[(u32)i].hasData) {
                        dst->data = (u8 *)tbl + rec[1];
                    } else {
                        dst->data = NULL;
                    }

                    if (((MapPatchHdr *)((u8 *)tbl + bytes))[(u32)i].hasRects) {
                        dst->rects = (MapPatchRect *)((u8 *)tbl + rec[2]);
                    } else {
                        dst->rects = NULL;
                    }

                    rec += 3;
                    dst++;
                }
            } else {
                this->work.mapPatchCount = 0;
                this->work.mapPatches = NULL;
            }

            if (this->work.sections[15].size != 0) {
                tbl = (u32 *)this->work.sections[15].data;

                this->work.tileAnimCount = tbl[0] / 4;

                for (i = 0; i < this->work.tileAnimCount; i++) {
                    this->work.tileAnims[i].def = (TileAnimDef *)((u8 *)tbl + (tbl[i] & ~3));
                    this->work.tileAnims[i].timer = 0;

                    if (this->work.tileAnims[i].def->mode >= 2) {
                        this->work.tileAnims[i].active = 0;
                    } else {
                        this->work.tileAnims[i].active = 1;
                    }
                }
            } else {
                this->work.tileAnimCount = 0;
            }

            if (this->work.sections[11].size != 0) {
                tbl = (u32 *)this->work.sections[11].data;

                this->work.blendAnimCount = tbl[0] / 4;

                for (i = 0; i < this->work.blendAnimCount; i++) {
                    BlendAnim * ent = (BlendAnim *)((u8 *)tbl + (tbl[i] & ~3));

                    if (!ent->manual) {
                        this->work.blendAnim = ent;
                        this->work.blendTimer = 0;
                        break;
                    }
                }
            } else {
                this->work.blendAnim = NULL;
                this->work.blendAnimCount = 0;
            }

            for (i = 0; i < 3; i++) {
                if (this->work.sections[i + 8].size != 0 && this->work.sections[i + 3].size != 0) {
                    if (this->work.color256Mask & (1 << i)) {
                        u16 * pal = this->work.layerPal[i];

                        this->palAnims[i].start(NULL, (u16 *)this->work.sections[i + 8].data);
                        this->palAnims[i].palette = pal;
                        static_cast<clYanaAnim *>(&this->palAnims[i])->loopsRemaining = 0;

                        this->work.palAnimMask |= 1 << i;
                    } else if ((this->work.palAnimMask & 8) == 0) {
                        u16 * pal = this->work.basePal;

                        this->palAnims[3].start(NULL, (u16 *)this->work.sections[i + 8].data);
                        this->palAnims[3].palette = pal;
                        this->palAnims[3].loopsRemaining = 0;

                        this->work.palAnimMask |= 8;
                    }
                }
            }

            this->resumePalAnims();

            this->work.state++;
        }
        case 2: {
            clDAP * dap = NULL;

            for (s32 layer = 0; layer < 3; layer++) {
                req = &this->work.layerGfxReqs[(u32)layer];

                if (this->work.async) {
                    if (this->work.isSub == 1 && GX_GetBankForSubBGExtPltt() == 0) {
                        this->work.charBlock[layer] = 0;
                    } else if (layer >= 1) {
                        if (this->work.isSub == 1 || (this->work.isSub == 0 && (!this->work.texMode || ((this->work.infoSection.data->texLayerMask >> (layer - 1)) & 1) == 0))) {
                            this->work.charBlock[layer] = this->work.charBlock[layer - 1] + (this->work.layerGfxSize[layer - 1] >> 14) + ((this->work.layerGfxSize[layer - 1] & 0x3fff) ? 1 : 0);
                        } else {
                            this->work.charBlock[layer] = this->work.charBlock[layer - 1];
                        }
                    } else {
                        this->work.charBlock[layer] = 2;
                    }
                }

                tbl = mapChunkIdx[this->work.mapId].idx;

                if (tbl[(u32)layer] == -1) {
                    req->fs.chunkIdx = 0xffff;
                    req->fs.bufSize = 0;
                    req->fs.buf = NULL;
                    this->work.layerGfxSize[(u32)layer] = 0;

                    if (layer > 0) {
                        this->work.texTileBase[layer] = this->work.texTileBase[layer - 1];
                    }
                } else if ((layer >= 1 && tbl[layer - 1] == tbl[(u32)layer]) || (layer >= 2 && tbl[layer - 2] == tbl[(u32)layer])) {
                    u8 prev;

                    if (tbl[layer - 1] == tbl[(u32)layer]) {
                        prev = layer - 1;
                    } else {
                        prev = layer - 2;
                    }

                    req->fs.chunkIdx = tbl[(u32)layer];
                    req->fs.bufSize = 0;
                    req->fs.buf = NULL;

                    this->work.layerGfxSize[layer] = this->work.dpp->readStreamHeader(&req->fs, this->work.fsSlot);
                    req->fs.streamState = 0;

                    this->work.layerGfxBuf[layer] = this->work.layerGfxBuf[prev];
                    this->work.charBlock[layer] = this->work.charBlock[prev];
                    this->work.texTileBase[layer] = this->work.texTileBase[prev];

                    if (layer == this->work.lastBgLayer && ((this->work.infoSection.data->texLayerMask >> prev) & 1)) {
                        if (this->work.async) {
                            req->fs.bufSize = 0;

                            if (this->work.gfxToRam) {
                                this->work.layerGfxBuf[layer] = new (this->work.heapId, NULL, FALSE) u8[this->work.layerGfxSize[layer]];
                                this->work.layerGfxOwned |= 1 << layer;

                                if (layer > 0) {
                                    this->work.texTileBase[layer] = this->work.texTileBase[layer - 1] + (this->work.layerGfxSize[layer - 1] >> 5);
                                }

                                this->work.dpp->queueStream(&req->fs, (u32 *)this->work.layerGfxBuf[layer], NULL, 0);
                            } else {
                                this->work.dpp->queueStream(&req->fs, (u32 *)this->getBgCharAddr(this->work.charBlock[layer]), NULL, 0);
                            }
                        } else {
                            if (dap == NULL) {
                                dap = new (this->work.tmpHeapId, NULL, TRUE) clDAP(8, 0, this->field04);
                            }

                            req->fs.bufSize = this->work.dpp->getChunkSize(this->work.fsSlot, req->fs.chunkIdx);
                            req->fs.buf = new (this->work.tmpHeapId, NULL, TRUE) u8[req->fs.bufSize];

                            this->work.dpp->queueRead(&req->fs, 0, this->work.fsSlot, NULL, 0, 0x8000);
                        }
                    }
                } else {
                    req->fs.chunkIdx = tbl[(u32)layer];

                    this->work.layerGfxSize[layer] = this->work.dpp->readStreamHeader(&req->fs, this->work.fsSlot);

                    if (this->work.async) {
                        req->fs.bufSize = 0;

                        if (this->work.gfxToRam) {
                            this->work.layerGfxBuf[layer] = new (this->work.heapId, NULL, FALSE) u8[this->work.layerGfxSize[layer]];
                            this->work.layerGfxOwned |= 1 << layer;

                            if (layer > 0) {
                                this->work.texTileBase[layer] = this->work.texTileBase[layer - 1] + (this->work.layerGfxSize[layer - 1] >> 5);
                            }

                            this->work.dpp->queueStream(&req->fs, (u32 *)this->work.layerGfxBuf[layer], NULL, 0);
                        } else if (this->work.texMode == 1 && ((this->work.infoSection.data->texLayerMask >> layer) & 1)) {
                            if (this->work.savedTexBank == GX_VRAM_TEX_NONE) {
                                this->work.texVram = getTexVramAddr(GX_GetBankForTex());
                                this->work.savedTexBank = GX_ResetBankForTex();
                            }

                            if (layer > 0) {
                                this->work.texTileBase[layer] = this->work.texTileBase[layer - 1] + (this->work.layerGfxSize[layer - 1] >> 5);
                            }

                            this->work.dpp->queueStream(&req->fs, (u32 *)(this->work.texVram + this->work.texTileBase[layer] * 32), NULL, 0);
                        } else {
                            if (layer > 0) {
                                this->work.texTileBase[layer] = this->work.texTileBase[layer - 1];
                            }

                            this->work.dpp->queueStream(&req->fs, (u32 *)this->getBgCharAddr(this->work.charBlock[layer]), NULL, 0);
                        }
                    } else {
                        if (dap == NULL) {
                            dap = new (this->work.tmpHeapId, NULL, TRUE) clDAP(8, 0, this->field04);
                        }

                        req->fs.bufSize = this->work.dpp->getChunkSize(this->work.fsSlot, req->fs.chunkIdx);
                        req->fs.buf = new (this->work.tmpHeapId, NULL, TRUE) u8[req->fs.bufSize];

                        this->work.dpp->queueRead(&req->fs, 0, this->work.fsSlot, NULL, 0, 0x8000);
                    }
                }
            }

            for (s32 n = 0; n < this->work.tileAnimCount; n++) {
                req = &this->work.tileAnimReqs[n];

                for (j = 0; j < n; j++) {
                    if (this->work.tileAnims[n].def->gfxId == this->work.tileAnims[j].def->gfxId) {
                        this->work.tileAnimGfx[n] = this->work.tileAnimGfx[j];
                        req->fs.buf = NULL;
                        break;
                    }
                }

                if (j == n) {
                    req->fs.chunkIdx = tileAnimChunkIdx[this->work.tileAnims[n].def->gfxId];

                    if (this->work.async) {
                        u32 size = this->work.dpp->readStreamHeader(&req->fs, this->work.fsSlot);

                        req->fs.bufSize = 0;
                        this->work.tileAnimGfx[n] = new (this->work.heapId, NULL, FALSE) u32[size / 4];

                        this->work.dpp->queueStream(&req->fs, this->work.tileAnimGfx[n], NULL, 0);
                    } else {
                        if (dap == NULL) {
                            dap = new (this->work.tmpHeapId, NULL, TRUE) clDAP(8, 0, this->field04);
                        }

                        req->fs.bufSize = this->work.dpp->getChunkSize(this->work.fsSlot, req->fs.chunkIdx);
                        req->fs.buf = new (this->work.tmpHeapId, NULL, TRUE) u8[req->fs.bufSize];

                        this->work.dpp->queueRead(&req->fs, 0, this->work.fsSlot, NULL, 0, 0x8000);
                    }
                }
            }

            if (this->work.async) {
                this->work.state++;
                return;
            }

            this->work.dpp->waitAll();

            for (i = 0; i < 3; i++) {
                req = &this->work.layerGfxReqs[i];

                if (this->work.isSub == 1 && GX_GetBankForSubBGExtPltt() == 0) {
                    this->work.charBlock[i] = 0;
                } else if (i >= 1) {
                    if (mapChunkIdx[this->work.mapId].idx[i] == mapChunkIdx[this->work.mapId].idx[i - 1]) {
                        this->work.charBlock[i] = this->work.charBlock[i - 1];
                    } else if (i >= 2 && mapChunkIdx[this->work.mapId].idx[i] == mapChunkIdx[this->work.mapId].idx[i - 2]) {
                        this->work.charBlock[i] = this->work.charBlock[i - 2];
                    } else if (this->work.isSub == 1 || (this->work.isSub == 0 && (!this->work.texMode || ((this->work.infoSection.data->texLayerMask >> (i - 1)) & 1) == 0))) {
                        this->work.charBlock[i] = this->work.charBlock[i - 1] + (this->work.layerGfxSize[i - 1] >> 14) + ((this->work.layerGfxSize[i - 1] & 0x3fff) ? 1 : 0);
                    } else {
                        this->work.charBlock[i] = this->work.charBlock[i - 1];
                    }
                } else {
                    this->work.charBlock[i] = 2;
                }

                if (req->fs.buf != NULL) {
                    this->work.layerGfxSize[i] = dap->beginJob(&req->fs);

                    if (this->work.gfxToRam) {
                        this->work.layerGfxBuf[i] = new (this->work.heapId, NULL, FALSE) u8[this->work.layerGfxSize[i]];
                        this->work.layerGfxOwned |= 1 << i;

                        dap->queueJob(&req->fs, (u32 *)this->work.layerGfxBuf[i]);
                    } else if (this->work.texMode == 1 && ((this->work.infoSection.data->texLayerMask >> i) & 1)) {
                        if (this->work.savedTexBank == GX_VRAM_TEX_NONE) {
                            this->work.texVram = getTexVramAddr(GX_GetBankForTex());
                            this->work.savedTexBank = GX_ResetBankForTex();
                        }

                        if (i > 0) {
                            u16 * base = &this->work.texTileBase[i];

                            this->work.texTileBase[i] = base[-1] + (this->work.layerGfxSize[i - 1] >> 5);
                        }

                        dap->queueJob(&req->fs, (u32 *)(this->work.texVram + this->work.texTileBase[i] * 32));
                    } else {
                        dap->queueJob(&req->fs, (u32 *)this->getBgCharAddr(this->work.charBlock[i]));
                    }
                } else {
                    this->work.layerGfxBuf[i] = NULL;

                    if (i > 0) {
                        u16 * base = &this->work.texTileBase[i];

                        this->work.texTileBase[i] = base[-1];
                    }
                }
            }

            for (s32 n = 0; n < this->work.tileAnimCount; n++) {
                req = &this->work.tileAnimReqs[n];

                if (req->fs.buf != NULL) {
                    this->work.tileAnimGfx[n] = (u32 *)heapAlloc(this->work.heapId, dap->beginJob(&req->fs), NULL, FALSE);
                    dap->queueJob(&req->fs, this->work.tileAnimGfx[n]);
                } else {
                    for (s32 k = 0; k < n; k++) {
                        if (this->work.tileAnims[n].def->gfxId == this->work.tileAnims[k].def->gfxId) {
                            this->work.tileAnimGfx[n] = this->work.tileAnimGfx[k];
                            break;
                        }
                    }
                }
            }

            while ((BOOL)(dap->head == NULL) == FALSE) {
                dap->pumpJobs();
            }

            this->setupTexLayers();

            if (this->work.gfxToRam) {
                for (i = 0; i < 3; i++) {
                    req = &this->work.layerGfxReqs[i];

                    if (this->work.layerGfxBuf[i] != NULL) {
                        MapInfo * info = this->work.infoSection.data;

                        if (this->work.texMode == 1 && ((info->texLayerMask >> i) & 1)) {
                            u16 * base = &this->work.texTileBase[i];

                            this->work.vblank->queueCopy(this->work.layerGfxBuf[i], this->work.texVram + this->work.texTileBase[i] * 32, (base[1] - this->work.texTileBase[i]) * 32);
                        } else {
                            u32 len = this->work.layerGfxSize[i];
                            void * dst = this->getBgCharAddr(this->work.charBlock[i]);

                            if (this->work.layerGfxBuf[i] < dst) {
                                cpu_copy32_rev(this->work.layerGfxBuf[i], dst, len);
                            } else {
                                cpu_copy32_fwd(this->work.layerGfxBuf[i], dst, len);
                            }
                        }

                        if (req->fs.buf != NULL) {
                            delete[] req->fs.buf;
                        }
                    }
                }
            } else {
                for (i = 0; i < 3; i++) {
                    req = &this->work.layerGfxReqs[i];

                    if (req->fs.buf != NULL) {
                        delete[] req->fs.buf;
                    }
                }
            }

            for (i = 0; i < this->work.tileAnimCount; i++) {
                req = &this->work.tileAnimReqs[i];

                if (req->fs.buf != NULL) {
                    delete req->fs.buf;
                }
            }

            delete dap;

            if (!this->work.gfxToRam && this->work.savedTexBank != GX_VRAM_TEX_NONE) {
                GX_SetBankForTex(this->work.savedTexBank);
                this->work.savedTexBank = GX_VRAM_TEX_NONE;
            }

            this->work.state = 100;
            break;
        }
        case 3: {
            for (i = 0; i < 3; i++) {
                req = &this->work.layerGfxReqs[i];

                if (req->fs.streamState != 0) {
                    break;
                }
            }

            if (i < 3) {
                return;
            }

            for (i = 0; i < this->work.tileAnimCount; i++) {
                req = &this->work.tileAnimReqs[i];

                if (req->fs.streamState != 0) {
                    break;
                }
            }

            if (i < this->work.tileAnimCount) {
                return;
            }

            this->setupTexLayers();

            if (this->work.gfxToRam) {
                if (this->work.texMode == 1 && this->work.savedTexBank == GX_VRAM_TEX_NONE) {
                    this->work.texVram = getTexVramAddr(GX_GetBankForTex());
                    this->work.savedTexBank = GX_ResetBankForTex();
                }

                for (s32 layer = 0; layer < 3; layer++) {
                    if (this->work.layerGfxBuf[layer] != NULL) {
                        MapInfo * info = this->work.infoSection.data;

                        if (this->work.texMode == 1 && ((info->texLayerMask >> layer) & 1)) {
                            u16 * base = &this->work.texTileBase[layer];
                            this->work.vblank->queueCopy(this->work.layerGfxBuf[layer], this->work.texVram + this->work.texTileBase[layer] * 32, (base[1] - this->work.texTileBase[layer]) * 32);
                        } else {
                            bytes = this->work.layerGfxSize[layer];
                            void * dst = this->getBgCharAddr(this->work.charBlock[layer]);

                            if (this->work.layerGfxBuf[layer] < dst) {
                                cpu_copy32_rev(this->work.layerGfxBuf[layer], dst, bytes);
                            } else {
                                cpu_copy32_fwd(this->work.layerGfxBuf[layer], dst, bytes);
                            }
                        }
                    }
                }
            }

            if (this->work.savedTexBank != GX_VRAM_TEX_NONE) {
                GX_SetBankForTex(this->work.savedTexBank);
                this->work.savedTexBank = GX_VRAM_TEX_NONE;
            }

            this->work.state = 100;
            break;
        }
        case 100: {
            this->setupBg(1);

            if (this->work.tileAnimCount != 0) {
                for (i = 0; i < this->work.tileAnimCount; i++) {
                    if (this->work.tileAnims[i].active == 1) {
                        this->restartTileAnim((u16)i);
                    }
                }
            }

            this->setScroll(this->work.scrollDX, this->work.scrollDY);
            this->work.scrollDX = 0;
            this->work.scrollDY = 0;

            s32 planes;
            if (this->work.isSub == 0) {
                planes = GX_GetVisiblePlane() & ~0xe;
            } else {
                planes = GXS_GetVisiblePlane() & ~0xe;
            }

            if (mapChunkIdx[this->work.mapId].idx[0] != -1) {
                planes |= 2;
            }
            if (mapChunkIdx[this->work.mapId].idx[1] != -1) {
                planes |= 4;
            }
            if (mapChunkIdx[this->work.mapId].idx[2] != -1) {
                planes |= 8;
            }

            if (this->work.texMode == 1) {
                MapInfo * info = this->work.infoSection.data;
                s32 w = info->width;
                s32 h = info->height;
                bytes = w * h * 2;

                for (i = 0; i < 3; i++) {
                    if ((info->texLayerMask >> i) & 1) {
                        this->work.texLayers[i].scrollX = 0;
                        this->work.texLayers[i].scrollY = 0;

                        TexLayer * bg = &this->work.texLayers[i];

                        bg->field698_2 = 0;
                        bg->hidden = 0;

                        cpu_fill32(bg->cache, -1, sizeof(bg->cache));

                        this->work.texLayers[i].map = this->work.sections[i].data;
                        this->work.texLayers[i].zTbl = (u16 *)heapAlloc(this->work.heapId, bytes, "Z-Tbl", FALSE);
                        this->work.texLayers[i].w = w;
                        this->work.texLayers[i].h = h;
                        this->work.texLayers[i].tileBase = this->work.texTileBase[i];
                        this->work.texLayers[i].palBase = this->work.texPalBase[i];
                        this->work.texLayers[i].color256 = (info->color256Mask >> i) & 1;
                        this->work.texLayers[i].layer = i;

                        MI_CpuClear16(this->work.texLayers[i].zTbl, bytes);
                    }
                }

                u32 * list = (u32 *)this->getZRects();
                u32 count = (list != NULL) ? *list++ : 0;

                if (count != 0) {
                    for (i = 0; i < count; i++) {
                        this->fillZRect((ZRect *)list);
                        list += 3;
                    }
                }

                func_ov006_02123500(&this->work.field808, 0, NULL);

                if (info->texLayerMask & 1) {
                    planes &= ~2;
                }
                if ((info->texLayerMask >> 1) & 1) {
                    planes &= ~4;
                }
                if ((info->texLayerMask >> 2) & 1) {
                    planes &= ~8;
                }
            }

            if (this->work.isSub == 1 || this->work.texMode == 0) {
                planes |= 1;
            } else {
                planes |= 2;
            }

            if (this->work.isSub == 0) {
                GX_SetVisiblePlane(planes);
            } else {
                GXS_SetVisiblePlane(planes);
            }

            this->work.vblank->enable();

            this->work.state = 101;
            break;
        }
        case 101: {
            if (this->work.palDirty != 0 || this->work.extPafMask != 0) {
                this->restorePalettes(0xff);
                this->work.palDirty = 0;
            }

            if (this->work.extPafMask != 0) {
                for (s32 slot = 0; slot < 4; slot++) {
                    if (this->work.extPaf[slot] != NULL && *this->work.extPaf[slot] == NULL) {
                        this->work.extPaf[slot] = NULL;
                        this->work.extPafMask &= ~(1 << slot);
                    }
                }
            }

            for (i = 0; i < 3; i++) {
                s32 dx = 0;
                s32 dy = 0;

                this->getLayerScrollRate(i, &dx, &dy);

                if ((this->work.infoSection.data->fixedRateMask & (1 << i)) == 0) {
                    dx = (dx * this->work.scrollDX) >> 8;
                    dy = (dy * this->work.scrollDY) >> 8;
                } else {
                    dx += this->work.scrollDX;
                    dy += this->work.scrollDY;
                }

                if (this->work.moves[i].active != 0) {
                    LayerMove * move = &this->work.moves[i];
                    s32 stepX;
                    s32 stepY;
                    s32 t = move->accel * move->frame + move->speedStart;

                    move->frame++;

                    if (move->frame >= move->frames) {
                        stepX = move->dstX - move->curX;
                        stepY = move->dstY - move->curY;
                        this->work.moves[i].active = 0;
                    } else {
                        stepX = (t * (move->dstX - move->startX)) / (s32)move->dist;
                        stepY = (t * (move->dstY - move->startY)) / (s32)move->dist;
                        move->curX += stepX;
                        move->curY += stepY;
                    }

                    dx += stepX;
                    dy += stepY;
                }

                if (dx != 0 || dy != 0) {
                    this->scrollLayer(i, dx, dy);
                }
            }

            this->work.scrollX += this->work.scrollDX;
            this->work.scrollY += this->work.scrollDY;
            this->work.scrollDX = 0;
            this->work.scrollDY = 0;

            if (this->work.tileAnimCount != 0) {
                this->updateTileAnims();
            }

            if (this->work.palAnimMask != 0 && this->work.palAnimState == 1) {
                this->restorePalettes(0xff);

                for (i = 0; i < 4; i++) {
                    if (this->work.palAnimMask & (1 << i)) {
                        this->palAnims[i].update();
                    }
                }
            }

            if (this->work.palEffectCount != 0) {
                this->updatePalEffects();
            }

            if (this->work.blendAnimCount != 0) {
                this->updateBlendAnim();
            }

            if (this->work.texMode != 1) {
                return;
            }

            MapInfo * info = this->work.infoSection.data;

            func_ov006_02123534(&this->work.field808);

            if (this->work.dirtyRectSet) {
                this->work.texLayers[0].scrollX = this->work.layerX[0] >> 11;
                this->work.texLayers[0].scrollY = this->work.layerY[0] >> 11;
                this->work.texLayers[1].scrollX = this->work.layerX[1] >> 11;
                this->work.texLayers[1].scrollY = this->work.layerY[1] >> 11;
                this->work.texLayers[2].scrollX = this->work.layerX[2] >> 11;
                this->work.texLayers[2].scrollY = this->work.layerY[2] >> 11;

                if (this->work.mapId == 0x25f) {
                    if (this->work.layerGfxSize[0] != 0 && this->work.sections[0].size != 0 && (info->texLayerMask & 1)) {
                        func_ov006_021235fc(&this->work.field808, &this->work.texLayers[0], this->work.dirtyX0, this->work.dirtyY0, this->work.dirtyX1, this->work.dirtyY1);
                    }
                } else if (info->texLayerMask & 1) {
                    func_ov006_021235fc(&this->work.field808, &this->work.texLayers[0], this->work.dirtyX0, this->work.dirtyY0, this->work.dirtyX1, this->work.dirtyY1);
                }

                if ((info->texLayerMask >> 1) & 1) {
                    func_ov006_021235fc(&this->work.field808, &this->work.texLayers[1], this->work.dirtyX0, this->work.dirtyY0, this->work.dirtyX1, this->work.dirtyY1);
                }

                if ((info->texLayerMask >> 2) & 1) {
                    func_ov006_021235fc(&this->work.field808, &this->work.texLayers[2], this->work.dirtyX0, this->work.dirtyY0, this->work.dirtyX1, this->work.dirtyY1);
                }

                this->resetDirtyRect();
                this->work.dirtyRectSet = 0;
            }

            this->work.texLayers[0].scrollX = this->work.layerX[0] >> 8;
            this->work.texLayers[0].scrollY = this->work.layerY[0] >> 8;
            this->work.texLayers[1].scrollX = this->work.layerX[1] >> 8;
            this->work.texLayers[1].scrollY = this->work.layerY[1] >> 8;
            this->work.texLayers[2].scrollX = this->work.layerX[2] >> 8;
            this->work.texLayers[2].scrollY = this->work.layerY[2] >> 8;

            BOOL draw0 = FALSE;
            if (this->work.layerGfxSize[0] != 0 && this->work.sections[0].size != 0 && (info->texLayerMask & 1)) {
                draw0 = TRUE;
            }

            BOOL draw1 = FALSE;
            if (this->work.layerGfxSize[1] != 0 && this->work.sections[1].size != 0 && ((info->texLayerMask >> 1) & 1)) {
                draw1 = TRUE;
            }

            BOOL draw2 = FALSE;
            if (this->work.layerGfxSize[2] != 0 && this->work.sections[2].size != 0 && ((info->texLayerMask >> 2) & 1)) {
                draw2 = TRUE;
            }

            if (draw0) {
                func_ov006_02123570(&this->work.field808, &this->work.texLayers[0]);
            }

            if (draw1) {
                func_ov006_02123570(&this->work.field808, &this->work.texLayers[1]);
            }

            if (draw2) {
                func_ov006_02123570(&this->work.field808, &this->work.texLayers[2]);
            }

            if (draw0) {
                func_ov006_02123598(&this->work.field808, &this->work.texLayers[0]);
            }

            if (draw1) {
                func_ov006_02123598(&this->work.field808, &this->work.texLayers[1]);
            }

            if (draw2) {
                func_ov006_02123598(&this->work.field808, &this->work.texLayers[2]);
            }

            if (draw0) {
                func_ov006_021235c0(&this->work.field808, &this->work.texLayers[0]);
            }

            if (draw1) {
                func_ov006_021235c0(&this->work.field808, &this->work.texLayers[1]);
            }

            if (draw2) {
                func_ov006_021235c0(&this->work.field808, &this->work.texLayers[2]);
            }

            func_ov006_02123640(&this->work.field808, 0);
            break;
        }
        case 102:
            break;
    }
}

// MATCHED 100%
void clFieldMap::resumePalAnims() {
    if (this->work.palAnimMask) {
        for (s32 i = 0; i < 4; i++) {
            if (this->work.palAnimMask & (1 << i)) {
                clYanaAnim * anim = &this->palAnims[i];
                anim->stopped = 0;
            }
        }

        this->work.palAnimState = 1;
    }
}

// MATCHED 100%
void clFieldMap::setupBg(u32 useMapOrder) {
    this->work.palDirty = 0xf;

    if (this->work.isSub == 0) {
        MapInfo * info = this->getMapInfo();

        GX_SetBGCharOffset(GX_BGCHAROFFSET_0x00000);

        if (this->work.texMode == 1) {
            G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01);
        } else {
            G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_512x256, (GXBGColorMode)(this->work.color256Mask & 1), GX_BG_SCRBASE_0x5000, (GXBGCharBase)this->work.charBlock[0], GX_BG_EXTPLTT_01);
        }

        if (this->work.texMode == 0 || ((info->texLayerMask >> 1) & 1) == 0) {
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x256, (GXBGColorMode)((this->work.color256Mask & 2) >> 1), GX_BG_SCRBASE_0x6000, (GXBGCharBase)this->work.charBlock[1]);
        }
        if (this->work.texMode == 0 || ((info->texLayerMask >> 2) & 1) == 0) {
            G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_512x256, (GXBGColorMode)((this->work.color256Mask & 4) >> 2), GX_BG_SCRBASE_0x7000, (GXBGCharBase)this->work.charBlock[2]);
        }

        if (this->work.blendSection.size != 0 && this->work.blendAnim != NULL) {
            G2_SetBlendAlpha(this->work.blendAnim->plane1 << 1, this->work.blendAnim->plane2 << 1, this->work.blendAnim->steps[0].eva, this->work.blendAnim->steps[0].evb);
            
            this->work.blendTimer = (this->work.blendAnim->steps[0].duration << 8);
            this->work.blendStep = 0;
        } else {
            G2_SetBlendAlpha(GX_BLEND_PLANEMASK_BG0, 0x3e, 16, 16);
        }

        this->uploadTexPalettes();

        G2_BG1Mosaic(FALSE);
        G2_BG2Mosaic(FALSE);
        G2_BG3Mosaic(FALSE);
    } else {
        s32 scrBase[3];
        s32 charBase[3];

        if (GX_GetBankForSubBGExtPltt() != GX_VRAM_SUB_BGEXTPLTT_NONE) {
            for (s32 layer = 0; layer < 3; layer++) {
                scrBase[layer] = 10 + layer * 2;
                charBase[layer] = this->work.charBlock[layer];
            }
        } else {
            charBase[0] = 0;
            charBase[1] = 0;
            charBase[2] = 0;

            scrBase[0] = 10;
            scrBase[1] = 12;
            scrBase[2] = 14;
        }

        G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_512x256, (GXBGColorMode)(this->work.color256Mask & 1), (GXBGScrBase)scrBase[0], (GXBGCharBase)charBase[0], GX_BG_EXTPLTT_01);
        G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_512x256, (GXBGColorMode)((this->work.color256Mask & 2) >> 1), (GXBGScrBase)scrBase[1], (GXBGCharBase)charBase[1]);
        G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_512x256, (GXBGColorMode)((this->work.color256Mask & 4) >> 2), (GXBGScrBase)scrBase[2], (GXBGCharBase)charBase[2]);

        if (this->work.blendSection.size != 0 && this->work.blendAnim != NULL) {
            G2S_SetBlendAlpha(this->work.blendAnim->plane1 << 1, this->work.blendAnim->plane2 << 1, this->work.blendAnim->steps[0].eva, this->work.blendAnim->steps[0].evb);

            this->work.blendTimer = (this->work.blendAnim->steps[0].duration << 8);
            this->work.blendStep = 0;
        } else {
            G2S_SetBlendAlpha(0, 0x3f, 16, 16);
        }

        G2S_BG1Mosaic(FALSE);
        G2S_BG2Mosaic(FALSE);
        G2S_BG3Mosaic(FALSE);
    }

    if (this->work.texMode == 1) {
        G2_SetBG0Priority(1);
        this->setLayerPriorities(0, 2, 3);
    } else {
        if (this->work.isSub == 0) {
            G2_SetBG0Priority(0);
        } else {
            G2S_SetBG0Priority(0);
        }

        if (useMapOrder == 1) {
            switch (this->work.infoSection.data->priorityOrder) {
                case 0:
                    this->setLayerPriorities(1, 2, 3);
                    break;
                case 1:
                    this->setLayerPriorities(1, 3, 2);
                    break;
                case 2:
                    this->setLayerPriorities(2, 3, 1);
                    break;
            }
        } else {
            this->setLayerPriorities(1, 2, 3);
        }
    }

    this->uploadPalettes();
    this->work.palDirty = 0;
}

// MATCHED 100%
void clFieldMap::setLayerPriorities(u32 prio0, u32 prio1, u32 prio2) {
    this->work.layerPriorities = prio0 | (prio1 << 2) | (prio2 << 4);
    this->work.prioritiesDirty = 1;
}

// MATCHED 100%
void clFieldMap::setScroll(s32 x, s32 y) {
    for (s32 layer = 0; layer < 3; layer++) {
        s32 rateX;
        s32 rateY;
        this->getLayerScrollRate(layer, &rateX, &rateY);

        s32 lx;
        s32 ly;
        MapInfo * info = this->work.infoSection.data;
        if ((info->fixedRateMask & (1 << layer)) == 0) {
            lx = (rateX * x) >> 8;
            ly = (rateY * y) >> 8;
            lx -= ((this->work.scrollX * rateX) >> 8) - this->work.layerX[layer];
            ly -= ((this->work.scrollY * rateY) >> 8) - this->work.layerY[layer];
        } else {
            lx = (rateX + this->work.layerX[layer]) + (x - this->work.scrollX);
            ly = (rateY + this->work.layerY[layer]) + (y - this->work.scrollY);
        }

        this->redrawLayer(layer, lx, ly);
    }

    this->work.scrollX = x;
    this->work.scrollY = y;
}

// MATCHED 100%
void clFieldMap::setLayerScroll(u8 layer, u32 x, u32 y) {
    this->redrawLayer(layer, x, y);
}

// MATCHED 100%
void clFieldMap::redrawLayer(u8 layer, u32 x, u32 y) {
    s32 tx;
    u16 * map;
    u16 * mapRow;
    u16 * screen;
    u16 * rowLo;
    s32 ty;
    s32 w;
    s32 txEnd;
    s32 sx;

    s32 oobX = 0;
    s32 oobY = 0;
    s32 clip = (this->work.clipMask & (1 << layer)) != 0;
    map = this->work.sectionsU16[layer].data;
    w = this->work.width;
    screen = this->work.screenBuf[layer];
    s32 h = this->work.height;

    if (mapChunkIdx[this->work.mapId].idx[layer] != -1) {
        tx = (s16)(x >> 11);
        ty = (s16)(y >> 11);
        s32 tyEnd = ty + 25;
        txEnd = tx + 33;

        if (clip) {
            ty--;
            while (ty < tyEnd) {
                s32 sy = ty;

                s32 my = ty;
                if (ty < 0) {
                    oobY = 1;
                    my = (h + ty % h) % h;
                } else if (h <= ty) {
                    oobY = 1;
                    my = ty % h;
                }

                if (ty < 0) {
                    sy = (ty % 32 + 32) % 32;
                } else if (ty >= 32) {
                    sy = sy % 32;
                }

                mapRow = map + my * w;
                rowLo = screen + (sy % 32) * 32;
                u16 * rowHi = screen + ((sy % 32) * 32 + 0x400);

                for (s32 col = tx - 1; col < txEnd; col++) {
                    sx = col;

                    s32 mx = col;
                    if (col < 0) {
                        oobX = 1;
                        mx = (w + col % w) % w;
                        sx = (col % 64 + 64) % 64;
                    } else if (w <= col) {
                        oobX = 1;
                        mx = col % w;
                    }

                    if (sx >= 64) {
                        sx = sx % 64;
                    }

                    s32 tile = (oobX != 0 || oobY != 0) ? 0 : mapRow[mx];

                    if ((sx / 32) & 1) {
                        rowHi[sx % 32] = tile;
                    } else {
                        rowLo[sx % 32] = tile;
                    }
                    oobX = 0;
                }

                oobY = 0;
                ty++;
            }
        } else {
            ty--;
            while (ty < tyEnd) {
                s32 sy = ty;

                s32 my = ty;
                if (ty < 0) {
                    my = (h + ty % h) % h;
                } else if (h <= ty) {
                    my = ty % h;
                }

                if (ty < 0) {
                    sy = (ty % 32 + 32) % 32;
                } else if (ty >= 32) {
                    sy = sy % 32;
                }

                u16 * rowLo = screen + (sy % 32) * 32;
                u16 * rowHi = screen + ((sy % 32) * 32 + 0x400);
                u16 * mapRow = map + my * w;

                for (s32 col = tx - 1; col < txEnd; col++) {
                    s32 sx = col;
    
                    s32 mx = col;
                    if (col < 0) {
                        mx = (w + col % w) % w;
                        sx = (col % 64 + 64) % 64;
                    } else if (w <= col) {
                        mx = col % w;
                    }

                    if (sx >= 64) {
                        sx = sx % 64;
                    }

                    if ((sx / 32) & 1) {
                        rowHi[sx % 32] = mapRow[mx];
                    } else {
                        rowLo[sx % 32] = mapRow[mx];
                    }
                }

                ty++;
            }
        }
    }

    this->work.screenDirty[layer] = 0xff;
    this->work.layerX[layer] = x;
    this->work.layerY[layer] = y;
}

// MATCHED 100%
void clFieldMap::scrollLayer(u8 layer, s32 dx, s32 dy) {
    s32 edge;
    s32 sy;
    s32 oobX;
    s32 oobY;
    s32 cur;
    s16 oldTx;
    s16 oldTy;
    s32 col;
    s32 row;

    if (MATH_ABS(dx) >= 0x800 || MATH_ABS(dy) >= 0x800) {
        this->work.layerX[layer] += dx;
        this->work.layerY[layer] += dy;
        this->setScroll(this->work.layerX[layer], this->work.layerY[layer]);
        return;
    }

    oobX = 0;
    oobY = 0;

    if (mapChunkIdx[this->work.mapId].idx[layer] == -1) {
        return;
    }

    oldTx = (u16)(this->work.layerX[layer] >> 11);
    oldTy = (u16)(this->work.layerY[layer] >> 11);

    this->work.layerX[layer] += dx;
    this->work.layerY[layer] += dy;

    if (this->work.layerX[layer] >> 8 >= this->work.wrapW) {
        this->work.layerX[layer] -= this->work.wrapW << 8;
        oldTx -= this->work.wrapW >> 3;
    } else if (this->work.layerX[layer] >> 8 <= -this->work.wrapW) {
        this->work.layerX[layer] += this->work.wrapW << 8;
        oldTx += this->work.wrapW >> 3;
    }

    if (this->work.layerY[layer] >> 8 >= this->work.wrapH) {
        this->work.layerY[layer] -= this->work.wrapH << 8;
        oldTy -= this->work.wrapH >> 3;
    } else if (this->work.layerY[layer] >> 8 <= -this->work.wrapH) {
        this->work.layerY[layer] += this->work.wrapH << 8;
        oldTy += this->work.wrapH >> 3;
    }

    s16 newTx = (u16)(this->work.layerX[layer] >> 11);
    s16 newTy = (u16)(this->work.layerY[layer] >> 11);
    if (oldTx == newTx && oldTy == newTy) {
        return;
    }

    if (oldTx != newTx) {
        s32 mapCol;
        if (oldTx > newTx) {
            mapCol = edge = newTx - 1;
        } else {
            mapCol = edge = newTx + 32;
        }

        if (mapCol < 0) {
            oobX = 1;
            mapCol = (this->work.width + mapCol % this->work.width) % this->work.width;
        } else if (this->work.width <= mapCol) {
            oobX = 1;
            mapCol = mapCol % this->work.width;
        }

        if (edge < 0) {
            edge = (edge % 64 + 64) % 64;
        } else if (edge >= 64) {
            edge = edge % 64;
        }

        cur = newTy - 1;
        s32 hiHalf = (edge / 32) & 1;
        s32 colOfs = edge % 32;
        for (row = -1; row < 25; row++) {
            sy = cur;
            s32 m = cur;
            if (cur < 0) {
                oobY = 1;
                m = (this->work.height + cur % this->work.height) % this->work.height;
            } else if (this->work.height <= cur) {
                oobY = 1;
                m = cur % this->work.height;
            }

            if (cur < 0) {
                sy = (cur % 32 + 32) % 32;
            } else if (cur >= 32) {
                sy = sy % 32;
            }

            s32 tile = ((oobX != 0 || oobY != 0) && (this->work.clipMask & (1 << layer))) ? 0 : this->work.sectionsU16[layer].data[m * this->work.width + mapCol];

            if (hiHalf == 0) {
                this->work.screenBuf[layer][(sy % 32) * 32 + colOfs] = tile;
            } else {
                this->work.screenBuf[layer][(sy % 32) * 32 + colOfs + 0x400] = tile;
            }
            oobY = 0;
            cur++;
        }

        if (oldTx > newTx) {
            this->work.screenDirty[layer] |= 4;
        } else {
            this->work.screenDirty[layer] |= 8;
        }
        oobX = 0;
    }

    if (oldTy == newTy) {
        return;
    }

    s32 mapRow;
    if (oldTy > newTy) {
        mapRow = edge = newTy - 1;
    } else {
        mapRow = edge = newTy + 24;
    }

    if (mapRow < 0) {
        oobY = 1;
        mapRow = (this->work.height + mapRow % this->work.height) % this->work.height;
    } else if (this->work.height <= mapRow) {
        oobY = 1;
        mapRow = mapRow % this->work.height;
    }

    if (edge < 0) {
        edge = (edge % 32 + 32) % 32;
    } else if (edge >= 32) {
        edge = edge % 32;
    }

    s32 x = newTx - 1;
    s32 rowOfs = (edge % 32) * 32;
    for (col = -1; col < 33; col++) {
        cur = x;
        s32 m = x;
        if (x < 0) {
            oobX = 1;
            m = (this->work.width + x % this->work.width) % this->work.width;
        } else if (this->work.width <= x) {
            oobX = 1;
            m = x % this->work.width;
        }

        if (x < 0) {
            cur = (x % 64 + 64) % 64;
        } else if (x >= 64) {
            cur = cur % 64;
        }

        s32 tile = ((oobX != 0 || oobY != 0) && (this->work.clipMask & (1 << layer))) ? 0 : this->work.sectionsU16[layer].data[mapRow * this->work.width + m];

        if (((cur / 32) & 1) == 0) {
            this->work.screenBuf[layer][rowOfs + cur % 32] = tile;
        } else {
            this->work.screenBuf[layer][rowOfs + cur % 32 + 0x400] = tile;
        }

        oobX = 0;
        x++;
    }

    if (oldTy > newTy) {
        this->work.screenDirty[layer] |= 1;
    } else {
        this->work.screenDirty[layer] |= 2;
    }
}

// MATCHED 100%
void clFieldMap::setScrollDelta(s32 dx, s32 dy) {
    this->work.scrollDX = dx;
    this->work.scrollDY = dy;
}

// MATCHED 100%
void clFieldMap::scrollTo(s32 x, s32 y) {
    s32 dx = x - this->work.scrollX;
    s32 dy = y - this->work.scrollY;

    if (MATH_ABS(dx) >= 0x800 || MATH_ABS(dy) >= 0x800) {
        this->setScroll(x, y);
        return;
    }

    this->setScrollDelta(dx, dy);

    if (this->work.mapId != 0x164 && this->work.mapId != 0x165 && (u16)(this->work.mapId + 0xff33) > 1) {
        return;
    }

    for (s32 layer = 0; layer < 3; layer++) {
        s32 rateX;
        s32 rateY;
        this->getLayerScrollRate(layer, &rateX, &rateY);

        s32 lx;
        s32 ly;
        s32 stepX;
        s32 stepY;
        if ((this->work.infoSection.data->fixedRateMask & (1 << layer)) == 0) {
            lx = (rateX * x) >> 8;
            ly = (rateY * y) >> 8;
            lx -= ((this->work.scrollX * rateX) >> 8) - this->work.layerX[layer];
            ly -= ((this->work.scrollY * rateY) >> 8) - this->work.layerY[layer];
            stepX = (rateX * this->work.scrollDX) >> 8;
            stepY = (rateY * this->work.scrollDY) >> 8;
        } else {
            lx = (rateX + this->work.layerX[layer]) + (x - this->work.scrollX);
            ly = (rateY + this->work.layerY[layer]) + (y - this->work.scrollY);
            stepX = rateX + this->work.scrollDX;
            stepY = rateY + this->work.scrollDY;
        }

        this->work.layerX[layer] = lx - stepX;
        this->work.layerY[layer] = ly - stepY;
    }
}

// MATCHED 100%
void clFieldMap::setPlanesVisible(s8 bg0, s8 bg1, s8 bg2, s8 bg3, s8 tex0, s8 tex1, s8 tex2) {
    int planes;

    if (this->work.isSub == 0) {
        planes = GX_GetVisiblePlane();
    } else {
        planes = GXS_GetVisiblePlane();
    }

    if (bg0 == 1) {
        planes |= GX_PLANEMASK_BG0;
    } else if (bg0 == 0) {
        planes &= ~GX_PLANEMASK_BG0;
    }
    if (bg1 == 1) {
        planes |= GX_PLANEMASK_BG1;
    } else if (bg1 == 0) {
        planes &= ~GX_PLANEMASK_BG1;
    }
    if (bg2 == 1) {
        planes |= GX_PLANEMASK_BG2;
    } else if (bg2 == 0) {
        planes &= ~GX_PLANEMASK_BG2;
    }
    if (bg3 == 1) {
        planes |= GX_PLANEMASK_BG3;
    } else if (bg3 == 0) {
        planes &= ~GX_PLANEMASK_BG3;
    }

    if (this->work.isSub == 0) {
        GX_SetVisiblePlane(planes);
    } else {
        GXS_SetVisiblePlane(planes);
    }

    if (this->work.texMode == 1) {
        if (tex0 == 1) {
            this->work.texVisibleMask |= 1;
            this->work.texLayers[0].hidden = 0;
        } else if (tex0 == 0) {
            this->work.texVisibleMask &= ~1;
            this->work.texLayers[0].hidden = 1;
        }
        if (tex1 == 1) {
            this->work.texVisibleMask |= 2;
            this->work.texLayers[1].hidden = 0;
        } else if (tex1 == 0) {
            this->work.texVisibleMask &= ~2;
            this->work.texLayers[1].hidden = 1;
        }
        if (tex2 == 1) {
            this->work.texVisibleMask |= 4;
            this->work.texLayers[2].hidden = 0;
        } else if (tex2 == 0) {
            this->work.texVisibleMask &= ~4;
            this->work.texLayers[2].hidden = 1;
        }
    }
}

// MATCHED 100%
void clFieldMap::moveLayerTo(u32 layer, s32 x, s32 y, s32 speed) {
    this->moveLayerToAccel(layer, x, y, speed, speed);
}

// MATCHED 100%
void clFieldMap::moveLayerBy(u32 layer, s32 dx, s32 dy, s32 speed) {
    this->moveLayerTo(layer, dx + this->work.layerX[layer], dy + this->work.layerY[layer], speed);
}

// MATCHED 100%
void clFieldMap::moveLayerToFrames(u32 layer, s32 x, s32 y, u16 frames) {
    s32 dx = (x - this->work.layerX[layer]) / 128;
    s32 dy = (y - this->work.layerY[layer]) / 128;
    CP_SetSqrt32(dx * dx + dy * dy);

    u32 dist = CP_GetSqrtResult32() * 128;
    if (dist != 0) {
        LayerMove * move = &this->work.moves[layer];
        move->active = 1;
        move->dist = dist;
        move->frame = 0;
        move->frames = frames;
        move->curX = this->work.layerX[layer];
        move->curY = this->work.layerY[layer];
        move->startX = this->work.layerX[layer];
        move->startY = this->work.layerY[layer];
        move->dstX = x;
        move->dstY = y;
        move->speedEnd = move->speedStart = move->dist / frames;
        move->accel = 0;
    }
}

// MATCHED 100%
void clFieldMap::moveLayerByFrames(u32 layer, s32 dx, s32 dy, u16 frames) {
    this->moveLayerToFrames(layer, dx + this->work.layerX[layer], dy + this->work.layerY[layer], frames);
}

// MATCHED 100%
void clFieldMap::moveLayerToAccel(u32 layer, s32 x, s32 y, s32 speedStart, s32 speedEnd) {
    s32 dy = (y - this->work.layerY[layer]) / 128;
    s32 dx = (x - this->work.layerX[layer]) / 128;
    CP_SetSqrt32(dx * dx + dy * dy);

    u32 dist = CP_GetSqrtResult32() * 128;
    if (dist != 0) {
        LayerMove * move = &this->work.moves[layer];
        move->active = 1;
        move->dist = dist;
        move->frame = 0;
        move->frames = (move->dist * 2) / (speedStart + speedEnd);
        move->curX = this->work.layerX[layer];
        move->curY = this->work.layerY[layer];
        move->startX = this->work.layerX[layer];
        move->startY = this->work.layerY[layer];
        move->dstX = x;
        move->dstY = y;
        move->speedStart = speedStart;
        move->speedEnd = speedEnd;
        move->accel = (speedEnd - speedStart) / move->frames;
    }
}

// MATCHED 100%
void clFieldMap::moveLayerByAccel(u32 layer, s32 dx, s32 dy, s32 speedStart, s32 speedEnd) {
    this->moveLayerToAccel(layer, dx + this->work.layerX[layer], dy + this->work.layerY[layer], speedStart, speedEnd);
}

// MATCHED 100%
void clFieldMap::getScroll(s32 * x, s32 * y) {
    *x = this->work.scrollX;
    *y = this->work.scrollY;
}

// MATCHED 100%
void clFieldMap::syncLayerScroll(s32 mask) {
    for (s32 layer = 0; layer < 3; layer++) {
        if ((mask >> layer) & 1) {
            this->work.layerX[layer] = this->work.scrollX;
            this->work.layerY[layer] = this->work.scrollY;
        }
    }
}

// MATCHED 100%
BOOL clFieldMap::isReady() {
    return (this->work.state == 0x65);
}

// MATCHED 100%
u32 clFieldMap::getLayerChunkIdx(u32 layer) {
    return mapChunkIdx[this->work.mapId].idx[layer];
}

// MATCHED 100%
MapInfo * clFieldMap::getMapInfo() {
    return this->work.infoSection.data;
}

// MATCHED 100%
void * clFieldMap::func_ov003_0206ffd8() {
    return (this->work.sections[12].size == 0) ? NULL : this->work.sections[12].data;
}

// MATCHED 100%
ZRectList * clFieldMap::getZRects() {
    return (this->work.zRectSection.size == 0) ? NULL : this->work.zRectSection.data;
}

// MATCHED 100%
void * clFieldMap::func_ov003_02070000() {
    return (this->work.sections[7].size == 0) ? NULL : this->work.sections[7].data;
}

// MATCHED 100%
u32 clFieldMap::func_ov003_02070014() { return 0; }

// MATCHED 100%
void * clFieldMap::func_ov003_0207001c() {
    return (this->work.sections[16].size == 0) ? NULL : this->work.sections[16].data;
}

// MATCHED 100%
u8 * clFieldMap::applyMapPatch(s16 idx) {
    ZRect * rects;
    MapPatchRect * pr;
    ZRect * rect;

    if (this->work.mapPatches[idx].setFlag != 0x1fff) {
        BITSET_SET(gameWork.field05c, this->work.mapPatches[idx].setFlag);
    }
    if (this->work.mapPatches[idx].clearFlag != 0x1fff) {
        BITSET_CLR(gameWork.field05c, this->work.mapPatches[idx].clearFlag);
    }

    if (this->work.texMode == 1 && this->work.mapPatches[idx].rects != NULL) {
        ZRectList * list = this->getZRects();
        rects = (ZRect *)list;

        u32 count;
        if (list != NULL) {
            rects = list->rects;
            count = list->count;
        } else {
            count = 0;
        }

        if (count != 0) {
            ZRect saved;
            MapPatchRect * first = this->work.mapPatches[idx].rects;
            rect = &saved;

            pr = first;
            while (TRUE) {
                if (&rects[pr->idx] < rect) {
                    cpu_copy32_rev(&rects[pr->idx], rect, sizeof(ZRect));
                } else {
                    cpu_copy32_fwd(&rects[pr->idx], rect, sizeof(ZRect));
                }

                saved.value = 0;
                this->fillZRect(rect);
                this->addDirtyRect(saved.x0 >> 3, saved.y0 >> 3, saved.x1 >> 3, saved.y1 >> 3);

                if (pr->last != 0) {
                    break;
                }
                pr++;
            }

            pr = first;
            while (TRUE) {
                rect = &rects[pr->idx];
                if (&pr->rect < rect) {
                    cpu_copy32_rev(&pr->rect, rect, sizeof(ZRect));
                } else {
                    cpu_copy32_fwd(&pr->rect, rect, sizeof(ZRect));
                }

                this->fillZRect(rect);
                this->addDirtyRect(rect->x0 >> 3, rect->y0 >> 3, rect->x1 >> 3, rect->y1 >> 3);
                
                if (pr->last != 0) {
                    break;
                }
                pr++;
            }
        }
    }

    if (this->work.mapPatches[idx].tiles != NULL) {
        this->patchTilemap(idx);
        this->redrawLayer(0, this->work.layerX[0], this->work.layerY[0]);
        this->redrawLayer(1, this->work.layerX[1], this->work.layerY[1]);
        this->redrawLayer(2, this->work.layerX[2], this->work.layerY[2]);

        if (this->work.texMode == 1) {
            MapPatch * patch = &this->work.mapPatches[idx];
            this->addDirtyRect(patch->x, patch->y, patch->x + patch->w - 1, patch->y + patch->h - 1);
        }
    }

    return this->work.mapPatches[idx].data;
}

// MATCHED 100%
void clFieldMap::patchTilemap(u32 idx) {
    s32 x;
    s32 n;

    for (s32 layer = 0; layer < 3; layer++) {
        u16 * map = this->work.sectionsU16[layer].data;
        u16 * src = this->work.mapPatches[idx].tiles + this->work.mapPatches[idx].w * this->work.mapPatches[idx].h * layer;
        n = 0;

        if (this->work.texMode == 1) {
            for (s32 y = 0; y < this->work.mapPatches[idx].h; y++) {
                s32 rowOfs = (this->work.mapPatches[idx].y + y) * this->work.width + this->work.mapPatches[idx].x;
                for (x = 0; x < this->work.mapPatches[idx].w; x++) {
                    u16 tile = src[n];
                    if ((tile & 0x3ff) != 0x3ff) {
                        if ((tile & 0x3ff) == 0) {
                            map[rowOfs + x] = 0xffff;
                        } else {
                            map[rowOfs + x] = tile;
                        }
                    }
                    n++;
                }
            }
        } else {
            for (s32 y = 0; y < this->work.mapPatches[idx].h; y++) {
                s32 rowOfs = (this->work.mapPatches[idx].y + y) * this->work.width + this->work.mapPatches[idx].x;
                for (s32 x = 0; x < this->work.mapPatches[idx].w; x++) {
                    u16 tile = src[n];
                    n++;

                    if ((tile & 0x3ff) != 0x3ff) {
                        map[rowOfs + x] = tile;
                    }
                }
            }
        }
    }
}

// MATCHED 100%
u8 * clFieldMap::getMapPatchData(u32 idx) {
    return this->work.mapPatches[idx].data;
}

// MATCHED 100%
BOOL clFieldMap::startTileAnim(u32 idx) {
    if (this->work.tileAnims[idx].def->mode >= 2 && this->work.tileAnims[idx].active == 0) {
        this->restartTileAnim(idx);
        return TRUE;
    }
    return FALSE;
}

// MATCHED 100%
void clFieldMap::restartTileAnim(u32 idx) {
    u32 size;
    u32 layer;

    u32 tileSize = this->getTileSize(this->work.tileAnims[idx].def->layer);
    TileAnimDef * def = this->work.tileAnims[idx].def;
    size = tileSize * def->tileCount;
    layer = def->layer;

    this->work.tileAnims[idx].active = 1;

    MapInfo * info = this->work.infoSection.data;
    if (this->work.texMode == 1 && ((info->texLayerMask >> layer) & 1)) {
        u8 * texVram = getTexVramAddr(GX_GetBankForTex());
        u8 * dst = texVram + this->work.texTileBase[layer] * 32;

        this->work.vblank->queueCopy(
            this->work.tileAnimGfx[idx] + size * this->work.tileAnims[(s32)idx].def->frames[0].srcFrame / 4,
            dst + tileSize * this->work.tileAnims[(s32)idx].def->dstTile,
            size
        );
    } else {
        TileAnimDef * def = this->work.tileAnims[(s32)idx].def;
        u16 * charAddr = this->getBgCharAddr(this->work.charBlock[def->layer]);
        
        this->work.vblank->queueDmaCopy(
            this->work.tileAnimGfx[idx] + size * this->work.tileAnims[(s32)idx].def->frames[0].srcFrame / 4,
            charAddr + tileSize * def->dstTile / 2,
            size
        );
    }

    this->work.tileAnims[idx].timer = (this->work.tileAnims[idx].def->frames[0].duration << 8);
    this->work.tileAnims[idx].frame = 0;
}

// MATCHED 100%
u32 clFieldMap::getTileSize(u32 layer) {
    return (((this->work.color256Mask >> layer) & 1) + 1) << 5;
}

// MATCHED 100%
void clFieldMap::updateTileAnims() {
    for (s32 i = 0; i < this->work.tileAnimCount; i++) {
        if (this->work.tileAnims[i].active == 1) {
            s32 tmp = this->work.tileAnims[i].timer - this->work.timeStep;
            this->work.tileAnims[i].timer = tmp;

            if (tmp <= 0) {
                TileAnimDef * def = this->work.tileAnims[i].def;
                TileAnimFrame * frames = def->frames;
                u32 tileSize = this->getTileSize(def->layer);
                u32 size = tileSize * this->work.tileAnims[i].def->tileCount;
                
                this->work.tileAnims[i].frame++;

                def = this->work.tileAnims[i].def;
                if (def->frameCount <= this->work.tileAnims[i].frame) {
                    this->work.tileAnims[i].frame = 0;
                    def = this->work.tileAnims[i].def;

                    if (def->mode & 1) {
                        this->work.tileAnims[i].active = 0;
                        continue;
                    }
                }

                tmp = def->layer;

                MapInfo * info = this->work.infoSection.data;
                if (this->work.texMode == 1 && ((info->texLayerMask >> tmp) & 1)) {
                    u8 * texVram = getTexVramAddr(GX_GetBankForTex());
                    u8 * dst = texVram + this->work.texTileBase[tmp] * 32;

                    this->work.vblank->queueCopy(
                        this->work.tileAnimGfx[i] + size * frames[this->work.tileAnims[i].frame].srcFrame / 4,
                        dst + tileSize * this->work.tileAnims[i].def->dstTile,
                        size
                    );
                } else {
                    u16 * charAddr = this->getBgCharAddr(this->work.charBlock[tmp]);

                    this->work.vblank->queueDmaCopy(
                        this->work.tileAnimGfx[i] + size * frames[this->work.tileAnims[i].frame].srcFrame / 4,
                        charAddr + tileSize * def->dstTile / 2,
                        size
                    );
                }

                this->work.tileAnims[i].timer += frames[this->work.tileAnims[i].frame].duration << 8;
            }
        }
    }
}

// MATCHED 100%
BOOL clFieldMap::stopTileAnim(u32 idx) {
    if (this->work.tileAnims[idx].active == 0) {
        return FALSE;
    }

    this->work.tileAnims[idx].active = 0;
    return TRUE;
}

// MATCHED 100%
BOOL clFieldMap::resumeTileAnim(u32 idx) {
    if (this->work.tileAnims[idx].active == 1) {
        return FALSE;
    }

    this->work.tileAnims[idx].active = 1;
    return TRUE;
}

// MATCHED 100%
void clFieldMap::reloadPalettes(u8 mask) {
    this->restorePalettes(mask);
    this->work.vblank->work.palDirty |= mask;
    this->work.palDirty |= mask;
}

// MATCHED 100%
void clFieldMap::restorePalettes(u8 mask) {
    s32 layer;
    s32 shared = -1;

    for (layer = 0; layer < 3; layer++) {
        if (((mask >> layer) & 1) && this->work.sectionsU16[layer+3].size != 0) {
            if (this->work.color256Mask & (1 << layer)) {
                if (this->work.sectionsU16[layer+3].data < this->work.layerPal[layer]) {
                    cpu_copy32_rev(this->work.sectionsU16[layer+3].data, this->work.layerPal[layer], this->work.sectionsU16[layer+3].size);
                } else {
                    cpu_copy32_fwd(this->work.sectionsU16[layer+3].data, this->work.layerPal[layer], this->work.sectionsU16[layer+3].size);
                }
            } else {
                shared = layer;
            }
        }
    }

    if (shared != -1) {
        if (this->work.basePalSrc < this->work.basePal) {
            cpu_copy32_rev(this->work.basePalSrc, this->work.basePal, 0x200);
        } else {
            cpu_copy32_fwd(this->work.basePalSrc, this->work.basePal, 0x200);
        }
    }
}

// MATCHED 100%
void clFieldMap::uploadPalettes() {
    s32 dirty = this->work.palDirty | this->work.palAnimMask | this->work.extPafMask;
    
    BOOL dirty0 = dirty & 1;
    if (dirty0 && this->work.layerPal[0] != NULL) {
        DC_FlushRange(this->work.layerPal[0], this->work.sectionsU16[3].size);
    }

    BOOL dirty1 = (dirty >> 1) & 1;
    if (dirty1 && this->work.layerPal[1] != NULL) {
        DC_FlushRange(this->work.layerPal[1], this->work.sectionsU16[4].size);
    }

    BOOL dirty2 = (dirty >> 2) & 1;
    if (dirty2 && this->work.layerPal[2] != NULL) {
        DC_FlushRange(this->work.layerPal[2], this->work.sectionsU16[5].size);
    }

    BOOL dirtyBase = (dirty >> 3) & 1;
    if (dirtyBase) {
        DC_FlushRange(this->work.basePal, 0x200);
    }

    if (this->work.isSub == 0) {
        if (this->work.texMode != 1) {
            if (dirty & 7) {
                GX_BeginLoadBGExtPltt();

                if (dirty0 && (this->work.color256Mask & 1) && this->work.sectionsU16[3].size != 0) {
                    GX_LoadBGExtPltt(this->work.layerPal[0], 0x2000, this->work.sectionsU16[3].size);
                }
                if (dirty1 && (this->work.color256Mask & 2) && this->work.sectionsU16[4].size != 0) {
                    GX_LoadBGExtPltt(this->work.layerPal[1], 0x4000, this->work.sectionsU16[4].size);
                }
                if (dirty2 && (this->work.color256Mask & 4) && this->work.sectionsU16[5].size != 0) {
                    GX_LoadBGExtPltt(this->work.layerPal[2], 0x6000, this->work.sectionsU16[5].size);
                }

                GX_EndLoadBGExtPltt();
            }

            if (dirtyBase) {
                if (!(this->work.color256Mask & 1) || !(this->work.color256Mask & 2) || !(this->work.color256Mask & 4) || this->work.basePalTaken == 1) {
                    GX_LoadBGPltt(this->work.basePal, 0, 0x200);
                } else {
                    *(u16 *)HW_BG_PLTT = *this->work.sectionsU16[5].data;
                    *this->work.basePalSrc = *this->work.sectionsU16[5].data;
                }
            }
        }
    } else {
        if (GX_GetBankForSubBGExtPltt() != GX_VRAM_SUB_BGEXTPLTT_NONE && (dirty & 7)) {
            GXS_BeginLoadBGExtPltt();
            
            if (dirty0 && (this->work.color256Mask & 1) && this->work.sectionsU16[3].size != 0) {
                GXS_LoadBGExtPltt(this->work.layerPal[0], 0x2000, this->work.sectionsU16[3].size);
            }
            if (dirty1 && (this->work.color256Mask & 2) && this->work.sectionsU16[4].size != 0) {
                GXS_LoadBGExtPltt(this->work.layerPal[1], 0x4000, this->work.sectionsU16[4].size);
            }
            if (dirty2 && (this->work.color256Mask & 4) && this->work.sectionsU16[5].size != 0) {
                GXS_LoadBGExtPltt(this->work.layerPal[2], 0x6000, this->work.sectionsU16[5].size);
            }

            GXS_EndLoadBGExtPltt();
        }

        if (dirtyBase) {
            if (!(this->work.color256Mask & 1) || !(this->work.color256Mask & 2) || !(this->work.color256Mask & 4) || this->work.basePalTaken == 1) {
                GXS_LoadBGPltt(this->work.basePal, 0, 0x200);
            } else {
                *(u16 *)HW_DB_BG_PLTT = *this->work.sectionsU16[5].data;
                *this->work.basePalSrc = *this->work.sectionsU16[5].data;
            }
        }
    }
}

// MATCHED 100%
void clFieldMap::allocPalettes() {
    s32 layer;
    s32 shared = -1;

    this->work.basePalSrc = new (this->work.heapId, NULL, FALSE) u16[0x100];
    cpu_fill32(this->work.basePalSrc, 0, 0x100 * sizeof(u16));

    for (layer = 0; layer < 3; layer++) {
        if (this->work.sectionsU16[layer+3].size != 0) {
            if (this->work.color256Mask & (1 << layer)) {
                this->work.layerPal[layer] = new (this->work.heapId, NULL, FALSE) u16[this->work.sectionsU16[layer+3].size / sizeof(u16)];
                MIi_CpuCopy32(this->work.sectionsU16[layer+3].data, this->work.layerPal[layer], this->work.sectionsU16[layer+3].size);
            } else {
                shared = (s8)layer;
            }
        }
    }

    this->work.basePal = new (this->work.heapId, NULL, FALSE) u16[0x100];

    if (shared != -1) {
        void * dst = this->work.basePal;
        if (this->work.sectionsU16[shared+3].data < dst) {
            cpu_copy32_rev(this->work.sectionsU16[shared+3].data, dst, 0x100 * sizeof(u16));
        } else {
            cpu_copy32_fwd(this->work.sectionsU16[shared+3].data, dst, 0x100* sizeof(u16));
        }

        dst = this->work.basePalSrc;
        if (this->work.sectionsU16[shared+3].data < dst) {
            cpu_copy32_rev(this->work.sectionsU16[shared+3].data, dst, 0x100 * sizeof(u16));
        } else {
            cpu_copy32_fwd(this->work.sectionsU16[shared+3].data, dst, 0x100 * sizeof(u16));
        }
    }
}

// MATCHED 100%
void clFieldMap::flushPalettes() {
    this->uploadPalettes();
}

// MATCHED 100%
void clFieldMap::attachPaf(u32 layer, clFPaf ** paf) {
    if (((this->work.color256Mask >> layer) & 1) == 0) {
        layer = 3;
    }

    this->work.extPafMask &= ~(1 << layer);
    this->work.extPafMask |= (paf != NULL && *paf != NULL) << layer;
    this->work.extPaf[layer] = paf;
}

// MATCHED 100%
BOOL clFieldMap::hasPaf(u32 layer) {
    if (((this->work.color256Mask >> layer) & 1) == 0) {
        layer = 3;
    }
    return (((this->work.extPafMask >> layer) & 1) != 0);
}

// MATCHED 100%
BOOL clFieldMap::startBlendAnim(u8 idx) {
    u32 * table = this->work.blendSection.data;

    this->work.blendAnim = (BlendAnim *)((u32)table + (table[idx] & ~3));

    this->work.vblank->queueBlend(
        this->work.blendAnim->plane1 * 2,
        this->work.blendAnim->plane2 * 2,
        this->work.blendAnim->steps[0].eva,
        this->work.blendAnim->steps[0].evb
    );

    this->work.blendTimer = (this->work.blendAnim->steps[0].duration << 8);
    this->work.blendStep = 0;
    this->work.blendAnimIdx = idx;
    this->work.blendPaused = 0;

    return TRUE;
}

// MATCHED 100%
BOOL clFieldMap::stopBlendAnim() {
    if (this->work.blendAnim != NULL) {
        this->work.blendAnim = NULL;
        return TRUE;
    }
    return FALSE;
}

// MATCHED 100%
BOOL clFieldMap::pauseBlendAnim() {
    this->work.blendPaused = 1;
    return TRUE;
}

// MATCHED 100%
BOOL clFieldMap::resumeBlendAnim(u32 idx) {
    u32 * table = this->work.blendSection.data;
    u8 prevIdx = this->work.blendAnimIdx;

    if (idx != 0xffff) {
        this->work.blendAnimIdx = idx;
    }

    if (prevIdx != this->work.blendAnimIdx) {
        this->work.blendAnim = (BlendAnim *)&table[table[this->work.blendAnimIdx] / 4];
    }

    this->work.blendPaused = 0;
    return TRUE;
}

// MATCHED 100%
void clFieldMap::updateBlendAnim() {
    if (this->work.blendAnim != NULL && this->work.blendPaused == 0) {
        this->work.blendTimer -= this->work.timeStep;

        if (this->work.blendTimer <= 0) {
            BlendAnimStep * steps = this->work.blendAnim->steps;
            this->work.blendStep++;

            if (this->work.blendAnim->stepCount <= this->work.blendStep) {
                this->work.blendStep = 0;

                if (this->work.blendAnim->once == 1) {
                    this->work.blendAnim = NULL;
                    return;
                }
            }

            this->work.vblank->queueBlend(
                this->work.blendAnim->plane1 * 2,
                this->work.blendAnim->plane2 * 2,
                steps[this->work.blendStep].eva,
                steps[this->work.blendStep].evb
            );

            this->work.blendTimer += steps[this->work.blendStep].duration << 8;
        }
    }
}

// MATCHED 100%
void clFieldMap::setupTexLayers() {
    if (this->work.texMode == 1) {
        MapInfo * info = this->work.infoSection.data;
        this->buildTexTiles();
        this->work.texPal16Layer = -1;

        s32 layer;
        u32 mask = info->texLayerMask;
        for (layer = 0; layer < 3; layer++) {
            if ((mask >> layer) & 1) {
                if (((info->color256Mask >> layer) & 1) == 0) {
                    this->work.texPal16Layer = layer;
                    break;
                }
            }
        }

        u32 base = this->work.texPalBase[0];
        u32 next = base;
        if (this->work.texPal16Layer >= 0) {
            next = base + 0x10;
        }

        for (layer = 0; layer < 3; layer++) {
            if ((info->texLayerMask >> layer) & 1) {
                if ((info->color256Mask >> layer) & 1) {
                    this->work.texPalBase[layer] = next;
                    next += 0x10;
                } else {
                    this->work.texPalBase[layer] = base;
                }
            }
        }
    }
}

// MATCHED 100%
void clFieldMap::buildTexTiles(void) {
    MapInfo * info = this->work.infoSection.data;
    s32 tile;
    s32 tile2;
    s32 otherCount;
    s32 layer;

    if (this->work.gfxToRam) {
        u16 * others[3];
        s32 tileCount = info->width * info->height;

        for (layer = 0; layer < 3; layer++) {
            if (this->work.layerGfxSize[layer] != 0 && ((info->texLayerMask >> layer) & 1)) {
                otherCount = 0;
                for (s32 other = layer + 1; other < 3; other++) {
                    if (this->work.layerGfxSize[other] != 0 && ((info->texLayerMask >> other) & 1)) {
                        if (this->work.sectionsU16[layer].data == this->work.sectionsU16[other].data) {
                            break;
                        }

                        others[otherCount] = this->work.sectionsU16[other].data;
                        otherCount++;
                    }
                }

                for (tile = 0; tile < tileCount; tile++) {
                    if (this->func_ov003_02071734(layer, tile)) {
                        func_ov006_02121fc4(tile, this->work.sectionsU16[layer].data, this->work.layerGfxBuf[layer], (info->color256Mask & (1 << layer)) != 0);
                    }
                    func_ov006_02122040(tile, this->work.sectionsU16[layer].data, this->work.layerGfxBuf[layer], (info->color256Mask & (1 << layer)) != 0, others, otherCount, this->work.tileAnims, this->work.tileAnimCount, layer);
                }
            }
        }
    } else {
        u16 * others[3];
        GXVRamTex texBank = GX_ResetBankForTex();
        GXVRamTexPltt plttBank = GX_ResetBankForTexPltt();
        s32 tileCount = info->width * info->height;

        for (layer = 0; layer < 3; layer++) {
            if (this->work.layerGfxSize[layer] != 0 && ((info->texLayerMask >> layer) & 1)) {
                s32 otherCount = 0;
                for (s32 other = layer + 1; other < 3; other++) {
                    if (this->work.layerGfxSize[other] != 0 && ((info->texLayerMask >> other) & 1)) {
                        if (this->work.sectionsU16[layer].data == this->work.sectionsU16[other].data) {
                            break;
                        }

                        others[otherCount] = this->work.sectionsU16[other].data;
                        otherCount++;
                    }
                }

                for (tile2 = 0; tile2 < tileCount; tile2++) {
                    if (this->func_ov003_02071734(layer, tile2)) {
                        func_ov006_02121fc4(tile2, this->work.sectionsU16[layer].data, this->work.texVram + this->work.texTileBase[layer] * 32, (info->color256Mask & (1 << layer)) != 0);
                    }
                    func_ov006_02122040(tile2, this->work.sectionsU16[layer].data, this->work.texVram + this->work.texTileBase[layer] * 32, (info->color256Mask & (1 << layer)) != 0, others, otherCount, this->work.tileAnims, this->work.tileAnimCount, layer);
                }
            }
        }

        GX_SetBankForTex(texBank);
        GX_SetBankForTexPltt(plttBank);
    }
}

// MATCHED 100%
u16 clFieldMap::func_ov003_020715fc(u32 param1, u32 param2) {
    s32 r7;
    u16 r10 = this->work.field808.fielda14;

    if (this->work.texMode == 1 && this->work.field808.fielda14 != param1) {
        if (param1 == 0) {
            if (this->work.field2600 != NULL) {
                delete[] this->work.field2600;
                this->work.field2600 = NULL;
            }
        } else if (this->work.field2600 == NULL) {
            if (param2 == -1) {
                param2 = this->work.heapId;
            }

            this->work.field2600 = new (param2, NULL, FALSE) u8[0x12c0c];
            cpu_fill32(this->work.field2600, 0, 0x12c0c);
        }

        func_ov006_02123500(&this->work.field808, param1, this->work.field2600);

        for (r7 = 0; r7 < 3; r7++) {
            s32 r0 = this->work.layerX[r7];
            s32 r2 = this->work.layerY[r7];

            this->work.texLayers[r7].scrollX = r0 >> 8;
            this->work.texLayers[r7].scrollY = r2 >> 8;

            TexLayer * r9 = &this->work.texLayers[r7];
            r9->field698_2 = 0;
            r9->hidden = 0;
            cpu_fill32(r9->cache, -1, sizeof(r9->cache));
        }
    }

    return r10;
}

// MATCHED 100%
BOOL clFieldMap::func_ov003_02071734(u8 param1, u16 param2) {
    u16 r2 = (this->work.sectionsU16[param1].data[param2] & 0x3ff);
    if (r2 == 0xffff) {
        return FALSE;
    }

    BOOL r4 = ((this->work.infoSection.data->color256Mask & (1 << param1)) != 0);

    u8 * local00;
    if (this->work.gfxToRam) {
        local00 = this->work.layerGfxBuf[param1];
    } else {
        local00 = this->work.texVram + this->work.texTileBase[param1] * 32;
    }

    for (s32 r11 = 0; r11 < this->work.tileAnimCount; r11++) {
        TileAnimDef * r5 = this->work.tileAnims[r11].def;

        if (param1 == r5->layer) {
            s32 r8;
            s32 r9 = r5->tileCount * r5->frameCount + r5->dstTile;
            BOOL r3 = (r2 >= (s32)r5->dstTile && r2 < r9);

            if (r3) {
                for (r8 = r2; r8 < r9; r8 += r5->tileCount) {
                    if (r4) {
                        u32 * r0 = (u32 *)(local00 + r8 * 64);
                        for (s32 r10 = 0; r10 < 16; r10++) {
                            if (r0[r10] != 0) {
                                return FALSE;
                            }
                        }
                    } else {
                        u32 * r0 = (u32 *)(local00 + r8 * 32);
                        for (s32 r10 = 0; r10 < 8; r10++) {
                            if (r0[r10] != 0) {
                                return FALSE;
                            }
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}

// MATCHED 100%
void clFieldMap::fillZRect(ZRect * rect) {
    BOOL gradient = ((rect->value & 0x8000) != 0);
    s16 x0 = rect->x0;
    s32 y0 = rect->y0 >> 3;
    s32 y1 = y0 + ((rect->y1 - rect->y0) >> 3);
    s32 bytes = ((rect->x1 - x0) >> 3) * 2;
    s32 y;

    TexLayer * tex = this->work.texLayers;
    for (s32 layer = 0; layer < 3; layer++, tex++) {
        if (((rect->layerMask >> layer) & 1) && tex->zTbl != NULL) {
            if (gradient) {
                for (y = y0; y < y1; y++) {
                    s32 value = rect->value - (y - y0) * 8;
                    if (value < 0) {
                        value = 0;
                    }
                    MIi_CpuClear16((u16)value, tex->zTbl + (tex->w * y + (x0 >> 3)), bytes);
                }
            } else {
                for (y = y0; y < y1; y++) {
                    MIi_CpuClear16(rect->value, tex->zTbl + (tex->w * y + (x0 >> 3)), bytes);
                }
            }
        }
    }
}

// MATCHED 100%
void clFieldMap::addDirtyRect(s32 x0, s32 y0, s32 x1, s32 y1) {
    if (x0 < this->work.dirtyX0) {
        this->work.dirtyX0 = x0;
    }
    if (y0 < this->work.dirtyY0) {
        this->work.dirtyY0 = y0;
    }
    if (x1 > this->work.dirtyX1) {
        this->work.dirtyX1 = x1;
    }
    if (y1 > this->work.dirtyY1) {
        this->work.dirtyY1 = y1;
    }

    this->work.dirtyRectSet = 1;
}

// MATCHED 100%
void clFieldMap::resetDirtyRect() {
    this->work.dirtyX0 = this->work.width;
    this->work.dirtyY0 = this->work.height;
    this->work.dirtyX1 = 0;
    this->work.dirtyY1 = 0;
}

// MATCHED 100%
u16 * clFieldMap::getBgCharAddr(u32 block) {
    if (this->work.isSub == 0) {
        return (u16 *)(HW_BG_VRAM + block * 0x4000);
    }

    if (GX_GetBankForSubBGExtPltt() == GX_VRAM_SUB_BGEXTPLTT_NONE) {
        return (u16 *)HW_DB_BG_VRAM;
    } else {
        return (u16 *)(HW_DB_BG_VRAM + block * 0x4000);
    }
}

// MATCHED 100%
void clFieldMap::getLayerScrollRate(u8 layer, s32 * rateX, s32 * rateY) {
    s32 rates[16] = {0x100, 0x40, 0x80, 0x200, -0x100, -0x40, -0x80, 0x200, 0, 0x400, -0x400, 0x180, -0x180};

    switch (layer) {
    case 0:
        *rateX = rates[this->work.infoSection.data->rateX0];
        *rateY = rates[this->work.infoSection.data->rateY0];
        break;
    case 1:
        *rateX = rates[this->work.infoSection.data->rateX1];
        *rateY = rates[this->work.infoSection.data->rateY1];
        break;
    case 2:
        *rateX = rates[this->work.infoSection.data->rateX2];
        *rateY = rates[this->work.infoSection.data->rateY2];
        break;
    }
}

// MATCHED 100%
u32 clFieldMap::calcWrapSize(u32 tiles) {
    for (s32 i = 0; i < 6; i++) {
        if (tiles & 1) {
            break;
        }

        tiles >>= 1;
    }

    if (tiles != 0) {
        return tiles << 9;
    }
    return 0x200;
}

// MATCHED 100%
void clFieldMap::setPalEffects(PalEffect * effects, u8 count, u32 reset) {
    this->work.palEffects = effects;
    this->work.palEffectCount = count;

    if (reset != 0) {
        for (s32 i = 0; i < this->work.palEffectCount; i++) {
            this->clearPalEffect(i);
        }
    }
}

// MATCHED 100%
void clFieldMap::startPalEffect(u32 idx, u16 mask0, u16 mask1, u16 mask2, u16 maskBase, s16 kind, u16 color, u32 dur, u8 endMode, s16 speed, u32 time, u8 done) {
    PalEffect * fx = &this->work.palEffects[idx];
    fx->kind = kind;
    fx->color = color;
    fx->rowMask[0] = mask0;
    fx->rowMask[1] = mask1;
    fx->rowMask[2] = mask2;
    fx->rowMask[3] = maskBase;
    fx->dur = dur;
    fx->time = time;
    fx->speed = speed;
    fx->endMode = endMode;
    fx->done = done;

    if (this->work.palAnimState == 2) {
        this->work.palAnimState = 3;
    }
}

// MATCHED 100%
void clFieldMap::clearPalEffect(u8 idx) {
    PalEffect * fx = &this->work.palEffects[idx];
    fx->kind = 0;
    fx->color = 0;
    fx->rowMask[0] = 0;
    fx->rowMask[1] = 0;
    fx->rowMask[2] = 0;
    fx->rowMask[3] = 0;
    fx->dur = 0;
    fx->time = 0;
    fx->speed = 0x100;
    fx->endMode = 0;
    fx->done = 1;
}

// MATCHED 100%
void clFieldMap::updatePalEffects(void) {
    u32 row;
    PalEffect * fx;
    u32 rows;
    s32 j;
    s32 layer;

    fx = this->work.palEffects;
    this->work.palEffectBusy = 0;

    for (s32 i = 0; i < this->work.palEffectCount; i++) {
        if (fx->kind != 0 && fx->done == 0) {
            s32 ended = 0;
            fx->time += fx->speed;
            if (fx->dur <= fx->time >> 8) {
                if (fx->endMode == 0) {
                    fx->time = fx->dur << 8;
                    ended = 1;
                } else if (fx->endMode == 1) {
                    fx->kind = -1;
                    fx->time = 0;
                } else {
                    fx->time = fx->time % (fx->dur << 8);
                }
            }

            if (ended == 0 && ((fx->rowMask[0] == 0 && fx->rowMask[1] == 0 && fx->rowMask[2] == 0 && fx->rowMask[3] == 0) || fx->kind == 0)) {
                if (fx->kind == -1) {
                    fx->kind = 0;
                    fx->done = 1;
                }
            } else {
                this->work.palEffectBusy = 1;

                s8 shared = -1;
                if (fx->kind == -1) {
                    for (j = 0; j < 3; j++) {
                        if (this->work.sectionsU16[j + 3].size != 0) {
                            if (this->work.color256Mask & (1 << j)) {
                                MIi_CpuCopy32(this->work.sectionsU16[j+3].data, this->work.layerPal[j], this->work.sectionsU16[j+3].size);
                            } else {
                                shared = j;
                            }
                        }
                    }

                    if (shared != -1) {
                        MIi_CpuCopy32(this->work.basePalSrc, this->work.basePal, 0x200);
                    }

                    fx->kind = 0;
                    fx->done = 1;
                } else {
                    u32 rowCount[3];
                    rowCount[0] = (this->work.sectionsU16[3].size >> 9) + ((this->work.sectionsU16[3].size & 0x1ff) != 0);
                    rowCount[1] = (this->work.sectionsU16[4].size >> 9) + ((this->work.sectionsU16[4].size & 0x1ff) != 0);
                    rowCount[2] = (this->work.sectionsU16[5].size >> 9) + ((this->work.sectionsU16[5].size & 0x1ff) != 0);

                    for (layer = 0; layer < 3; layer++) {
                        if (this->work.sectionsU16[layer + 3].size != 0) {
                            rows = rowCount[layer];
                            for (row = 0; row < rows; row++) {
                                if (fx->rowMask[layer] & (1 << row)) {
                                    s32 ofs = row * ((this->work.color256Mask & (1 << layer)) ? 0x100 : 0x10);
                                    palAnimRunOne(fx->kind, fx->color, fx->dur, fx->time, this->work.layerPal[layer] + ofs, 0x100,
                                                  (this->work.palAnimState == 1 ? this->work.layerPal[layer] : this->work.sectionsU16[layer + 3].data) + ofs, 0);
                                }
                            }

                            this->work.palDirty |= 1 << layer;
                        }
                    }

                    for (layer = 0; layer < 16; layer++) {
                        if (fx->rowMask[3] & (1 << layer)) {
                            palAnimRunOne(fx->kind, fx->color, fx->dur, fx->time, this->work.basePal + (layer << 4), 0x10,
                                          (this->work.palAnimState == 1 ? this->work.basePal : this->work.basePalSrc) + (layer << 4), 0);
                        }
                    }

                    this->work.palDirty |= 8;

                    if (ended != 0) {
                        fx->done = 1;
                        fx->kind = 0;
                    }
                }
            }
        }

        fx++;
    }
}

// MATCHED 100%
void clFieldMap::reversePalEffect(u32 idx) {
    PalEffect * fx = &this->work.palEffects[idx];
    fx->time = (fx->dur << 8) - fx->time;
    fx->kind = -fx->kind;
}

// MATCHED 100%
u16 * clFieldMap::getBasePalette() {
    this->work.basePalTaken = 1;
    return this->work.basePalSrc;
}

// MATCHED 100%
void clFieldMap::queuePaletteLoad(u32 offset, u16 size) {
    MIi_CpuCopy32(this->work.basePalSrc, this->work.basePal, 0x200);

    this->work.vblank->work.palDirty = 8;
    this->work.vblank->work.palLoadOffset = offset;
    this->work.vblank->work.palLoadSize = size;
}

// MATCHED 100%
void clFieldMap::markPaletteDirty(u32 layer) {
    if (this->work.layerPal[layer] == NULL && this->work.sectionsU16[layer+3].size != 0) {
        layer = 3;
    }

    this->work.palDirty |= (1 << layer);
}

// MATCHED 100%
void clFieldMap::markAllPalettesDirty() {
    this->work.palDirty = 15;
}

// MATCHED 100%
void clFieldMap::uploadTexPalettes() {
    if (this->work.texMode == 1) {
        s32 dirty = this->work.palDirty | this->work.palAnimMask | this->work.extPafMask;
        MapInfo * info = this->work.infoSection.data;
        u8 * plttVram = getTexPlttVramAddr(GX_GetBankForTexPltt());
        
        s8 r2 = this->work.texPal16Layer;
        if (r2 >= 0) {
            if (this->work.layerPal[r2] != NULL) {
                if ((dirty >> r2) & 1) {
                    this->work.vblank->queueCopy(this->work.layerPal[r2], plttVram, 0x200);
                }
            } else if (this->work.basePal != NULL && ((dirty >> 3) & 1)) {
                this->work.vblank->queueCopy(this->work.basePal, plttVram + this->work.texPalBase[r2] * 32, 0x200);
            }
        }

        if ((dirty & 1) && (info->texLayerMask & 1) && this->work.layerPal[0] != NULL && (info->color256Mask & 1)) {
            this->work.vblank->queueCopy(this->work.layerPal[0], plttVram + this->work.texPalBase[0] * 32, 0x200);
        }
        if (((dirty >> 1) & 1) && ((info->texLayerMask >> 1) & 1) && this->work.layerPal[1] != NULL && ((info->color256Mask >> 1) & 1)) {
            this->work.vblank->queueCopy(this->work.layerPal[1], plttVram + this->work.texPalBase[1] * 32, 0x200);
        }
        if (((dirty >> 2) & 1) && ((info->texLayerMask >> 2) & 1) && this->work.layerPal[2] != NULL && ((info->color256Mask >> 2) & 1)) {
            this->work.vblank->queueCopy(this->work.layerPal[2], plttVram + this->work.texPalBase[2] * 32, 0x200);
        }

        s8 r1 = this->work.lastBgLayer;
        if (((dirty >> r1) & 1) && r1 >= 0 && this->work.layerPal[r1] != NULL) {
            GX_LoadBGPltt(this->work.layerPal[r1], 0, 0x200);
        }

        if ((dirty >> 3) & 1) {
            if ((this->work.color256Mask & 1) == 0 || (this->work.color256Mask & 2) == 0 || (this->work.color256Mask & 4) == 0 || this->work.basePalTaken == 1) {
                GX_LoadBGPltt(this->work.basePal, 0, 0x200);
            }
        }
    }
}

// MATCHED 100%
void clFieldMap::init(u8 isSub, u32 texMode, u16 mapId, u32 heapId, u32 tmpHeapId, clDPP * dpp, u16 slot, s32 scrollDX, s32 scrollDY, u8 async) {
    cpu_fill32(&this->work, 0, sizeof(FieldMapWork));

    this->work.isSub = isSub;

    this->work.texMode = texMode;

    this->work.mapId = mapId;

    this->work.dpp = dpp;

    this->work.state = 0;

    this->work.heapId = heapId;
    this->work.tmpHeapId = tmpHeapId;

    this->work.scrollDX = scrollDX;
    this->work.scrollDY = scrollDY;

    this->work.timeStep = 0x100;

    this->work.gfxToRam = 0;
    this->work.async = async;

    this->work.fsSlot = slot;

    this->work.texVisibleMask = 7;

    this->work.vblank = new(this->work.heapId, NULL, FALSE) clFieldMapVBlank(8, 0, this);
}

// MATCHED 100%
clFieldMapVBlank::clFieldMapVBlank(u8 priority, u32 param2, clFieldMap * map) : clVCountTask(0xc0, priority, param2, map) {
    cpu_fill32(&this->work, 0, sizeof(this->work));

    this->work.offsetsSet = 0;

    for (s32 i = 0; i < 16; i++) {
        this->work.dmaSrc[i] = 0;
        this->work.dmaDst[i] = 0;
    }

    this->work.dmaCount = 0;

    this->work.blendPlane1 = 0;
    this->work.blendPlane2 = 0;
    this->work.blendEva = 0;
    this->work.blendEvb = 0;

    this->work.blendPending = 0;
    this->work.palDirty = 0;
    this->work.blendBg0 = 0;

    this->work.field0c = 0xffff;
    this->work.field0e = 0xffff;

    this->work.field10 = 0;
    this->work.field12 = 0;

    this->work.field18_2 = 0;
    this->work.field18_4 = 100;
    this->work.field18_12 = 12;
}

// MATCHED 100%
clFieldMapVBlank::~clFieldMapVBlank() {}

// MATCHED 100%
void clFieldMapVBlank::onVCount() {
    clFieldMap * map = (clFieldMap *)this->arg;
    GXVRamTex texBank;
    GXVRamTexPltt plttBank;

    if (map->work.texMode == 1) {
        if (this->work.palDirty != 0 || map->work.palDirty != 0 || map->work.palAnimMask != 0 || map->work.extPafMask != 0) {
            u32 saved = map->work.palDirty;

            map->work.palDirty |= this->work.palDirty;
            map->uploadTexPalettes();
            map->work.palDirty = saved;
        }
    }

    getTexVramAddr(GX_GetBankForTex());

    if (map->work.texMode == 1) {
        texBank = GX_ResetBankForTex();
        plttBank = GX_ResetBankForTexPltt();
    }

    if (this->work.copyHead != this->work.copyTail) {
        while (this->work.copyHead != this->work.copyTail) {
            VramCopy * copy = &this->work.copies[this->work.copyHead];
            if (copy->bytewise) {
                MI_CpuCopy8(copy->src, copy->dst, copy->size);
            } else if (copy->src < copy->dst) {
                cpu_copy32_rev(copy->src, copy->dst, copy->size);
            } else {
                cpu_copy32_fwd(copy->src, copy->dst, copy->size);
            }

            if (++this->work.copyHead >= 32) {
                this->work.copyHead = 0;
            }
        }
    }

    if (map->work.texMode == 1) {
        GX_SetBankForTex(texBank);
        GX_SetBankForTexPltt(plttBank);
    }

    if (this->work.offsetsSet != 0) {
        if (this->work.offsetsSet != 0xff) {
            return;
        }
    } else {
        this->setBgOffset(0, map->work.layerX[0] >> 8, map->work.layerY[0] >> 8);
        this->setBgOffset(1, map->work.layerX[1] >> 8, map->work.layerY[1] >> 8);
        this->setBgOffset(2, map->work.layerX[2] >> 8, map->work.layerY[2] >> 8);
        this->work.offsetsSet = 0xff;
    }

    if (map->work.screenDirty[0] != 0 || map->work.screenDirty[1] != 0 || map->work.screenDirty[2] != 0) {
        DC_FlushRange(map->work.screenBuf[0], 0x3000);
        this->flushScreens();
    }

    this->setBgOffset(0, map->work.layerX[0] >> 8, map->work.layerY[0] >> 8);
    this->setBgOffset(1, map->work.layerX[1] >> 8, map->work.layerY[1] >> 8);
    this->setBgOffset(2, map->work.layerX[2] >> 8, map->work.layerY[2] >> 8);

    if ((this->work.palDirty >> 3) & 1) {
        this->loadBgPalette();
        this->work.palDirty &= ~8;
    } else if (this->work.palDirty != 0 || map->work.palDirty != 0 || map->work.palAnimMask != 0 || map->work.extPafMask != 0) {
        u32 saved = map->work.palDirty;
        map->work.palDirty |= this->work.palDirty;
        map->uploadPalettes();
        map->work.palDirty = saved;

        this->work.palDirty = 0;
    }

    if (this->work.dmaCount != 0) {
        this->flushDmaCopies();
    }

    if (this->work.blendPending == 1) {
        this->applyBlend();
    }

    if (map->work.prioritiesDirty) {
        if (map->work.isSub == 0) {
            G2_SetBG1Priority(map->getLayerPriority(0));
            G2_SetBG2Priority(map->getLayerPriority(1));
            G2_SetBG3Priority(map->getLayerPriority(2));
        } else {
            G2S_SetBG1Priority(map->getLayerPriority(0));
            G2S_SetBG2Priority(map->getLayerPriority(1));
            G2S_SetBG3Priority(map->getLayerPriority(2));
        }
        map->work.prioritiesDirty = 0;
    }
}

// MATCHED 100%
u32 clFieldMap::getLayerPriority(u32 layer) {
    return (this->work.layerPriorities >> (layer * 2)) & 3;
}

// MATCHED 100%
void clFieldMapVBlank::flushScreens() {
    s32 layer;
    clFieldMap * map = (clFieldMap *)this->arg;

    for (layer = 0; layer < 3; layer++) {
        if (map->work.screenDirty[layer] == 0xff) {
            this->loadScreen(layer);
        } else if (map->work.screenDirty[layer] != 0) {
            u16 tx = map->work.layerX[layer] >> 11;
            u16 ty = map->work.layerY[layer] >> 11;
            u16 * dst = this->getScreenAddr(layer);

            this->copyScreenEdges(map->work.screenDirty[layer], tx, ty, map->work.screenBuf[layer], dst);
        }

        map->work.screenDirty[layer] = 0;
    }
}

// MATCHED 100%
void clFieldMapVBlank::loadScreen(u8 layer) {
    clFieldMap * map = (clFieldMap *)this->arg;

    if (map->work.isSub == 0) {
        if (map->work.texMode != 1 || ((map->getMapInfo()->texLayerMask >> layer) & 1) == 0) {
            switch (layer) {
                case 0: {
                    GX_LoadBG1Scr(map->work.screenBuf[0], 0, 0x1000);
                    break;
                }
                case 1: {
                    GX_LoadBG2Scr(map->work.screenBuf[1], 0, 0x1000);
                    break;
                }
                case 2: {
                    GX_LoadBG3Scr(map->work.screenBuf[2], 0, 0x1000);
                    break;
                }
            }
        }
    } else {
        switch (layer) {
            case 0: {
                GXS_LoadBG1Scr(map->work.screenBuf[0], 0, 0x1000);
                break;
            }
            case 1: {
                GXS_LoadBG2Scr(map->work.screenBuf[1], 0, 0x1000);
                break;
            }
            case 2: {
                GXS_LoadBG3Scr(map->work.screenBuf[2], 0, 0x1000);
                break;
            }
        }
    }
}

// MATCHED 100%
void clFieldMapVBlank::copyScreenEdges(u8 edges, u16 tx, u16 ty, u16 * src, u16 * dst) {
    s32 sy;
    s32 r4;
    s32 r5;
    s32 r6;
    s32 r7;

    if ((edges & 1) || (edges & 2)) {
        if (edges & 1) {
            sy = ty - 1;
        } else if (edges & 2) {
            sy = ty + 24;
        }

        if (sy < 0) {
            sy = (sy % 32 + 32) % 32;
        } else if (sy >= 32) {
            sy = sy % 32;
        }

        s32 r7 = (sy % 32) * 32;
        s32 rowHi = r7 + 0x400;

        r6 = tx - 1;
        for (r5 = -1; r5 < 33; r5++, r6++) {
            r4 = r6;
            if (r6 < 0) {
                r4 = (r6 % 64 + 64) % 64;
            } else if (r6 >= 64) {
                r4 = r6 % 64;
            }

            s32 ofs;
            if (((r4 / 32) & 1) == 0) {
                ofs = r4 % 32 + r7;
            } else {
                ofs = r4 % 32 + rowHi;
            }

            MIi_CpuCopy16(src + ofs, dst + ofs, 2);
        }
    }

    if ((edges & 4) || (edges & 8)) {
        if (edges & 4) {
            r4 = tx - 1;
        } else if (edges & 8) {
            r4 = tx + 32;
        }

        if (r4 < 0) {
            r4 = (r4 % 64 + 64) % 64;
        } else if (r4 >= 64) {
            r4 = r4 % 64;
        }

        r7 = ty - 1;
        r6 = (r4 / 32) & 1;
        r5 = r4 % 32;

        for (r4 = -1; r4 < 25; r4++, r7++) {
            sy = r7;
            if (r7 < 0) {
                sy = (r7 % 32 + 32) % 32;
            } else if (r7 >= 0x400) {
                sy = r7 % 32;
            }

            s32 ofs;
            if (r6 == 0) {
                ofs = r5 + (sy % 32) * 32;
            } else {
                ofs = r5 + ((sy % 32) * 32 + 0x400);
            }

            MIi_CpuCopy16(src + ofs, dst + ofs, 2);
        }
    }
}

// MATCHED 100%
void clFieldMapVBlank::setBgOffset(u32 layer, u32 x, u32 y) {
    clFieldMap * map = (clFieldMap *)this->arg;

    if (map->work.isSub == 0) {
        switch (layer) {
            case 0: {
                G2_SetBG1Offset(x, y);
                break;
            }
            case 1: {
                G2_SetBG2Offset(x, y);
                break;
            }
            case 2: {
                G2_SetBG3Offset(x, y);
                break;
            }
        }
    } else {
        switch (layer) {
            case 0: {
                G2S_SetBG1Offset(x, y);
                break;
            }
            case 1: {
                G2S_SetBG2Offset(x, y);
                break;
            }
            case 2: {
                G2S_SetBG3Offset(x, y);
                break;
            }
        }
    }
}

// MATCHED 100%
void clFieldMapVBlank::loadBgPalette() {
    clFieldMap * map = (clFieldMap *)this->arg;

    DC_FlushRange(map->work.basePal, this->work.palLoadSize);

    if (map->work.isSub == 0) {
        GX_LoadBGPltt(map->work.basePal, this->work.palLoadOffset, this->work.palLoadSize);
    } else {
        GXS_LoadBGPltt(map->work.basePal, this->work.palLoadOffset, this->work.palLoadSize);
    }
}

// MATCHED 100%
u16 * clFieldMapVBlank::getScreenAddr(u8 layer) {
    clFieldMap * map = (clFieldMap *)this->arg;

    if (map->work.isSub) {
        return (u16 *)(HW_DB_BG_VRAM + 0x5000 + layer * 0x1000);
    } else {
        return (u16 *)(HW_BG_VRAM + 0x5000 + layer * 0x1000);
    }
}

// MATCHED 100%
void clFieldMapVBlank::queueDmaCopy(void * src, void * dst, u32 size) {
    for (s32 i = 0; i < this->work.dmaCount; i++) {
        if (this->work.dmaSrc[i] == src && this->work.dmaDst[i] == dst) {
            return;
        }
    }

    this->work.dmaSrc[this->work.dmaCount] = src;
    this->work.dmaDst[this->work.dmaCount] = dst;
    this->work.dmaSize[this->work.dmaCount] = size;
    this->work.dmaCount++;
}

// MATCHED 100%
void clFieldMapVBlank::flushDmaCopies() {
    for (s32 i = 0; i < this->work.dmaCount; i++) {
        DC_FlushRange(this->work.dmaDst[i], this->work.dmaSize[i]);
        MI_DmaCopy32(GX_DEFAULT_DMAID, this->work.dmaSrc[i], this->work.dmaDst[i], this->work.dmaSize[i]);

        this->work.dmaSrc[i] = NULL;
        this->work.dmaDst[i] = NULL;
        this->work.dmaSize[i] = 0;
    }

    this->work.dmaCount = 0;
}

// MATCHED 100%
void clFieldMapVBlank::queueBlend(u32 plane1, u32 plane2, u32 eva, u32 evb) {
    this->work.blendPlane1 = plane1;
    this->work.blendPlane2 = plane2;
    this->work.blendEva = eva;
    this->work.blendEvb = evb;

    this->work.blendPending = 1;
}

// MATCHED 100%
void clFieldMapVBlank::applyBlend() {
    if (((clFieldMap *)this->arg)->work.isSub == 0) {
        G2_SetBlendAlpha(this->work.blendPlane1 | (this->work.blendBg0 ? 1 : 0), this->work.blendPlane2, this->work.blendEva, this->work.blendEvb);
    } else {
        G2S_SetBlendAlpha(this->work.blendPlane1 | (this->work.blendBg0 ? 1 : 0), this->work.blendPlane2, this->work.blendEva, this->work.blendEvb);
    }

    this->work.blendPlane1 = 0;
    this->work.blendPlane2 = 0;
    this->work.blendEva = 0;
    this->work.blendEvb = 0;

    this->work.blendPending = 0;
}

// MATCHED 100%
void clFieldMapVBlank::queueCopy(void * src, void * dst, u32 size) {
    VramCopy * copy = &this->work.copies[this->work.copyTail];
    copy->src = src;
    copy->dst = dst;
    copy->size = size;
    copy->bytewise = ((size & 3) != 0);

    if (++this->work.copyTail >= 32) {
        this->work.copyTail = 0;
    }
}

// MATCHED 100%
u32 clFieldMap::isTexMode() {
    return this->work.texMode;
}

// MATCHED 100%
BOOL clFieldMap::getBlendBg0() {
    return (this->work.vblank->work.blendBg0 != 0);
}

// MATCHED 100%
void clFieldMap::setBlendBg0(u8 enable) {
    if (this->work.isSub == 0) {
        this->work.vblank->queueBlend(
            (reg_G2_BLDCNT & REG_G2_BLDCNT_PLANE1_MASK) >> REG_G2_BLDCNT_PLANE1_SHIFT,
            (reg_G2_BLDCNT & REG_G2_BLDCNT_PLANE2_MASK) >> REG_G2_BLDCNT_PLANE2_SHIFT,
            (reg_G2_BLDALPHA & REG_G2_BLDALPHA_EVA_MASK) >> REG_G2_BLDALPHA_EVA_SHIFT,
            (reg_G2_BLDALPHA & REG_G2_BLDALPHA_EVB_MASK) >> REG_G2_BLDALPHA_EVB_SHIFT
        );
    } else {
        this->work.vblank->queueBlend(
            (reg_G2S_DB_BLDCNT & REG_G2S_DB_BLDCNT_PLANE1_MASK) >> REG_G2S_DB_BLDCNT_PLANE1_SHIFT,
            (reg_G2S_DB_BLDCNT & REG_G2S_DB_BLDCNT_PLANE2_MASK) >> REG_G2S_DB_BLDCNT_PLANE2_SHIFT,
            (reg_G2S_DB_BLDALPHA & REG_G2S_DB_BLDALPHA_EVA_MASK) >> REG_G2S_DB_BLDALPHA_EVA_SHIFT,
            (reg_G2S_DB_BLDALPHA & REG_G2S_DB_BLDALPHA_EVB_MASK) >> REG_G2S_DB_BLDALPHA_EVB_SHIFT
        );
    }

    this->work.vblank->work.blendBg0 = enable;
}

// MATCHED 100%
void clFieldMap::setLayerPriority(u32 layer, u32 prio) {
    this->work.layerPriorities = this->work.layerPriorities & ~(3 << (layer * 2)) | (prio << (layer * 2));
    this->work.prioritiesDirty = 1;
}

// MATCHED 100%
u16 clFieldMap::getMapId() {
    return this->work.mapId;
}

// MATCHED 100%
u32 clFieldMap::func_ov003_02073470() {
    return this->work.vblank->work.field18_12;
}

// MATCHED 100%
void clFieldMap::func_ov003_02073484(u32 param1) {
    this->work.vblank->work.field18_12 = param1;
}

// MATCHED 100%
u32 clFieldMap::func_ov003_020734a0() {
    return this->work.vblank->work.field18_4;
}

// MATCHED 100%
void clFieldMap::func_ov003_020734b4(u32 param1) {
    this->work.vblank->work.field18_4 = param1;
}

// MATCHED 100%
u16 clFieldMap::func_ov003_020734d0() {
    return this->work.field808.fielda14;
}

// MATCHED 100%
u8 clFieldMap::getBlendAnimCount() {
    return this->work.blendAnimCount;
}

// MATCHED 100%
void clFieldMap::pausePalAnims() {
    if (this->work.palAnimMask) {
        for (s32 i = 0; i < 4; i++) {
            if (this->work.palAnimMask & (1 << i)) {
                clYanaAnim * anim = &this->palAnims[i];
                anim->stopped = 1;
            }
        }

        this->work.palAnimState = 3;
    }
}

// MATCHED 100%
BOOL clFieldMap::isPalAnimActive() {
    return (this->work.palAnimState == 1);
}

// MATCHED 100%
u8 clFieldMap::getTileAnimCount() {
    return this->work.tileAnimCount;
}

// MATCHED 100%
u8 clFieldMap::getMapPatchCount() {
    return this->work.mapPatchCount;
}

// MATCHED 100%
u16 * clFieldMap::getLayerPalette(u32 layer) {
    u16 * pal = this->work.layerPal[layer];
    if (pal == NULL && this->work.sectionsU16[layer+3].size != 0) {
        pal = this->work.basePal;
    }
    return pal;
}

// MATCHED 100%
u16 * clFieldMap::getLayerScreenBuf(u32 layer) {
    return this->work.screenBuf[layer];
}

// MATCHED 100%
u8 * clFieldMap::getLayerCharBuf(u32 layer) {
    return this->work.layerGfxBuf[layer];
}

// MATCHED 100%
void * clFieldMap::getSection(u32 idx) {
    return this->work.sections[idx].data;
}

// MATCHED 100%
u16 * clFieldMap::getLayerPaletteSrc(u32 layer) {
    return this->work.sectionsU16[layer+3].data;
}

// MATCHED 100%
BOOL clFieldMap::isLayerMoving(u32 layer) {
    return (this->work.moves[layer].active != 0);
}
