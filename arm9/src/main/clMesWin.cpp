// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clMesWin.h"

#include "main/clGameMain.h"

#include "main/heap.h"
#include "main/sound.h"

extern "C" {

#include <nitro.h>

#include "main/cpu_mem32.h"
#include "main/system.h"

#include "data/clMesWin.h"

// --- Struct Definitions --- //

#define MES_SCREEN_COLS 32
#define MES_SCREEN_ROWS 30
#define MES_SCREEN_SIZE (MES_SCREEN_COLS * MES_SCREEN_ROWS * sizeof(u16))

typedef struct {
    u8 pad_00[0x8];
    s16 x;  // +0x8
    s16 y;  // +0xa
} IconOffset;

typedef struct {
    struct {
        u32 cell : 24;  // +0x0
        u32 numObj : 8;
    };
    u16 time;  // +0x4
    u16 offsetIdx;  // +0x6
} IconFrame;

typedef struct {
    struct {
        u32 : 2;  // +0x0
        u32 mode : 2;
        u32 mosaic : 1;
        u32 color : 1;
        u32 shape : 2;
        u32 flip : 2;
        u32 size : 2;
        u32 : 2;
        u32 pltt : 4;
        u32 : 14;
    };
    s16 x;  // +0x4
    s16 y;  // +0x6
} IconObj;

typedef struct {
    u32 frames;  // +0x0
    u16 count;  // +0x4
} IconAnim;

#define MES_ICON_RES ((YanaRes *)mesIconRes)

// --- Extern Data Variables --- //

extern u8 data_0205c25c[];

extern u32 texBankAddrs[];

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

extern void func_02028950(MesFrameEntry *, u32, u32, u32);

extern u32 func_0202af48(u32, u32, u32, YanaRes *);

extern u32 func_0202afbc(u32, u32, u32, void *);

extern u32 func_0202b170(YanaRes *, void *);

extern void func_0202d10c(u32, u32, void *, void *, void *, u32, u32, u32, u8);

// --- Variable Definitions --- //

extern u32 data_0204ac60[2];
extern vu32 * data_0204ac68[2];

// --- Function Declarations --- //

void func_0200e238(MesFrameEntry *, u32, u32, u32);  // 0x0200e238
void func_0200e35c(MesFrameEntry *, u32, u32, u32);  // 0x0200e35c

void putTileColumn(void *, void *, s32);  // 0x02011320

void func_02014070(MesFrameEntry *, u32, u32, u32);  // 0x02014070

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clMesWin::update() {
    if (this->updated) {
        return;
    }

    if (this->screenMask & 4) {
        MesWinSlot * window = this->listMain.next;
        u32 depth = this->baseDepth + this->winCountMain * 2 - 1;

        while (window->link.next != (MesWinSlot *)-1) {
            window->depth = depth;

            if (window->cursor.active) {
                window->cursor.depth = depth-1;
            }

            if (window->tail.active) {
                window->tail.depth = depth-1;
            }

            window = window->link.next;
            depth -= 2;
        }
    }

    this->updateList(this->listMain.next);
    this->updateList(this->listSub.next);

    this->icons->update();

    this->scrollMainCur.x = this->scrollMain.x;
    this->scrollMainCur.y = this->scrollMain.y;
    this->scrollSubCur.x = this->scrollSub.x;
    this->scrollSubCur.y = this->scrollSub.y;

    if (this->screenDirtyMain) {
        DC_FlushRange(this->screenMain, 0x780);
    }

    if (this->screenDirtySub) {
        DC_FlushRange(this->screenSub, 0x780);
    }

    this->updated = 1;
}

// MATCHED 100%
void clMesWin::flush() {
    this->updated = 0;

    if (this->active) {
        this->applyScroll();

        if (this->screenDirtyMain) {
            this->screenDirtyMain = 0;

            if (this->screenMain != NULL) {
                this->loadScreen();
            }
        }

        if (this->screenDirtySub) {
            this->screenDirtySub = 0;

            if (this->screenSub != NULL) {
                this->loadScreenSub();
            }
        }

        if (this->charsDirty) {
            this->loadChars();
        }
    }
}

// MATCHED 100%
void clMesWin::initParams(MesWinParams * params) {
    params->screen = 0;
    params->style = 1;
    params->width = 0;
    params->height = 0;
    params->mode = 2;

    params->voice = 0;

    params->tailKind = 0;
    params->tailAlt = 0;
    params->tailUp = 0;
    params->tailFlipH = 0;
    params->tailX = 0;
    params->textId = 0;
    params->field04_28 = 0;

    params->x = 0;
    params->y = 0;
    params->indent = 0;
    params->persistent = 0;

    params->numbers = NULL;
    params->font = NULL;
    params->text = NULL;
    params->result = NULL;

    params->field24 = 0;

    params->fixedSize = 0;

    params->voiceX = 0x7fff;
}

// MATCHED 100%
void clMesWin::open(MesWinParams * params, s16 idx) {
    this->update();

    if (idx != -1 && this->windows[idx].chars != NULL) {
        this->destroy(idx);
        this->rebuildScreen(this->windows[idx].params.screen);

        if (this->windows[idx].params.screen == 0) {
            this->screenDirtyMain = 0;
        } else {
            this->screenDirtySub = 0;
        }
    }

    this->create(params, idx);
}

// MATCHED 100%
void clMesWin::close(s16 idx) {
    if (this->destroy(idx)) {
        this->rebuildScreen(this->windows[idx].params.screen);
    }
}

// MATCHED 100%
void clMesWin::allocBuffers(u32 heap, u16 count, u32 charSizeMain, u32 charSizeSub, u8 singleMain, u8 singleSub) {
    u32 nBytes = this->calcBufferSize(count, charSizeMain, charSizeSub, singleMain, singleSub);

    MesWinSlot * arr = (MesWinSlot *)::operator new[](nBytes, heap, NULL, FALSE);

    this->assignBuffers(arr);

    this->ownsBuffers = 1;
}

// MATCHED 100%
u32 clMesWin::calcBufferSize(u16 count, u32 charSizeMain, u32 charSizeSub, u8 singleMain, u8 singleSub) {
    this->charPoolSizeMain = 0;
    this->charPoolSizeSub = 0;

    u32 size = count * sizeof(MesWinSlot);

    if (this->screenMask & 5) {
        size += charSizeMain;
        this->charPoolSizeMain = charSizeMain;
        
        if (singleMain == 0) {
            size += charSizeMain;
        }
    }

    if (this->screenMask & 2) {
        size += charSizeSub;
        this->charPoolSizeSub = charSizeSub;

        if (singleSub == 0) {
            size += charSizeSub;
        }
    }

    this->screenSizeMain = 0;
    this->screenSizeSub = 0;

    if (this->screenMask & 1) {
        this->screenSizeMain = MES_SCREEN_SIZE;
        size += MES_SCREEN_SIZE;
    }

    if (this->screenMask & 2) {
        this->screenSizeSub = MES_SCREEN_SIZE;
        size += MES_SCREEN_SIZE;
    }

    this->singleBuf[0] = singleMain;
    this->singleBuf[1] = singleSub;

    this->windowCount = count;
    this->bufSize = size;

    return size;
}

// MATCHED 100%
void clMesWin::assignBuffers(MesWinSlot * buf) {
    func_01ff8228(buf, 0, this->bufSize);

    u8 * ptr = (u8 *)&buf[this->windowCount];

    this->windows = buf;

    if (this->screenMask == 3 || this->screenMask == 6) {
        this->charPoolMain = ptr;
        this->charPoolSub = ptr + this->charPoolSizeMain;
    } else if (this->screenMask & 5) {
        this->charPoolMain = ptr;
        this->charPoolSub = NULL;
    } else {
        this->charPoolSub = ptr;
        this->charPoolMain = NULL;
    }

    ptr += this->charPoolSizeMain + this->charPoolSizeSub;

    this->screenMain = NULL;
    this->screenSub = NULL;

    if (this->screenMask == 3) {
        this->screenMain = ptr;
        this->screenSub = ptr + this->screenSizeMain;
    } else {
        if (this->screenMask & 1) {
            this->screenMain = ptr;
        }

         if (this->screenMask & 2) {
            this->screenSub = ptr;
         }
    }

    ptr += this->screenSizeMain + this->screenSizeSub;

    this->backBuf[0] = NULL;
    this->backBuf[1] = NULL;

    if (this->singleBuf[0] == 0) {
        this->backBuf[0] = ptr;
        ptr += this->charPoolSizeMain;
    }

    if (this->singleBuf[1] == 0) {
        this->backBuf[1] = ptr;
    }
}

// MATCHED 100%
void clMesWin::release() {
    if (this->windows != NULL) {
        s16 i = this->windowCount-1;
        while (i >= 0) {
            this->destroy(i);
            i--;
        }

        if (this->ownsBuffers) {
            delete[] this->windows;
            this->windows = NULL;
        }
    }
    this->windowCount = 0;
}

// MATCHED 100%
u32 clMesWin::measureText(const char * text, UnkStruct_0200b8bc_10 ** font) {
    UnkStruct_0200b8bc_24 layout;

    this->layoutText(text, font, &layout, 1);

    u16 w = layout.field28_0 + 24;
    if (w & 1) {
        w += 8;
    }
    w = (w >> 3) - 1;

    u16 h = layout.field28_16 + 16;
    if (h & 1) {
        h += 8;
    }
    h = (h >> 3) - 1;

    return (h << 16) + w;
}

// MATCHED 100%
u16 clMesWin::getBorderWidth(u8 style, u8 height) {
    const MesFrameEntry * entry = this->findFrame(style, height);

    return entry->frame->cornerW;
}

// MATCHED 100%
void clMesWin::attachTexMan(nsObjSys::clObjPltVramMan * manager) {
    if (this->screenMask & 4 && this->texAlloc == NULL) {
        this->texMan = manager;

        this->icons->setTexMan(manager, NULL);

        this->texAlloc = this->texMan->func_02014380(this->charPoolSizeMain, 0x19720329 | 0x80000000, 4);
        this->texAddr = this->texAlloc->field08;
    }
}

// MATCHED 100%
void func_0200e238(MesFrameEntry * entry, u32 size, u32 param3, u32 param4) {
    func_02028950(entry, MATH_ROUNDUP(size, 8), param3, param4);
}

// MATCHED 100%
void clMesWin::loadPalette(BOOL sub, u32 slot, void * src) {
    if (!sub) {
        this->plttMain = slot;

        if (src == NULL) {
            GX_LoadBGPltt(this->palette, slot * MES_SCREEN_COLS, MES_SCREEN_COLS);
        } else {
            GX_LoadBGPltt(src, slot * MES_SCREEN_COLS, MES_SCREEN_COLS);
        }
    } else {
        this->plttSub = slot;

        if (src == NULL) {
            GXS_LoadBGPltt(this->palette, slot * MES_SCREEN_COLS, MES_SCREEN_COLS);
        } else {
            GXS_LoadBGPltt(src, slot * MES_SCREEN_COLS, MES_SCREEN_COLS);
        }
    }
}

// MATCHED 100%
void clMesWin::attachTexPlttMan(nsObjSys::clObjPltVramMan * manager, u32 kind) {
    this->texPlttMan = manager;

    this->icons->setTexMan(NULL, manager);

    u32 size = mesTexPlttSize;

    this->texPlttAlloc = this->texPlttMan->func_02014380(size, 0x19720329, kind);
    this->texPlttAddr = this->texPlttAlloc->field08;

    GX_BeginLoadTexPltt();

    GX_LoadTexPltt(this->palette, this->texPlttAddr, size);
    GX_EndLoadTexPltt();

    this->icons->loadPltt(0, kind);
}

// MATCHED 100%
void func_0200e35c(MesFrameEntry * entry, u32 size, u32 param3, u32 param4) {
    func_02028950(entry, MATH_ROUNDUP(size, 16), param3, param4);
}

// MATCHED 100%
clMesWin::clMesWin(u32 screenMask, u32 bgMain, u32 bgSub, u8 priorityMain, u8 prioritySub, u32 initBG, clMesIcon * icons) {
    this->flags = 0;
    this->state = 0;

    this->scrollMain.word = 0;
    this->scrollSub.word = 0;

    this->scrollMainCur.word = 0;
    this->scrollSubCur.word = 0;

    this->windows = NULL;

    this->charPoolMain = NULL;
    this->charPoolSub = NULL;

    this->windowCount = 0;
    this->bufSize = 0;

    this->texAddr = 0;
    this->texPlttAddr = 0;
    this->baseDepth = 0;
    this->scale = 1;

    this->texMan = NULL;
    this->texPlttMan = NULL;
    this->texAlloc = NULL;
    this->texPlttAlloc = NULL;

    this->screenMask = screenMask;

    this->frameTiles = mesFrameTileData;
    this->palette = mesPltt;

    this->bgMain = bgMain;
    this->bgSub = bgSub;
    this->priorityMain = priorityMain;
    this->prioritySub = prioritySub;

    if (initBG == 1) {
        if (screenMask & 1) {
            this->setBGBase(GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x00000);
        }

        if (screenMask & 2) {
            this->setBGBaseSub(GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x00000);
        }
    }

    u32 heap = GET_HEADER(this)->heapId;
    this->icons = icons;

    if (icons == NULL) {
        this->icons = new(heap, NULL, FALSE) clMesIcon((screenMask & 4) != 0);
        this->ownsIcons = 1;
    }

    this->active = 1;

    this->scrollLockMain = 0;
    this->scrollLockSub = 0;

    this->decideKeys = 0x401;
    this->field12 = 2;

    this->plttMain = 0xf;
    this->plttSub = 0xf;

    this->listMain.prev = (MesWinSlot *)0x11111111;
    this->listMain.next = (MesWinSlot *)&this->listMainEnd;
    this->listMainEnd.prev = (MesWinSlot *)&this->listMain;
    this->listMainEnd.next = (MesWinSlot *)-1;

    this->listSub.prev = (MesWinSlot *)0x11111111;
    this->listSub.next = (MesWinSlot *)&this->listSubEnd;
    this->listSubEnd.prev = (MesWinSlot *)&this->listSub;
    this->listSubEnd.next = (MesWinSlot *)-1;

    MI_CpuFill8(this->textCallbacks, 0, sizeof(this->textCallbacks));

    func_0200baf0(this->textCallbacks, 11, 1, NULL, NULL);
    func_0200baf0(this->textCallbacks, 12, 1, NULL, NULL);
    func_0200baf0(this->textCallbacks, 15, 2, (UnkCallback_0200b8bc_2c)callbackNumber, this);
    func_0200baf0(this->textCallbacks, 16, 2, (UnkCallback_0200b8bc_2c)callbackNumberPadded, this);
    func_0200baf0(this->textCallbacks, 17, 1, NULL, NULL);
}

// MATCHED 100%
clMesWin::~clMesWin() {
    if (this->screenMask & 4) {
        if (this->texAlloc != NULL) {
            if (this->texMan != NULL) {
                this->texMan->func_0202a134(this->texAlloc, 0, -1);
            }
            this->texAlloc = NULL;
            this->texAddr = NULL;
        }

        if (this->texPlttAlloc != NULL) {
            if (this->texPlttMan != NULL) {
                this->texPlttMan->func_0202a134(this->texPlttAlloc, 0, -1);
            }
            this->texPlttAlloc = NULL;
            this->texPlttAddr = NULL;
        }
    }

    this->release();

    if (this->icons != NULL && this->ownsIcons) {
        if (this->icons != NULL) {
            delete this->icons;
        }
        this->icons = NULL;
    }
}

inline void clMesWin::updateWindow(MesWinSlot * window) {
    if (window->clearStep) {
        this->stepClear(window);
    } else if (window->text.field04 != NULL && window->paused == 0) {
        if (!this->checkClose(window)) {
            this->advanceText(window);
        } else if (window->chars == 0) {
            return;
        }
    }

    switch (window->params.mode) {
        case 0: {
            if (window->tail.active == 0) {
                this->addTail(window);
                this->writeScreen(window);

                if ((window->params.screen & 0xff) == 0) {
                    this->screenDirtyMain = 1;
                } else {
                    this->screenDirtySub = 1;
                }
            }

            this->scrollIcon(&window->cursor);
            this->scrollIcon(&window->tail);
            break;
        }
        case 1: {
            if (window->tail.active == 0) {
                this->addTail(window);
            }

            this->drawQuad(window);
        }
    }
}

// MATCHED 100%
void clMesWin::updateList(MesWinSlot * window) {
    while (window->link.next != (MesWinSlot *)-1) {
        this->curSlot = window->slot;

        if (window->copyPending) {
            window->copyPending = 0;

            u32 size = window->charSize;
            if (window->charsBack < window->chars) {
                func_01ff8380(window->charsBack, window->chars, size);
            } else {
                func_01ff82ac(window->charsBack, window->chars, size);
            }

            this->windows[(s16)window->slot].dirty = 1;
            this->charsDirty = 1;
        }

        if (window->voiceRestart) {
            this->setVoice(window, (window->voiceRestart != 1));
            window->voiceRestart = 0;
        }

        this->updateWindow(window);

        if (window->dirty) {
            DC_FlushRange(window->chars, window->charSize);
        }

        window = window->link.next;
    }
}

// MATCHED 100%
void clMesWin::scrollIcon(MesIcon * icon) {
    if (icon->active) {
        if (icon->fresh) {
            icon->fresh = 0;
        } else {
            s16 dx = 0, dy = 0;

            if (!icon->screen) {
                if (this->scrollMain.x != this->scrollMainCur.x) {
                    dx = this->scrollMainCur.x - this->scrollMain.x;
                }

                if (this->scrollMain.y != this->scrollMainCur.y) {
                    dy = this->scrollMainCur.y - this->scrollMain.y;
                }
            } else {
                if (this->scrollSub.x != this->scrollSubCur.x) {
                    dx = this->scrollSubCur.x - this->scrollSub.x;
                }

                if (this->scrollSub.y != this->scrollSubCur.y) {
                    dy = this->scrollSubCur.y - this->scrollSub.y;
                }
            }

            icon->x += dx;
            icon->y += dy;
        }
    }
}

// MATCHED 100%
BOOL clMesWin::checkClose(MesWinSlot * window) {
    if (window->closeReq) {
        window->closed = 1;

        if (window->voiceState != 0 && window->voiceState != 3) {
            window->voiceState = 0;

            if (window->params.voice != 0) {
                window->voice.stop(2);
            }
        }

        if (window->params.persistent == 0) {
            window->closeReq = 0;

            this->close(window->slot);
        }

        return TRUE;
    }

    return FALSE;
}

// MATCHED 100%
void clMesWin::stepClear(MesWinSlot * window) {
    switch (window->clearStep) {
        case 1: {
            u32 * dst = (this->singleBuf[window->params.screen] == 0) ? window->charsBack : window->chars;
            func_01ff8228(dst, 0, window->charSize);
            window->clearStep++;
            break;
        }
        case 2: {
            this->drawFrame(window, FALSE);
            window->clearStep++;
            break;
        }
        case 3: {
            if (this->singleBuf[window->params.screen] == 0) {
                u32 size = window->charSize;
                if (window->charsBack < window->chars) {
                    func_01ff8380(window->charsBack, window->chars, size);
                } else {
                    func_01ff82ac(window->charsBack, window->chars, size);
                }
            }

            this->windows[(s16)window->slot].dirty = 1;
            this->charsDirty = 1;
            window->clearStep = 0;
        }
    }
}

// MATCHED 100%
void clMesWin::advanceText(MesWinSlot * window) {
    if (window->choiceCount) {
        u16 key = systemState.inputState.press;
        if ((key & PAD_KEY_RIGHT) || (key & PAD_KEY_DOWN)) {
            window->choice++;

            if (window->choice == window->choiceCount) {
                window->choice = 0;
            }

            this->playCursorSound();
        } else if ((key & PAD_KEY_LEFT) || (key & PAD_KEY_UP)) {
            if (window->choice == 0) {
                window->choice = window->choiceCount;
            }

            window->choice--;

            this->playCursorSound();
        }

        if (window->cursor.active) {
            s16 x = window->choicePos[window->choice][0];
            s16 y = window->choicePos[window->choice][1];
            if (window->params.mode == 0) {
                x -= (window->params.screen == 0) ? this->scrollMain.x : this->scrollSub.x;
                y -= (window->params.screen == 0) ? this->scrollMain.y : this->scrollSub.y;
            }

            window->cursor.x = (u16)x;
            window->cursor.y = (u16)y;
        }
    }

    if (window->promptMode) {
        if (systemState.inputState.press & this->decideKeys) {
            s32 result;
            switch (window->promptMode) {
                case 1: {
                    window->choiceCount = 0;

                    result = window->choice;

                    this->playDecideSound();

                    break;
                }
                default:
                    result = -1;
            }

            if (window->params.result != NULL) {
                *window->params.result = result;
            }

            if (window->cursor.active) {
                window->cursor.active = 0;

                window->cursor.link.prev->link.next = window->cursor.link.next;
                window->cursor.link.next->link.prev = window->cursor.link.prev;
            }

            window->promptMode = 0;
            window->oddFrame = 0;
            window->waitCount = 0;
        }
    } else if (window->newPage) {
        this->windows[(s16)window->slot].clearStep = 1;

        this->stepClear(window);

        window->newPage = 0;

        window->oddFrame = 0;
        window->waitCount = 0;

        window->line = 0;

        window->lineWidth = 0;

        window->text.field08_0 = window->textX;
        window->text.field08_10 = window->textY;

        func_0200b9f4(&window->text);

        window->choiceCount = 0;

        if (window->params.style == 2) {
            this->alignLine(window, &window->text, 1);
        }

        window->lineWidth = 0;

        this->alignLine(window, &window->text, 1);
    } else {
        if ((systemState.inputState.press & this->decideKeys) && window->waitCount == 0) {
            window->skip = 1;
        }

        if (window->skip == 0) {
            window->oddFrame ^= 1;

            if (window->oddFrame) {
                return;
            }

            if (window->waitCount) {
                if (window->voiceState != 0 && window->voiceState != 3) {
                    window->voiceState = 2;
                    window->voice.stop(2);
                }

                window->waitCount--;
                return;
            }
        }

        do {
            do {
                if (window->numSlot) {
                    if (this->printNumber(window)) {
                        window->numSlot = 0;
                    }
                    break;
                }
            } while (this->execCode(window, &window->text, func_0200bce8(&window->text, 0, 0)));
        } while (window->waitKey || window->hold || window->skip);
    }
}

// MATCHED 100%
BOOL clMesWin::execCode(MesWinSlot * window, UnkStruct_0200b8bc * text, UnkStruct_0200bce8_24 code) {
    BOOL result = FALSE;

    switch (code.field02h) {
        case 0:
        case 0xff20:
            break;
        case 0xff00: {
            window->line++;

            result = TRUE;
            break;
        }
        case 0xff01: {
            window->waitKey = 0;
            window->skip = 0;

            window->newPage = 1;

            text->field04++;
            break;
        }
        case 0xff03: {
            result = TRUE;

            window->waitKey = 1;
            window->voice.stop(0);

            this->setVoice(window, TRUE);
            break;
        }
        case 0xff04: {
            u16 x = window->params.x;
            u16 y = window->params.y;
            if (window->params.mode == 0) {
                if (x & 7) {
                    x -= (x & 7);
                }
                if (y & 7) {
                    y -= (y & 7);
                }
            }

            window->choicePos[window->choiceCount][0] = x + text->field08_0 + 8;
            window->choicePos[window->choiceCount][1] = y + text->field08_10 +7;

            result = TRUE;

            window->choice = 0;
            window->choiceCount++;

            window->skip = 0;
            break;
        }
        case 0xff0a: {
            window->waitKey = 0;
            window->skip = 0;

            window->closeReq = 1;
            break;
        }
        case 0xff0b: {
            this->alignLine(window, text, code.field04[0]);

            result = TRUE;
            break;
        }
        case 0xff0c: {
            window->waitCount = code.field04[0];

            window->waitKey = 0;
            window->skip = 0;
            break;
        }
        case 0xff0f: {
            this->formatNumber(window, code.field04[0], code.field04[1]-1, FALSE);

            window->numSlot = code.field04[0]+1;
            break;
        }
        case 0xff10: {
            this->formatNumber(window, code.field04[0], code.field04[1]-1, TRUE);

            window->numSlot = code.field04[0]+1;
            break;
        }
        case 0xff11: {
            if (window->voiceState != 0 && window->voiceState != 3) {
                window->voiceState = 0;
                window->voice.stop(2);
            }

            s16 x;
            s16 y;
            u16 anim;
            if (window->choiceCount) {
                anim = 10;

                window->promptMode = 1;

                x = window->choicePos[0][0];
                y = window->choicePos[0][1];
            } else {
                switch (code.field04[0]) {
                    case 0: {
                        window->promptMode = 2;

                        if (window->params.style == 3 || window->params.style == 0) {
                            anim = 0xe;
                         } else {
                            anim = 0xc;
                         }
                        break;
                    }
                    case 1: {
                        window->promptMode = 3;

                        if (window->params.style == 3 || window->params.style == 0) {
                            anim = 0xe;
                         } else {
                            anim = 0xc;
                         }
                    }
                }

                switch (window->params.style) {
                    case 0:
                    case 1: {
                        x = window->params.width * 8 + window->params.x - 2;
                        y = window->params.height * 8 + window->params.y - 2;
                        break;
                    }
                    case 3: {
                        x = window->params.width * 8 + window->params.x - 4;
                        y = window->params.height * 8 + window->params.y - 4;
                        break;
                    }
                    case 2: {
                        x = window->params.width * 8 + window->params.x - 7;
                        y = window->params.height * 8 + window->params.y - 7;
                    }
                }
            }

            this->addIcon(window, x, y, anim, 0, &window->cursor);

            window->waitKey = 0;
            window->skip = 0;
            break;
        }
        case 0xff14: {
            window->hold = 1;
            break;
        }
        case 0xff15: {
            window->hold = 0;
            break;
        }
        case 0xff30:
        case 0xff31:
        case 0xff32: {
            result = TRUE;
            break;
        }
        default: {
            if (window->skip == 0 && window->waitKey == 0 && window->params.voice && window->voiceState != 1) {
                this->setVoice(window, FALSE);
            }

            this->copyRect(window, text->field20_0, text->field20_10, text->field20_20, text->field20_26);

            this->windows[(s16)window->slot].dirty = 1;
            this->charsDirty = 1;
        }
    }

    return result;
}

// MATCHED 100%
void clMesWin::setVoice(MesWinSlot * window, BOOL restart) {
    if (window->params.voice == 0) {
        return;
    }

    BOOL playing = (window->voice.next != NULL || window->voice.prev != NULL);

    if (playing) {
        window->voice.stop(0);
        window->voiceRestart = (restart == TRUE) ? 2 : 1;
        return;
    }

    window->voiceRestart = 0;

    s16 pan = 0x7f;
    if (window->params.tailKind) {
        pan = window->params.voiceX;
        if (window->params.voiceX == 0x7fff) {
            if (window->params.tailX == 0) {
                pan = (window->params.width + 1) * 8 / 2 + window->params.x;
            } else {
                pan = window->params.tailX + window->params.x;
            }
        }
    }

    s32 voiceId = window->params.voice;
    if (restart == FALSE) {
        window->voiceState = 1;
    } else {
        window->voiceState = 3;
        voiceId++;
    }

    window->voice.start(voiceId, 0, 0, 0, pan, 0x60, 0x7f, 0);
}

// MATCHED 100%
void clMesWin::playCursorSound() {
    if (gameMain->mode != 2) {
        sndPlayById(0x200201d8, 0, 0x7f, 0x60, 0x7f, 0);
    } else {
        sndPlayById(0x2003000a, 0, 0x7f, 0x60, 0x7f, 0);
    }
}

// MATCHED 100%
void clMesWin::playDecideSound() {
    if (gameMain->mode != 2) {
        sndPlayById(0x201d9, 0, 0x7f, 0x60, 0x7f, 0);
    } else {
        sndPlayById(0x3000b, 0, 0x7f, 0x60, 0x7f, 0);
    }
}

// MATCHED 100%
void clMesWin::alignLine(MesWinSlot * window, UnkStruct_0200b8bc * text, u32 align) {
    if (window->lineWidth == 0) {
        window->lineWidth = func_0200bbb0(text);
    }

    u16 height = window->params.height;

    switch (align) {
        case 0: {
            text->field08_10 = window->textY;
            break;
        }
        case 1: {
            text->field08_10 = (u32)((height+1) * 8 - (u8)window->lineWidth) / 2;

            if (window->params.mode == 0) {
                text->field08_10 += window->params.y & 7;
            }
            break;
        }
        case 2: {
            text->field08_10 = (height + 2 - window->frame->frame->bottomH) * 8;
            text->field08_10 -= text->field08_24 * (window->lineWidth >> 8) + (u8)window->lineWidth + 1;

            if (window->params.mode == 0) {
                text->field08_10 += window->params.y & 7;
            }
        }
    }

    if (window->line == 0) {
        text->unkStruct24.field24_18 = text->field08_10;
    }
}

// MATCHED 100%
void clMesWin::addIcon(MesWinSlot * window, s16 x, s16 y, u16 anim, u8 flip, MesIcon * icon) {
    if (window->params.mode == 0) {
        if (window->params.screen == 0) {
            x -= this->scrollMain.x;
            y -= this->scrollMain.y;
        } else {
            x -= this->scrollSub.x;
            y -= this->scrollSub.y;
        }
    }

    u8 priority;
    if (window->params.mode != 0) {
        if (window->params.mode == 1) {
            priority = 0;
        }
    } else {
        priority = (window->params.screen != 0) ? this->prioritySub : this->priorityMain;
    }

    this->icons->add(icon, (u8)window->params.screen, (s8)anim, x, y, flip, priority, window->depth-1);

    icon->fresh = 1;
}

// MATCHED 100%
#pragma optimization_level 3
void clMesWin::copyRect(MesWinSlot * window, u8 x, u8 y, u8 w, u8 h) {
    if (this->singleBuf[window->params.screen] != 0) {
        return;
    }

    switch (window->params.mode) {
        case 0: {
            u16 pitch = (window->cols + 1) * 8;

            u32 offset = (y >> 3) * pitch + (x & 0xfff8);
            s32 bytes = (((w + (x & 7)) >> 3) + 1) << 5;

            s32 bottom = (y & 7);
            bottom = (u32)h + bottom;

            for (s16 i = bottom >> 3; i >= 0; i--) {
                u32 * dst = &window->chars[offset];
                u32 * src = &window->charsBack[offset];

                if (src < dst) {
                    func_01ff8380(src, dst, bytes);
                } else {
                    func_01ff82ac(src, dst, bytes);
                }

                offset += pitch;
            }
            break;
        }
        case 1: {
            u32 j;
            u32 rowEnd;

            u16 pitch = window->cols + 1;
            u32 rowStart = y * pitch + (x >> 3);
            s32 right = w + (x & 0x7);

            s16 i = h;
            if (i >= 0) {
                rowEnd = rowStart + (right >> 3);
                do {
                    for (j = rowEnd; j >= rowStart; j--) {
                        window->chars[j] = window->charsBack[j];
                    }

                    rowEnd += pitch;
                    rowStart += pitch;
                } while (--i >= 0);
            }
        }
    }
}
#pragma optimization_level reset

// NONMATCHING 97.98% -- 0x400 rsb on first `if` block ; case1 log expression
s16 clMesWin::create(MesWinParams * params, s16 idx) {
    u16 w;
    MesWinSlot * window;
    MesWinParams * windowParams;
    u8 * str;
    u16 h;

    if (params->textId == 0x3ff) {
        str = (u8*)params->text;
    } else if (params->text == NULL) {
        str = NULL;
    } else {
        str = (u8*)params->text + params->text[params->textId];
    }

    if (str != NULL) {
        if (params->textId == 0x3ff) {
            s32 size;
            if (params->width == 0 || params->height == 0) {
                size = (u16)this->measureText((const char *)str, params->font);
            }

            if (params->width == 0) {
                w = size;
            } else {
                w = params->width;
            }  

            if (params->height == 0) {
                h = size >> 16;
            } else {
                h = params->height;
            }
        } else {
            if (params->width) {
                w = params->width;
            } else {
                w = str[0]+1;
            }

            if (params->height) {
                h = params->height;
            } else {
                h = str[1]+1;
            }
        }
    } else {
        w = params->width;
        h = params->height;
    }

    if (params->screen == 0) {
        if (this->screenMask & 4) {
            params->mode = 1;
        } else {
            params->mode = 0;
        }
    } else {
        params->mode = 0;
    }

    this->curSlot = idx;

    if (params->fixedSize == 0) {
        BOOL hasHeader = (params->textId != 0x3ff && str != NULL);
        u8 * body = hasHeader ? &str[2] : str;
        u32 size = this->calcSize(params->style, w, h, body, params->indent);
        w = size;
        h = size >> 16;
    }

    u16 rows = h;
    u16 cols = w;
    switch (params->mode) {
        case 0: {
            if (params->x & 7) {
                cols++;
            }

            if (params->y & 7) {
                rows++;
            }
            break;
        }
        case 1: {
            u32 px = w * 8;
            if (px != 0) {
                px -= 8;
            }

            cols = MATH_ROUNDUP(px, 2 << MATH_ILog2(px)) >> 3;
        }
    }

    window = this->allocWindow(params->screen, cols * rows * 32, idx);
    if (window == NULL) {
        return -1;
    }

    window->frame = this->findFrame(params->style, h);

    windowParams = &window->params;

    if (params < windowParams) {
        func_01ff8380(params, windowParams, sizeof(MesWinParams));
    } else {
        func_01ff82ac(params, windowParams, sizeof(MesWinParams));
    }

    windowParams->width = w-1;
    windowParams->height = h-1;

    window->cols = cols-1;
    window->rows = rows-1;

    window->voiceState = 0;

    this->drawFrame(window, 1);

    if (str != NULL) {
        const MesFrame * frame = window->frame->frame;

        switch (params->style) {
            case 1:
            case 2:
            case 3: {
                window->textX = (frame->cornerW * 8 >> 1) + 4;
                window->textY = (frame->topH * 8 >> 1) + 2;
                break;
            }
            case 0:
            default: {
                window->textX = frame->cornerW * 8 >> 1;
                window->textY = frame->topH * 8 >> 1;
            }
        }

        u32 xFrac = 0;
        if (windowParams->mode == 0) {
            xFrac = windowParams->x & 7;

            window->textX += xFrac;
            window->textY += windowParams->y & 7;
        }

        window->textX += params->indent;

        u32 * chars = (this->singleBuf[params->screen] == 0) ? window->charsBack : window->chars;

        if (windowParams->textId != 0x3ff) {
            str += 2;
        }
 
        u32 lines = (windowParams->textId == 0x3ff) ? windowParams->field04_28 : 1;

        // !! Hacky check to force the strange `movs`/`moveq` instructions
        // Maybe DEBUG code existed here??
        u32 p7;
        u32 p8;
        if (TRUE) p8 = 1;
        p8 = p8 ? p8 : 3;
        if (TRUE) p7 = 1;
        p7 = p7 ? p7 : 2;
        
        func_0200b8bc(&window->text, windowParams->font, (u32)chars, (const char *)str, window->textX, window->textY, p7, p8, lines, 6, window->cols, windowParams->width, xFrac, windowParams->mode, this->textCallbacks);

        window->lineWidth = 0;

        if (windowParams->style == 2) {
            this->alignLine(window, &window->text, 1);
        }

        window->lineWidth = 0;

        this->alignLine(window, &window->text, 1);
    }

    return (u16)window->slot;
}

// MATCHED 100%
void clMesWin::layoutText(const char * str, UnkStruct_0200b8bc_10 ** font, UnkStruct_0200b8bc_24 * out, u8 alignW) {
    UnkStruct_0200b8bc text;
    func_0200b8bc(&text, font, 0, str, 0, 0, (alignW == 0) ? 2 : 1, (alignW == 0) ? 3 : 1, 1, 6, 0x40, alignW, 0, 0, this->textCallbacks);
    func_0200bb1c(&text, out);
}

// MATCHED 100%
bool clMesWin::destroy(s16 idx) {
    MesWinSlot * window = &this->windows[idx];

    if (this->winCountMain + this->winCountSub == 0) {
        return false;
    }

    if (window->chars == NULL) {
        return false;
    }

    func_01ff8228(window->chars, 0, window->charSize);

    if (this->singleBuf[window->params.screen] == 0) {
        func_01ff8228(window->charsBack, 0, window->charSize);
    }

    window->chars = NULL;

    window->closed = 1;

    if (window->voiceState) {
        window->voiceState = 0;
        if (window->params.voice != 0) {
            window->voice.stop(2);
        }
    }

    if (window->cursor.active) {
        window->cursor.active = 0;
        window->cursor.link.prev->link.next = window->cursor.link.next;
        window->cursor.link.next->link.prev = window->cursor.link.prev;
    }

    if (window->tail.active) {
        window->tail.active = 0;
        window->tail.link.prev->link.next = window->tail.link.next;
        window->tail.link.next->link.prev = window->tail.link.prev;
    }

    window->link.prev->link.next = window->link.next;
    window->link.next->link.prev = window->link.prev;

    if (window->params.screen == 0) {
        this->winCountMain--;
    } else {
        this->winCountSub--;
    }

    return true;
}

// MATCHED 100%
void clMesWin::rebuildScreen(u8 screen) {
    MesWinSlot * window;

    if (screen == 0) {
        if (this->screenMain != NULL) {
            func_01ff8228(this->screenMain, 0, 0x780);
        }

        window = this->listMain.next;
    } else {
        if (this->screenSub != NULL) {
            func_01ff8228(this->screenSub, 0, 0x780);
        }

        window = this->listSub.next;
    }

    while (window->link.next != (MesWinSlot *)-1) {
        if (window->params.mode == 0) {
            this->writeScreen(window);
        }

        window = window->link.next;
    }

    if (screen == 0) {
        this->screenDirtyMain = 1;
    } else {
        this->screenDirtySub = 1;
    }
}

// MATCHED 100%
MesWinSlot * clMesWin::allocWindow(u8 screen, u32 size, s16 idx) {
    BOOL found = FALSE;

    u8 * pool;
    MesWinSlot * head;
    u32 poolSize;
    if (screen == 0) {
        pool = this->charPoolMain;
        head = this->listMain.next;
        poolSize = this->charPoolSizeMain;
    } else {
        pool = this->charPoolSub;
        head = this->listSub.next;
        poolSize = this->charPoolSizeSub;
    }

    MesWinSlot * cur = head;
    u8 * ptr = pool;
    do {
        MesWinSlot * next = cur->link.next;
        if (next == (MesWinSlot *)-1) {
            if (pool + poolSize - ptr >= size) {
                found = TRUE;
            }
            break;
        }

        u32 gap = (u8 *)cur->chars - ptr;
        if (gap >= size) {
            found = TRUE;
            break;
        }

        ptr += gap + cur->charSize;
        cur = next;
    } while (TRUE);

    if (found == FALSE) {
        return NULL;
    }

    s16 slot = 0;
    MesWinSlot * window;
    if (idx != -1) {
        slot = idx;
        window = &this->windows[idx];
    } else {
        window = this->windows;

        while (window->chars != NULL) {
            window++;
            slot++;
        }
    }

    MI_CpuFill8(window, 0, sizeof(MesWinSlot));

    window->chars = (u32 *)ptr;

    window->charSize = size;
    window->tileBase = ((u32)(ptr - pool) >> 5) + 1;
    window->slot = slot;

    if (this->singleBuf[screen] == 0) {
        window->charsBack = (u32 *)((u8 *)this->backBuf[screen] + (ptr - pool));
    }

    u32 * chars;
    MesWinSlot * next = head->link.next;
    if (next != (MesWinSlot *)-1) {
        chars = window->chars;
        do {
            if (head->chars > chars) {
                break;
            }
            head = next;
            next = next->link.next;
        } while (next != (MesWinSlot *)-1);
    }

    window->link.prev = head->link.prev;
    window->link.next = head->link.prev->link.next;
    head->link.prev->link.next = window;
    head->link.prev = window;

    if (screen == 0) {
        this->winCountMain++;
    } else {
        this->winCountSub++;
    }

    return window;
}

// MATCHED 100%
const MesFrameEntry * clMesWin::findFrame(u8 style, u8 height) {
    const MesFrameEntry * entry = mesFrameTables[style];

    while (entry->maxH < height) {
        entry++;
    }

    return entry;
}

// MATCHED 100%
void clMesWin::drawFrame(MesWinSlot * window, BOOL flush) {
    u32 * chars = (this->singleBuf[window->params.screen] == 0) ? window->charsBack : window->chars;

    if (window->params.style) {
        switch (window->params.mode) {
            case 0: {
                if ((window->params.x & 7) || (window->params.y & 7)) {
                    this->drawFrameBGShifted(window, chars);
                } else {
                    this->drawFrameBG(window, chars);
                }
                break;
            }
            case 1: {
                this->drawFrameTex(window, chars);
            }
        }

        if (this->singleBuf[window->params.screen] == 0) {
            window->copyPending = 1;
        }
    }

    if (flush && window->copyPending == 0) {
        this->windows[(s16)window->slot].dirty = 1;

        this->charsDirty = 1;

        if (window->dirty) {
            DC_FlushRange(window->chars, window->charSize);
        }
    }
}

// MATCHED 100%
void clMesWin::drawFrameTex(MesWinSlot * window, u32 * chars) {
    const MesFrame * frame = window->frame->frame;
    u16 w = window->params.width + 1;
    u32 * row;
    s16 rows;
    u16 rightEdgeOff;
    const u16 * rightEdge;
    const u16 * bottomEdge;

    rows = frame->topH;

    u16 h = window->params.height + 1;
    u16 pitch = window->cols + 1;

    u16 rightOff = w - frame->cornerW;
    row = chars;

    const u16 * left = frame->topLeft;
    const u16 * right = frame->topRight;
    s16 i;
    while (rows > 0) {
        for (i = 0; i < frame->cornerW; i++) {
            putTileColumn(this->frameTiles + *left++, row + i, pitch);
            putTileColumn(this->frameTiles + *right++, row + i + rightOff, pitch);
        }

        row += pitch * 8;
        rows--;
    }

    left = frame->bottomLeft;

    u16 bottomOff = pitch * (h - frame->bottomH) * 8;
    u16 bottomRightOff = rightOff + bottomOff;

    rows = frame->bottomH;

    u32 * p = chars;
    right = frame->bottomRight;
    while (rows > 0) {
        for (i = 0; i < frame->cornerW; i++) {
            putTileColumn(this->frameTiles + *left, p + i + bottomOff, pitch);
            left++;

            putTileColumn(this->frameTiles + *right, p + i + bottomRightOff, pitch);
            right++;
        }

        p += pitch * 8;
        rows--;
    }

    const u16 * tile;
    u32 * q;
    s16 y;
    s16 i2;
    for (s16 x = frame->cornerW; x < w - frame->cornerW; x += frame->edgeH[0]) {
        tile = frame->edgeH + 1;
        q = chars + x;
        for (y = frame->topH; y > 0; y--) {
            for (i2 = frame->edgeH[0]; i2 > 0; i2--) {
                putTileColumn(this->frameTiles + *tile, q, pitch);
                tile++;
                q++;
            }
            q += pitch * 8 - frame->edgeH[0];
        }
    }

    u16 bottomEdgeOff = pitch * (h - frame->bottomH) * 8;
    bottomEdge = frame->edgeH + 1 + frame->edgeH[0] * frame->topH;
    for (s16 x = frame->cornerW; x < w - frame->cornerW; x += frame->edgeH[0]) {
        q = chars + x + bottomEdgeOff;
        tile = bottomEdge;
        for (y = frame->bottomH; y > 0; y--) {
            for (i2 = frame->edgeH[0]; i2 > 0; i2--) {
                putTileColumn(this->frameTiles + *tile, q, pitch);
                tile++;
                q++;
            }

            q += pitch * 8 - frame->edgeH[0];
        }
    }

    if (frame->edgeV != NULL) {
        rightEdgeOff = w - frame->cornerW;
        for (y = frame->topH; y < h - frame->bottomH; y += frame->edgeV[0]) {
            tile = frame->edgeV + 1;
            q = chars + pitch * y * 8;
            for (rows = frame->edgeV[0]; rows > 0; rows--) {
                for (i = frame->cornerW; i > 0; i--) {
                    putTileColumn(this->frameTiles + *tile, q, pitch);
                    tile++;
                    q++;
                }
                q += pitch * 8 - frame->cornerW;
            }
        }

        rightEdge = frame->edgeV + 1 + frame->cornerW * frame->edgeV[0];
        for (y = frame->topH; y < h - frame->bottomH; y += frame->edgeV[0]) {
            q = chars + pitch * y * 8 + rightEdgeOff;
            tile = rightEdge;
            for (rows = frame->edgeV[0]; rows > 0; rows--) {
                for (i = frame->cornerW; i > 0; i--) {
                    putTileColumn(this->frameTiles + *tile, q, pitch);
                    tile++;
                    q++;
                }
                q += pitch * 8 - frame->cornerW;
            }
        }
    }

    if (frame->edgeV != NULL) {
        u32 fill = frame->fill;
        s32 bytes = (w - frame->cornerW * 2) * 4;
        p = chars + pitch * frame->topH * 8 + frame->cornerW;

        for (y = (h - (frame->topH + frame->bottomH)) * 8; y > 0; y--) {
            func_01ff8228(p, fill, bytes);
            p += pitch;
        }
    }
}

// MATCHED 100%
#ifndef IDE_PARSE
asm void putTileColumn(register void * src, register void * dst, register s32 stride) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    ldmia r0!, {r4, r5, r6, r7, r8, r9, r10, r11}
    mov r2, r2, lsl #0x2
    str r4, [r1, #0x0]
    add r1, r1, r2
    str r5, [r1, #0x0]
    add r1, r1, r2
    str r6, [r1, #0x0]
    add r1, r1, r2
    str r7, [r1, #0x0]
    add r1, r1, r2
    str r8, [r1, #0x0]
    add r1, r1, r2
    str r9, [r1, #0x0]
    add r1, r1, r2
    str r10, [r1, #0x0]
    add r1, r1, r2
    str r11, [r1, #0x0]
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}
#endif

// MATCHED 100%
void clMesWin::drawFrameBG(MesWinSlot * window, u32 * chars) {
    const u16 * rightEdge;
    u32 * dst;
    s16 rows;
    s16 y;
    s16 i;
    u32 * tile;

    const MesFrame * frame = window->frame->frame;
    u16 w = window->params.width + 1;
    u16 h = window->params.height + 1;

    u16 offA = (w - frame->cornerW) * 8;

    const u16 * left = frame->topLeft;
    const u16 * right = frame->topRight;
    dst = chars;
    rows = frame->topH;
    while (rows > 0) {
        for (i = frame->cornerW; i > 0; i--) {
            tile = this->frameTiles + *left++;
            if (tile < dst) {
                func_01ff8380(tile, dst, 0x20);
            } else {
                func_01ff82ac(tile, dst, 0x20);
            }

            tile = this->frameTiles + *right++;
            if (tile < dst + offA) {
                func_01ff8380(tile, dst + offA, 0x20);
            } else {
                func_01ff82ac(tile, dst + offA, 0x20);
            }

            dst += 8;
        }

        dst += offA;
        rows--;
    }

    dst = chars;

    left = frame->bottomLeft;

    u16 offB = w * (h - frame->bottomH) * 8;
    u16 offC = offA + offB;

    rows = frame->bottomH;

    right = frame->bottomRight;

    while (rows > 0) {
        for (i = frame->cornerW; i > 0; i--) {
            tile = this->frameTiles + *left++;
            if (tile < dst + offB) {
                func_01ff8380(tile, dst + offB, 0x20);
            } else {
                func_01ff82ac(tile, dst + offB, 0x20);
            }

            tile = this->frameTiles + *right++;
            if (tile < dst + offC) {
                func_01ff8380(tile, dst + offC, 0x20);
            } else {
                func_01ff82ac(tile, dst + offC, 0x20);
            }

            dst += 8;
        }

        dst += offA;

        rows--;
    }

    offA = (w - frame->edgeH[0]) * 8;
    s16 x;
    for (x = frame->cornerW; x < w - frame->cornerW; x += frame->edgeH[0]) {
        dst = chars + x * 8;
        left = frame->edgeH + 1;

        for (y = frame->topH; y > 0; y--) {
            for (i = frame->edgeH[0]; i > 0; i--) {
                tile = this->frameTiles + *left++;
                if (tile < dst) {
                    func_01ff8380(tile, dst, 0x20);
                } else {
                    func_01ff82ac(tile, dst, 0x20);
                }

                dst += 8;
            }

            dst += offA;
        }
    }

    right = frame->edgeH + 1 + frame->edgeH[0] * frame->topH;

    offB = w * (h - frame->bottomH) * 8;
    offC = (w - frame->edgeH[0]) * 8;

    for (y = frame->cornerW; y < w - frame->cornerW; y += frame->edgeH[0]) {
        dst = chars + y * 8 + offB;
        left = right;

        for (rows = frame->bottomH; rows > 0; rows--) {
            for (i = frame->edgeH[0]; i > 0; i--) {
                tile = this->frameTiles + *left++;
                if (tile < dst) {
                    func_01ff8380(tile, dst, 0x20);
                } else {
                    func_01ff82ac(tile, dst, 0x20);
                }

                dst += 8;
            }

            dst += offC;
        }
    }

    if (frame->edgeV != NULL) {
        rightEdge = frame->edgeV + 1 + frame->cornerW * frame->edgeV[0];
        offA = (w - frame->cornerW) * 8;

        for (y = frame->topH; y < h - frame->bottomH; y += frame->edgeV[0]) {
            right = rightEdge;
            dst = chars + w * y * 8;
            left = frame->edgeV + 1;

            for (rows = frame->edgeV[0]; rows > 0; rows--) {
                for (i = frame->cornerW; i > 0; i--) {
                    tile = this->frameTiles + *left++;
                    if (tile < dst) {
                        func_01ff8380(tile, dst, 0x20);
                    } else {
                        func_01ff82ac(tile, dst, 0x20);
                    }

                    tile = this->frameTiles + *right++;
                    if (tile < dst + offA) {
                        func_01ff8380(tile, dst + offA, 0x20);
                    } else {
                        func_01ff82ac(tile, dst + offA, 0x20);
                    }

                    dst += 8;
                }

                dst += offA;
            }
        }
    }

    if (frame->edgeV != NULL) {
        u32 bytes = (w - frame->cornerW * 2) * 32;
        u32 fill = frame->fill;
        u32 * dst2 = chars + (w * frame->topH + frame->cornerW) * 8;

        for (rows = h - (frame->topH + frame->bottomH); rows > 0; rows--) {
            MIi_CpuClearFast(fill, dst2, bytes);
            dst2 += (u16)(w * 8);
        }
    }
}

// MATCHED 100%
void clMesWin::drawFrameBGShifted(MesWinSlot * window, u32 * chars) {
    u32 fill;
    const MesFrame * frame;
    s16 x;
    s16 i;
    s16 n;
    u16 xFrac;
    frame = window->frame->frame;
    u8 yFrac = window->params.y & 7;
    xFrac = window->params.x & 7;
    u8 shl = xFrac * 4;
    u8 cols = window->cols;
    u16 rows = yFrac ? window->rows : window->params.height + 1;
    u32 shr = 32 - shl;
    fill = frame->fill;
    u16 rightOff = xFrac ? (cols - frame->cornerW) * 8 : (cols - frame->cornerW + 1) * 8;

    u32 * d;
    u32 * src2;
    u32 * src1;
    u32 idx;
    u32 val;
    u8 k = 0;
    u32 * p = chars;
    n = frame->topH;
    s16 y;
    while (n > 0) {
        for (i = frame->cornerW; i > 0; i--) {
            src1 = this->frameTiles + frame->topLeft[k];
            src2 = this->frameTiles + frame->topRight[k];
            
            for (y = yFrac; y < yFrac + 8; y++) {
                idx = (y & 0xfff8) * cols + y;
                d = p + idx;

                val = *src1++;
                d[0] += val << shl;
                d[8] += val >> shr;

                val = *src2++;
                (d + rightOff)[0] += val << shl;
                (d + rightOff)[8] += val >> shr;
            }

            p += 8;
            k++;
        }

        p += (cols + 1 - frame->cornerW) * 8;
        n--;
    }

    shr = 32 - shl;
    p = chars;
    k = 0;
    u16 bottomOff = (cols + 1) * (rows - frame->bottomH) * 8;
    u16 bottomRightOff = rightOff + bottomOff;
    n = frame->bottomH;
    while (n > 0) {
        for (i = frame->cornerW; i > 0; i--) {
            src1 = this->frameTiles + frame->bottomLeft[k];
            src2 = this->frameTiles + frame->bottomRight[k];

            for (y = yFrac; y < yFrac + 8; y++) {
                idx = (y & 0xfff8) * cols + y;
                d = p + idx;
                val = *src1++;
                (d + bottomOff)[0] += val << shl;
                (d + bottomOff)[8] += val >> shr;
                val = *src2++;
                (d + bottomRightOff)[0] += val << shl;
                (d + bottomRightOff)[8] += val >> shr;
            }

            p += 8;
            k++;
        }

        p += (cols + 1 - frame->cornerW) * 8;
        n--;
    }

    s16 localA0 = xFrac ? cols - frame->cornerW : cols - frame->cornerW + 1;
    for (x = frame->cornerW; x < localA0; x += frame->edgeH[0]) {
        d = chars + x * 8;
        k = 1;
        for (n = frame->topH; n > 0; n--) {
            for (i = frame->edgeH[0]; i > 0; i--) {
                p = this->frameTiles + frame->edgeH[k];

                for (y = yFrac; y < yFrac + 8; y++) {
                    val = *p++;
                    (d + ((y & 0xfff8) * cols + y))[0] += val << shl;
                    (d + ((y & 0xfff8) * cols + y))[8] += val >> shr;
                }
                d += 8;
                k++;
            }

            d += (cols + 1 - frame->edgeH[0]) * 8;
        }
    }

    x = frame->cornerW;
    u16 local98 = (cols + 1) * (rows - frame->bottomH) * 8;
    for (; x < localA0; x += frame->edgeH[0]) {
        d = chars + x * 8 + local98;
        k = 1;
        for (n = frame->bottomH; n > 0; n--) {
            for (i = frame->edgeH[0]; i > 0; i--) {
                p = this->frameTiles + (frame->edgeH + frame->edgeH[0] * frame->topH)[k];

                for (y = yFrac; y < yFrac + 8; y++) {
                    val = *p++;
                    (d + ((y & 0xfff8) * cols + y))[0] += val << shl;
                    (d + ((y & 0xfff8) * cols + y))[8] += val >> shr;
                }

                d += 8;
                k++;
            }

            d += (cols + 1 - frame->edgeH[0]) * 8;
        }
    }

    s16 i2;
    if (yFrac != 0 && frame->edgeV != NULL) {
        p = chars + ((cols + 1) * (rows - frame->bottomH) + frame->cornerW) * 8;
        u32 *q = chars + (frame->topH * (cols + 1) + frame->cornerW) * 8 + yFrac;
        i2 = xFrac ? cols - frame->cornerW * 2 : cols - frame->cornerW * 2 + 1;
        for (; i2 > 0; i2--) {
            for (i = 8 - yFrac; i > 0; i--) {
                *q += fill << shl;
                q[8] += fill >> shr;
                q++;
            }

            q += yFrac;

            for (i = yFrac; i > 0; i--) {
                *p += fill << shl;
                p[8] += fill >> shr;
                p++;
            }

            p += 8 - yFrac;
        }
    }

    if (frame->edgeV != NULL) {
        u16 rightOff2;
        u16 rowSkip;
        if (xFrac != 0) {
            rightOff2 = (cols - frame->cornerW) * 8;
            rowSkip = rightOff2 + 8;
        } else {
            rightOff2 = (cols - frame->cornerW + 1) * 8;
            rowSkip = rightOff2;
        }

        for (x = frame->topH; x < rows - frame->bottomH; x += frame->edgeV[0]) {
            p = chars + (cols + 1) * x * 8;
            k = 1;
            for (n = frame->edgeV[0]; n > 0; n--) {
                for (i = frame->cornerW; i > 0; i--) {
                    src1 = this->frameTiles + frame->edgeV[k];
                    src2 = this->frameTiles + (frame->edgeV + frame->cornerW * frame->edgeV[0])[k];
                    for (y = yFrac; y < yFrac + 8; y++) {
                        idx = (y & 0xfff8) * cols + y;
                        d = p + idx;

                        val = *src1++;
                        d[0] += val << shl;
                        d[8] += val >> shr;

                        val = *src2++;
                        (d + rightOff2)[0] += val << shl;
                        (d + rightOff2)[8] += val >> shr;
                    }
                    p += 8;
                    k++;
                }
                p += rowSkip;
            }
        }

        if (xFrac != 0) {
            s16 n;
            if (yFrac != 0) {
                p = chars + ((frame->topH + 1) * (cols + 1) + frame->cornerW) * 8;
                n = window->params.height - (frame->topH + frame->bottomH);
            } else {
                p = chars + (frame->topH * (cols + 1) + frame->cornerW) * 8;
                n = rows - (frame->topH + frame->bottomH);
            }

            u16 fillOff = (cols - frame->cornerW * 2) * 8;
            for (; n > 0; n--) {
                for (i = 8; i > 0; i--) {
                    *p += fill << shl;
                    u32 * q = p++;
                    q[fillOff] += fill >> shr;
                }
                p += cols * 8;
            }
        }
    }

    if (frame->edgeV != NULL) {
        u16 inset;
        u16 pitch = window->cols + 1;
        inset = xFrac ? frame->cornerW + 1 : frame->cornerW;
        u16 top = yFrac ? frame->topH + 1 : frame->topH;
        u16 bottom = yFrac ? frame->bottomH + 1 : frame->bottomH;
        u32 fill = frame->fill;
        u32 sz = (pitch - inset * 2) * 32;
        u32 * dst = chars + (pitch * top + inset) * 8;
        
        for (s16 n = window->rows + 1 - (top + bottom); n > 0; n--) {
            func_01ff8228(dst, fill, sz);
            dst += (u16)(pitch * 8);
        }
    }
}

// MATCHED 100%
void clMesWin::writeScreen(MesWinSlot * window) {
    if (window->params.mode != 0) {
        return;
    }

    s16 col;
    s16 row;

    u16 col0 = window->params.x >> 3;
    u16 row0 = window->params.y >> 3;

    u16 * screen;
    u16 tile = window->tileBase;
    if (window->params.screen == 0) {
        screen = (u16 *)this->screenMain;
        tile += this->plttMain * 4096;
    } else {
        screen = (u16 *)this->screenSub;
        tile += this->plttSub * 4096;
    }

    for (row = row0; row < row0 + window->rows + 1; row++) {
        for (col = col0; col < col0 + window->cols + 1; col++) {
            screen[row * MES_SCREEN_COLS + col] = tile;
            tile++;
        }
    }
}

// MATCHED 100%
void clMesWin::drawQuad(MesWinSlot * window) {
    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);

    fx32 z = window->depth * this->scale;
    fx32 y = (window->params.y << 5) * this->scale;
    fx32 x = (window->params.x << 5) * this->scale;
    G3_Translate(x, y, z);

    fx32 scale = this->scale << FX32_SHIFT;
    G3_Scale(scale, scale, scale);
    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    u32 texShift = (window->params.width + 1) * 8;
    if (texShift != 0) {
        texShift -= 8;
    }
    texShift = MATH_ILog2(texShift);

    u32 addr = this->texAddr + window->tileBase * 32;
    G3_TexImageParam(GX_TEXFMT_PLTT16, GX_TEXGEN_TEXCOORD, (GXTexSizeS)(texShift-2), GX_TEXSIZE_T256, GX_TEXREPEAT_NONE, GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_TRNS, addr);
    G3_TexPlttBase(this->texPlttAddr, GX_TEXFMT_PLTT16);

    u32 vx = (window->params.width+1) << 8;
    u32 vy = (window->params.height+1) << 8;

    fx32 s = ((window->params.width+1) * 8) << FX32_SHIFT;
    fx32 t = ((window->params.height+1) * 8) << FX32_SHIFT;

    G3_Begin(GX_BEGIN_QUADS);
    G3_Color(GX_RGB(31, 31, 31));
    G3_TexCoord(0, 0);
    G3_Vtx(0, 0, 0);
    G3_TexCoord(s, 0);
    G3_Vtx(vx, 0, 0);
    G3_TexCoord(s, t);
    G3_Vtx(vx, vy, 0);
    G3_TexCoord(0, t);
    G3_Vtx(0, vy, 0);
    G3_End();
    G3_PopMtx(1);
}

// MATCHED 100%
void clMesWin::setBGBase(GXBGScrBase screenBase, GXBGCharBase charBase) {
    switch (this->bgMain) {
        case 0:
            G2_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase, GX_BG_EXTPLTT_01);
            G2_SetBG0Priority(this->priorityMain);
            break;
        case 1:
            G2_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase, GX_BG_EXTPLTT_01);
            G2_SetBG1Priority(this->priorityMain);
            break;
        case 2:
            G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase);
            G2_SetBG2Priority(this->priorityMain);
            break;
        case 3:
            G2_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase);
            G2_SetBG3Priority(this->priorityMain);
    }
}

// MATCHED 100%
void clMesWin::setBGBaseSub(GXBGScrBase screenBase, GXBGCharBase charBase) {
    switch (this->bgSub) {
        case 0:
            G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase, GX_BG_EXTPLTT_01);
            G2S_SetBG0Priority(this->prioritySub);
            break;
        case 1:
            G2S_SetBG1Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase, GX_BG_EXTPLTT_01);
            G2S_SetBG1Priority(this->prioritySub);
            break;
        case 2:
            G2S_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase);
            G2S_SetBG2Priority(this->prioritySub);
            break;
        case 3:
            G2S_SetBG3ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, (GXBGScrBase)screenBase, (GXBGCharBase)charBase);
            G2S_SetBG3Priority(this->prioritySub);
    }
}

