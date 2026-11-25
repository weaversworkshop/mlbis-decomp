// objdiff-target: build/delinks/_dsd_gap@ov000_2.o

#include "ov000/clMesWinEff.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"

// --- Variable Definitions --- //

const u16 wipeHCloseSeq[4] = { 0x00b0, 0x00b8, 0x00c8, 0x00c0 };  // 0x02067300

const u16 wipeVOpenSeq[4] = { 0x00d0, 0x00d8, 0x00e8, 0x00e0 };  // 0x020672f8

const u16 wipeVCloseSeq[4] = { 0x00f0, 0x00f8, 0x0108, 0x0100 };  // 0x020672f0

const u16 wipeHOpenSeq[4] = { 0x0090, 0x0098, 0x00a8, 0x00a0 };  // 0x02067308

const MesEffSeq diagCloseSeq[6] = { { 0x0000 }, { 0x8110 }, { 0x0118 }, { 0x0120 }, { 0x0128 }, { 0x0088 } };  // 0x0206731c

const MesEffSeq diagOpenSeq[6] = { { 0x0088 }, { 0x8130 }, { 0x0138 }, { 0x0140 }, { 0x0148 }, { 0x0000 } };  // 0x02067310

const u16 fadeOpenSeq[7] = { 0x0088, 0x0028, 0x0020, 0x0018, 0x0010, 0x0008, 0x0000 };  // 0x02067344

const u16 fadeCloseSeq[7] = { 0x0000, 0x0008, 0x0010, 0x0018, 0x0020, 0x0028, 0x0088 };  // 0x02067352

const MesEffSeq diagDitherCloseSeq[7] = { { 0x0000 }, { 0x8038 }, { 0x0040 }, { 0x0048 }, { 0x0050 }, { 0x0058 }, { 0x0088 } };  // 0x02067328

const MesEffSeq diagDitherOpenSeq[7] = { { 0x0088 }, { 0x8060 }, { 0x0068 }, { 0x0070 }, { 0x0078 }, { 0x0080 }, { 0x0000 } };  // 0x02067336

const u32 effMasks[42 * 8] = {  // 0x02067360
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xf0fff0ff, 0xffffffff, 0xfff0fff0, 0xffffffff, 0xf0fff0ff, 0xffffffff, 0xfff0fff0, 0xffffffff,
    0xf0fff0ff, 0xff0fff0f, 0xfff0fff0, 0xffffffff, 0xf0fff0ff, 0xff0fff0f, 0xfff0fff0, 0xffffffff,
    0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0,
    0x0f000f00, 0x00f000f0, 0x000f000f, 0x00000000, 0x0f000f00, 0x00f000f0, 0x000f000f, 0x00000000,
    0x0f000f00, 0x00000000, 0x000f000f, 0x00000000, 0x0f000f00, 0x00000000, 0x000f000f, 0x00000000,
    0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f, 0xf0f0f0f0, 0x0f0f0f0f,
    0xffffff00, 0xfffff0f0, 0xffffffff, 0xfffffff0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffff0000, 0xfff0f0f0, 0xffffff00, 0xfffff0f0, 0xffffffff, 0xfffffff0, 0xffffffff, 0xffffffff,
    0xf0000000, 0x0f0f0000, 0xfff00000, 0xff0f0000, 0xfffff0f0, 0xffff0f00, 0xfffffff0, 0xffffff0f,
    0x00000000, 0x00000000, 0x0f000000, 0xf0000000, 0xff0f0000, 0xf0f00000, 0xffff0f00, 0xfff0f000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0f000000, 0xf0000000, 0xff0f0000, 0xfff00000,
    0x000000ff, 0x00000f0f, 0x00000000, 0x0000000f, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000ffff, 0x000f0f0f, 0x000000ff, 0x00000f0f, 0x00000000, 0x0000000f, 0x00000000, 0x00000000,
    0x0fffffff, 0xf0f0ffff, 0x000fffff, 0x00f0ffff, 0x00000f0f, 0x0000f0ff, 0x0000000f, 0x000000f0,
    0xffffffff, 0xffffffff, 0xf0ffffff, 0x0fffffff, 0x00f0ffff, 0x0f0fffff, 0x0000ffff, 0x000f0fff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xf0ffffff, 0x0fffffff, 0x00f0ffff, 0x000fffff,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0000ffff, 0x00000000, 0x0000ffff, 0x00000000, 0x0000ffff, 0x00000000, 0x0000ffff, 0x00000000,
    0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
    0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
    0x00000000, 0xffff0000, 0x00000000, 0xffff0000, 0x00000000, 0xffff0000, 0x00000000, 0xffff0000,
    0xffff0000, 0xffffffff, 0xffff0000, 0xffffffff, 0xffff0000, 0xffffffff, 0xffff0000, 0xffffffff,
    0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff,
    0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000, 0xffffffff, 0x00000000,
    0xffffffff, 0x0000ffff, 0xffffffff, 0x0000ffff, 0xffffffff, 0x0000ffff, 0xffffffff, 0x0000ffff,
    0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f, 0x0f0f0f0f,
    0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0, 0xf0f0f0f0,
    0xffffff00, 0xfffffff0, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xfff00000, 0xffff0000, 0xfffff000, 0xffffff00, 0xfffffff0, 0xffffffff, 0xffffffff, 0xffffffff,
    0x00000000, 0x00000000, 0x00000000, 0xf0000000, 0xff000000, 0xfff00000, 0xffff0000, 0xfffff000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xf0000000, 0xff000000,
    0x000000ff, 0x0000000f, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x000fffff, 0x0000ffff, 0x00000fff, 0x000000ff, 0x0000000f, 0x00000000, 0x00000000, 0x00000000,
    0xffffffff, 0xffffffff, 0xffffffff, 0x0fffffff, 0x00ffffff, 0x000fffff, 0x0000ffff, 0x00000fff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0x0fffffff, 0x00ffffff,
};

MesEffSeqPair diagClosePair = { diagCloseSeq, diagOpenSeq };  // 0x02069700

MesEffSeqPair diagDitherClosePair = { diagDitherCloseSeq, diagDitherOpenSeq };  // 0x020696f8

