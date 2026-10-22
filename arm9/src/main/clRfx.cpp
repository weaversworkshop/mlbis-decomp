// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clRfx.h"

extern "C" {

#include <nitro.h>

// --- Struct Definitions --- //

typedef struct {
    VecFx32 axisX;  // +0x0
    VecFx32 axisY;  // +0xc
    VecFx32 axisZ;  // +0x18
    VecFx32 pos;  // +0x24
    u8 pad_30[0x30];
} RfxNode;

typedef struct {
    u8 pad_00[0x40];
} UnkStruct_02021df8;

typedef struct {
    u8 pad_00[0x30];
} UnkStruct_02021e10;

typedef struct {
    u8 pad_00[0x2c];
} UnkStruct_02021e20;

typedef struct {
    u8 pad_00[0x24];
} UnkStruct_02021e30;

typedef struct {
    u8 pad_00[0x20];
} UnkStruct_02021e40;

typedef struct {
    u8 pad_00[0x14];
} UnkStruct_02021e50;

typedef struct {
    s32 key;  // +0x0
    s32 nextKey;  // +0x4
    s32 t;  // +0x8
    s32 dur;  // +0xc
} RfxKeySeg;

typedef struct {
    u8 pad_00[0x20];
    RfxNode * cur;  // +0x20
} RfxStack;

// --- Extern Data Variables --- //

extern s16 sinCosTable[];  // 0x0204b1dc

extern u8 rfxNodeBuf[0x1000];  // 0x027e00fc
extern RfxStack rfxStack;  // 0x027e10fc

// --- Extern Functions --- //

// --- Variable Definitions --- //

// --- Function Declarations --- //

void evalRow(s16 *, u32, u32, u32, u32);  // 0x02020ba8
u32 interpolateKey(s16 *, s32, s32, s32, u16 *, s32);  // 0x02020bec
void applyRows(RfxNode *, s16 *, s32, s32, s32, s32, s32);  // 0x02020d90

void applyKeyedRows(s32, s16 *);  // 0x02021b80
s32 findKeySeg(RfxKeySeg *, s32, s16 *);  // 0x02021c48

void vecMulAdd16(s32, VecFx32 *, VecFx32 *);  // 0x02021cd0
void vecScale16(s32, VecFx32 *);  // 0x02021d14
void vecRotPair(s32, s32, VecFx32 *, VecFx32 *);  // 0x02021d48
void copyNode(RfxNode *, RfxNode *);  // 0x02021de0
void copy64(UnkStruct_02021df8 *, UnkStruct_02021df8 *);  // 0x02021df8
void copy48(UnkStruct_02021e10 *, UnkStruct_02021e10 *);  // 0x02021e10
void copy44(UnkStruct_02021e20 *, UnkStruct_02021e20 *);  // 0x02021e20
void copy36(UnkStruct_02021e30 *, UnkStruct_02021e30 *);  // 0x02021e30
void copy32(UnkStruct_02021e40 *, UnkStruct_02021e40 *);  // 0x02021e40
void copy20(UnkStruct_02021e50 *, UnkStruct_02021e50 *);  // 0x02021e50


} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clRfx::start(inYanaUnit ** list, u16 * script) {
    this->moveToList(list);
    this->init(0);

    this->script = script;

    if (script != NULL) {
        this->length = script[script[0]];
    } else {
        this->length = 0;
        this->autoDestroy = 1;
        this->stopped = 0;
        this->loopsRemaining = 1;
    }

    this->autoDestroy = 1;
    this->handle = NULL;
}

// MATCHED 100%
void clRfx::finish() {
    this->script = NULL;
    this->atEnd = 1;
    this->detach();
}

// MATCHED 100%
void clRfx::update() {
    this->beginUpdate();
    this->exec();
    this->advance();
    this->updateLoop();
}

// MATCHED 100%
void clRfx::exec() {
    if (this->script != NULL) {
        rfxStack.cur = (RfxNode *)rfxNodeBuf;

        this->unk_virtual1(this->pos, NULL, 0x40);
        this->runTrack(this->pos, 0);
        this->unk_virtual1(this->pos, NULL, 0x41);
    }
}

// MATCHED 100%
void clRfx::onEnd() {
    if (this->handle != NULL) {
        this->handle[0] = 0;
        this->handle = NULL;
    }

    if (this->autoDestroy) {
        delete this;
    } else {
        this->atEnd = 1;
    }
}