// MATCHED 100%
void clMesWin::loadScreen() {
    switch (this->bgMain) {
        case 0:
            GX_LoadBG0Scr(this->screenMain, 0x80, this->screenSizeMain);
            break;
        case 1:
            GX_LoadBG1Scr(this->screenMain, 0x80, this->screenSizeMain);
            break;
        case 2:
            GX_LoadBG2Scr(this->screenMain, 0x80, this->screenSizeMain);
            break;
        case 3:
            GX_LoadBG3Scr(this->screenMain, 0x80, this->screenSizeMain);
    }
}

// MATCHED 100%
void clMesWin::loadScreenSub() {
    switch (this->bgSub) {
        case 0:
            GXS_LoadBG0Scr(this->screenSub, 0x80, this->screenSizeSub);
            break;
        case 1:
            GXS_LoadBG1Scr(this->screenSub, 0x80, this->screenSizeSub);
            break;
        case 2:
            GXS_LoadBG2Scr(this->screenSub, 0x80, this->screenSizeSub);
            break;
        case 3:
            GXS_LoadBG3Scr(this->screenSub, 0x80, this->screenSizeSub);
    }
}

// MATCHED 100%
void clMesWin::loadChars() {
    this->charsDirty = 0;

    MesWinSlot  * window = this->listMain.next;

    if (window->params.mode == 0) {
        while (window->link.next != (MesWinSlot *)-1) {
            if (window->dirty) {
                window->dirty = 0;

                this->loadWinChars(window);
            }

            window = window->link.next;
        }
    } else {
        while (window->link.next != (MesWinSlot *)-1) {
            if (window->dirty) {
                window->dirty = 0;

                GX_BeginLoadTex();
                GX_LoadTex(window->chars, this->texAddr + (window->tileBase << 5), window->charSize);
                GX_EndLoadTex();
            }

            window = window->link.next;
        }
    }

    window = this->listSub.next;

    if (window->params.mode == 0) {
        while (window->link.next != (MesWinSlot *)-1) {
            if (window->dirty) {
                window->dirty = 0;

                this->loadWinChars(window);
            }

            window = window->link.next;
        }
    } else {
        while (window->link.next != (MesWinSlot *)-1) {
            if (window->dirty) {
                window->dirty = 0;

                GX_BeginLoadTex();
                GX_LoadTex(window->chars, this->texAddr + (window->tileBase << 5), window->charSize);
                GX_EndLoadTex();
            }

            window = window->link.next;
        }
    }
}