MesEffSeqPair diagOpenPair = { diagOpenSeq, diagCloseSeq };  // 0x02069710

MesEffSeqPair diagDitherOpenPair = { diagDitherOpenSeq, diagDitherCloseSeq };  // 0x02069708

u32 splitMaskLow[8] = {  // 0x02069718
    0x00000000, 0x0000000f, 0x000000ff, 0x00000fff, 0x0000ffff, 0x000fffff, 0x00ffffff, 0x0fffffff
};

u32 splitMaskHigh[19] = {  // 0x02069774
    0xffffffff, 0xfffffff0, 0xffffff00, 0xfffff000, 0xffff0000, 0xfff00000, 0xff000000, 0xf0000000, 0x00000000, 0x00000000, 0x00000000, 0xfffff000, 0xffff0000, 0xfff00000, 0xff000000, 0xf0000000, 0x00000000, 0x00000000, 0x00000000
};

u32 splitMaskHighAlt[19] = {  // 0x020697c0
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffff0, 0xffffff00, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xfffffff0, 0xffffff00
};

// --- Function Declarations --- //

BOOL stepNone(clMesWinEff *, MesWinSlot *);  // 0x02064454
BOOL stepFade(clMesWinEff *, MesWinSlot *);  // 0x020645d4
BOOL stepDiag(clMesWinEff *, MesWinSlot *);  // 0x02064814
BOOL stepDiagPair(clMesWinEff *, MesWinSlot *);  // 0x02064cdc
BOOL stepWipeH(clMesWinEff *, MesWinSlot *);  // 0x02065308
BOOL drawWipeColumn(clMesWinEff *, MesWinSlot *, MesEffWork *, BOOL);  // 0x02065500
BOOL stepWipeV(clMesWinEff *, MesWinSlot *);  // 0x02065710
BOOL drawWipeRow(clMesWinEff *, MesWinSlot *, MesEffWork *, BOOL);  // 0x02065908
BOOL stepSplitV(clMesWinEff *, MesWinSlot *);  // 0x02065b08
BOOL stepSplitH(clMesWinEff *, MesWinSlot *);  // 0x02065e7c

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clMesWinEff::allocEffBuffers(u32 heap, u16 count, u32 charSizeMain, u32 charSizeSub) {
    this->calcEffBufferSize(count, charSizeMain, charSizeSub);

    MesWinSlot * slots = (MesWinSlot *)::operator new[](this->bufSize, heap, NULL, FALSE);
    this->assignEffBuffers(slots);

    this->ownsBuffers = 1;
}

// MATCHED 100%
u32 clMesWinEff::calcEffBufferSize(u16 count, u32 charSizeMain, u32 charSizeSub) {
    this->effWorkOffset = this->calcBufferSize(count, charSizeMain, charSizeSub, 0, 0);
    this->bufSize = count * 0x60 + this->effWorkOffset;
    return this->bufSize;
}

// MATCHED 100%
void clMesWinEff::assignEffBuffers(MesWinSlot * buf) {
    this->assignBuffers(buf);
    this->effWork = (MesEffWork *)((u8*)buf + this->effWorkOffset);
}

// MATCHED 100%
s16 clMesWinEff::createEff(MesWinParams * params, s16 idx) {
    this->update();

    if (idx != -1 && this->windows[idx].chars != NULL) {
        this->destroy(idx);

        this->windows[idx].effCloseReq = 0;

        this->rebuildScreen(this->windows[idx].params.screen);

        if (this->windows[idx].params.screen == 0) {
            this->screenDirtyMain = 0;
        } else {
            this->screenDirtySub = 0;
        }

        this->replaced = 1;
    }

    s16 newIdx = this->create(params, idx);
    this->startOpen(&this->windows[newIdx]);
    return newIdx;
}

// MATCHED 100%
clMesWinEff::clMesWinEff(u32 screenMask, u32 bgMain, u32 bgSub, u8 priorityMain, u8 prioritySub, u32 initBG, clMesIcon * icons) 
    : clMesWin(screenMask, bgMain, bgSub, priorityMain, prioritySub, initBG, icons) {}

// MATCHED 100%
void clMesWinEff::updateList(MesWinSlot * window) {
    while (window->next != (MesWinSlot *)-1) {
        this->curSlot = window->slot;

        if (window->copyPending) {
            window->copyPending = 0;

            u32 size = window->charSize;
            if (window->charsBack < window->chars) {
                cpu_copy32_rev(window->charsBack, window->chars, size);
            } else {
                cpu_copy32_fwd(window->charsBack, window->chars, size);
            }

            this->windows[(s16)window->slot].dirty = 1;
            this->charsDirty = 1;
        }

        if (window->voiceRestart) {
            this->setVoice(window, (window->voiceRestart != 1));
            window->voiceRestart = 0;
        }

        if (window->clearStep != 0) {
            this->stepClear(window);
        } else {
            this->updateEffect(window);
        }

        switch (window->params.mode) {
            case 0: {
                this->scrollIcon(&window->cursor);
                this->scrollIcon(&window->tail);
                break;
            }
            case 1: {
                this->drawQuad(window);
                break;
            }
        }

        if (window->dirty) {
            DC_FlushRange(window->chars, window->charSize);
        }

        window = window->next;
    }
}

// MATCHED 100%
void clMesWinEff::startOpen(MesWinSlot * window) {
    this->writeScreen(&this->windows[window->slot]);

    this->screenFlagged = 0;

    switch (window->params.effect) {
        case 1: {
            this->startFade(window, fadeOpenSeq);
            break;
        }
        case 2: {
            this->startDiag(window, diagOpenSeq, 6);
            break;
        }
        case 3: {
            this->startDiagPair(window, &diagOpenPair, 6);
            break;
        }
        case 4: {
            this->startWipeH(window, wipeHOpenSeq);
            break;
        }
        case 5: {
            this->startWipeV(window, wipeVOpenSeq);
            break;
        }
        case 6: {
            this->startDiag(window, diagDitherOpenSeq, 7);
            break;
        }
        case 7: {
            this->startDiagPair(window, &diagDitherOpenPair, 7);
            break;
        }
        case 8: {
            this->startSplitV(window);
            break;
        }
        case 9: {
            this->startSplitH(window);
            break;
        }
        default: {
            this->startNone(window);
            break;
        }
    }
}