// MATCHED 100%
clRfx::~clRfx() {
    this->script = NULL;
}

// MATCHED 100%
u32 clRfx::func_02020b98() {
    return 0;
}

// MATCHED 100%
u16 * clRfx::func_02020ba0(s32 param1) {
    return NULL;
}

// MATCHED 100%
void evalRow(s16 * cmd, u32 row, u32 key, u32 t, u32 dur) {
    u32 numKeys = (((u16)cmd[0] >> 9) & 0x7f) + 1;
    u32 rowOffset = (numKeys+1) * row;

    interpolateKey(&cmd[rowOffset], key, t, dur, (u16*)cmd, numKeys);
}

// MATCHED 100%
u32 interpolateKey(s16 * row, s32 key, s32 t, s32 dur, u16 * cmd, s32 numKeys) {
    if (dur == 0) {
        return row[key] << 8;
    }

    switch (((u16)row[0] >> 10) & 0x3f) {
        case 1: {
            s32 next = key+1;
            s32 v0 = row[key];
            s32 v1 = row[next];
            return (t * (v1 - v0)) / (dur >> 8) + (v0 << 8);
        }
        case 2: {
            s32 fromNext, fromPrev, wNext, wPrev, nDur;

            s32 next = key+1;
            s32 v0 = row[key];
            s32 v1 = row[next];

            s32 lin = (t * (v1 - v0)) / (dur >> 8) + (v0 << 8);
            fromPrev = lin;
            fromNext = lin;
            if (key > 1) {
                nDur = cmd[key];
                if (nDur > 0) {
                    s32 next = key-1;
                    fromPrev = (lin + (v0 << 8) + (t * ((v0 - row[next])) / nDur)) / 2;
                }
            }

            if (key+1 < numKeys) {
                s32 next = key+2;
                nDur = cmd[next];
                if (nDur > 0) {
                    fromNext = (lin + (v1 << 8) - (((row[next] - v1) * (dur - t)) / nDur)) / 2;
                }
            }

            wPrev = (dur - t) >> 8;
            wNext = t >> 8;
            if (fromPrev != lin && fromNext != lin) {
                wPrev = wPrev*wPrev;
                wNext = wNext*wNext;
            }

            return ((fromPrev * wPrev) + (fromNext * wNext)) / (wPrev + wNext);
        }
    }

    return row[key] << 8;
}