// MATCHED 100%
void clMesWin::loadWinChars(MesWinSlot * window) {
    if (window->params.screen == 0) {
        switch (this->bgMain) {
            case 0:
                GX_LoadBG0Char(window->chars, window->tileBase * 32, window->charSize);
                break;
            case 1:
                GX_LoadBG1Char(window->chars, window->tileBase * 32, window->charSize);
                break;
            case 2:
                GX_LoadBG2Char(window->chars, window->tileBase * 32, window->charSize);
                break;
            case 3:
                GX_LoadBG3Char(window->chars, window->tileBase * 32, window->charSize);
        }
    } else {
        switch (this->bgSub) {
            case 0:
                GXS_LoadBG0Char(window->chars, window->tileBase * 32, window->charSize);
                break;
            case 1:
                GXS_LoadBG1Char(window->chars, window->tileBase * 32, window->charSize);
                break;
            case 2:
                GXS_LoadBG2Char(window->chars, window->tileBase * 32, window->charSize);
                break;
            case 3:
                GXS_LoadBG3Char(window->chars, window->tileBase * 32, window->charSize);
        }
    }
}

// MATCHED 100%
void clMesWin::applyScroll() {
    if (this->scrollLockMain == 0 && (this->screenMask & 1)) {
        switch (this->bgMain) {
            case 0: {
                G2_SetBG0Offset(this->scrollMainCur.x, this->scrollMainCur.y + 0x10);
                break;
            }
            case 1: {
                G2_SetBG1Offset(this->scrollMainCur.x, this->scrollMainCur.y + 0x10);
                break;
            }
            case 2: {
                G2_SetBG2Offset(this->scrollMainCur.x, this->scrollMainCur.y + 0x10);
                break;
            }
            case 3: {
                G2_SetBG3Offset(this->scrollMainCur.x, this->scrollMainCur.y + 0x10);
            }
        }
    }

    if (this->scrollLockSub == 0 && (this->screenMask & 2)) {
        switch (this->bgSub) {
            case 0: {
                G2S_SetBG0Offset(this->scrollSubCur.x, this->scrollSubCur.y + 0x10);
                break;
            }
            case 1: {
                G2S_SetBG1Offset(this->scrollSubCur.x, this->scrollSubCur.y + 0x10);
                break;
            }
            case 2: {
                G2S_SetBG2Offset(this->scrollSubCur.x, this->scrollSubCur.y + 0x10);
                break;
            }
            case 3: {
                G2S_SetBG3Offset(this->scrollSubCur.x, this->scrollSubCur.y + 0x10);
            }
        }
    }
}