// MATCHED 100% -- NOTE: Source bug ? case 2 falls through to case 3
void clMesWinEff::updateEffect(MesWinSlot * window) {
    switch (window->effState) {
        case 0: {
            for (s32 step = window->params.effectFast; step >= 0; step--) {
                this->subStep = step;

                if (window->effStep(this, window)) {
                    if (window->tail.active == 0) {
                        this->addTail(window);
                    }

                    window->effState = 1;
                    break;
                }
            }

            if (window->params.mode == 0 && this->screenFlagged == 0) {
                this->screenFlagged = 1;

                if ((this->windows[window->slot].params.screen & 0xff) == 0) {
                    this->screenDirtyMain = 1;
                } else {
                    this->screenDirtySub = 1;
                }
            }

            break;
        }
        case 1: {
            if (window->effCloseReq) {
                window->effState = 2;
            } else if (window->writer.text != 0 && window->paused == 0 && this->checkClose(window) == FALSE) {
                this->advanceText(window);
            }
            break;
        }
        case 2: {
            switch (window->params.effect) {
                case 1: {
                    this->startFade(window, fadeCloseSeq);
                    break;
                }
                case 2: {
                    this->startDiag(window, diagCloseSeq, 6);
                    break;
                }
                case 3: {
                    this->startDiagPair(window, &diagClosePair, 6);
                    break;
                }
                case 4: {
                    this->startWipeH(window, wipeHCloseSeq);
                    break;
                }
                case 5: {
                    this->startWipeV(window, wipeVCloseSeq);
                    break;
                }
                case 6: {
                    this->startDiag(window, diagDitherCloseSeq, 7);
                    break;
                }
                case 7: {
                    this->startDiagPair(window, &diagDitherClosePair, 7);
                    break;
                }
                case 8: {
                    this->startSplitV(window);
                    break;
                }
                case 9: {
                    this->startSplitH(window);
                    break;
                }
                default: {
                    this->startNone(window);
                    break;
                }
            }

            window->effState = 3;

            if (window->params.effect != 0 || window->params.mode == 1) {
                if (window->cursor.active) {
                    window->cursor.active = 0;
                    window->cursor.prev->next = window->cursor.next;
                    window->cursor.next->prev = window->cursor.prev;
                }

                if (window->tail.active) {
                    window->tail.active = 0;
                    window->tail.prev->next = window->tail.next;
                    window->tail.next->prev = window->tail.prev;
                }
            }
        }
        case 3: {
            for (s32 step = window->params.effectFast; step >= 0; step--) {
                this->subStep = step;

                if (window->effStep(this, window)) {
                    window->effState = 4;
                    break;
                }
            }
            break;
        }
        case 4: {
            if (this->destroy(window->slot)) {
                window->effCloseReq = 0;

                this->rebuildScreen(window->params.screen);
            }

            window->closeReq = 0;
            break;
        }
    }
}

// MATCHED 100%
void clMesWinEff::startNone(MesWinSlot * window) {
    window->effStep = stepNone;

    if (window->params.mode == 1) {
        u32 size = window->charSize;
        if (window->charsBack < window->chars) {
            cpu_copy32_rev(window->charsBack, window->chars, size);
        } else {
            cpu_copy32_fwd(window->charsBack, window->chars, size);
        }

        if (this->replaced == 0) {
            this->windows[(s16)window->slot].dirty = 1;
            this->charsDirty = 1;
        }

        this->replaced = 0;
    }

    window->effErase = 0;
    if (window->effState == 2) {
        window->effErase = 1;
    }
}

// MATCHED 100%
BOOL stepNone(clMesWinEff * self, MesWinSlot * window) {
    if (window->effErase == 0) {
        self->windows[(s16)window->slot].dirty = 1;
        self->charsDirty = 1;

        if (window->params.mode == 0 && window->tail.active == 0) {
            self->addTail(&self->windows[window->slot]);

            u32 size = window->charSize;
            if (window->charsBack < window->chars) {
                cpu_copy32_rev(window->charsBack, window->chars, size);
            } else {
                cpu_copy32_fwd(window->charsBack, window->chars, size);
            }

            self->windows[(s16)window->slot].dirty = 1;
            self->charsDirty = 1;
        }

        return TRUE;
    }

    cpu_fill32(window->chars, 0, window->charSize);
    return TRUE;
}

// MATCHED 100%
void clMesWinEff::startFade(MesWinSlot * window, const u16 * seq) {
    MesEffWork * work = &this->effWork[(u16)window->slot * 12];
    work->frame = 0;
    work->timer = 1;

    window->effStep = stepFade;
    window->effData = seq;

    window->effErase = 0;
    if (window->effState == 2) {
        window->effErase = 1;
    }
}

// MATCHED 100%
BOOL stepFade(clMesWinEff * self, MesWinSlot * window) {
    const u32 * mask;
    u32 * src;
    u32 * dst;
    BOOL done;
    s32 tile;

    MesEffWork * work = &self->effWork[(u16)window->slot * 12];
    src = window->charsBack;
    dst = window->chars;
    mask = &effMasks[((u16*)window->effData)[work->frame]];

    done = FALSE;

    if (--work->timer & 0xff) {
        return done;
    }

    work->timer = 3;

    if (work->frame+1 == 7) {
        done = TRUE;
    }

    if (self->subStep == 0 || done) {
        for (tile = (window->charSize / 32) - 1; tile >= 0; tile--) {
            dst[0] = src[0] & mask[0];
            dst[1] = src[1] & mask[1];
            dst[2] = src[2] & mask[2];
            dst[3] = src[3] & mask[3];
            dst[4] = src[4] & mask[4];
            dst[5] = src[5] & mask[5];
            dst[6] = src[6] & mask[6];
            dst[7] = src[7] & mask[7];

            src += 8;
            dst += 8;
        }

        self->windows[(s16)window->slot].dirty = 1;
        self->charsDirty = 1;
    }

    work->frame++;

    return done;
}

