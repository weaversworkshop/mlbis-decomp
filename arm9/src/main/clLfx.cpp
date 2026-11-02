// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clLfx.h"

extern "C" {

#include <nitro.h>

#include "main/palAnim.h"

// --- Extern Data Variables --- //

extern RfxKeySeg keySeg;
extern RfxStack rfxStack;

extern LfxNode lfxNode;  // 0x027e1190

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);
extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

// --- Function Declarations --- //

void blitTile4bppMax(u32 *, s32, u32 *, s32, s32, s32, s32, s32, s32);  // 0x02028094
void decayTiles4bpp(u32 *, s32, s32);  // 0x020282f0
void scrollTiles4bpp(u32 *, u32, u32, s32);  // 0x02028330

void lfxLayerInit(LfxLayer *, u32 *, u16, u16, u16 *);  // 0x02028490
void lfxLayerUpdate(LfxLayer *);  // 0x020284d0
void lfxLayerApply(LfxLayer *, u32);  // 0x02028604

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
LfxLayer * clLfx::func_02027b54() {
    return NULL;
}

// MATCHED 100%
void clLfx::unk_virtual1(s32 t, u16 * cmd, u8 type) {
    switch (type) {
        case 7: {
            this->cmdDrawTex(t, cmd);
            break;
        }
        case 8: {
            this->cmdLayerColor(t, cmd);
            break;
        }
        case 9: {
            this->cmdLayerDecay(t, cmd);
            break;
        }
        case 11: {
            this->runColorBlendTrack(t, cmd);
            break;
        }
        case 0:
        case 1:
        case 2: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 3: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 39: {
            this->cmdLayerRows(t, cmd);
            break;
        }
        case 4: {
            this->runSubTracks(t, cmd);
            break;
        }
        case 5: {
            this->runSubTrackSeg(t, cmd);
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
u32 clLfx::unk_virtual2() {
    return 0x30;
}

// MATCHED 100%
void clLfx::beginDraw() {
    RfxNode * node = rfxStack.cur;
    MTX_Identity33((MtxFx33 *)SDK_AUTOLOAD_DTCM_START);
    func_01ff82ac(SDK_AUTOLOAD_DTCM_START, &node->mtx, sizeof(MtxFx33));

    node->pos.x = this->originX * 16;
    node->pos.y = this->originY * 16;

    lfxNode.field30 = 31;
    lfxNode.field32 = 31;
    lfxNode.field34 = 31;

    lfxNode.field36 = 0;
    lfxNode.field38 = 0;
    lfxNode.field3a = 0;

    lfxNode.field3c = 64;

    lfxNode.field44 = -1;

    lfxNode.field3e = 0;
    lfxNode.field40 = 0;
    lfxNode.field42 = 0;

    lfxNode.field46 = 0;
    lfxNode.field48 = 16;
    lfxNode.field4a = 16;

    LfxLayer * layer = this->func_02027b54();
    if (layer != NULL) {
        layer->colorStart = 0;
        layer->colorEnd = 0x7fff;
        layer->colorMid = 0;
        layer->midIdx = -1;
        layer->decaySpeed = 64;
        layer->scrollVelocity = 0;
        layer->ev = 16;
        layer->evb = 16;
    }
}

// MATCHED 100%
void clLfx::endDraw() {}

// MATCHED 100%
void clLfx::cmdDrawTex(s32 t, u16 * cmd) {
    LfxLayer * layer = this->func_02027b54();
    if (layer != NULL && layer->bitmap != NULL && findKeySeg(&keySeg, t, cmd)) {
        RfxNode * node = &rfxStack.cur[1];
        copyNode(node-1, node);

        s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;

        s32 key = keySeg.key;
        if (key <= keySeg.nextKey) {
            s32 offW = (numKeys + 1) * 4;
            s32 offH = (numKeys + 1) * 5;
            s32 offTy = (numKeys + 1) * 3;
            s32 offTx = (numKeys + 1) * 2;

            do {
                RfxTexHeader * tex = this->getTexHeader((s16)cmd[(numKeys + 1) + key]);
                if (tex != NULL) {
                    s16 tx = (s16)cmd[offTx + key];
                    s16 ty = (s16)cmd[offTy + key];
                    s16 h = (s16)cmd[offH + key];
                    s16 w = (s16)cmd[offW + key];
                    
                    blitTile4bppMax((u32 *)tex->data + ty * (tex->wBlocks * 8) + tx * 8,
                                    tex->wBlocks, layer->bitmap, layer->wTiles, layer->hTiles,
                                    node->pos.x / 16 - w * 4, node->pos.y / 16 - h * 4, w, h);
                    layer->dirty = 1;
                }
            } while (++key <= keySeg.nextKey);
        }
    }
}

// MATCHED 100%
void clLfx::cmdLayerColor(s32 t, u16 * cmd) {
    LfxLayer * layer = this->func_02027b54();
    if (layer != NULL && layer->pltt != NULL && findKeySeg(&keySeg, t, cmd)) {
        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows((RfxNode *)&lfxNode, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        layer->colorEnd = (lfxNode.field34 << 10) | (lfxNode.field32 << 5) | lfxNode.field30;
        layer->colorStart = (lfxNode.field3a << 10) | (lfxNode.field38 << 5) | lfxNode.field36;
        layer->colorMid = (lfxNode.field42 << 10) | (lfxNode.field40 << 5) | lfxNode.field3e;

        layer->midIdx = lfxNode.field44;
    }
}

// MATCHED 100%
void clLfx::cmdLayerDecay(s32 t, u16 * cmd) {
    LfxLayer * layer = this->func_02027b54();
    if (layer != NULL && layer->pltt != NULL && findKeySeg(&keySeg, t, cmd)) {
        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows((RfxNode *)&lfxNode, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);

        layer->decaySpeed = lfxNode.field3c;
        layer->scrollVelocity = lfxNode.field46;
        layer->ev = lfxNode.field48;
        layer->evb = lfxNode.field4a;
    }
}


static inline u32 max4bpp(u32 dst, u32 src) {
    u32 lo = (((dst & 0x0f0f0f0f) | 0x10101010) - (src & 0x0f0f0f0f)) & 0x10101010;
    u32 hi = ((((dst >> 4) & 0x0f0f0f0f) | 0x10101010) - ((src >> 4) & 0x0f0f0f0f)) & 0x10101010;
    u32 m = (lo >> 4) | hi;
    m = (m << 4) - m;
    return (dst & m) | (src & ~m);
}

// MATCHED 100%
void blitTile4bppMax(u32 * src, s32 srcW, u32 * dst, s32 dstW, s32 dstH, s32 x, s32 y, s32 w, s32 h) {
    s32 srcY = 0;
    s32 right = x + w * 8;
    s32 bottom = y + h * 8;
    s32 nibShift = (x & 7) * 4;

    if (right > dstW * 8) {
        right = dstW * 8;
    }
    if (bottom > dstH * 8) {
        bottom = dstH * 8;
    }

    s32 spanW = right & ~7;
    s32 srcSkip = 0;

    if (x < 0) {
        srcSkip = (7 - x) & ~7;
    } else {
        dst += x >> 3;
        spanW -= x & ~7;
    }

    if (spanW < 0) {
        return;
    }

    if (y < 0) {
        srcY -= y;
        y = 0;
    }

    dst += dstW * y;

    s32 dstStep = dstW - spanW / 8;
    for (s32 endY = srcY + (bottom - y); srcY < endY; srcY++) {
        u32 * srcRow = src + (srcY & ~7) * srcW + (srcY & 7);
        u32 carry = 0;

        if (srcSkip != 0) {
            srcRow += srcSkip;
            carry = srcRow[-8] >> (32 - nibShift);
        }

        for (s32 i = 0; i < spanW; i += 8) {
            u32 srcWord = srcRow[i];
            u32 dstWord = *dst;
            u32 shifted = carry | (srcWord << nibShift);
            carry = srcWord >> (32 - nibShift);
            *dst++ = max4bpp(dstWord, shifted);
        }

        if (carry != 0 && right < dstW * 8) {
            *dst = max4bpp(*dst, carry);
        }

        dst += dstStep;
    }
}

// MATCHED 100%
void decayTiles4bpp(u32 * tiles, s32 wTiles, s32 rows) {
    s32 n = wTiles * rows;
    while (n-- > 0) {
        u32 val = *tiles;
        *tiles++ -= ((val | (val >> 1)) | ((val | (val >> 1)) >> 2)) & 0x11111111;
    }
}

// MATCHED 100%
void scrollTiles4bpp(u32 * tiles, u32 wTiles, u32 hTiles, s32 rows) {
    if (rows > 0) {
        s32 offset = wTiles * rows;
        s32 count = (wTiles * hTiles * 8) - offset;

        if (count > 0) {
            u32 * shifted = &tiles[offset];
            if (shifted < tiles) {
                func_01ff8380(shifted, tiles, count * sizeof(u32));
            } else {
                func_01ff82ac(shifted, tiles, count * sizeof(u32));
            }
        }
    }

    if (rows < 0) {
        s32 offset = wTiles * -rows;
        s32 count = (wTiles * hTiles * 8) - offset;

        if (count > 0) {
            u32 * shifted = &tiles[offset];
            if (tiles < shifted) {
                func_01ff8380(tiles, shifted, count * sizeof(u32));
            } else {
                func_01ff82ac(tiles, shifted, count * sizeof(u32));
            }
        }
    }
}

// MATCHED 100%
void clLfx::cmdLayerRows(s32 t, u16 * cmd) {
    LfxLayer * layer = this->func_02027b54();
    if (layer != NULL && layer->pltt != NULL && findKeySeg(&keySeg, t, cmd)) {
        s32 numRows = (cmd[0] >> 6) & 7;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;
        applyRows((RfxNode *)&lfxNode, cmd, numRows+1, numKeys+1, keySeg.nextKey, keySeg.t, keySeg.dur);
    }
}

// MATCHED 100%
u16 clLfx::getBlendColor() {
    LfxNode * node = &lfxNode;
    return (node->field50 << 10) | (node->field4e << 5) | node->field4c;
}

// MATCHED 100%
void lfxLayerInit(LfxLayer * layer, u32 * bitmap, u16 wTiles, u16 hTiles, u16 * pltt) {
    layer->bitmap = bitmap;
    layer->wTiles = wTiles;
    layer->hTiles = hTiles;
    layer->pltt = pltt;

    layer->decayCount = 0;
    layer->scrollCount = 0;
    layer->framesRemaining = 0;

    func_01ff8228(bitmap, 0, wTiles * hTiles * 32);
}

// MATCHED 100%
void lfxLayerUpdate(LfxLayer * layer) {
    if (layer->framesRemaining > 0) {
        layer->framesRemaining--;

        if (layer->bitmap != NULL) {
            s32 wTiles = layer->wTiles;
            s32 hPixels = layer->hTiles * 8;

            if (layer->scrollVelocity != 0) {
                s32 oldScroll = layer->scrollCount;
                s32 newScroll = layer->scrollCount + layer->scrollVelocity;
                s32 oldRowFx = oldScroll * hPixels;
                s32 newRowFx = newScroll * hPixels;

                layer->scrollCount = newScroll;

                if (oldRowFx < 0) {
                    oldRowFx -= 0xffff;
                }
                if (newRowFx < 0) {
                    newRowFx -= 0xffff;
                }

                scrollTiles4bpp(layer->bitmap, wTiles, layer->hTiles, newRowFx / 65536 - oldRowFx / 65536);
            }

            layer->decayCount -= layer->scrollVelocity;

            if (layer->decaySpeed != 0) {
                s32 decayRow = layer->decayCount * hPixels;
                decayRow /= 65536;

                s32 newCount = layer->decayCount + (layer->decaySpeed << 8);
                layer->decayCount = newCount;

                s32 decayEnd = newCount * hPixels;
                decayEnd /= 65536;

                while (TRUE) {
                    s32 bandRows = decayEnd - decayRow;
                    if (bandRows <= 0) {
                        break;
                    }

                    s32 startRow = decayRow % hPixels;
                    s32 rowsToEdge = hPixels - startRow;
 
                    if (rowsToEdge < bandRows) {
                        bandRows = rowsToEdge;
                    }
                    if (layer->scrollVelocity < 0) {
                        startRow = rowsToEdge - bandRows;
                    }

                    decayTiles4bpp(&layer->bitmap[startRow * wTiles], wTiles, bandRows);

                    decayRow += bandRows;
                }
            }
        }
    }

    layer->dirty = 0;
}

// MATCHED 100%
void lfxLayerApply(LfxLayer * layer, u32 engine) {
    if (layer->dirty) {
        layer->dirty = 0;
        layer->framesRemaining = 4096 / layer->decaySpeed;
    }

    if (layer->framesRemaining > 0) {
        u16 * pltt = layer->pltt;
        if (pltt != NULL) {
            if (layer->midIdx >= 0) {
                s32 n = layer->midIdx;
                for (s32 i = 0; i < n; i++) {
                    *pltt++ = colorLerp(layer->colorStart, layer->colorMid, (i * 32 + n/2) / n);
                }

                n = 16 - layer->midIdx;
                for (s32 i = 0; i < n; i++) {
                    *pltt++ = colorLerp(layer->colorMid, layer->colorEnd, (i * 32 + n/2) / n);
                }
            } else {
                for (s32 i = 0; i < 16; i++) {
                    *pltt++ = colorLerp(layer->colorStart, layer->colorEnd, (i * 32 + 8) / 16);
                }
            }
        }

        if (engine == 0) {
            G2_ChangeBlendAlpha(layer->ev, layer->evb);
        }

        if (engine == 1) {
            G2S_ChangeBlendAlpha(layer->ev, layer->evb);
        }
    }
}