// MATCHED 100%
u32 clMesWin::calcSize(u8 style, u8 w, u8 h, u8 * str, u8 indent) {
    u8 width = w;
    u8 height = h;

    if (str != NULL && style == 2) {
        u32 maxLines = 0;
        u8 lines = 1;

        while (TRUE) {
            u16 code = (str[0] << 8) | str[1];

            if (code == 0xff0a) {
                break;
            }
            
            if (code == 0xff01) {
                lines = 1;
            } else if (code == 0xff11) {
                if (str[2] == 0) {
                    lines = 1;
                }
            } else if (code == 0xff00) {
                lines++;

                if (maxLines < lines) {
                    maxLines = lines;
                }
            }
            str++;
        }

        if (maxLines >= 2) {
            height++;
        }
    }

    const MesFrameEntry * entry = this->findFrame(style, height);
    const MesFrame * frame = entry->frame;

    u8 cornerW2 = frame->cornerW * 2;
    u8 capH = (u8)(frame->topH + frame->bottomH);
    u8 stepW = frame->edgeH[0];
    u8 stepH = (frame->edgeV != NULL) ? frame->edgeV[0] : 0;

    width += indent >> 3;

    if (indent & 1) {
        width++;
    }

    if (width < cornerW2 + stepW) {
        width = cornerW2 + stepW;
    }

    u8 maxH = entry->maxH;

    if (maxH == 0xff) {
        if (height < capH + stepH) {
            height = capH + stepH;
        }
    } else {
        height = maxH;
    }

    if (style == 2) {
        if (str != NULL) {
            width++;
        }

        s16 n = width-cornerW2;
        if (n % stepW != 0) {
            width = stepW * (n / stepW + 1) + cornerW2;
        }

        if (maxH == 0xff) {
            n = height - capH;
            if (n % stepH != 0) {
                height = stepH * (n / stepH + 1) + capH;
            }
        }
    } else {
        s16 val = (((width - cornerW2) << 8) / stepW + 0x80) >> 8;
        width = val * stepW + cornerW2;

        if (maxH == 0xff) {
            val = (((height - capH) << 8) / stepH + 0x80) >> 8;
            height = val * stepH + capH;
        }
    }

    if (width > 0x20) {
        width -= stepW;
    }

    if (height > 0x18) {
        height -= stepH;
    }

    return width | (height << 16);
}