// MATCHED 100%
void clMesWinEff::startDiag(MesWinSlot * window, const MesEffSeq * seq, u32 seqLen) {
    MesEffWork * work = &this->effWork[(u16)window->slot * 12];
    cpu_fill32(work, 0, 12 * sizeof(MesEffWork));

    window->effErase = 0;
    if (window->effState == 2) {
        window->effErase = 1;
    }

    work->active = 1;
    work->frame = 0;
    work->timer = 1;

    window->effStep = stepDiag;
    window->effData = seq;
    window->effLen = seqLen;
}


static inline MesEffWork * findFreeWork(MesEffWork * work) {
    for (s32 i = 11; i >= 0; i--, work++) {
        if (!work->active) {
            return work;
        }
    }
    return NULL;
}

// MATCHED 100%
BOOL stepDiag(clMesWinEff * self, MesWinSlot * window) {
    BOOL done = FALSE;
    MesEffSeq * seq = (MesEffSeq *)window->effData;
    MesEffWork * works = &self->effWork[(u16)window->slot * 12];
    BOOL drew = FALSE;
    MesEffWork * work = works;

    for (s32 n = 11; n >= 0; n--, work++) {
        if (work->active) {
            if (work->skip) {
                work->skip = 0;
            } else if ((--work->timer & 0xff) == 0) {
                work->timer = 1;

                if (work->frame+1 == window->effLen) {
                    work->active = 0;
                }

                if (self->subStep == 0 || !work->active) {
                    u16 maskOfs = seq[work->frame].maskOfs;
                    s8 col = work->x;
                    s8 row = work->y;
                    const u32 * mask = &effMasks[maskOfs];

                    while (col >= 0 && row <= window->rows) {
                        s32 tileOfs = (row * (window->cols + 1) + col) * 8;
                        u32 * src = &window->charsBack[tileOfs];
                        u32 * dst = &window->chars[tileOfs];

                        col--;
                        row++;

                        dst[0] = src[0] & mask[0];
                        dst[1] = src[1] & mask[1];
                        dst[2] = src[2] & mask[2];
                        dst[3] = src[3] & mask[3];
                        dst[4] = src[4] & mask[4];
                        dst[5] = src[5] & mask[5];
                        dst[6] = src[6] & mask[6];
                        dst[7] = src[7] & mask[7];
                    }

                    drew = TRUE;
                }

                work->frame++;

                if (!work->active) {
                    if (work->last) {
                        done = TRUE;
                        break;
                    }
                } else if (seq[work->frame].spawn && work->last == 0) {
                    MesEffWork * spawned = findFreeWork(works);
                    spawned->active = 1;

                    if (work->x == window->cols) {
                        spawned->x = work->x;

                        if (work->y == window->rows) {
                            spawned->last = 1;
                        }

                        spawned->y = work->y + 1;
                    } else {
                        spawned->x = work->x + 1;
                        spawned->y = work->y;
                    }

                    spawned->timer = 2;
                    spawned->frame = 0;

                    if (spawned > work) {
                        spawned->skip = 1;
                    }
                }
            }
        }
    }

    if (drew) {
        self->windows[(s16)window->slot].dirty = 1;
        self->charsDirty = 1;
    }

    return done;
}

// MATCHED 100%
void clMesWinEff::startDiagPair(MesWinSlot * window, MesEffSeqPair * pair, u32 seqLen) {
    s16 backCol;
    s16 backRow;
    s16 fwdCol;
    s16 fwdRow;

    MesEffWork * fwdWork = &this->effWork[(u16)window->slot * 12];
    cpu_fill32(fwdWork, 0, 12 * sizeof(MesEffWork));

    window->effErase = 0;
    if (window->effState == 2) {
        window->effErase = 1;
    }

    fwdWork->active = 1;
    fwdWork->frame = 0;
    fwdWork->timer = 1;
    fwdWork->field00_2 = 0;

    fwdCol = fwdWork->x;
    fwdRow = fwdWork->y;

    MesEffWork * backWork = fwdWork+1;

    backWork->active = 1;
    backWork->x = window->cols;
    backWork->y = window->rows;
    backWork->frame = 0;
    backWork->timer = 1;
    backWork->field00_2 = 1;

    backCol = backWork->x;
    backRow = backWork->y;

    window->effStep = stepDiagPair;
    window->effData = pair;
    window->effMet = 0;
    window->effLen = seqLen;

    s16 tipCol;
    s16 tipRow;
    do {
        if (fwdCol == window->cols) {
            fwdRow++;
        } else {
            fwdCol++;
        }

        tipCol = fwdCol;
        tipRow = fwdRow;

        while (tipCol > 0 && tipRow < window->rows) {
            tipCol--;
            tipRow++;
        }

        if (backCol == 0) {
            backRow--;
        } else {
            backCol--;
        }
    } while (tipCol <= backCol && backRow >= tipRow);

    window->effMeetCol = fwdCol;
    window->effMeetRow = fwdRow;
}

