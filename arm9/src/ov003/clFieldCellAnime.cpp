// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clFieldCellAnime.h"

extern "C" {

#include <nitro.h>

// --- Variable Definitions --- //

#include "data/clFieldCellAnime.h"

CellAnimeList parkedCellAnimeList;  // 0x02087280

// --- Function Declarations --- //

void parkRange(nsObjSys::clCellAnime *, nsObjSys::clCellAnime *);  // 0x0206a178
void queueLoads(nsObjSys::clCellAnime *);  // 0x0206a424

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
clFieldCellAnime::clFieldCellAnime() {
    this->ofsZ = 0x80000000;
    this->basePriorityW = 0;
    this->fieldc0 = 0;
    this->rot = 0;
    this->scaleX = this->scaleY = 0x1000;
    this->ofsX = this->ofsY = 0;

    this->fieldc0_7 = 1;
    this->usePlttBuf = 0;
}

// MATCHED 100%
clFieldCellAnime::~clFieldCellAnime() {}

// MATCHED 100%
void clFieldCellAnime::reset() {
    this->ofsZ = 0x80000000;
    this->basePriorityW = 0;
    this->fieldc0 = 0;
    this->rot = 0;
    this->scaleX = this->scaleY = 0x1000;

    this->fieldc0_7 = 1;
    this->usePlttBuf = 0;
}

// MATCHED 100%
void clFieldCellAnime::updateXform() {
    if (this->usePlttBuf) {
        getPlttBuf();
    }

    if (this->mtxDirty) {
        s32 sx = this->flipX ? -this->scaleX : this->scaleX;
        s32 sy = this->flipY ? -this->scaleY : this->scaleY;

        fx16 cosVal = FX_CosIdx(this->rot);
        fx16 sinVal = FX_SinIdx(this->rot);

        this->mtx._00 = (sx * cosVal) >> FX32_SHIFT;
        this->mtx._01 = (sx * sinVal) >> FX32_SHIFT;
        this->mtx._10 = (-sy * sinVal) >> FX32_SHIFT;
        this->mtx._11 = (sy * cosVal) >> FX32_SHIFT;

        this->mtxDirty = 0;
    }

    this->mtx._30 += this->ofsX;
    this->mtx._31 += this->ofsY;
    this->mtx._32 += this->ofsZ;

    this->priorityW = this->basePriorityW;

    if (this->charHandle != NULL && this->charHandle->movedBit) {
        if (this->visible) {
            this->loadedCell = -1;
        }

        this->charOffset = this->charHandle->offset;
        this->charHandle->movedBit = 0;
    }
}

// MATCHED 100%
FieldAnimeRes * getFieldAnimeRes(u32 id) {
    switch (id & 0xff000000) {
        case 0: {
            return &fieldAnimeRes0[id & 0xffffff];
        }
        case 0x1000000: {
            return &fieldAnimeRes1[id & 0xffffff];
        }
        case 0x2000000: {
            return &fieldAnimeRes2[id & 0xffffff];
        }
        case 0x3000000: {
            return &fieldAnimeRes3[id & 0xffffff];
        }
    }
    return NULL;
}

// MATCHED 100%
u32 * func_ov003_0206a0e8(u32 id) {
    switch (id & 0xff000000) {
        case 0: {
            return &data_ov003_0207e634[id & 0xffffff];
        }
        case 0x1000000: {
            return &data_ov003_0207ee28[id & 0xffffff];
        }
        case 0x2000000: {
            return &data_ov003_0207ea2c[id & 0xffffff];
        }
        case 0x3000000: {
            return &data_ov003_0207e5e0[id & 0xffffff];
        }
    }
    return NULL;
}

// MATCHED 100%
void parkRange(nsObjSys::clCellAnime * first, nsObjSys::clCellAnime * end) {
    if (cellAnimeList.head == first) {
        cellAnimeList.head = end;
    }

    if (parkedCellAnimeList.tail != NULL) {
        parkedCellAnimeList.tail->next = first;
    } else {
        parkedCellAnimeList.head = first;
    }

    if (first->prev != NULL) {
        first->prev->next = end;
    }

    if (end != NULL) {
        nsObjSys::clCellAnime * prev = first->prev;

        first->prev = parkedCellAnimeList.tail;

        parkedCellAnimeList.tail = end->prev;
        parkedCellAnimeList.tail->next = NULL;

        end->prev = prev;
    } else {
        nsObjSys::clCellAnime * prev = first->prev;

        first->prev = parkedCellAnimeList.tail;

        parkedCellAnimeList.tail = cellAnimeList.tail;

        if (prev != NULL) {
            cellAnimeList.tail = prev;
            prev->next = NULL;
        } else {
            cellAnimeList.tail = NULL;
        }
    }
}

// MATCHED 100%
void parkScreen(u32 screen) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);

    if (screen < 2) {
        nsObjSys::clCellAnime * cur = cellAnimeList.head;

        do {
            nsObjSys::clCellAnime * first = NULL;

            while (cur != NULL) {
                if (cur->screen == screen) {
                    first = cur;
                    cur = cur->next;
                    break;
                }

                cur = cur->next;
            }

            if (first != NULL) {
                nsObjSys::clCellAnime * end = NULL;

                while (cur != NULL) {
                    if (cur->screen != screen) {
                        end = cur;
                        cur = cur->next;
                        break;
                    }

                    cur = cur->next;
                }

                parkRange(first, end);
            }
        } while (cur != NULL);
    } else {
        parkedCellAnimeList.head = cellAnimeList.head;
        parkedCellAnimeList.tail = cellAnimeList.tail;

        cellAnimeList.head = NULL;
        cellAnimeList.tail = NULL;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void parkCount(nsObjSys::clCellAnime * first, u32 count) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);

    nsObjSys::clCellAnime * cur = first;
    while (cur != 0) {
        if (count == 1) {
            parkRange(first, cur->next);
            break;
        }

        cur = cur->next;
        count--;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void unparkAll() {
    if (parkedCellAnimeList.head != NULL) {
        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT | OS_IE_V_BLANK);

        if (cellAnimeList.tail != NULL) {
            cellAnimeList.head->prev = parkedCellAnimeList.tail;
            parkedCellAnimeList.tail->next = cellAnimeList.head;
        } else {
            cellAnimeList.tail = parkedCellAnimeList.tail;
        }

        cellAnimeList.head = parkedCellAnimeList.head;

        parkedCellAnimeList.head = NULL;
        parkedCellAnimeList.tail = NULL;

        OS_EnableIrqMask(mask);
    }
}