// MATCHED 100%
void clMesWin::addTail(MesWinSlot * window) {
    if (window->params.tailKind == 0) {
        return;
    }

    MesWinParams * params = &window->params;

    u32 up = params->tailUp;

    u32 idx = (u8)((up != 0) ? 1 : 0);
    u32 kind = params->tailKind;
    idx += (kind-1)*2;

    u8 * row = &mesTailAnims[idx * 2];
    u8 anim = row[params->tailAlt];

    s16 x0 = params->x;
    s16 y0 = params->y;

    s16 x;
    s16 y;

    switch (kind) {
        case 1:
        case 2:
        case 5:
        case 6: {
            if (params->tailX != 0) {
                x = x0 + params->tailX;
            } else {
                x = x0 + ((params->width+1) * 8 >> 1);
            }

            if (up != 0) {
                y = y0;
            } else {
                y = y0 + ((params->height+1) * 8);
            }
            break;
        }
        case 3:
        case 4: {
            if (params->tailX != 0) {
                x = x0 + params->tailX + 1;
            } else {
                x = x0 + (((params->width+1) * 8) >> 1);
            }

            if (up != 0) {
                y = y0;
            } else {
                y = y0 + params->height*8 + 8;
            }
        }
    }

    this->addIcon(window, x, y, anim & ~0x80, ((anim & 0x80) ? 2 : 0) | params->tailFlipH, &window->tail);
}