// MATCHED 100%
void applyRows(RfxNode * node, s16 * cmd, s32 numRows, s32 numKeys, s32 key, s32 t, s32 dur) {
    s16 * row = cmd;
    if (dur != 0) {
        while (numRows-- != 0) {
            row += numKeys + 1;
            s32 val = interpolateKey(row, key, t, dur, (u16 *)cmd, numKeys);
            u16 ch = row[0] & 0x3ff;
            if (ch < 0x19) {
                switch (ch) {
                    case 0: {
                        vecMulAdd16(val, &node->axisX, &node->pos);
                        break;
                    }
                    case 1: {
                        vecMulAdd16(val, &node->axisY, &node->pos);
                        break;
                    }
                    case 2: {
                        vecMulAdd16(val, &node->axisZ, &node->pos);
                        break;
                    }
                    case 3: {
                        vecScale16(val, &node->axisX);
                        break;
                    }
                    case 4: {
                        vecScale16(val, &node->axisY);
                        break;
                    }
                    case 5: {
                        vecScale16(val, &node->axisZ);
                        break;
                    }
                    case 6: {
                        s32 angleIdx = ((u16)val >> 4) * 2;
                        vecRotPair(sinCosTable[angleIdx + 1], sinCosTable[angleIdx], &node->axisY, &node->axisZ);
                        break;
                    }
                    case 7: {
                        s32 angleIdx = ((u16)val >> 4) * 2;
                        vecRotPair(sinCosTable[angleIdx + 1], sinCosTable[angleIdx], &node->axisX, &node->axisZ);
                        break;
                    }
                    case 8: {
                        s32 angleIdx = ((u16)val >> 4) * 2;
                        vecRotPair(sinCosTable[angleIdx + 1], sinCosTable[angleIdx], &node->axisX, &node->axisY);
                        break;
                    }
                    case 9: {
                        vecMulAdd16(val, &node->axisX, &node->axisX);
                        break;
                    }
                    case 10: {
                        vecMulAdd16(val, &node->axisY, &node->axisX);
                        break;
                    }
                    case 11: {
                        vecMulAdd16(val, &node->axisZ, &node->axisX);
                        break;
                    }
                    case 12: {
                        vecMulAdd16(val, &node->axisX, &node->axisY);
                        break;
                    }
                    case 13: {
                        vecMulAdd16(val, &node->axisY, &node->axisY);
                        break;
                    }
                    case 14: {
                        vecMulAdd16(val, &node->axisZ, &node->axisY);
                        break;
                    }
                    case 15: {
                        vecMulAdd16(val, &node->axisX, &node->axisZ);
                        break;
                    }
                    case 16: {
                        vecMulAdd16(val, &node->axisY, &node->axisZ);
                        break;
                    }
                    case 17: {
                        vecMulAdd16(val, &node->axisZ, &node->axisZ);
                    }
                }
            } else {
                if (ch < 0x39) {
                    ch -= 0x19;
                } else {
                    ch -= 0x21;
                }
                s32 offset = ch * 2;
                if (offset < 0x60) {
                    s16 * dst = (s16 *)((u8 *)node + offset);
                    s32 clamped = val >> 8;
                    if (clamped <= -0x8000) {
                        *dst = -0x8000;
                    } else if (clamped < 0x8000) {
                        *dst = clamped;
                    } else {
                        *dst = 0x7fff;
                    }
                }
            }
        }
    } else {
        while (numRows-- != 0) {
            row += numKeys + 1;
            u16 ch = row[0] & 0x3ff;
            s32 raw = row[key];
            if (ch < 0x19) {
                s32 val = raw << 8;
                switch (ch) {
                    case 0: {
                        vecMulAdd16(val, &node->axisX, &node->pos);
                        break;
                    }
                    case 1: {
                        vecMulAdd16(val, &node->axisY, &node->pos);
                        break;
                    }
                    case 2: {
                        vecMulAdd16(val, &node->axisZ, &node->pos);
                        break;
                    }
                    case 3: {
                        vecScale16(val, &node->axisX);
                        break;
                    }
                    case 4: {
                        vecScale16(val, &node->axisY);
                        break;
                    }
                    case 5: {
                        vecScale16(val, &node->axisZ);
                        break;
                    }
                    case 6: {
                        s32 angleIdx = ((u16)val >> 4) * 2;
                        vecRotPair(sinCosTable[angleIdx + 1], sinCosTable[angleIdx], &node->axisY, &node->axisZ);
                        break;
                    }
                    case 7: {
                        s32 angleIdx = ((u16)val >> 4) * 2;
                        vecRotPair(sinCosTable[angleIdx + 1], sinCosTable[angleIdx], &node->axisX, &node->axisZ);
                        break;
                    }
                    case 8: {
                        s32 angleIdx = ((u16)val >> 4) * 2;
                        vecRotPair(sinCosTable[angleIdx + 1], sinCosTable[angleIdx], &node->axisX, &node->axisY);
                        break;
                    }
                    case 9: {
                        vecMulAdd16(val, &node->axisX, &node->axisX);
                        break;
                    }
                    case 10: {
                        vecMulAdd16(val, &node->axisY, &node->axisX);
                        break;
                    }
                    case 11: {
                        vecMulAdd16(val, &node->axisZ, &node->axisX);
                        break;
                    }
                    case 12: {
                        vecMulAdd16(val, &node->axisX, &node->axisY);
                        break;
                    }
                    case 13: {
                        vecMulAdd16(val, &node->axisY, &node->axisY);
                        break;
                    }
                    case 14: {
                        vecMulAdd16(val, &node->axisZ, &node->axisY);
                        break;
                    }
                    case 15: {
                        vecMulAdd16(val, &node->axisX, &node->axisZ);
                        break;
                    }
                    case 16: {
                        vecMulAdd16(val, &node->axisY, &node->axisZ);
                        break;
                    }
                    case 17: {
                        vecMulAdd16(val, &node->axisZ, &node->axisZ);
                    }
                }
            } else {
                if (ch < 0x39) {
                    ch -= 0x19;
                } else {
                    ch -= 0x21;
                }

                s32 offset = ch * 2;
                if (offset < 0x60) {
                    *(s16 *)((u8 *)node + offset) = raw;
                }
            }
        }
    }
}