// MATCHED 100%
BOOL stepDiagPair(clMesWinEff * self, MesWinSlot * window) {
    const MesEffSeq * seq;
    BOOL done = FALSE;
    s32 n;
    MesEffWork * works = &self->effWork[(u16)window->slot * 12];
    BOOL drew = done;
    MesEffWork * work = works;

    for (n = 11; n >= 0; n--, work++) {
        if (work->active) {
            if (work->skip) {
                work->skip = 0;
            } else if ((--work->timer & 0xff) == 0) {
                work->timer = 1;

                if (work->frame + 1 == window->effLen) {
                    work->active = 0;
                }

                s8 col = work->x;
                s8 row = work->y;

                if (!work->field00_2) {
                    seq = ((MesEffSeqPair *)window->effData)->fwd;
                    const u32 * mask = &effMasks[seq[work->frame].maskOfs];

                    if (self->subStep == 0 || !work->active) {
                        while (col >= 0 && row <= window->rows) {
                            s32 tileOfs = (row * (window->cols + 1) + col) * 8;
                            u32 * src = &window->charsBack[tileOfs];
                            u32 * dst = &window->chars[tileOfs];

                            if (window->effErase) {
                                dst[0] = dst[0] & mask[0];
                                dst[1] = dst[1] & mask[1];
                                dst[2] = dst[2] & mask[2];
                                dst[3] = dst[3] & mask[3];
                                dst[4] = dst[4] & mask[4];
                                dst[5] = dst[5] & mask[5];
                                dst[6] = dst[6] & mask[6];
                                dst[7] = dst[7] & mask[7];
                            } else {
                                for (s32 i = 7; i >= 0; i--) {
                                    dst[i] = (src[i] & mask[i]) | (dst[i] & (mask[i] ^ 0xffffffff));
                                }
                            }

                            col--;
                            row++;
                        }

                        drew = TRUE;
                    }
                } else {
                    seq = ((MesEffSeqPair *)window->effData)->back;
                    const u32 * mask = &effMasks[seq[window->effLen - 1 - work->frame].maskOfs];

                    if (self->subStep == 0 || !work->active) {
                        while (col <= window->cols && row >= 0) {
                            s32 tileOfs = (row * (window->cols + 1) + col) * 8;
                            u32 * src = &window->charsBack[tileOfs];
                            u32 * dst = &window->chars[tileOfs];

                            if (window->effErase) {
                                dst[0] = dst[0] & mask[0];
                                dst[1] = dst[1] & mask[1];
                                dst[2] = dst[2] & mask[2];
                                dst[3] = dst[3] & mask[3];
                                dst[4] = dst[4] & mask[4];
                                dst[5] = dst[5] & mask[5];
                                dst[6] = dst[6] & mask[6];
                                dst[7] = dst[7] & mask[7];
                            } else {
                                for (s32 i = 7; i >= 0; i--) {
                                    dst[i] = (src[i] & mask[i]) | (dst[i] & (mask[i] ^ 0xffffffff));
                                }
                            }

                            col++;
                            row--;
                        }

                        drew = TRUE;
                    }
                }

                work->frame++;

                if (!work->active) {
                    if (work->last) {
                        done = TRUE;
                        break;
                    }
                } else if (seq[work->frame].spawn && work->last == 0) {
                    MesEffWork * spawned = findFreeWork(works);
                    spawned->active = 1;

                    if (!work->field00_2) {
                        if (work->x == window->cols) {
                            spawned->x = work->x;
                            spawned->y = work->y + 1;
                        } else {
                            spawned->x = work->x + 1;
                            spawned->y = work->y;
                        }

                        if (spawned->x == window->effMeetCol && spawned->y == window->effMeetRow) {
                            spawned->last = 1;
                            window->effMet = 1;
                        }
                    } else {
                        if (work->x == 0) {
                            spawned->x = work->x;
                            spawned->y = work->y - 1;
                        } else {
                            spawned->x = work->x - 1;
                            spawned->y = work->y;
                        }

                        if (window->effMet) {
                            spawned->last = 1;
                        }
                    }

                    spawned->timer = 2;
                    spawned->frame = 0;
                    spawned->field00_2 = work->field00_2;

                    if (spawned > work) {
                        spawned->skip = 1;
                    }
                }
            }
        }
    }

    if (drew) {
        self->windows[(s16)window->slot].dirty = 1;
        self->charsDirty = 1;
    }

    return done;
}

// MATCHED 100%
void clMesWinEff::startWipeH(MesWinSlot * window, const u16 * seq) {
    MesEffWork * fwdWork = &this->effWork[(u16)window->slot * 12];
    cpu_fill32(fwdWork, 0, 12 * sizeof(MesEffWork));

    window->effErase = 0;
    if (window->effState == 2) {
        window->effErase = 1;
    }

    fwdWork->frame = 0;
    fwdWork->timer = 1;

    MesEffWork * backWork = fwdWork+1;
    backWork->x = window->cols;
    backWork->frame = 2;
    backWork->timer = 1;

    window->effStep = stepWipeH;
    window->effData = seq;
}

// MATCHED 100%
BOOL stepWipeH(clMesWinEff * self, MesWinSlot * window) {
    BOOL lastFrame = FALSE;
    MesEffWork * fwdWork = &self->effWork[(u16)window->slot * 12];

    if (fwdWork->field00_2 == 0 && (--fwdWork->timer & 0xff) == 0) {
        fwdWork->timer = 1;

        if (fwdWork->frame+1 == 2) {
            lastFrame = TRUE;
        }

        if (drawWipeColumn(self, window, fwdWork, lastFrame)) {
            self->windows[(s16)window->slot].dirty = 1;
            self->charsDirty = 1;
        }

        fwdWork->frame++;

        if (lastFrame) {
            fwdWork->frame = 0;

            if (++fwdWork->x > window->cols) {
                fwdWork->field00_2 = 1;
            }
        }
    }

    MesEffWork * backWork = fwdWork+1;

    if (backWork->field00_2 == 0 && (--backWork->timer & 0xff) == 0) {
        backWork->timer = 1;

        lastFrame = FALSE;
        if (backWork->frame+1 == 4) {
            lastFrame = TRUE;
        }

        if (drawWipeColumn(self, window, backWork, lastFrame)) {
            self->windows[(s16)window->slot].dirty = 1;
            self->charsDirty = 1;
        }

        backWork->frame++;

        if (lastFrame) {
            backWork->frame = 2;

            if (--backWork->x < 0) {
                backWork->field00_2 = 1;
            }
        }
    }

    if (fwdWork->field00_2 && backWork->field00_2) {
        return TRUE;
    }
    return FALSE;
}

// MATCHED 100%
BOOL drawWipeColumn(clMesWinEff * self, MesWinSlot * window, MesEffWork * work, BOOL lastFrame) {
    if (self->subStep != 0 && lastFrame == FALSE) {
        return FALSE;
    }

    const u32 * mask = &effMasks[((u16 *)window->effData)[work->frame]];
    u32 * src = &window->charsBack[work->x * 8];
    u32 * dst = &window->chars[work->x * 8];

    if (window->effErase == 0) {
        for (s32 row = window->rows; row >= 0; row--) {
            for (s32 i = 7; i >= 0; i--) {
                dst[i] = (src[i] & mask[i]) | (dst[i] & (mask[i] ^ 0xffffffff));
            }

            src += (window->cols+1) * 8;
            dst += (window->cols+1) * 8;
        }
    } else {
        for (s32 row = window->rows; row >= 0; row--) {
            dst[0] &= mask[0];
            dst[1] &= mask[1];
            dst[2] &= mask[2];
            dst[3] &= mask[3];
            dst[4] &= mask[4];
            dst[5] &= mask[5];
            dst[6] &= mask[6];
            dst[7] &= mask[7];

            dst += (window->cols+1) * 8;
        }
    }

    return TRUE;
}