// MATCHED 100%
void clMesWin::formatNumber(MesWinSlot * window, u8 slot, u8 digits, BOOL padded) {
    MesNumStr * num = &window->nums[slot];
    num->pos = num->buf;

    if (!num->ready) {
        num->ready = true;

        s32 value = window->params.numbers[slot];
        bool negative = true;
        if (value < 0) {
            value = value * (negative-2);
        } else {
            negative = false;
        }

        if (value > mesNumMax[digits]) {
            value = mesNumMax[digits];
        }

        u32 * place = &mesPow10[6 - (digits+1)];
        char * out = num->pos;
        bool started = false;

        while (*place != 0) {
            u8 digit = value / (*place);
            value -= digit * (*place);

            if (*place++ == 1 || digit != 0) {
                started = true;
                if (negative) {
                    negative = false;
                    *out++ = 0x2d;
                }
            } else if (started == false) {
                if (padded) {
                    *out++ = 0x20;
                }
                continue;
            }

            *out++ = digit + 0x30;
        }

        *out = 0;
    }
}

// NONMATCHING 84.01% -- else branch has function param setup interleaved ; (r3 < 0x100) checks don't output lo/hi instructions as expected
BOOL clMesWin::printNumber(MesWinSlot * window) {
    UnkStruct_0200b8bc_10 ** fonts;
    UnkStruct_0200b8bc_10 * font;
    UnkStruct_02013640 size;

    u8 slot = window->params.numbers == NULL ? 0 : window->numSlot - 1;

    u16 c = *(u8 *)window->nums[slot].pos;
    if (c == ' ') {
        u8 spaceW = window->text.field0c_4;

        window->text.field20_0 = window->text.field08_0;
        window->text.field20_10 = window->text.field08_10;

        if (window->text.field1c_22 == 0x3ff) {
            window->text.field1c_22 = window->text.field08_0;
        }

        window->text.field08_0 = spaceW + 1 + window->text.field1c_17 + window->text.field08_0;
        window->text.field20_20 = spaceW;
    } else {
        u8 single = this->singleBuf[window->params.screen];

        u8 local18 = window->text.field14_10;

        u8 r11 = window->text.field0c_0;
        u32 r14 = window->text.field1c_10;
        u16 fontIdx = window->text.field18_0;

        u32 * chars = (single == 0) ? window->charsBack : window->chars;

        u8 r2 = window->text.field14_0;
        u16 local1c = window->text.field0c_9;
        u16 r1 = window->text.field08_10;
        u16 r0 = window->text.field08_0;

        fonts = window->text.field10;

        u16 glyph = c;
        if (c < 0x100) {
            font = fonts[fontIdx * 2];
        } else {
            font = fonts[fontIdx * 2 + 1];
            glyph -= 0x100;
        }

        s32 gylphInfo = ((u16 *)PTR_10(font, font->field04))[glyph];

        if (c < 0x100) {
            font = fonts[fontIdx * 2];
        } else {
            font = fonts[fontIdx * 2 + 1];
        }

        u32 * glyphData = PTR_10(font, WORD_10(font, (u8)gylphInfo + 2));

        size.struct18.field00 = func_0200c330(r0, r1, r2, gylphInfo >> 8, local1c, (u32)chars, glyphData, r11, r14, local18);
        size.s20.field00 = size.struct18.field00;

        *(u16 *)size.struct18.field02 = size.struct18.field00;

        this->copyRect(window, window->text.field08_0, window->text.field08_10, size.s20.field00b, size.s20.field01b);

        this->windows[(s16)window->slot].dirty = 1;

        this->charsDirty = 1;

        window->text.field20_0 = window->text.field08_0;
        window->text.field20_10 = window->text.field08_10;

        if (window->text.field1c_22 == 0x3ff) {
            window->text.field1c_22 = window->text.field08_0;
        }

        window->text.field08_0 = size.s20.field00b + 1 + window->text.field1c_17 + window->text.field08_0;
        window->text.field20_20 = size.s20.field00b;
    }

    window->nums[slot].pos++;

    return (*(u8 *)window->nums[slot].pos == 0);
}