// MATCHED 100%
void clRfx::runTrackLooped(s32 t, u32 track, s32 loops) {
    s32 period = (this->script[this->script[track]] << 8);
    if (loops == 0 || t < period * loops) {
        t %= period;
        if (t < 0) {
            t += period;
        }
    } else {
        t = period;
    }

    RfxNode ** cur = &rfxStack.cur;
    (*cur)++;
    copyNode(*cur-1, *cur);
    this->runTrack(t, track);
    (*cur)--;
}

// MATCHED 100%
void clRfx::runTrack(s32 t, u32 track) {
    u32 next = track+1;
    u16 * cmd = &this->script[this->script[track]]+1;
    u16 * end = &this->script[this->script[next]];

    while (cmd < end) {
        u16 hdr = cmd[0];
        this->unk_virtual1(t, cmd, hdr & 0x3f);

        s32 rowBits = (hdr >> 6) & 7;
        s32 keyBits = (hdr >> 9) & 0x7f;
        cmd = &cmd[(rowBits+2) * (keyBits+2)];
    }
}

// MATCHED 100%
void clRfx::func_020213a8() {}

// MATCHED 100%
void clRfx::runSubTracks(s32 t, s16 * cmd) {
    s32 numKeys;
    s32 key;
    u16 * script;
    s32 sub;
    RfxNode ** cur;

    s32 elapsed = t - (cmd[1] << 8);
    if (elapsed < 0) {
        return;
    }

    script = this->script;

    s32 keyBits = ((u16)cmd[0] >> 9) & 0x7f;
    s32 rowBits = ((u16)cmd[0] >> 6) & 0x7;
    s32 numRows = rowBits + 1;
    numKeys = keyBits + 1;
    s32 tailOffset = (numKeys + 1) * numRows;

    s16 * subRow = &cmd[tailOffset];
    cur = &rfxStack.cur;
    (*cur)++;

    u16 flag = cmd[tailOffset];
    key = 1;
    numRows--;
    
    if (flag == 1) {
        do {
            sub = subRow[key];
            if (sub >= 0 && elapsed <= script[script[sub]] << 8) {
                copyNode(*cur-1, *cur);

                if (numRows > 0) {
                    applyRows(*cur, cmd, numRows, numKeys, key, 0, 0);
                }

                copy36((UnkStruct_02021e30 *)SDK_AUTOLOAD_DTCM_START, (UnkStruct_02021e30 *)*cur);
                this->runTrack(elapsed, sub);
            }

            key++;
            if (numKeys < key) {
                break;
            }

            elapsed -= (u16)cmd[key] << 8;
            if (elapsed < 0) {
                break;
            }
        } while (TRUE); // do-while hack, mismatch when using r9 condition like in the 'else' arm ...
    } else {
        do {
            sub = subRow[key];
            if (sub >= 0 && elapsed <= script[script[sub]] << 8) {
                copyNode(*cur-1, *cur);

                if (numRows > 0) {
                    applyRows(*cur, cmd, numRows, numKeys, key, 0, 0);
                }

                this->runTrack(elapsed, sub);
            }

            key++;
            if (numKeys < key) {
                break;
            }

            elapsed -= (u16)cmd[key] << 8;
        } while (elapsed >= 0);
    }
    (*cur)--;
}