// MATCHED 100%
void clMesWinEff::startWipeV(MesWinSlot * window, const u16 * seq) {
    MesEffWork * fwdWork = &this->effWork[(u16)window->slot * 12];
    cpu_fill32(fwdWork, 0, 12 * sizeof(MesEffWork));

    window->effErase = 0;
    if (window->effState == 2) {
        window->effErase = 1;
    }

    fwdWork->frame = 0;
    fwdWork->timer = 1;

    MesEffWork * backWork = fwdWork+1;
    backWork->y = window->rows;
    backWork->frame = 2;
    backWork->timer = 1;

    window->effStep = stepWipeV;
    window->effData = seq;
}

// MATCHED 100%
BOOL stepWipeV(clMesWinEff * self, MesWinSlot * window) {
    BOOL lastFrame = FALSE;
    MesEffWork * fwdWork = &self->effWork[(u16)window->slot * 12];

    if (fwdWork->field00_2 == 0 && (--fwdWork->timer & 0xff) == 0) {
        fwdWork->timer = 1;

        if (fwdWork->frame+1 == 2) {
            lastFrame = TRUE;
        }

        if (drawWipeRow(self, window, fwdWork, lastFrame)) {
            self->windows[(s16)window->slot].dirty = 1;
            self->charsDirty = 1;
        }

        fwdWork->frame++;

        if (lastFrame) {
            fwdWork->frame = 0;

            if (++fwdWork->y > window->rows) {
                fwdWork->field00_2 = 1;
            }
        }
    }

    MesEffWork * backWork = fwdWork+1;

    if (backWork->field00_2 == 0 && (--backWork->timer & 0xff) == 0) {
        backWork->timer = 1;

        lastFrame = FALSE;

        if (backWork->frame+1 == 4) {
            lastFrame = TRUE;
        }

        if (drawWipeRow(self, window, backWork, lastFrame)) {
            self->windows[(s16)window->slot].dirty = 1;
            self->charsDirty = 1;
        }

        backWork->frame++;

        if (lastFrame) {
            backWork->frame = 2;

            if (--backWork->y < 0) {
                backWork->field00_2 = 1;
            }
        }
    }

    if (fwdWork->field00_2 && backWork->field00_2) {
        return TRUE;
    }
    return FALSE;
}

// MATCHED 100%
BOOL drawWipeRow(clMesWinEff * self, MesWinSlot * window, MesEffWork * work, BOOL lastFrame) {
    if (self->subStep != 0 && lastFrame == FALSE) {
        return FALSE;
    }

    const u32 * mask = &effMasks[((u16 *)window->effData)[work->frame]];
    u32 * src = &window->charsBack[work->y * (window->cols + 1) * 8];
    u32 * dst = &window->chars[work->y * (window->cols + 1) * 8];

    if (window->effErase == 0) {
        for (s32 col = window->cols; col >= 0; col--) {
            for (s32 i = 7; i >= 0; i--) {
                dst[i] = (src[i] & mask[i]) | (dst[i] & (mask[i] ^ 0xffffffff));
            }

            src += 8;
            dst += 8;
        }
    } else {
        for (s32 col = window->cols; col >= 0; col--) {
            dst[0] &= mask[0];
            dst[1] &= mask[1];
            dst[2] &= mask[2];
            dst[3] &= mask[3];
            dst[4] &= mask[4];
            dst[5] &= mask[5];
            dst[6] &= mask[6];
            dst[7] &= mask[7];

            dst += 8;
        }
    }

    return TRUE;
}

// MATCHED 100%
void clMesWinEff::startSplitV(MesWinSlot * window) {
    MesEffWork * work = &this->effWork[(u16)window->slot * 12];
    cpu_fill32(work, 0, 12 * sizeof(MesEffWork));

    window->effHalf = (window->params.height + 1) * 8 / 2;
    window->effErase = 0;

    if (window->effState == 2) {
        window->effErase = 1;

        work->y = window->effHalf;
    }

    window->effStep = stepSplitV;
}

// MATCHED 100%
BOOL stepSplitV(clMesWinEff * self, MesWinSlot * window) {
    s16 inset;
    u32 fillSize;
    BOOL done;
    u32 stride;
    u32 subY;
    MesEffWork * work;
    u32 lastLine;

    work = &self->effWork[(u16)window->slot * 12];
    stride = (window->cols + 1) * 8;
    done = FALSE;
    lastLine = (window->params.height + 1) * 8 - 1;
    subY = window->params.y % 8;

    if (!window->effErase) {
        if (work->y + 2 > window->effHalf) {
            done = TRUE;
        }
    } else {
        if (work->y - 2 < 0) {
            done = TRUE;
        }
    }

    if (self->subStep == 0 || done) {
        if (window->effErase) {
            inset = window->effHalf - work->y;
            s32 topOfs = ((subY + inset) & ~7) - 8;

            if (topOfs < 0) {
                topOfs += 8;
            }

            topOfs *= window->cols + 1;
            fillSize = (window->cols + 1) * 64;
            cpu_fill32(&window->chars[topOfs], 0, fillSize);

            u32 bottomOfs;

            if (subY != 0) {
                bottomOfs = (subY + window->rows * 8 - inset) & ~7;

                if (bottomOfs >= window->rows * 8) {
                    bottomOfs -= 8;
                }
            } else {
                bottomOfs = (window->rows * 8 + 8 - inset) & ~7;

                if (bottomOfs >= window->rows * 8) {
                    fillSize /= 2;
                }
            }

            bottomOfs *= window->cols + 1;
            cpu_fill32(&window->chars[bottomOfs], 0, fillSize);
        }

        for (s32 line = subY; line < work->y + subY; line++) {
            s32 srcOfs = line & 7;
            srcOfs += (line >> 3) * stride;

            u32 * src = &window->charsBack[srcOfs];
            u32 dstLine = line + (window->effHalf - work->y);
            u32 * dst = &window->chars[(dstLine / 8) * stride + (dstLine % 8)];

            for (s32 i = 0; i <= window->cols * 8; i += 8) {
                dst[i] = src[i];
            }
        }

        for (s32 n = 0; n <= work->y; n++) {
            u32 srcLine = lastLine + subY - n;
            u32 * src = &window->charsBack[(srcLine / 8) * stride + (srcLine % 8)];
            u32 dstLine = subY + (window->effHalf - 1 + work->y) - n;
            u32 * dst = &window->chars[(dstLine / 8) * stride + (dstLine % 8)];

            for (s32 i = 0; i <= window->cols * 8; i += 8) {
                dst[i] = src[i];
            }
        }

        self->windows[(s16)window->slot].dirty = 1;
        self->charsDirty = 1;
    }

    if (!window->effErase) {
        work->y += 2;
    } else {
        work->y -= 2;
    }

    return done;
}