// MATCHED 100%
u32 clMesWin::callbackNumber(clMesWin * self, UnkStruct_0200b8bc * text, BOOL draw) {
    if (!draw) {
        return 0;
    }

    MesWinSlot * window = &self->windows[self->curSlot];

    u8 slot = text->field04[2];

    MesNumStr * num = &window->nums[slot];

    if (!num->ready) {
        self->formatNumber(window, slot, text->field04[3]-1, FALSE);
    }

    self->measureString(text, num);

    return 0;
}

// MATCHED 100%
u32 clMesWin::callbackNumberPadded(clMesWin * self, UnkStruct_0200b8bc * text, BOOL draw) {
    if (!draw) {
        return 0;
    }

    MesWinSlot * window = &self->windows[self->curSlot];

    u8 slot = text->field04[2];

    MesNumStr * num = &window->nums[slot];

    if (!num->ready) {
        self->formatNumber(window, slot, text->field04[3]-1, TRUE);
    }

    self->measureString(text, num);

    return 0;
}

// MATCHED 100%
void clMesWin::measureString(UnkStruct_0200b8bc * text, MesNumStr * num) {
    u16 glyph;
    UnkStruct_0200b8bc_10 * font;
    UnkStruct_0200b8bc_10 ** fonts;
    u8 size[2];
    UnkStruct_0200bce8_18 out;
    u8 c = num->buf[0];
    u8 * p = (u8 *)num->buf;

    while (c != '\0') {
        u16 ch = *p;
        p++;

        if (ch == ' ') {
            out.field02[0] = text->field0c_4;
        } else {
            fonts = text->field10;
            u16 fontIdx = text->field18_0;

            glyph = ch;
            if (ch < 0x100) {
                font = fonts[fontIdx * 2];
            } else {
                font = fonts[fontIdx * 2 + 1];
                glyph = ch - 0x100;
            }

            s32 glyphInfo = ((u16 *)PTR_10(font, font->field04))[glyph];
            u16 fontIdx2 = text->field18_0;
            UnkStruct_0200b8bc_10 * font2;
            if (ch < 0x100) {
                font2 = fonts[fontIdx2 * 2];
            } else {
                font2 = fonts[fontIdx2 * 2 + 1];
            }

            u32 * glyphData = PTR_10(font2, *(u32 *)((u8 *)font2 + ((glyphInfo & 0xff) << 2) + 8));
            u8 row = glyphInfo >> 8;

            u8 scale = text->field14_0;
            u8 word = (row >> 3) + 1;
            u8 shift = (row & 7) * 4;
            u32 header = glyphData[0];
            u32 width = (u8)((glyphData[word] >> shift) & 0xf);

            if (((header & 0xf0) >> 4) >= 5) {
                u8 word2 = word + ((header & 0xff000000) >> 25);
                width = (u8)(width | ((glyphData[word2] >> shift) << 4));
            }

            size[0] = width;
            size[1] = (header & 0xf) * 4 - 1;

            if (scale & 1) {
                size[0] = (size[0] + 1) * 2 - 1;
            }

            if (scale & 2) {
                size[1] = (size[1] + 1) * 2 - 1;
            }

            out.field00 = *(u16 *)size;
            *(u16 *)out.field02 = out.field00;
        }

        text->field20_0 = text->field08_0;
        text->field20_10 = text->field08_10;

        if (text->field1c_22 == 0x3ff) {
            text->field1c_22 = text->field08_0;
        }

        text->field08_0 = out.field02[0] + 1 + text->field1c_17 + text->field08_0;
        text->field20_20 = out.field02[0];

        c = *p;
    }

    out.field02[1] += 1;

    if (out.field02[1] > text->field1c_11) {
        text->field1c_11 = out.field02[1];
        text->unkStruct24.field28_16 = text->field08_10 - text->unkStruct24.field24_18 + text->field1c_11;
    }

    num->pos = num->buf;
}

// MATCHED 100%
void clMesIcon::update() {
    MesIcon * icon = this->list.next;
    while (icon->link.next != (MesIcon *)-1) {
        if (icon->hidden == 0) {
            if (--icon->timer == 0) {
                IconAnim * anims = (IconAnim *)RES_PTR(MES_ICON_RES, MES_ICON_RES->header.field04);
                IconAnim * anim = &anims[icon->anim];
                IconFrame * frames;
                u8 frame;
                u16 delay;

                if (++icon->frame == anim->count) {
                    icon->frame = 0;
                }
                frame = icon->frame;

                frames = (IconFrame *)RES_PTR(MES_ICON_RES, anim->frames);

                if (frame == 0) {
                    delay = frames[0].time;
                } else {
                    delay = frames[frame].time - frames[frame-1].time;
                }
                icon->timer = delay;
            }

            if (this->useTex == 0 || icon->screen == 1) {
                this->drawOam(icon);
            } else {
                this->drawTex(icon);
            }
        }

        icon = icon->link.next;
    }
}

// MATCHED 100%
void clMesIcon::setObjMan(BOOL sub, nsObjSys::clObjPltVramMan * oam, nsObjSys::clObjPltVramMan * chars, nsObjSys::clObjPltVramMan * pltt) {
    if (!sub) {
        this->objManMain[0] = oam;
        if (chars != NULL) {
            this->objManMain[1] = chars;
        }
        if (pltt != NULL) {
            this->objManMain[2] = pltt;
        }
    } else {
        this->objManSub[0] = oam;
        if (chars != NULL) {
            this->objManSub[1] = chars;
        }
        if (pltt != NULL) {
            this->objManSub[2] = pltt;
        }
    }
}

// MATCHED 100%
void clMesIcon::setTexMan(nsObjSys::clObjPltVramMan * chars, nsObjSys::clObjPltVramMan * pltt) {
    if (chars != NULL) {
        this->texMan[0] = chars;
    }

    if (pltt != NULL) {
        this->texMan[1] = pltt;
    }
}

// MATCHED 100%
void clMesIcon::loadChars(u32 screen, u32 addr) {
    if (this->useTex == FALSE || screen == 1) {
        this->loadObjChars(screen, addr);
    } else {
        this->loadTex(addr);
    }
}

// NONMATCHING 91.14 % -- r0 param is lifted above sp params in target asm
void clMesIcon::loadObjChars(u32 screen, u32 addr) {
    UnkStruct_02029b08 ** alloc = (screen == 0) ? &this->charAllocMan : &this->charAllocSub;

    if (addr == -1) {
        u32 size = this->getCharSize(screen);

        if (screen == 0) {
            *alloc = this->objManMain[1]->func_02014380(size, (u32)mesIconChars | 0x80000000, 5);
        } else {
            *alloc = this->objManSub[1]->func_02014380(size, (u32)mesIconChars | 0x40000000, 5);
        }
    }

    u32 entry = func_0202af48(screen, 0, 0, MES_ICON_RES);
    this->tileRes[screen] = RES_ENTRY(MES_ICON_RES, entry);

    if (addr == -1) {
        addr = (*alloc)->field08;
    }

    u16 shift = (*data_0204ac68[screen] & 0x300000) >> 0x14;

    u32 dst = addr + data_0204ac60[screen];

    func_0202d10c(dst, 0, MES_ICON_RES, mesIconChars, (u8 *)MES_ICON_RES + this->tileRes[screen]->offset, shift+5, 0, 0, 0);
}

// MATCHED 100%
void func_02014070(MesFrameEntry * param1, u32 param2, u32 param3, u32 param4) {
    u32 base = (1 << data_0205c25c[param1->maxH]);
    func_02028950(param1, MATH_ROUNDUP(param2, base), param3, param4);
}

// MATCHED 100%
void clMesIcon::loadTex(u32 addr) {
    if (addr == -1) {
        u32 size = func_0202afbc(0, 1, 0, MES_ICON_RES);
        this->charAllocMan = this->texMan[0]->func_02014380(size, (u32)mesIconChars, 5);
    }

    u32 entry = func_0202af48(0, 1, 0, MES_ICON_RES);
    this->tileRes[0] = RES_ENTRY(MES_ICON_RES, entry);

    GXVRamTex bank = GX_GetBankForTex();
    u32 * p = texBankAddrs;
    if (bank & GX_VRAM_TEX_0_A) {
        *p++ = HW_LCDC_VRAM_A;
    }
    if (bank & GX_VRAM_TEX_0_B) {
        *p++ = HW_LCDC_VRAM_B;
    }
    if (bank & GX_VRAM_TEX_0_C) {
        *p++ = HW_LCDC_VRAM_C;
    }
    if (bank & GX_VRAM_TEX_0_D) {
        *p = HW_LCDC_VRAM_D;
    }

    GXVRamTex saved = GX_ResetBankForTex();

    u32 dst;
    if (addr == -1) {
        dst = (this->charAllocMan->field08 & 0x1FFFF) + texBankAddrs[this->charAllocMan->field08 >> 17];
    } else {
        dst = (addr & 0x1FFFF) + texBankAddrs[addr >> 17];
    }
    func_0202d10c(dst, 1, MES_ICON_RES, mesIconChars, (u8 *)MES_ICON_RES + this->tileRes[0]->offset, 3, 0, 0, 0);

    GX_SetBankForTex(saved);
}

// MATCHED 100%
u32 clMesIcon::getCharSize(u32 screen) {
    return func_0202afbc(screen, 0, 0, MES_ICON_RES);
}

// MATCHED 100%
void clMesIcon::loadPltt(u32 screen, u32 addr) {
    if (this->useTex == FALSE || screen == 1) {
        this->loadObjPltt(screen, addr);
        this->plttIdx[0] = 0xff;
        this->plttIdx[1] = 0xff;
    } else {
        this->loadTexPltt(addr);
        this->texPlttAddr = -1;
    }
}

// MATCHED 100%
void clMesIcon::loadObjPltt(u32 screen, u32 slot) {
    UnkStruct_02029b08 ** alloc = (screen == 0) ? &this->plttAllocMain : &this->plttAllocSub;

    if (*alloc == NULL) {
        if (slot == 0) {
            slot = 0x104;
        }

        u32 size = func_0202b170(MES_ICON_RES, &mesIconPlttSize);
        if (screen == 0) {
            *alloc = this->objManMain[2]->func_02014380(size, (u32)&mesIconPlttSize, slot);
        } else {
            *alloc = this->objManSub[2]->func_02014380(size, (u32)&mesIconPlttSize, slot);
        }

        UnkStruct_02029b08 * a = *alloc;

        void * src = mesIconPltt;
        u32 bytes = a->field0c * 32;
        u32 offset = (a->field08 & 0xff) * 32;

        if (screen == 0) {
            if ((a->field08 & 0x100) == 0) {
                GX_BeginLoadOBJExtPltt();
                GX_LoadOBJExtPltt(src, offset, bytes);
                GX_EndLoadOBJExtPltt();
            } else {
                GX_LoadOBJPltt(src, offset, bytes);
            }
        } else {
            if ((a->field08 & 0x100) == 0) {
                GXS_BeginLoadOBJExtPltt();
                GXS_LoadOBJExtPltt(src, offset, bytes);
                GXS_EndLoadOBJExtPltt();
            } else {
                GXS_LoadOBJPltt(src, offset, bytes);
            }
        }
    }
}