// MATCHED 100%
void clRfx::runSubTrackSeg(s32 t, s16 * cmd) {
    s32 key;
    s32 nextKey;
    s32 segDur;
    s16 * subRow;
    u16 * script;
    s16 sub;

    s32 elapsed = t - (cmd[1] << 8);
    if (elapsed < 0) {
        return;
    }

    script = this->script;
    key = 1;

    s32 keyBits = ((u16)cmd[0] >> 9) & 0x7f;
    s32 rowBits = ((u16)cmd[0] >> 6) & 0x7;
    s32 numRows = rowBits + 1;
    s32 numKeys = keyBits + 1;
    s32 tailOffset = (numKeys + 1) * numRows;

    nextKey = key;
    subRow = &cmd[tailOffset];
    segDur = 0;
    while (TRUE) {
        nextKey++;
        if (nextKey > numKeys) {
            if (segDur != 0) {
                return;
            }
            break;
        }

        segDur = (u16)cmd[nextKey];
        if (segDur == 0) {
            segDur = script[script[(u16)subRow[nextKey - 1]]];
            if (segDur == 0) {
                continue;
            }
        }

        segDur <<= 8;
        if (elapsed < segDur) {
            break;
        }

        key = nextKey;
        elapsed -= segDur;
    }

    RfxNode ** cur = &rfxStack.cur;
    (*cur)++;

    numRows--;

    if ((u16)subRow[0] == 1) {
        do {
            sub = subRow[key];
            if (sub >= 0) {
                s32 subLen = script[script[sub]];
                if (subLen <= 0) {
                    elapsed = 0;
                } else {
                    elapsed %= subLen << 8;
                }

                copyNode(*cur-1, *cur);

                if (numRows > 0) {
                    applyRows(*cur, cmd, numRows, numKeys, key, elapsed, segDur);
                }

                copy36((UnkStruct_02021e30 *)SDK_AUTOLOAD_DTCM_START, (UnkStruct_02021e30 *)*cur);
                this->runTrack(elapsed, sub);
            }

            key++;
        } while (key != nextKey);
    } else {
        do {
            sub = subRow[key];
            if (sub >= 0) {
                s32 w = script[script[sub]];
                if (w <= 0) {
                    elapsed = 0;
                } else {
                    elapsed %= w << 8;
                }

                copyNode(*cur-1, *cur);

                if (numRows > 0) {
                    applyRows(*cur, cmd, numRows, numKeys, key, elapsed, segDur);
                }

                this->runTrack(elapsed, sub);
            }

            key++;
        } while (key != nextKey);
    }
    (*cur)--;
}

// MATCHED 100%
void clRfx::runExtTracks(s32 t, s16 * cmd) {
    s32 key;
    u16 * ext;
    u16 * saved;
    s32 numKeys;

    s32 elapsed = t - (cmd[1] << 8);
    if (elapsed < 0) {
        return;
    }

    key = 1;

    s32 keyBits = ((u16)cmd[0] >> 9) & 0x7f;
    s32 rowBits = ((u16)cmd[0] >> 6) & 0x7;
    s32 numRows = rowBits + 1;
    numKeys = keyBits + 1;
    s32 tailOffset = (numKeys + 1) * numRows;
    s16 * subRow = &cmd[tailOffset];

    RfxNode ** cur = &rfxStack.cur;
    (*cur)++;

    u16 flag = (u16)subRow[0];
    numRows--;

    if (flag == 1) {
        while (TRUE) {
            s32 sub = subRow[key];
            if (sub >= 0) {
                ext = this->func_02020ba0(sub);
                if (ext != NULL && elapsed <= ext[ext[0]] << 8) {
                    copyNode(*cur-1, *cur);

                    if (numRows > 0) {
                        applyRows(*cur, cmd, numRows, numKeys, key, 0, 0);
                    }

                    copy36((UnkStruct_02021e30 *)SDK_AUTOLOAD_DTCM_START, (UnkStruct_02021e30 *)*cur);

                    saved = this->script;
                    this->script = ext;

                    if (ext != NULL) {
                        this->length = ext[ext[0]];
                    } else {
                        this->length = 0;
                        this->autoDestroy = 1;
                        this->stopped = 0;
                        this->loopsRemaining = 1;
                    }

                    this->runTrack(elapsed, 0);

                    this->script = saved;

                    if (saved != NULL) {
                        this->length = saved[saved[0]];
                    } else {
                        this->length = 0;
                        this->autoDestroy = 1;
                        this->stopped = 0;
                        this->loopsRemaining = 1;
                    }
                }
            }

            key++;
            if (numKeys < key) {
                break;
            }

            elapsed -= (u16)cmd[key] << 8;
            if (elapsed < 0) {
                break;
            }
        }
    } else {
        while (TRUE) {
            s32 sub = subRow[key];
            if (sub >= 0) {
                ext = this->func_02020ba0(sub);
                if (ext != NULL && elapsed <= ext[ext[0]] << 8) {
                    copyNode(*cur-1, *cur);

                    if (numRows > 0) {
                        applyRows(*cur, cmd, numRows, numKeys, key, 0, 0);
                    }

                    saved = this->script;
                    this->script = ext;

                    if (ext != NULL) {
                        this->length = ext[ext[0]];
                    } else {
                        this->length = 0;
                        this->autoDestroy = 1;
                        this->stopped = 0;
                        this->loopsRemaining = 1;
                    }

                    this->runTrack(elapsed, 0);

                    this->script = saved;

                    if (saved != NULL) {
                        this->length = saved[saved[0]];
                    } else {
                        this->length = 0;
                        this->autoDestroy = 1;
                        this->stopped = 0;
                        this->loopsRemaining = 1;
                    }
                }
            }

            key++;
            if (numKeys < key) {
                break;
            }

            elapsed -= (u16)cmd[key] << 8;
            if (elapsed < 0) {
                break;
            }
        }
    }
    (*cur)--;
}