// MATCHED 100%
void flushLoads(u32 screen, u32 perFrame, nsObjSys::clCellAnime * head) {
    u32 modeMask = 0;

    while (head != NULL) {
        if (head->screen == screen && head->perFrame == perFrame) {
            queueLoads(head);
            modeMask |= (1 << head->mode);
        }

        head = head->next;
    }

    flushCharLoads(modeMask);
    flushPlttLoads(modeMask);
}

// MATCHED 100%
void queueLoads(nsObjSys::clCellAnime * anim) {
    if (anim->charDirty && anim->charData != NULL && anim->needsCharLoad()) {
        queuePush(anim, charLoadQueue[anim->mode], anim->nextCharLoad, 0);

        if (anim->charHandle != NULL) {
            anim->charHandle->queued = 1;
        }
    }

    if (anim->plttDirty && anim->plttData != NULL) {
        queuePush(anim, plttLoadQueue[anim->mode], anim->nextPlttLoad, 0);
    }
}

// MATCHED 100%
void clFieldCellAnime::initAlloc(u32 mode, u32 perFrame, nsObjSys::clVramManBase * charMan, nsObjSys::clVramManBase * plttMan, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    this->initAllocEx(mode, perFrame, 1, charMan, plttMan, res, charData, plttData, pafData);
}

// MATCHED 100%
void clFieldCellAnime::initAllocFront(u32 mode, nsObjSys::clVramManBase * charMan, nsObjSys::clVramManBase * plttMan, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    this->initAllocEx(mode, 0, 0, charMan, plttMan, res, charData, plttData, pafData);
}