// MATCHED 100%
UnkStruct_02029b08 * nsObjSys::clObjPltVramMan::func_02014380(u32 param1, u32 param2, u32 param3) {
    u32 p3 = param3 & ~0x100;

    if (param3 & 0x100) {
        return this->func_02029c9c(param1, param2, p3);
    } else {
        return this->func_02029b08(param1, param2, p3);
    }
}

// MATCHED 100%
void clMesIcon::loadTexPltt(u32 slot) {
    if (this->plttAllocMain == NULL) {
        if (slot == 0) { // TODO: Check for a Enum or macro value replacement
            slot = 0;
        }

        UnkStruct_02029b08 * alloc = this->texMan[1]->func_02014380(0x20, NULL, slot);
        this->plttAllocMain = alloc;

        u32 offset = alloc->field08;
        u32 bytes = alloc->field0c;

        GX_BeginLoadTexPltt();
        GX_LoadTexPltt(mesIconPltt, offset, bytes);
        GX_EndLoadTexPltt();
    }
}

// MATCHED 100%
void clMesIcon::add(MesIcon * icon, u8 screen, s8 anim, u16 x, u16 y, u8 flip, u8 priority, s32 depth) {
    cpu_clear32(0, icon, sizeof(MesIcon));

    icon->active = 1;
    icon->screen = screen;
    icon->x = x;
    icon->y = y;
    icon->flip = flip;
    icon->priority = priority;

    icon->anim = anim;

    u32 frame = icon->frame;

    u8 * base = (u8 *)MES_ICON_RES;
    IconAnim * anims = (IconAnim *)(base + MES_ICON_RES->header.field04);
    IconFrame * frames = (IconFrame *)(base + anims[icon->anim].frames);

    u16 delay;
    if (frame == 0) {
        delay = frames[0].time;
    } else {
        delay = frames[frame].time - frames[frame-1].time;
    }

    icon->timer = delay;
    icon->depth = depth;

    this->listEnd.prev->link.next = icon;
    icon->link.prev = this->listEnd.prev;
    icon->link.next = (MesIcon *)&this->listEnd;
    this->listEnd.prev = icon;
}

// MATCHED 100%
void clMesIcon::freeChars(BOOL sub) {
    if (!sub) {
        if (!this->useTex) {
            if (this->charAllocMan != NULL) {
                this->objManMain[1]->func_0202a134(this->charAllocMan, 0, -1);
                this->charAllocMan = NULL;
            }
        } else {
            if (this->charAllocMan != NULL) {
                this->texMan[0]->func_0202a134(this->charAllocMan, 0, -1);
                this->charAllocMan = NULL;
            }
        }
    } else {
        if (this->charAllocSub != NULL) {
            this->objManSub[1]->func_0202a134(this->charAllocSub, 0, -1);
            this->charAllocSub = NULL;
        }
    }
}

// MATCHED 100%
void clMesIcon::freePltt(BOOL sub) {
    if (!sub) {
        if (!this->useTex) {
            if (this->plttAllocMain != NULL) {
                this->objManMain[2]->func_0202a134(this->plttAllocMain, 0, -1);
                this->plttAllocMain = NULL;
            }
        } else {
            if (this->plttAllocMain != NULL) {
                this->texMan[1]->func_0202a134(this->plttAllocMain, 0, -1);
                this->plttAllocMain = NULL;
            }
        }
    } else {
        if (this->plttAllocSub != NULL) {
            this->objManSub[2]->func_0202a134(this->plttAllocSub, 0, -1);
            this->plttAllocSub = NULL;
        }
    }
}

// MATCHED 100%
clMesIcon::clMesIcon(BOOL useTex) {
    func_01ff8228(this->objManMain, 0, sizeof(this->objManMain));
    func_01ff8228(this->objManSub, 0, sizeof(this->objManSub));
    func_01ff8228(this->texMan, 0, sizeof(this->texMan));

    this->useTex = FALSE;

    if (useTex) {
        this->useTex = TRUE;
    }

    this->charAllocMan = NULL;
    this->charAllocSub = NULL;
    this->plttAllocMain = NULL;
    this->plttAllocSub = NULL;

    this->tileRes[0] = NULL;
    this->tileRes[1] = NULL;

    this->list.prev = (MesIcon *)0x11111111;
    this->list.next = (MesIcon *)&this->listEnd;
    this->listEnd.prev = (MesIcon *)&this->list;
    this->listEnd.next = (MesIcon *)-1;

    this->scale = 1;

    this->plttIdx[0] = 0xff;
    this->plttIdx[1] = 0xff;

    this->texPlttAddr = -1;
}

// MATCHED 100%
clMesIcon::~clMesIcon() {
    this->freeChars(FALSE);
    this->freeChars(TRUE);

    this->freePltt(FALSE);
    this->freePltt(TRUE);

    this->charAllocMan = NULL;
    this->charAllocSub = NULL;
    this->plttAllocMain = NULL;
    this->plttAllocSub = NULL;
}

// NONMATCHING 98.11 % -- register rotation with r1/r2/r3
void clMesIcon::drawOam(MesIcon * icon) {
    u8 * base = (u8 *)MES_ICON_RES;

    IconAnim * anims = (IconAnim *)RES_PTR(base, MES_ICON_RES->header.field04);
    IconFrame * frame = (IconFrame *)RES_PTR(base, anims[icon->anim].frames) + icon->frame;

    IconObj * obj = (IconObj *)RES_PTR(base, MES_ICON_RES->header.field08) + frame->cell;

    IconOffset * offsets = (IconOffset *)RES_PTR(base, MES_ICON_RES->header.field10);
    IconOffset * offset = &offsets[frame->offsetIdx-1];

    UnkStruct_02029b08 * charAlloc;
    UnkStruct_02029b08 * plttAlloc;
    nsObjSys::clObjPltVramMan * oamManager;

    if (icon->screen == 0) {
        charAlloc = this->charAllocMan;
        plttAlloc = this->plttAllocMain;
        oamManager = this->objManMain[0];
    } else {
        charAlloc = this->charAllocSub;
        plttAlloc = this->plttAllocSub;
        oamManager = this->objManSub[0];
    }

    if (this->tileRes[icon->screen] == NULL) {
        this->tileRes[icon->screen] = RES_ENTRY(MES_ICON_RES, func_0202af48(icon->screen, 0, 0, MES_ICON_RES));
    }

    u32 screen = icon->screen;
    u16 * tiles = (u16 *)RES_PTR(base, this->tileRes[screen]->offset) + frame->cell;
    GXOamAttr * oam = oamManager->field18;

    u32 charBase;
    if (charAlloc != NULL) {
        u16 mapShift = (*data_0204ac68[screen] & 0x300000) >> 0x14;
        charBase = charAlloc->field08 >> (mapShift + 5);
    } else {
        u16 mapShift = (*data_0204ac68[screen] & 0x300000) >> 0x14;
        charBase = this->charBase[screen] >> (mapShift + 5);
    }

    u16 plttBase;
    if (plttAlloc != NULL) {
        if ((u16)plttAlloc->field08 & 0x100) {
            plttBase = (u8)plttAlloc->field08;
        } else {
            plttBase = plttAlloc->field08 >> 4;
        }
    } else {
        plttBase = this->plttIdx[screen];
    }

    s32 i;
    s16 x;
    s16 y;
    for (i = frame->numObj; i > 0; i--) {
        u32 size = obj->size;
        u32 shape = obj->shape;

        u8 shapeIdx = size + shape * 4;
        s32 w = iconObjWidths[shapeIdx];
        s32 h = iconObjHeights[shapeIdx];

        x = obj->x - (w >> 1);
        y = obj->y - (h >> 1);

        u8 flip = obj->flip;

        if (frame->offsetIdx != 0) {
            x += offset->x;
            y += offset->y;
        }

        if (icon->flip) {
            if (icon->flip & 1) {
                x = -(x + w);
                flip ^= 0x8;
            }

            if (icon->flip & 2) {
                y = -(y + h);
                flip ^= 0x10;
            }
        }

        x += icon->x;
        y += icon->y;

        oam->attr01 = (y & 0xff) | (obj->mode << 8) | (obj->mosaic << 12) | (obj->color << 13) | (shape << 14) | ((x << 16) & 0x1ff0000) | (flip << 25) | (size << 30);
        oam->attr2 = (*tiles++ + charBase) | (icon->priority << 10) | ((plttBase + obj->pltt) << 12);
        
        obj++;
        oam++;
    }

    oamManager->func_02014380(oamManager->field18 - oamManager->field04, frame->numObj, 0x7fffffff);

    oamManager->field18 += frame->numObj;
}

// MATCHED 100%
void clMesIcon::drawTex(MesIcon * icon) {
    u8 * base = (u8 *)MES_ICON_RES;
    IconAnim * anims = (IconAnim *)RES_PTR(base, MES_ICON_RES->header.field04);
    IconFrame * frame = (IconFrame *)RES_PTR(base, anims[icon->anim].frames) + icon->frame;
    IconObj * obj = (IconObj *)RES_PTR(base, MES_ICON_RES->header.field08) + frame->cell;
    IconOffset * offsets = (IconOffset *)RES_PTR(base, MES_ICON_RES->header.field10);

    IconOffset * offset = &offsets[frame->offsetIdx-1];

    if (this->tileRes[0] == NULL) {
        this->tileRes[0] = RES_ENTRY(MES_ICON_RES, func_0202af48(icon->screen, 0, 0, MES_ICON_RES));
    }

    u16 * tiles = (u16 *)RES_PTR(base, this->tileRes[0]->offset) + frame->cell;
    u32 pltt;
    if (this->plttAllocMain != NULL) {
        pltt = this->plttAllocMain->field08;
    } else {
        pltt = this->texPlttAddr;
    }

    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PolygonAttr(0, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 31, 0);

    u32 texOff;
    u32 fmt = ((YanaRes *)base)->header.field02_0;
    if (this->charAllocMan != NULL) {
        texOff = this->charAllocMan->field08 >> 3;
    } else {
        texOff = this->charBase[0] >> 3;
    }

    u32 texParam = texOff | (fmt << 26);
    s32 i;
    s32 x;
    s32 y;
    for (i = frame->numObj; i > 0; i--) {
        x = obj->x;
        y = obj->y;

        if (frame->offsetIdx != 0) {
            x += offset->x;
            y += offset->y;
        }

        u32 flip = icon->flip;
        if (flip & 1) {
            x *= -1;
        }
        if (flip & 2) {
            y *= -1;
        }

        u32 attr = *(u32 *)obj;
        u32 shapeIdx = (((attr >> 6) & 3) << 2) | ((attr >> 10) & 3);
        G3_Direct1(G3OP_TEXIMAGE_PARAM, ((texParam | iconTexSizeParams[shapeIdx]) + *tiles) | (GX_TEXPLTTCOLOR0_TRNS << REG_G3_TEXIMAGE_PARAM_TR_SHIFT) | (GX_TEXGEN_TEXCOORD << REG_G3_TEXIMAGE_PARAM_TGEN_SHIFT));
        G3_TexPlttBase(pltt, GX_TEXFMT_PLTT16);
        G3_PushMtx();

        x += icon->x;
        y += icon->y;

        G3_Translate((x << 5) * this->scale, (y << 5) * this->scale, icon->depth * this->scale);
        G3_Scale(this->scale << FX32_SHIFT, this->scale << FX32_SHIFT, this->scale << FX32_SHIFT);
        
        const u32 * vtx = iconQuadVtx[shapeIdx];
        const u32 * tc = iconTexCoords[shapeIdx].field00[icon->flip];
        G3_Begin(GX_BEGIN_QUADS);
        G3_Color(GX_RGB(31, 31, 31));
        G3_Direct1(G3OP_TEXCOORD, tc[0]);
        G3_Direct2(G3OP_VTX_16, vtx[0], 0);
        G3_Direct1(G3OP_TEXCOORD, tc[1]);
        G3_Direct1(G3OP_VTX_XY, vtx[1]);
        G3_Direct1(G3OP_TEXCOORD, tc[2]);
        G3_Direct1(G3OP_VTX_XY, vtx[2]);
        G3_Direct1(G3OP_TEXCOORD, tc[3]);
        G3_Direct1(G3OP_VTX_XY, vtx[3]);
        G3_End();
        G3_PopMtx(1);

        obj++;
        tiles++;
    }

    G3_PopMtx(1);
}