// MATCHED 100%
void applyKeyedRows(s32 t, s16 * cmd) {
    s32 key;
    s32 segDur;
    s32 numKeys;

    s32 elapsed = t - (cmd[1] << 8);
    if (elapsed < 0) {
        return;
    }

    key = 1;
    segDur = 0;

    s32 keyBits = ((u16)cmd[0] >> 9) & 0x7f;
    s32 rowBits = ((u16)cmd[0] >> 6) & 0x7;
    numKeys = keyBits + 1;

    while (TRUE) {
        key++;
        if (key > numKeys) {
            if (segDur == 0) {
                applyRows(rfxStack.cur, cmd, rowBits+1, numKeys, key-1, elapsed, segDur);
            }
            break;
        }

        segDur = (u16)cmd[key];
        if (segDur != 0) {
            segDur <<= 8;
            if (elapsed <= segDur) {
                applyRows(rfxStack.cur, cmd, rowBits+1, numKeys, key-1, elapsed, segDur);
                break;
            }

            elapsed -= segDur;
        }
    }
}

// MATCHED 100%  -- possibly return BOOL
s32 findKeySeg(RfxKeySeg * out, s32 t, s16 * cmd) {
    s32 numKeys;
    s32 key;
    s32 nextKey;
    s32 segDur;

    s32 elapsed = t - (cmd[1] << 8);
    if (elapsed < 0) {
        return 0;
    }

    key = 1;
    nextKey = key;

    numKeys = (((u16)cmd[0] >> 9) & 0x7f) + 1;
    segDur = 0;

    while (TRUE) {
        nextKey++;
        if (nextKey > numKeys) {
            if (segDur != 0) {
                return 0;
            }
            break;
        }

        segDur = (u16)cmd[nextKey];
        if (segDur != 0) {
            segDur <<= 8;
            if (elapsed <= segDur) {
                break;
            }

            key = nextKey;
            elapsed -= segDur;
        }
    }

    out->key = key;
    out->nextKey = nextKey-1;
    out->t = elapsed;
    out->dur = segDur;

    return 1;
}

#ifndef IDE_PARSE
// MATCHED 100%
asm void vecMulAdd16(s32 scale, VecFx32 * src, VecFx32 * dst) {
    stmdb sp!, {r4, r5, r6, r7, r8, lr}
    ldmia r1, {r3, r4, r5}
    smull r3, r6, r0, r3
    smull r4, r7, r0, r4
    smull r5, r8, r0, r5
    mov r3, r3, lsr #0x10
    orr r3, r3, r6, lsl #0x10
    mov r4, r4, lsr #0x10
    orr r4, r4, r7, lsl #0x10
    mov r5, r5, lsr #0x10
    orr r5, r5, r8, lsl #0x10
    ldmia r2, {r6, r7, r8}
    add r3, r3, r6
    add r4, r4, r7
    add r5, r5, r8
    stmia r2, {r3, r4, r5}
    ldmia sp!, {r4, r5, r6, r7, r8, pc}
}

// MATCHED 100%
asm void vecScale16(s32 scale, VecFx32 * v) {
    stmdb sp!, {r4, r5, r6, r7, lr}
    ldmia r1, {r2, r3, r4}
    smull r2, r5, r0, r2
    smull r3, r6, r0, r3
    smull r4, r7, r0, r4
    mov r2, r2, lsr #0x10
    orr r2, r2, r5, lsl #0x10
    mov r3, r3, lsr #0x10
    orr r3, r3, r6, lsl #0x10
    mov r4, r4, lsr #0x10
    orr r4, r4, r7, lsl #0x10
    stmia r1, {r2, r3, r4}
    ldmia sp!, {r4, r5, r6, r7, pc}
}

