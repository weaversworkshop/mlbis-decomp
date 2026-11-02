// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clPaf.h"

extern "C" {

#include <nitro.h>

#include "main/cpu_mem16.h"
#include "main/palAnim.h"

// --- Extern Data Variables --- //

extern RfxKeySeg keySeg;
extern u8 rfxNodeBuf[0x1000];
extern RfxStack rfxStack;

extern clPaf data_0205bb84;  // 0x0205bb84

extern u16 data_0205bbac[];  // 0x0205bbac
extern u16 data_0205bdac[];  // 0x0205bdac
extern u16 data_0205bfac[];  // 0x0205bfac

extern BlendFunc pafBlendFuncs[4];  // 0x020558fc

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

// --- Function Declarations --- //

void cmdSetRange(u16 *);  // 0x02027420

void pafUpdate(u16 *, u16 *, s32, s32, s32 *, s16);  // 0x020279a8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clPaf::unk_virtual1(s32 t, u16 * cmd, u8 type) {
    switch (type) {
        case 2: {
            cmdSetRange(cmd);
            break;
        }
        case 3: {
            applyKeyedRows(t, cmd);
            break;
        }
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9: {
            this->cmdPaletteOp(t, cmd);
            break;
        }
        case 10: {
            this->cmdCrossFade(t, cmd);
            break;
        }
        case 11: {
            this->runIdTracks(t, cmd);
            break;
        }
        case 64: {
            this->beginDraw();
            break;
        }
    }
}

// MATCHED 100%
u32 clPaf::unk_virtual2() {
    return sizeof(PafNode);
}

// MATCHED 100%
void clPaf::beginDraw() {
    func_01ff8228(rfxStack.cur, 0, 0xe);
}

// MATCHED 100%
void cmdSetRange(u16 * cmd) {
    PafNode * node = (PafNode *)rfxStack.cur;
    node->dstIdx = cmd[3];
    node->count = cmd[5];
    node->srcIdx = cmd[7];
}

// MATCHED 100%
void clPaf::cmdPaletteOp(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        int op = cmd[0] & 0x3f;
        s32 numKeys = (cmd[0] >> 9) & 0x7f;

        PafNode * node = (PafNode *)&rfxStack.cur[1];
        copyNode((RfxNode *)node-1, (RfxNode *)node);

        applyRows((RfxNode *)node, cmd, 1, numKeys + 1, keySeg.nextKey, keySeg.t, keySeg.dur);

        s32 count = node->count;
        s16 dstIdx = node->dstIdx;
        u16 * dst = &this->palette[dstIdx];
        s16 srcIdx = node->srcIdx;
        u16 * src = &this->palette[srcIdx];

        if (0x100 - srcIdx < count) {
            count = 0x100 - srcIdx;
        }
        if (0x100 - dstIdx < count) {
            count = 0x100 - dstIdx;
        }

        switch (op) {
            case 4: {
                s32 shift;
                if (count > 0) {
                    shift = (node->field0c * count + 50) / 100;
                    if (shift <= 0 && count <= shift) {
                        cpu_copy16(src, dst, count * sizeof(u16));
                    } else {
                        cpu_copy16(src, data_0205bfac, count * sizeof(u16));
                        cpu_copy16(&data_0205bfac[shift], dst, (count - shift) * sizeof(u16));
                        cpu_copy16(data_0205bfac, &dst[count - shift], shift * sizeof(u16));
                    }
                }
                break;
            }
            case 5:
            case 6:
            case 7:
            case 8: {
                if (count > 0) {
                    u16 color;
                    s32 w = (node->field0c * 32 + 50) / 100;
                    color = node->r | ((node->b << 10) | (node->g << 5));
                    
                    if (w <= 0) {
                        cpu_copy16(src, dst, count * 2);
                    } else {
                        if (w > 32) {
                            w = 32;
                        }

                        BlendFunc fn = pafBlendFuncs[op - 5];
                        s32 n = count;
                        
                        if (dst < src) {
                            while (n > 0) {
                                *dst++ = fn(*src++, color, w);
                                n--;
                            }
                        } else {
                            dst += n;
                            src += n;

                            while (n > 0) {
                                dst--;
                                src--;

                                *dst = fn(*src, color, w);
                                n--;
                            }
                        }
                    }
                }
                break;
            }
            case 9: {
                if (count > 0) {
                    s32 w = (node->field0c * 32 + 50) / 100;
                    if (w <= 0) {
                        cpu_copy16(src, dst, count * 2);
                    } else {
                        if (w > 32) {
                            w = 32;
                        }

                        s32 n = count;

                        if (dst < src) {
                            while (n > 0) {
                                *dst++ = colorInvertLerp(*src++, w);
                                n--;
                            }
                        } else {
                            dst += n;
                            src += n;
 
                            while (n > 0) {
                                dst--;
                                src--;

                                *dst = colorInvertLerp(*src, w);
                                n--;
                            }
                        }
                    }
                }
                break;
            }
        }
    }
}

