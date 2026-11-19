// objdiff-target: build/delinks/_dsd_gap@ov000_2.o

#include "ov000/clMesWinEff.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"

// --- Extern Data Variables --- //

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

    s16 r4 = this->create(params, idx);
    this->startOpen(&this->windows[r4]);
    return r4;
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
            for (s32 r4 = window->params.effectFast; r4 >= 0; r4--) {
                this->subStep = r4;

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
            } else if (window->text.field04 != 0 && window->paused == 0 && this->checkClose(window) == FALSE) {
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
            for (s32 r4 = window->params.effectFast; r4 >= 0; r4--) {
                this->subStep = r4;

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
void clMesWinEff::startNone(MesWinSlot * param1) {
    param1->effStep = stepNone;

    if (param1->params.mode == 1) {
        u32 size = param1->charSize;
        if (param1->charsBack < param1->chars) {
            cpu_copy32_rev(param1->charsBack, param1->chars, size);
        } else {
            cpu_copy32_fwd(param1->charsBack, param1->chars, size);
        }

        if (this->replaced == 0) {
            this->windows[(s16)param1->slot].dirty = 1;
            this->charsDirty = 1;
        }

        this->replaced = 0;
    }

    param1->effErase = 0;
    if (param1->effState == 2) {
        param1->effErase = 1;
    }
}

// MATCHED 100%
BOOL stepNone(clMesWinEff * param1, MesWinSlot * param2) {
    if (param2->effErase == 0) {
        param1->windows[(s16)param2->slot].dirty = 1;
        param1->charsDirty = 1;

        if (param2->params.mode == 0 && param2->tail.active == 0) {
            param1->addTail(&param1->windows[param2->slot]);

            u32 size = param2->charSize;
            if (param2->charsBack < param2->chars) {
                cpu_copy32_rev(param2->charsBack, param2->chars, size);
            } else {
                cpu_copy32_fwd(param2->charsBack, param2->chars, size);
            }

            param1->windows[(s16)param2->slot].dirty = 1;
            param1->charsDirty = 1;
        }

        return TRUE;
    }

    cpu_fill32(param2->chars, 0, param2->charSize);
    return TRUE;
}

// MATCHED 100%
void clMesWinEff::startFade(MesWinSlot * param1, const u16 * param2) {
    MesEffWork * r12 = &this->effWork[(u16)param1->slot * 12];
    r12->frame = 0;
    r12->timer = 1;

    param1->effStep = stepFade;
    param1->effData = param2;

    param1->effErase = 0;
    if (param1->effState == 2) {
        param1->effErase = 1;
    }
}

// MATCHED 100%
BOOL stepFade(clMesWinEff * param1, MesWinSlot * param2) {
    const u32 * r2;
    u32 * r3;
    u32 * r12;
    BOOL r14;
    s32 r4;

    MesEffWork * r5 = &param1->effWork[(u16)param2->slot * 12];
    r3 = param2->charsBack;
    r12 = param2->chars;
    r2 = &effMasks[((u16*)param2->effData)[r5->frame]];

    r14 = FALSE;

    if (--r5->timer & 0xff) {
        return r14;
    }

    r5->timer = 3;

    if (r5->frame+1 == 7) {
        r14 = TRUE;
    }

    if (param1->subStep == 0 || r14) {
        for (r4 = (param2->charSize / 32) - 1; r4 >= 0; r4--) {
            r12[0] = r3[0] & r2[0];
            r12[1] = r3[1] & r2[1];
            r12[2] = r3[2] & r2[2];
            r12[3] = r3[3] & r2[3];
            r12[4] = r3[4] & r2[4];
            r12[5] = r3[5] & r2[5];
            r12[6] = r3[6] & r2[6];
            r12[7] = r3[7] & r2[7];

            r3 += 8;
            r12 += 8;
        }

        param1->windows[(s16)param2->slot].dirty = 1;
        param1->charsDirty = 1;
    }

    r5->frame++;

    return r14;
}

// MATCHED 100%
void clMesWinEff::startDiag(MesWinSlot * param1, const MesEffSeq * param2, u32 param3) {
    MesEffWork * r4 = &this->effWork[(u16)param1->slot * 12];
    cpu_fill32(r4, 0, 12 * sizeof(MesEffWork));

    param1->effErase = 0;
    if (param1->effState == 2) {
        param1->effErase = 1;
    }

    r4->active = 1;
    r4->frame = 0;
    r4->timer = 1;

    param1->effStep = stepDiag;
    param1->effData = param2;
    param1->effLen = param3;
}


static inline MesEffWork * findFreeWork(MesEffWork * r3) {
    for (s32 i = 11; i >= 0; i--, r3++) {
        if (!r3->active) {
            return r3;
        }
    }
    return NULL;
}

// MATCHED 100%
BOOL stepDiag(clMesWinEff * param1, MesWinSlot * param2) {
    BOOL local04 = FALSE;
    MesEffSeq * r6 = (MesEffSeq *)param2->effData;
    MesEffWork * local00 = &param1->effWork[(u16)param2->slot * 12];
    BOOL r2 = FALSE;
    MesEffWork * r7 = local00;

    for (s32 r11 = 11; r11 >= 0; r11--, r7++) {
        if (r7->active) {
            if (r7->skip) {
                r7->skip = 0;
            } else if ((--r7->timer & 0xff) == 0) {
                r7->timer = 1;

                if (r7->frame+1 == param2->effLen) {
                    r7->active = 0;
                }

                if (param1->subStep == 0 || !r7->active) {
                    u16 r4 = r6[r7->frame].maskOfs;
                    s8 r8 = r7->x;
                    s8 r10 = r7->y;
                    const u32 * r9 = &effMasks[r4];

                    while (r8 >= 0 && r10 <= param2->rows) {
                        s32 r5 = (r10 * (param2->cols + 1) + r8) * 8;
                        u32 * r3 = &param2->charsBack[r5];
                        u32 * r12 = &param2->chars[r5];

                        r8--;
                        r10++;

                        r12[0] = r3[0] & r9[0];
                        r12[1] = r3[1] & r9[1];
                        r12[2] = r3[2] & r9[2];
                        r12[3] = r3[3] & r9[3];
                        r12[4] = r3[4] & r9[4];
                        r12[5] = r3[5] & r9[5];
                        r12[6] = r3[6] & r9[6];
                        r12[7] = r3[7] & r9[7];
                    }

                    r2 = TRUE;
                }

                r7->frame++;

                if (!r7->active) {
                    if (r7->last) {
                        local04 = TRUE;
                        break;
                    }
                } else if (r6[r7->frame].spawn && r7->last == 0) {
                    MesEffWork * r3 = findFreeWork(local00);
                    r3->active = 1;

                    if (r7->x == param2->cols) {
                        r3->x = r7->x;

                        if (r7->y == param2->rows) {
                            r3->last = 1;
                        }

                        r3->y = r7->y + 1;
                    } else {
                        r3->x = r7->x + 1;
                        r3->y = r7->y;
                    }

                    r3->timer = 2;
                    r3->frame = 0;

                    if (r3 > r7) {
                        r3->skip = 1;
                    }
                }
            }
        }
    }

    if (r2) {
        param1->windows[(s16)param2->slot].dirty = 1;
        param1->charsDirty = 1;
    }

    return local04;
}

// MATCHED 100%
void clMesWinEff::startDiagPair(MesWinSlot * param1, MesEffSeqPair * param2, u32 param3) {
    s16 r0;
    s16 r1;
    s16 r2;
    s16 r3;

    MesEffWork * r4 = &this->effWork[(u16)param1->slot * 12];
    cpu_fill32(r4, 0, 12 * sizeof(MesEffWork));

    param1->effErase = 0;
    if (param1->effState == 2) {
        param1->effErase = 1;
    }

    r4->active = 1;
    r4->frame = 0;
    r4->timer = 1;
    r4->field00_2 = 0;

    r2 = r4->x;
    r3 = r4->y;

    MesEffWork * r4_next = r4+1;

    r4_next->active = 1;
    r4_next->x = param1->cols;
    r4_next->y = param1->rows;
    r4_next->frame = 0;
    r4_next->timer = 1;
    r4_next->field00_2 = 1;

    r0 = r4_next->x;
    r1 = r4_next->y;

    param1->effStep = stepDiagPair;
    param1->effData = param2;
    param1->effMet = 0;
    param1->effLen = param3;

    s16 r12;
    s16 r7;
    do {
        if (r2 == param1->cols) {
            r3++;
        } else {
            r2++;
        }

        r12 = r2;
        r7 = r3;

        while (r12 > 0 && r7 < param1->rows) {
            r12--;
            r7++;
        }

        if (r0 == 0) {
            r1--;
        } else {
            r0--;
        }
    } while (r12 <= r0 && r1 >= r7);

    param1->effMeetCol = r2;
    param1->effMeetRow = r3;
}

// MATCHED 100%
BOOL stepDiagPair(clMesWinEff * param1, MesWinSlot * param2) {
    const MesEffSeq * local0c;
    BOOL local08 = FALSE;
    s32 local04;
    MesEffWork * local00 = &param1->effWork[(u16)param2->slot * 12];
    BOOL r3 = local08;
    MesEffWork * r14 = local00;

    for (local04 = 11; local04 >= 0; local04--, r14++) {
        if (r14->active) {
            if (r14->skip) {
                r14->skip = 0;
            } else if ((--r14->timer & 0xff) == 0) {
                r14->timer = 1;

                if (r14->frame + 1 == param2->effLen) {
                    r14->active = 0;
                }

                s8 r9 = r14->x;
                s8 r8 = r14->y;

                if (!r14->field00_2) {
                    local0c = ((MesEffSeqPair *)param2->effData)->fwd;
                    const u32 * r4 = &effMasks[local0c[r14->frame].maskOfs];

                    if (param1->subStep == 0 || !r14->active) {
                        while (r9 >= 0 && r8 <= param2->rows) {
                            s32 r2 = (r8 * (param2->cols + 1) + r9) * 8;
                            u32 * r5 = &param2->charsBack[r2];
                            u32 * r6 = &param2->chars[r2];

                            if (param2->effErase) {
                                r6[0] = r6[0] & r4[0];
                                r6[1] = r6[1] & r4[1];
                                r6[2] = r6[2] & r4[2];
                                r6[3] = r6[3] & r4[3];
                                r6[4] = r6[4] & r4[4];
                                r6[5] = r6[5] & r4[5];
                                r6[6] = r6[6] & r4[6];
                                r6[7] = r6[7] & r4[7];
                            } else {
                                for (s32 r7 = 7; r7 >= 0; r7--) {
                                    r6[r7] = (r5[r7] & r4[r7]) | (r6[r7] & (r4[r7] ^ 0xffffffff));
                                }
                            }

                            r9--;
                            r8++;
                        }

                        r3 = TRUE;
                    }
                } else {
                    local0c = ((MesEffSeqPair *)param2->effData)->back;
                    const u32 * r5 = &effMasks[local0c[param2->effLen - 1 - r14->frame].maskOfs];

                    if (param1->subStep == 0 || !r14->active) {
                        while (r9 <= param2->cols && r8 >= 0) {
                            s32 r2 = (r8 * (param2->cols + 1) + r9) * 8;
                            u32 * r6 = &param2->charsBack[r2];
                            u32 * r7 = &param2->chars[r2];

                            if (param2->effErase) {
                                r7[0] = r7[0] & r5[0];
                                r7[1] = r7[1] & r5[1];
                                r7[2] = r7[2] & r5[2];
                                r7[3] = r7[3] & r5[3];
                                r7[4] = r7[4] & r5[4];
                                r7[5] = r7[5] & r5[5];
                                r7[6] = r7[6] & r5[6];
                                r7[7] = r7[7] & r5[7];
                            } else {
                                for (s32 r10 = 7; r10 >= 0; r10--) {
                                    r7[r10] = (r6[r10] & r5[r10]) | (r7[r10] & (r5[r10] ^ 0xffffffff));
                                }
                            }

                            r9++;
                            r8--;
                        }

                        r3 = TRUE;
                    }
                }

                r14->frame++;

                if (!r14->active) {
                    if (r14->last) {
                        local08 = TRUE;
                        break;
                    }
                } else if (local0c[r14->frame].spawn && r14->last == 0) {
                    MesEffWork * r2 = findFreeWork(local00);
                    r2->active = 1;

                    if (!r14->field00_2) {
                        if (r14->x == param2->cols) {
                            r2->x = r14->x;
                            r2->y = r14->y + 1;
                        } else {
                            r2->x = r14->x + 1;
                            r2->y = r14->y;
                        }

                        if (r2->x == param2->effMeetCol && r2->y == param2->effMeetRow) {
                            r2->last = 1;
                            param2->effMet = 1;
                        }
                    } else {
                        if (r14->x == 0) {
                            r2->x = r14->x;
                            r2->y = r14->y - 1;
                        } else {
                            r2->x = r14->x - 1;
                            r2->y = r14->y;
                        }

                        if (param2->effMet) {
                            r2->last = 1;
                        }
                    }

                    r2->timer = 2;
                    r2->frame = 0;
                    r2->field00_2 = r14->field00_2;

                    if (r2 > r14) {
                        r2->skip = 1;
                    }
                }
            }
        }
    }

    if (r3) {
        param1->windows[(s16)param2->slot].dirty = 1;
        param1->charsDirty = 1;
    }

    return local08;
}

// MATCHED 100%
void clMesWinEff::startWipeH(MesWinSlot * param1, const u16 * param2) {
    MesEffWork * r4 = &this->effWork[(u16)param1->slot * 12];
    cpu_fill32(r4, 0, 12 * sizeof(MesEffWork));

    param1->effErase = 0;
    if (param1->effState == 2) {
        param1->effErase = 1;
    }

    r4->frame = 0;
    r4->timer = 1;

    MesEffWork * r4_next = r4+1;
    r4_next->x = param1->cols;
    r4_next->frame = 2;
    r4_next->timer = 1;

    param1->effStep = stepWipeH;
    param1->effData = param2;
}

// MATCHED 100%
BOOL stepWipeH(clMesWinEff * param1, MesWinSlot * param2) {
    BOOL r4 = FALSE;
    MesEffWork * r5 = &param1->effWork[(u16)param2->slot * 12];

    if (r5->field00_2 == 0 && (--r5->timer & 0xff) == 0) {
        r5->timer = 1;

        if (r5->frame+1 == 2) {
            r4 = TRUE;
        }

        if (drawWipeColumn(param1, param2, r5, r4)) {
            param1->windows[(s16)param2->slot].dirty = 1;
            param1->charsDirty = 1;
        }

        r5->frame++;

        if (r4) {
            r5->frame = 0;

            if (++r5->x > param2->cols) {
                r5->field00_2 = 1;
            }
        }
    }

    MesEffWork * r5_next = r5+1;

    if (r5_next->field00_2 == 0 && (--r5_next->timer & 0xff) == 0) {
        r5_next->timer = 1;

        r4 = FALSE;
        if (r5_next->frame+1 == 4) {
            r4 = TRUE;
        }

        if (drawWipeColumn(param1, param2, r5_next, r4)) {
            param1->windows[(s16)param2->slot].dirty = 1;
            param1->charsDirty = 1;
        }

        r5_next->frame++;

        if (r4) {
            r5_next->frame = 2;

            if (--r5_next->x < 0) {
                r5_next->field00_2 = 1;
            }
        }
    }

    if (r5->field00_2 && r5_next->field00_2) {
        return TRUE;
    }
    return FALSE;
}

// MATCHED 100%
BOOL drawWipeColumn(clMesWinEff * param1, MesWinSlot * param2, MesEffWork * param3, BOOL param4) {
    if (param1->subStep != 0 && param4 == FALSE) {
        return FALSE;
    }

    const u32 * r2 = &effMasks[((u16 *)param2->effData)[param3->frame]];
    u32 * r3 = &param2->charsBack[param3->x * 8];
    u32 * r12 = &param2->chars[param3->x * 8];

    if (param2->effErase == 0) {
        for (s32 r14 = param2->rows; r14 >= 0; r14--) {
            for (s32 r4 = 7; r4 >= 0; r4--) {
                r12[r4] = (r3[r4] & r2[r4]) | (r12[r4] & (r2[r4] ^ 0xffffffff));
            }

            r3 += (param2->cols+1) * 8;
            r12 += (param2->cols+1) * 8;
        }
    } else {
        for (s32 r0 = param2->rows; r0 >= 0; r0--) {
            r12[0] &= r2[0];
            r12[1] &= r2[1];
            r12[2] &= r2[2];
            r12[3] &= r2[3];
            r12[4] &= r2[4];
            r12[5] &= r2[5];
            r12[6] &= r2[6];
            r12[7] &= r2[7];

            r12 += (param2->cols+1) * 8;
        }
    }

    return TRUE;
}

// MATCHED 100%
void clMesWinEff::startWipeV(MesWinSlot * param1, const u16 * param2) {
    MesEffWork * r4 = &this->effWork[(u16)param1->slot * 12];
    cpu_fill32(r4, 0, 12 * sizeof(MesEffWork));

    param1->effErase = 0;
    if (param1->effState == 2) {
        param1->effErase = 1;
    }

    r4->frame = 0;
    r4->timer = 1;

    MesEffWork * r4_next = r4+1;
    r4_next->y = param1->rows;
    r4_next->frame = 2;
    r4_next->timer = 1;

    param1->effStep = stepWipeV;
    param1->effData = param2;
}

// MATCHED 100%
BOOL stepWipeV(clMesWinEff * param1, MesWinSlot * param2) {
    BOOL r4 = FALSE;
    MesEffWork * r5 = &param1->effWork[(u16)param2->slot * 12];

    if (r5->field00_2 == 0 && (--r5->timer & 0xff) == 0) {
        r5->timer = 1;

        if (r5->frame+1 == 2) {
            r4 = TRUE;
        }

        if (drawWipeRow(param1, param2, r5, r4)) {
            param1->windows[(s16)param2->slot].dirty = 1;
            param1->charsDirty = 1;
        }

        r5->frame++;

        if (r4) {
            r5->frame = 0;

            if (++r5->y > param2->rows) {
                r5->field00_2 = 1;
            }
        }
    }

    MesEffWork * r5_next = r5+1;

    if (r5_next->field00_2 == 0 && (--r5_next->timer & 0xff) == 0) {
        r5_next->timer = 1;

        r4 = FALSE;

        if (r5_next->frame+1 == 4) {
            r4 = TRUE;
        }

        if (drawWipeRow(param1, param2, r5_next, r4)) {
            param1->windows[(s16)param2->slot].dirty = 1;
            param1->charsDirty = 1;
        }

        r5_next->frame++;

        if (r4) {
            r5_next->frame = 2;

            if (--r5_next->y < 0) {
                r5_next->field00_2 = 1;
            }
        }
    }

    if (r5->field00_2 && r5_next->field00_2) {
        return TRUE;
    }
    return FALSE;
}

// MATCHED 100%
BOOL drawWipeRow(clMesWinEff * param1, MesWinSlot * param2, MesEffWork * param3, BOOL param4) {
    if (param1->subStep != 0 && param4 == FALSE) {
        return FALSE;
    }

    const u32 * r0 = &effMasks[((u16 *)param2->effData)[param3->frame]];
    u32 * r1 = &param2->charsBack[param3->y * (param2->cols + 1) * 8];
    u32 * r2 = &param2->chars[param3->y * (param2->cols + 1) * 8];

    if (param2->effErase == 0) {
        for (s32 r3 = param2->cols; r3 >= 0; r3--) {
            for (s32 r8 = 7; r8 >= 0; r8--) {
                r2[r8] = (r1[r8] & r0[r8]) | (r2[r8] & (r0[r8] ^ 0xffffffff));
            }

            r1 += 8;
            r2 += 8;
        }
    } else {
        for (s32 r3 = param2->cols; r3 >= 0; r3--) {
            r2[0] &= r0[0];
            r2[1] &= r0[1];
            r2[2] &= r0[2];
            r2[3] &= r0[3];
            r2[4] &= r0[4];
            r2[5] &= r0[5];
            r2[6] &= r0[6];
            r2[7] &= r0[7];

            r2 += 8;
        }
    }

    return TRUE;
}

// MATCHED 100%
void clMesWinEff::startSplitV(MesWinSlot * param1) {
    MesEffWork * r4 = &this->effWork[(u16)param1->slot * 12];
    cpu_fill32(r4, 0, 12 * sizeof(MesEffWork));

    param1->effHalf = (param1->params.height + 1) * 8 / 2;
    param1->effErase = 0;

    if (param1->effState == 2) {
        param1->effErase = 1;

        r4->y = param1->effHalf;
    }

    param1->effStep = stepSplitV;
}

// MATCHED 100%
BOOL stepSplitV(clMesWinEff * param1, MesWinSlot * param2) {
    s16 r4;
    u32 r11;
    BOOL r5;
    u32 r6;
    u32 r7;
    MesEffWork * r8;
    u32 local00;

    r8 = &param1->effWork[(u16)param2->slot * 12];
    r6 = (param2->cols + 1) * 8;
    r5 = FALSE;
    local00 = (param2->params.height + 1) * 8 - 1;
    r7 = param2->params.y % 8;

    if (!param2->effErase) {
        if (r8->y + 2 > param2->effHalf) {
            r5 = TRUE;
        }
    } else {
        if (r8->y - 2 < 0) {
            r5 = TRUE;
        }
    }

    if (param1->subStep == 0 || r5) {
        if (param2->effErase) {
            r4 = param2->effHalf - r8->y;
            s32 r0 = ((r7 + r4) & ~7) - 8;

            if (r0 < 0) {
                r0 += 8;
            }

            r0 *= param2->cols + 1;
            r11 = (param2->cols + 1) * 64;
            cpu_fill32(&param2->chars[r0], 0, r11);

            u32 r0b;

            if (r7 != 0) {
                r0b = (r7 + param2->rows * 8 - r4) & ~7;

                if (r0b >= param2->rows * 8) {
                    r0b -= 8;
                }
            } else {
                r0b = (param2->rows * 8 + 8 - r4) & ~7;

                if (r0b >= param2->rows * 8) {
                    r11 /= 2;
                }
            }

            r0b *= param2->cols + 1;
            cpu_fill32(&param2->chars[r0b], 0, r11);
        }

        for (s32 r2 = r7; r2 < r8->y + r7; r2++) {
            s32 r1i = r2 & 7;
            r1i += (r2 >> 3) * r6;

            u32 * r1 = &param2->charsBack[r1i];
            u32 r0 = r2 + (param2->effHalf - r8->y);
            u32 * r11 = &param2->chars[(r0 / 8) * r6 + (r0 % 8)];

            for (s32 r0 = 0; r0 <= param2->cols * 8; r0 += 8) {
                r11[r0] = r1[r0];
            }
        }

        for (s32 r0 = 0; r0 <= r8->y; r0++) {
            u32 r3 = local00 + r7 - r0;
            u32 * r3p = &param2->charsBack[(r3 / 8) * r6 + (r3 % 8)];
            u32 r14 = r7 + (param2->effHalf - 1 + r8->y) - r0;
            u32 * r4 = &param2->chars[(r14 / 8) * r6 + (r14 % 8)];

            for (s32 r11 = 0; r11 <= param2->cols * 8; r11 += 8) {
                r4[r11] = r3p[r11];
            }
        }

        param1->windows[(s16)param2->slot].dirty = 1;
        param1->charsDirty = 1;
    }

    if (!param2->effErase) {
        r8->y += 2;
    } else {
        r8->y -= 2;
    }

    return r5;
}

// MATCHED 100%
void clMesWinEff::startSplitH(MesWinSlot * param1) {
    MesEffWork * r4 = &this->effWork[(u16)param1->slot * 12];
    cpu_fill32(r4, 0, 12 * sizeof(MesEffWork));

    param1->effHalf = (param1->params.width + 1) * 8 / 2;
    param1->effErase = 0;

    if (param1->effState == 2) {
        param1->effErase = 1;

        r4->x = param1->effHalf;
    }

    param1->effStep = stepSplitH;
}

// MATCHED 100%
BOOL stepSplitH(clMesWinEff * param1, MesWinSlot * param2) {
    BOOL local24;
    BOOL local20;
    s32 local1c;
    s32 local18;
    u32 * r10;
    u32 * r9;
    u32 r0;
    u32 r6;
    s32 r7;
    u32 r1m;

    MesEffWork * local14 =&param1->effWork[(u16)param2->slot * 12];
    local24 = FALSE;

    if (!param2->effErase) {
        if (local14->x + 2 > param2->effHalf) {
            local24 = TRUE;
        }
    } else {
        if (local14->x - 2 < 0) {
            local24 = TRUE;
        }
    }

    if (param1->subStep == 0 || local24) {
        cpu_fill32(param2->chars, 0, param2->charSize);

        r0 = (param2->cols + 1) * 8;
    
        u8 local10 = param2->effHalf & 7;
        u8 local0c = param2->params.x & 7;

        r6 = (local10 + (param2->effHalf - local14->x)) & ~7;
        if ((local14->x & 7) == 0) {
            r6 -= 8;
        }

        s8 r2 = (7 - (local14->x & 7) - (local10 - 1)) * 4;
        if (r2 >= 0) {
            local20 = FALSE;
        } else {
            r2 *= -1;
            local20 = TRUE;
        }

        s8 r1 = 32 - r2;
        s32 r14 = (local14->x & ~7) + local0c - local10;

        if (local20) {
            for (r7 = 0; r7 < r14; r7 += 8) {
                r9 = &param2->chars[r6];
                r10 = &param2->charsBack[r7];

                for (s32 r8 = param2->rows; r8 >= 0; r8--) {
                    r9[0] += (r10[0] >> r2);
                    r9[1] += (r10[1] >> r2);
                    r9[2] += (r10[2] >> r2);
                    r9[3] += (r10[3] >> r2);
                    r9[4] += (r10[4] >> r2);
                    r9[5] += (r10[5] >> r2);
                    r9[6] += (r10[6] >> r2);
                    r9[7] += (r10[7] >> r2);

                    r9[0] += (r10[8] << r1);
                    r9[1] += (r10[9] << r1);
                    r9[2] += (r10[10] << r1);
                    r9[3] += (r10[11] << r1);
                    r9[4] += (r10[12] << r1);
                    r9[5] += (r10[13] << r1);
                    r9[6] += (r10[14] << r1);
                    r9[7] += (r10[15] << r1);

                    r10 += r0;
                    r9 += r0;
                }

                r6 += 8;
            }
        } else {
            for (r7 = 0; r7 < r14; r7 += 8) {
                r9 = &param2->chars[r6];
                r10 = &param2->charsBack[r7];

                for (s32 r3 = param2->rows; r3 >= 0; r3--) {
                    r9[0] += (r10[0] << r2);
                    r9[1] += (r10[1] << r2);
                    r9[2] += (r10[2] << r2);
                    r9[3] += (r10[3] << r2);
                    r9[4] += (r10[4] << r2);
                    r9[5] += (r10[5] << r2);
                    r9[6] += (r10[6] << r2);
                    r9[7] += (r10[7] << r2);
                    r9[8] += (r10[0] >> r1);
                    r9[9] += (r10[1] >> r1);
                    r9[10] += (r10[2] >> r1);
                    r9[11] += (r10[3] >> r1);
                    r9[12] += (r10[4] >> r1);
                    r9[13] += (r10[5] >> r1);
                    r9[14] += (r10[6] >> r1);
                    r9[15] += (r10[7] >> r1);

                    r10 += r0;
                    r9 += r0;
                }

                r6 += 8;
            }
        }

        u32 r8;
        if (local10 != 0) {
            r8 = splitMaskLow[(local10 + param2->params.x - 1) & 7];
        } else {
            r8 = splitMaskLow[local0c];
            if (r8 == 0) {
                r8 = 0xffffffff;
            }
        }

        r9 = &param2->chars[r6];
        r10 = &param2->charsBack[r7];

        if (local20) {
            for (s32 r6 = param2->rows; r6 >= 0; r6--) {
                for (s32 r7 = 7; r7 >= 0; r7--) {
                    r9[r7] = r8 & (r9[r7] + (r10[r7] >> r2) + (r10[r7+8] << r1));
                }

                r10 += r0;
                r9 += r0;
            }
        } else {
            for (s32 r6 = param2->rows; r6 >= 0; r6--) {
                for (s32 r7 = 7; r7 >= 0; r7--) {
                    r9[r7] = r8 & (r9[r7] + (r10[r7] << r2));
                }

                r10 += r0;
                r9 += r0;
            }
        }

        u32 r2b = ((param2->effHalf + local14->x) - local10) & ~7;
        if (local0c != 0) {
            r2b += 8;
        }

        r2 = (8 - (local14->x & 7) - local10) * 4;
        if (r2 >= 0) {
            local20 = FALSE;
        } else {
            r2 *= -1;
            local20 = TRUE;
        }

        s32 r11 = 0;
        r1 = 32 - r2;

        r14 = (local14->x & ~7) + local0c;
        if (local10 != 0 || local0c != 0) {
            r14 -= 8;
        }

        if (local20) {
            while (r11 < r14) {
                r9 = &param2->chars[r2b];
                r10 = &param2->charsBack[param2->cols * 8] - r11;

                for (s32 r1i = param2->rows; r1i >= 0; r1i--) {
                    r9[0] += (r10[0] << r2);
                    r9[1] += (r10[1] << r2);
                    r9[2] += (r10[2] << r2);
                    r9[3] += (r10[3] << r2);
                    r9[4] += (r10[4] << r2);
                    r9[5] += (r10[5] << r2);
                    r9[6] += (r10[6] << r2);
                    r9[7] += (r10[7] << r2);
                    r9[8] += (r10[0] >> r1);
                    r9[9] += (r10[1] >> r1);
                    r9[10] += (r10[2] >> r1);
                    r9[11] += (r10[3] >> r1);
                    r9[12] += (r10[4] >> r1);
                    r9[13] += (r10[5] >> r1);
                    r9[14] += (r10[6] >> r1);
                    r9[15] += (r10[7] >> r1);

                    r10 += r0;
                    r9 += r0;
                }

                r2b -= 8;
                r11 += 8;
            }
        } else {
            while (r11 < r14) {
                r9 = &param2->chars[r2b];
                r10 = &param2->charsBack[param2->cols * 8] - r11;

                for (s32 r1i = param2->rows; r1i >= 0; r1i--) {
                    r9[0] = r9[0] + (r10[0] >> r2);
                    r9[1] = r9[1] + (r10[1] >> r2);
                    r9[2] = r9[2] + (r10[2] >> r2);
                    r9[3] = r9[3] + (r10[3] >> r2);
                    r9[4] = r9[4] + (r10[4] >> r2);
                    r9[5] = r9[5] + (r10[5] >> r2);
                    r9[6] = r9[6] + (r10[6] >> r2);
                    r9[7] = r9[7] + (r10[7] >> r2);
                    r9[-8] = r9[-8] + (r10[0] << r1);
                    r9[-7] = r9[-7] + (r10[1] << r1);
                    r9[-6] = r9[-6] + (r10[2] << r1);
                    r9[-5] = r9[-5] + (r10[3] << r1);
                    r9[-4] = r9[-4] + (r10[4] << r1);
                    r9[-3] = r9[-3] + (r10[5] << r1);
                    r9[-2] = r9[-2] + (r10[6] << r1);
                    r9[-1] = r9[-1] + (r10[7] << r1);
                    r10 += r0;
                    r9 += r0;
                }

                r2b -= 8;
                r11 += 8;
            }
        }


        if (local10 != 0) {
            u32 r7i = (param2->params.x & 0xF) + (local10 - 1);
            r8 = splitMaskHigh[r7i];
            r1m = splitMaskHighAlt[r7i];
        } else {
            r8 = splitMaskHigh[local0c];
            r1m = 0xFFFFFFFF;
        }

        if (r14 + 8 > 0 && (local10 != 0 || local0c != 0)) {
            r10 = &param2->charsBack[param2->cols * 8] - r11;
            r9 = &param2->chars[r2b];

            if (local20) {
                for (local1c = param2->rows; local1c >= 0; local1c--) {
                    for (s32 r9i = 7; r9i >= 0; r9i--) {
                        r9[r9i] = r9[r9i] + (r1m & (r10[r9i] << r2));
                        r9[r9i + 8] = r9[r9i + 8] + (r10[r9i] >> r1);
                    }

                    r10 += r0;
                    r9 += r0;
                }
            } else {
                for (local18 = param2->rows; local18 >= 0; local18--) {
                    for (s32 r9i = 7; r9i >= 0; r9i--) {
                        r9[r9i] = r9[r9i] + (r1m & (r10[r9i] >> r2));
                        r9[r9i - 8] = r9[r9i - 8] + (r8 & (r10[r9i] << r1));
                    }

                    r10 += r0;
                    r9 += r0;
                }
            }

            r2b -= 8;
            r11 += 8;
        }

        r9 = &param2->chars[r2b];
        r10 = &param2->charsBack[param2->cols * 8] - r11;
        if (local20) {
            for (s32 r7 = param2->rows; r7 >= 0; r7--) {
                for (s32 r8i = 7; r8i >= 0; r8i--) {
                    r9[r8i] = r9[r8i] + (r8 & (r10[r8i] << r2));
                    r9[r8i + 8] = r9[r8i + 8] + (r1m & (r10[r8i] >> r1));
                }

                r10 += r0;
                r9 += r0;
            }
        } else {
            for (s32 r7 = param2->rows; r7 >= 0; r7--) {
                for (s32 r8i = 7; r8i >= 0; r8i--) {
                    r9[r8i] = r9[r8i] + (r8 & (r10[r8i] >> r2));
                }

                r10 += r0;
                r9 += r0;
            }
        }

        param1->windows[(s16)param2->slot].dirty = 1;
        param1->charsDirty = 1;
    }

    if (!param2->effErase) {
        local14->x += 2;
    } else {
        local14->x -= 2;
    }

    return local24;
}

// MATCHED 100%
void clMesWinEff::drawFrame(MesWinSlot * param1, BOOL param2) {
    if (param1->params.style != 0) {
        switch (param1->params.mode) {
            case 0: {
                if ((param1->params.x & 7) || (param1->params.y & 7)) {
                    this->drawFrameBGShifted(param1, param1->charsBack);
                } else {
                    this->drawFrameBG(param1, param1->charsBack);
                }
                break;
            }
            case 1: {
                this->drawFrameTex(param1, param1->charsBack);
                break;
            }
        }
    }
}

// MATCHED 100%
void clMesWinEff::close(s16 param1) {
    this->windows[param1].effCloseReq = 1;
}

// MATCHED 100%
void clMesWinEff::open(MesWinParams * param1, s16 param2) {
    this->createEff(param1, param2);
}