// MATCHED 100%
void clFieldCellAnime::initAllocEx(u32 mode, u32 perFrame, u32 side, nsObjSys::clVramManBase * charMan, nsObjSys::clVramManBase * plttMan, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    u32 charAttr = (perFrame != 0) ? 2 : (side | 4);

    u32 charSize = getResCharSize(charMan->manBank, mode, perFrame, res);
    VramHandle * charHandle = charMan->alloc(charSize, (u32)charData, charAttr);

    u16 plttAttr = (pafData != NULL) ? 0 : 4;
    u32 plttSize;

    if (mode == 0) {
        plttSize = getObjPlttUnits(res, plttData);
        plttAttr |= (usesExtPltt(plttMan->manBank, res)) ? 0x100 : 0;
    } else {
        plttSize = getTexPlttSize(res, plttData);
    }

    VramHandle * plttHandle = plttMan->alloc(plttSize, (u32)plttData, plttAttr);

    this->setup(charHandle->manBank, mode, perFrame, charHandle, charHandle->offset, plttHandle, plttHandle->offset, res, charData, plttData, pafData);

    this->ownsChar = 1;
    this->ownsPltt = 1;

    this->state = 1;
}

// MATCHED 100%
void clFieldCellAnime::copyState(clFieldCellAnime * src) {
    this->func_ov003_0206a720(src->getAnim(), src->getFrame());

    this->ofsX = src->ofsX;
    this->ofsY = src->ofsY;

    this->rot = src->rot;
    this->scaleX = src->scaleX;
    this->scaleY = src->scaleY;

    this->fieldc0_6 = src->fieldc0_6;
}

// MATCHED 100%
void clFieldCellAnime::func_ov003_0206a720(s16 anim, s16 frame) {
    this->nsObjSys::clCellAnime::setAnim(anim, frame);
}

// MATCHED 100%
clFieldSysCellAnime::clFieldSysCellAnime() {
    this->lifeTimer = 0;

    this->hideReq = 0;
    this->fieldda_1 = 0;
    this->releaseOnHide = 0;
}

// MATCHED 100%
clFieldSysCellAnime::~clFieldSysCellAnime() {}

// MATCHED 100%
void clFieldSysCellAnime::reset() {
    this->clFieldCellAnime::reset();

    this->lifeTimer = 0;

    this->hideReq = 0;
    this->fieldda_1 = 0;
    this->releaseOnHide = 0;
}

// MATCHED 100%
void clFieldSysCellAnime::updateXform() {
    if (this->visible == 0) {
        if (this->releaseOnHide) {
            this->release();
        }
        return;
    }

    if (this->hideReq) {
        this->visible = 0;

        if (this->releaseOnHide) {
            this->release();
            return;
        }
    }

    this->clFieldCellAnime::updateXform();

    if (this->lifeTimer != 0 && --this->lifeTimer == 0) {
        this->hideReq = 1;
    }
}

// MATCHED 100%
void clFieldCellAnime::update() {
    u16 * pafData = this->pafData;
    this->pafData = NULL;
    this->prevEnded = this->ended;
    this->nsObjSys::clCellAnime::update();
    this->pafData = pafData; 
}

// MATCHED 100%
u32 clFieldCellAnime::getPolygonAttrMisc() {
    return 0x1000;
}

// MATCHED 100%
void clFieldCellAnime::setAllPriority(u32 priority) {
    this->setAllBasePriority(priority);
}

// MATCHED 100%
void clFieldCellAnime::setAllBasePriority(u32 priority) {
    this->basePriorityW = (priority << 24) | (priority << 16) | (priority << 8) | priority;
}

// MATCHED 100%
void clFieldCellAnime::setPriority(u32 idx, u8 priority) {
    this->setBasePriority(idx, priority);
}

// MATCHED 100%
void clFieldCellAnime::setBasePriority(u32 idx, u8 priority) {
    this->basePriority[idx] = priority;
}

// MATCHED 100%
void clFieldCellAnime::setBasePriorityWord(u32 priorities) {
    this->basePriorityW = priorities;
}

// MATCHED 100%
void clFieldCellAnime::setAlpha(u32 alpha) {
    this->alpha = alpha;
}

// MATCHED 100%
void clFieldCellAnime::initWithHandles(VramHandle * charHandle, VramHandle * plttHandle, u32 perFrame, YanaRes * res, u32 * charData, u32 * plttData, u16 * pafData) {
    this->nsObjSys::clCellAnime::initWithHandles(charHandle, plttHandle, perFrame, res, charData, plttData, pafData);
}