// MATCHED 100%
asm void vecRotPair(s32 cos, s32 sin, VecFx32 * vecA, VecFx32 * vecB) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, lr}
    mov r0, r0, lsl #0x12
    mov r1, r1, lsl #0x12
    ldmia r2, {r4, r5, r6}
    mov r4, r4, lsl #0x2
    mov r5, r5, lsl #0x2
    mov r6, r6, lsl #0x2
    ldmia r3, {r7, r8, r9}
    mov r7, r7, lsl #0x2
    mov r8, r8, lsl #0x2
    mov r9, r9, lsl #0x2
    smull ip, r7, r1, r7
    smull ip, r8, r1, r8
    smull ip, r9, r1, r9
    mov ip, #0x0
    smlal ip, r7, r0, r4
    mov ip, #0x0
    smlal ip, r8, r0, r5
    mov ip, #0x0
    smlal ip, r9, r0, r6
    stmia r2, {r7, r8, r9}
    rsb r1, r1, #0x0
    ldmia r3, {r7, r8, r9}
    mov r7, r7, lsl #0x2
    mov r8, r8, lsl #0x2
    mov r9, r9, lsl #0x2
    smull ip, r7, r0, r7
    smull ip, r8, r0, r8
    smull ip, r9, r0, r9
    mov ip, #0x0
    smlal ip, r7, r1, r4
    mov ip, #0x0
    smlal ip, r8, r1, r5
    mov ip, #0x0
    smlal ip, r9, r1, r6
    mov ip, #0x0
    stmia r3, {r7, r8, r9}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
}

// MATCHED 100%
asm void copyNode(RfxNode * src, RfxNode * dst) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    stmia r1!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    stmia r1!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, ip, lr}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc}
}

// MATCHED 100%
asm void copy64(UnkStruct_02021df8 * src, UnkStruct_02021df8 * dst) {
    stmdb sp!, {r4, r5, r6, r7, lr}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, ip, lr}
    stmia r1!, {r2, r3, r4, r5, r6, r7, ip, lr}
    ldmia r0!, {r2, r3, r4, r5, r6, r7, ip, lr}
    stmia r1!, {r2, r3, r4, r5, r6, r7, ip, lr}
    ldmia sp!, {r4, r5, r6, r7, pc}
}

// MATCHED 100%
asm void copy48(UnkStruct_02021e10 * src, UnkStruct_02021e10 * dst) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, lr}
    ldmia r0, {r0, r2, r3, r4, r5, r6, r7, r8, r9, r10, ip, lr}
    stmia r1, {r0, r2, r3, r4, r5, r6, r7, r8, r9, r10, ip, lr}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, pc}
}

// MATCHED 100%
asm void copy44(UnkStruct_02021e20 * src, UnkStruct_02021e20 * dst) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, lr}
    ldmia r0, {r0, r2, r3, r4, r5, r6, r7, r8, r9, ip, lr}
    stmia r1, {r0, r2, r3, r4, r5, r6, r7, r8, r9, ip, lr}
    ldmia sp!, {r4, r5, r6, r7, r8, r9, pc}
}

// MATCHED 100%
asm void copy36(UnkStruct_02021e30 * src, UnkStruct_02021e30 * dst) {
    stmdb sp!, {r4, r5, r6, r7, lr}
    ldmia r0, {r0, r2, r3, r4, r5, r6, r7, ip, lr}
    stmia r1, {r0, r2, r3, r4, r5, r6, r7, ip, lr}
    ldmia sp!, {r4, r5, r6, r7, pc}
}

// MATCHED 100%
asm void copy32(UnkStruct_02021e40 * src, UnkStruct_02021e40 * dst) {
    stmdb sp!, {r4, r5, r6, lr}
    ldmia r0, {r0, r2, r3, r4, r5, r6, ip, lr}
    stmia r1, {r0, r2, r3, r4, r5, r6, ip, lr}
    ldmia sp!, {r4, r5, r6, pc}
}

// MATCHED 100%
asm void copy20(UnkStruct_02021e50 * src, UnkStruct_02021e50 * dst) {
    stmdb sp!, {lr}
    ldmia r0, {r0, r2, r3, ip, lr}
    stmia r1, {r0, r2, r3, ip, lr}
    ldmia sp!, {pc}
}
#endif