// MATCHED 100%
void clMesWinEff::startSplitH(MesWinSlot * window) {
    MesEffWork * work = &this->effWork[(u16)window->slot * 12];
    cpu_fill32(work, 0, 12 * sizeof(MesEffWork));

    window->effHalf = (window->params.width + 1) * 8 / 2;
    window->effErase = 0;

    if (window->effState == 2) {
        window->effErase = 1;

        work->x = window->effHalf;
    }

    window->effStep = stepSplitH;
}

// MATCHED 100%
BOOL stepSplitH(clMesWinEff * self, MesWinSlot * window) {
    BOOL done;
    BOOL shiftNeg;
    s32 rowShl;
    s32 rowShr;
    u32 * src;
    u32 * dst;
    u32 stride;
    u32 leftOfs;
    s32 leftSrc;
    u32 maskAlt;

    MesEffWork * work =&self->effWork[(u16)window->slot * 12];
    done = FALSE;

    if (!window->effErase) {
        if (work->x + 2 > window->effHalf) {
            done = TRUE;
        }
    } else {
        if (work->x - 2 < 0) {
            done = TRUE;
        }
    }

    if (self->subStep == 0 || done) {
        cpu_fill32(window->chars, 0, window->charSize);

        stride = (window->cols + 1) * 8;
    
        u8 subHalf = window->effHalf & 7;
        u8 subX = window->params.x & 7;

        leftOfs = (subHalf + (window->effHalf - work->x)) & ~7;
        if ((work->x & 7) == 0) {
            leftOfs -= 8;
        }

        s8 shift = (7 - (work->x & 7) - (subHalf - 1)) * 4;
        if (shift >= 0) {
            shiftNeg = FALSE;
        } else {
            shift *= -1;
            shiftNeg = TRUE;
        }

        s8 carryShift = 32 - shift;
        s32 srcEnd = (work->x & ~7) + subX - subHalf;

        if (shiftNeg) {
            for (leftSrc = 0; leftSrc < srcEnd; leftSrc += 8) {
                dst = &window->chars[leftOfs];
                src = &window->charsBack[leftSrc];

                for (s32 row = window->rows; row >= 0; row--) {
                    dst[0] += (src[0] >> shift);
                    dst[1] += (src[1] >> shift);
                    dst[2] += (src[2] >> shift);
                    dst[3] += (src[3] >> shift);
                    dst[4] += (src[4] >> shift);
                    dst[5] += (src[5] >> shift);
                    dst[6] += (src[6] >> shift);
                    dst[7] += (src[7] >> shift);

                    dst[0] += (src[8] << carryShift);
                    dst[1] += (src[9] << carryShift);
                    dst[2] += (src[10] << carryShift);
                    dst[3] += (src[11] << carryShift);
                    dst[4] += (src[12] << carryShift);
                    dst[5] += (src[13] << carryShift);
                    dst[6] += (src[14] << carryShift);
                    dst[7] += (src[15] << carryShift);

                    src += stride;
                    dst += stride;
                }

                leftOfs += 8;
            }
        } else {
            for (leftSrc = 0; leftSrc < srcEnd; leftSrc += 8) {
                dst = &window->chars[leftOfs];
                src = &window->charsBack[leftSrc];

                for (s32 row = window->rows; row >= 0; row--) {
                    dst[0] += (src[0] << shift);
                    dst[1] += (src[1] << shift);
                    dst[2] += (src[2] << shift);
                    dst[3] += (src[3] << shift);
                    dst[4] += (src[4] << shift);
                    dst[5] += (src[5] << shift);
                    dst[6] += (src[6] << shift);
                    dst[7] += (src[7] << shift);
                    dst[8] += (src[0] >> carryShift);
                    dst[9] += (src[1] >> carryShift);
                    dst[10] += (src[2] >> carryShift);
                    dst[11] += (src[3] >> carryShift);
                    dst[12] += (src[4] >> carryShift);
                    dst[13] += (src[5] >> carryShift);
                    dst[14] += (src[6] >> carryShift);
                    dst[15] += (src[7] >> carryShift);

                    src += stride;
                    dst += stride;
                }

                leftOfs += 8;
            }
        }

        u32 mask;
        if (subHalf != 0) {
            mask = splitMaskLow[(subHalf + window->params.x - 1) & 7];
        } else {
            mask = splitMaskLow[subX];
            if (mask == 0) {
                mask = 0xffffffff;
            }
        }

        dst = &window->chars[leftOfs];
        src = &window->charsBack[leftSrc];

        if (shiftNeg) {
            for (s32 row = window->rows; row >= 0; row--) {
                for (s32 i = 7; i >= 0; i--) {
                    dst[i] = mask & (dst[i] + (src[i] >> shift) + (src[i+8] << carryShift));
                }

                src += stride;
                dst += stride;
            }
        } else {
            for (s32 row = window->rows; row >= 0; row--) {
                for (s32 i = 7; i >= 0; i--) {
                    dst[i] = mask & (dst[i] + (src[i] << shift));
                }

                src += stride;
                dst += stride;
            }
        }

        u32 rightOfs = ((window->effHalf + work->x) - subHalf) & ~7;
        if (subX != 0) {
            rightOfs += 8;
        }

        shift = (8 - (work->x & 7) - subHalf) * 4;
        if (shift >= 0) {
            shiftNeg = FALSE;
        } else {
            shift *= -1;
            shiftNeg = TRUE;
        }

        s32 rightSrc = 0;
        carryShift = 32 - shift;

        srcEnd = (work->x & ~7) + subX;
        if (subHalf != 0 || subX != 0) {
            srcEnd -= 8;
        }

        if (shiftNeg) {
            while (rightSrc < srcEnd) {
                dst = &window->chars[rightOfs];
                src = &window->charsBack[window->cols * 8] - rightSrc;

                for (s32 row = window->rows; row >= 0; row--) {
                    dst[0] += (src[0] << shift);
                    dst[1] += (src[1] << shift);
                    dst[2] += (src[2] << shift);
                    dst[3] += (src[3] << shift);
                    dst[4] += (src[4] << shift);
                    dst[5] += (src[5] << shift);
                    dst[6] += (src[6] << shift);
                    dst[7] += (src[7] << shift);
                    dst[8] += (src[0] >> carryShift);
                    dst[9] += (src[1] >> carryShift);
                    dst[10] += (src[2] >> carryShift);
                    dst[11] += (src[3] >> carryShift);
                    dst[12] += (src[4] >> carryShift);
                    dst[13] += (src[5] >> carryShift);
                    dst[14] += (src[6] >> carryShift);
                    dst[15] += (src[7] >> carryShift);

                    src += stride;
                    dst += stride;
                }

                rightOfs -= 8;
                rightSrc += 8;
            }
        } else {
            while (rightSrc < srcEnd) {
                dst = &window->chars[rightOfs];
                src = &window->charsBack[window->cols * 8] - rightSrc;

                for (s32 row = window->rows; row >= 0; row--) {
                    dst[0] = dst[0] + (src[0] >> shift);
                    dst[1] = dst[1] + (src[1] >> shift);
                    dst[2] = dst[2] + (src[2] >> shift);
                    dst[3] = dst[3] + (src[3] >> shift);
                    dst[4] = dst[4] + (src[4] >> shift);
                    dst[5] = dst[5] + (src[5] >> shift);
                    dst[6] = dst[6] + (src[6] >> shift);
                    dst[7] = dst[7] + (src[7] >> shift);
                    dst[-8] = dst[-8] + (src[0] << carryShift);
                    dst[-7] = dst[-7] + (src[1] << carryShift);
                    dst[-6] = dst[-6] + (src[2] << carryShift);
                    dst[-5] = dst[-5] + (src[3] << carryShift);
                    dst[-4] = dst[-4] + (src[4] << carryShift);
                    dst[-3] = dst[-3] + (src[5] << carryShift);
                    dst[-2] = dst[-2] + (src[6] << carryShift);
                    dst[-1] = dst[-1] + (src[7] << carryShift);
                    src += stride;
                    dst += stride;
                }

                rightOfs -= 8;
                rightSrc += 8;
            }
        }


        if (subHalf != 0) {
            u32 maskIdx = (window->params.x & 0xF) + (subHalf - 1);
            mask = splitMaskHigh[maskIdx];
            maskAlt = splitMaskHighAlt[maskIdx];
        } else {
            mask = splitMaskHigh[subX];
            maskAlt = 0xFFFFFFFF;
        }

        if (srcEnd + 8 > 0 && (subHalf != 0 || subX != 0)) {
            src = &window->charsBack[window->cols * 8] - rightSrc;
            dst = &window->chars[rightOfs];

            if (shiftNeg) {
                for (rowShl = window->rows; rowShl >= 0; rowShl--) {
                    for (s32 i = 7; i >= 0; i--) {
                        dst[i] = dst[i] + (maskAlt & (src[i] << shift));
                        dst[i + 8] = dst[i + 8] + (src[i] >> carryShift);
                    }

                    src += stride;
                    dst += stride;
                }
            } else {
                for (rowShr = window->rows; rowShr >= 0; rowShr--) {
                    for (s32 i = 7; i >= 0; i--) {
                        dst[i] = dst[i] + (maskAlt & (src[i] >> shift));
                        dst[i - 8] = dst[i - 8] + (mask & (src[i] << carryShift));
                    }

                    src += stride;
                    dst += stride;
                }
            }

            rightOfs -= 8;
            rightSrc += 8;
        }

        dst = &window->chars[rightOfs];
        src = &window->charsBack[window->cols * 8] - rightSrc;
        if (shiftNeg) {
            for (s32 row = window->rows; row >= 0; row--) {
                for (s32 i = 7; i >= 0; i--) {
                    dst[i] = dst[i] + (mask & (src[i] << shift));
                    dst[i + 8] = dst[i + 8] + (maskAlt & (src[i] >> carryShift));
                }

                src += stride;
                dst += stride;
            }
        } else {
            for (s32 row = window->rows; row >= 0; row--) {
                for (s32 i = 7; i >= 0; i--) {
                    dst[i] = dst[i] + (mask & (src[i] >> shift));
                }

                src += stride;
                dst += stride;
            }
        }

        self->windows[(s16)window->slot].dirty = 1;
        self->charsDirty = 1;
    }

    if (!window->effErase) {
        work->x += 2;
    } else {
        work->x -= 2;
    }

    return done;
}

// MATCHED 100%
void clMesWinEff::drawFrame(MesWinSlot * window, BOOL flush) {
    if (window->params.style != 0) {
        switch (window->params.mode) {
            case 0: {
                if ((window->params.x & 7) || (window->params.y & 7)) {
                    this->drawFrameBGShifted(window, window->charsBack);
                } else {
                    this->drawFrameBG(window, window->charsBack);
                }
                break;
            }
            case 1: {
                this->drawFrameTex(window, window->charsBack);
                break;
            }
        }
    }
}

// MATCHED 100%
void clMesWinEff::close(s16 idx) {
    this->windows[idx].effCloseReq = 1;
}

// MATCHED 100%
void clMesWinEff::open(MesWinParams * params, s16 idx) {
    this->createEff(params, idx);
}