// MATCHED 100%
void clPaf::cmdCrossFade(s32 t, u16 * cmd) {
    if (findKeySeg(&keySeg, t, cmd)) {
        s32 numKeys = ((cmd[0] >> 9) & 0x7f) + 1;

        PafNode * node = (PafNode *)rfxStack.cur;

        s32 key = keySeg.nextKey;
        s32 row = numKeys+1;
        s16 srcIdx = cmd[row + key];
        s16 dstIdx = node->dstIdx;
        s32 count = node->count;

        u16 * dst = &this->palette[dstIdx];
        u16 * src = &this->palette[srcIdx];

        if (0x100 - dstIdx < count) {
            count = 0x100 - dstIdx;
        }
        if (0x100 - srcIdx < count) {
            count = 0x100 - srcIdx;
        }

        if (key == numKeys) {
            keySeg.dur = 0;
        }

        if (keySeg.dur == 0) {
            cpu_copy16(src, dst, count * 2);
        } else {
            s16 nextIdx = cmd[row + keySeg.nextKey + 1];

            if (0x100 - nextIdx < count) {
                count = 0x100 - nextIdx;
            }

            if (count > 0) {
                cpu_copy16(src, data_0205bbac, count * sizeof(u16));

                u16 * a = data_0205bbac;

                cpu_copy16(&this->palette[nextIdx], data_0205bdac, count * sizeof(u16));

                u16 * b = data_0205bdac;
                s32 w = keySeg.t * 31 / keySeg.dur;

                while (count > 0) {
                    *dst++ = colorLerp(*a++, *b++, w);
                    count--;
                }
            }
        }
    }
}

// MATCHED 100%
void clPaf::runIdTracks(s32 t, u16 * cmd) {
    s32 numKeys = ((cmd[0] >> 9) & 0x7f);
    s32 stride = numKeys + 2;

    s32 i = 1;
    do {
        s16 id = cmd[stride + i];
        if (id == -1 || id == this->id) {
            s32 dur = cmd[stride * 4 + i];
            if (dur > 0) {
                u16 t0 = cmd[i];
                if (cmd[stride * 2 + i] != 0) {
                    this->runTrackLooped(this->pos - (t0 << 8), dur, 0);
                } else {
                    this->runTrackLooped(t - (t0 << 8), dur, cmd[stride * 3 + i]);
                }
            }
        }
    } while (++i < stride);
}

// MATCHED 100%
void pafUpdate(u16 * script, u16 * palette, s32 id, s32 t, s32 * pos, s16 delta) {
    clPaf * paf = &data_0205bb84;

    if (script != NULL) {
        paf->script = script;

        if (script != NULL) {
            paf->length = script[script[0]];
        } else {
            paf->length = 0;
            paf->autoDestroy = 1;
            paf->stopped = 0;
            paf->loopsRemaining = 1;
        }
    }

    if (palette != NULL) {
        paf->palette = palette;
    }
    if (id >= 0) {
        paf->id = id;
    }

    if (pos != NULL) {
        if (!paf->stopped) {
            *pos += delta;
        }
        paf->pos = *pos;
    }

    script = paf->script;
    if (paf->script != NULL) {
        rfxStack.cur = (RfxNode *)&rfxNodeBuf;

        paf->beginDraw();
        paf->runIdTracks(t, &script[script[0]]+1);
    }
}
