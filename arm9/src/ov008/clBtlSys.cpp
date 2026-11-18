// objdiff-target: build/delinks/_dsd_gap@ov008_2.o

#include "ov008/clBtlSys.h"

#include "main/clCellAnime.h"
#include "main/clGameMain.h"
#include "main/clVCountTask.h"
#include "main/heap.h"
#include "main/sndEmitter.h"
#include "main/softReset.h"
#include "main/sound.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"

#include "main/system.h"
#include "main/touchpad.h"

// --- Variable Definitions --- //

const char * btlSysName = "clBtlSys";    // 0x02064d80
clBtlSys * btlSys;  // 0x02064f04
clBtlMainSys * btlMainSys;  // ov008 0x02064f0c

// --- Function Declarations --- //

void btlOvyQueueJob(BtlJob *);  // 0x020649c0
void btlRestartSoundJob(BtlJob *);  // 0x02064b54

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
clBtlSys::clBtlSys(u16 reqCount, u16 numSlots) : clFSEx(200, (u32)btlSysName, reqCount, numSlots, -1, 0) {
    btlSys = this;

    cpu_fill32(&this->frameFn, 0, (u32)(this+1) - (u32)&this->frameFn);

    OS_DisableIrqMask(OS_IE_V_COUNT);
    OS_EnableIrqMask(OS_IE_V_BLANK);

    this->vblankDone = 1;

    this->vblankTask = new(HEAP_MAIN_HI, NULL, FALSE) clBtlSysVBlank(8, 0, this);
    this->vblankTask->enable();

    this->texTrans.init(1, TEX_TRANS_ENTRY_COUNT, this->texTransEntries, 1);
    this->vblankTrans.init(0, VBLANK_TRANS_ENTRY_COUNT, this->vblankTransEntries, 1);
    this->proTrans.init(0, PRO_TRANS_ENTRY_COUNT, this->proTransEntries, 1);
}

// MATCHED 100%
clBtlProTransMan::~clBtlProTransMan() {}

// MATCHED 100%
clBtlSys::~clBtlSys() {
    btlSys = NULL;

    deleteVBlankTask(this->vblankTask);

    this->vblankTask = NULL;

    initVCountTaskMgmt();

    OS_DisableIrqMask(OS_IE_V_BLANK);
    OS_EnableIrqMask(OS_IE_V_COUNT);

    while (this->field1cec != 0) {
        this->field1cec--;

        s16 slot = this->field1cec;
        u32 word = slot >> 5;
        u32 bit = slot & 0x1f;

        if (this->externTable[word] & (1 << bit)) {
            this->releaseSlot(slot);
            this->externTable[word] &= ~(1 << bit);
        } else {
            heapFree(this->releaseSlot(slot));
        }
    }
}

// MATCHED 100%
void btlDisableVramBanks() {
    GX_DisableBankForBG();
    GX_DisableBankForOBJ();
    GX_DisableBankForTex();
    GX_DisableBankForTexPltt();
    GX_DisableBankForSubBG();
    GX_DisableBankForSubOBJ();
    GX_DisableBankForLCDC();
    GX_DisableBankForBGExtPltt();
    GX_DisableBankForOBJExtPltt();
    GX_DisableBankForSubBGExtPltt();
    GX_DisableBankForSubOBJExtPltt();
}

// MATCHED 100%
void clBtlSys::init3D() {
    G3X_SetClearColor(0, 0, 0x7fff, 0x3f, FALSE);
    G3X_SetShading(GX_SHADING_TOON);
    G3X_AntiAlias(TRUE);
    G3X_AlphaBlend(TRUE);
    G3X_AlphaTest(FALSE, 0);
    G3X_EdgeMarking(FALSE);
    G3X_SetFog(FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0);
    G3_ViewPort(0, 0, 0xff, 0xbf);

    setupOrtho(8);

    G3X_Reset();
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_Scale(0x8000, 0x8000, 0x8000);

    this->use3D = 1;
}

// MATCHED 100%
void clBtlSys::run() {
    if (btlSys != NULL) {
        readKeypad();

        if (systemState.touchpad.sampling) {
            readTouchpad(&systemState.touchpad);
        }

        if (this->use3D) {
            G3X_Reset();
            G3_MtxMode(GX_MTXMODE_POSITION);
            G3_Scale(0x8000, 0x8000, 0x8000);
        }

        this->proTrans.flush();

        if (this->frameFn != NULL) {
            this->frameFn();
        }

        this->frameCt++;

        void (*exitFn)(void) = this->exitFn;
        if (exitFn != NULL) {
            delete this;
            exitFn();
            return;
        }

        if (this->use3D) {
            G3_SwapBuffers(GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z);
        }

        this->vblankDone = 0;

        while (TRUE) {
            BOOL flag = (ovyState == 0);

            if (flag == FALSE) {
                this->pumpOverlay();
                this->startOverlay();
            } else if (!(flag = (this->tableReqHead == NULL)) != FALSE) {
                this->pumpTableLoads();
            } else if (!(flag = (this->reqHead == NULL)) != FALSE) {
                this->pumpReads();
            } else {
                u16 head = this->jobHead;

                if (head != this->jobTail) {
                    BtlJob * job = &this->jobs[head];

                    if (job->fn != NULL) {
                        job->fn(job);
                    }

                    if (job->fn == NULL) {
                        if (head+1 == BTL_JOB_COUNT) {
                            this->jobHead = 0;
                        } else {
                            this->jobHead = head+1;
                        }
                    }
                } else {
                    waitVBlankSync();
                }
            }

            if (softResetState.active != 0) {
                return;
            }

            if (this->vblankDone) {
                BOOL retry = FALSE;

                if (this->texTrans.flushBlocked) {
                    s32 vcount = GX_GetVCount();
                    if (vcount <= HW_LCD_HEIGHT-1 || vcount >= 210) {
                        retry = TRUE;
                    } else {
                        BOOL ime = OS_DisableIrq();
                        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);
                        if (ime) {
                            OS_EnableIrq();
                        }

                        this->inTransfer = 1;
                        this->texTrans.flush();
                        this->inTransfer = 0;

                        ime = OS_DisableIrq();
                        OS_EnableIrqMask(mask);
                        if (ime) {
                            OS_EnableIrq();
                        }
                    }
                }

                if (retry == FALSE && this->vblankTrans.flushBlocked) {
                    if (GX_GetVCount() <= HW_LCD_HEIGHT-1) {
                        retry = TRUE;
                    } else {
                        BOOL ime = OS_DisableIrq();
                        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);
                        if (ime) {
                            OS_EnableIrq();
                        }

                        this->inTransfer = 1;
                        this->vblankTrans.flush();
                        this->inTransfer = 0;

                        ime = OS_DisableIrq();
                        OS_EnableIrqMask(mask);
                        if (ime) {
                            OS_EnableIrq();
                        }
                    }
                }

                if (retry == FALSE) {
                    break;
                }

                this->vblankDone = 0;
            }
        }
    }
}

// MATCHED 100%
void clBtlSysVBlank::run() {
    if (btlSys != NULL) {
        ((clBtlSys *)this->field04)->onVBlank();
    }
}

// MATCHED 100%
void clBtlSys::onVBlank() {
    if (this->vblankDone == 0) {
        this->vblankDone = 1;

        if (this->field1cee_1 == 0) {
            this->inTransfer = 1;

            this->vblankFlush();

            if (this->texTrans.flushBlocked) {
                this->vblankTrans.flushBlocked = 1;
            } else {
                this->vblankTrans.flush();
            }

            this->inTransfer = 0;
        }
    }
}

// MATCHED 100%
void clBtlSys::vblankFlush() {
    this->texTrans.flush();
    flushCharLoads(2);
    flushPlttLoads(2);
}

// MATCHED 100%
BtlJob * clBtlSys::queueJob(void (*fn)(BtlJob *), u32 arg0, u32 arg1, u32 arg2) {
    s32 tail = this->jobTail;

    BtlJob * job = &this->jobs[tail];
    job->fn = fn;
    job->arg0 = arg0;
    job->arg1 = arg1;
    job->arg2 = arg2;

    this->jobTail = (tail < BTL_JOB_COUNT-1) ? tail+1 : 0;

    return job;
}

// MATCHED 100%
u8 * clBtlSys::readChunkWait(s16 slot, u16 chunkIdx, u8 * dst, s32 * pSize, void (*fn)(FSFileReq *, s32), u32 arg, u32 heapId, u32 flags) {
    if (this->getChunkSize(slot, chunkIdx) == 0) {
        if (fn != NULL) {
            FSFileReqEx * req = this->waitFreeReq();
            req->arg = arg;

            fn(req, -1);
        }

        return NULL;
    } else {
        this->waitFreeReq();

        return this->readChunk(slot, chunkIdx, 0, dst, pSize, fn, arg, flags, heapId, FALSE);
    }
}

// MATCHED 100%
u8 * clBtlSys::readChunkStreamWait(s16 slot, u16 chunkIdx, u8 * dst, s32 * pSize, void (*fn)(FSFileReq *, s32), u32 arg, u32 heapId) {
    if (this->getChunkSize(slot, chunkIdx) == 0) {
        if (fn != NULL) {
            FSFileReqEx * req = this->waitFreeReq();
            req->arg = arg;

            fn(req, -1);
        }

        return NULL;
    } else {
        this->waitFreeReq();

        return this->readChunkStream(slot, chunkIdx, NULL, 0, dst, pSize, fn, arg, heapId, FALSE);
    }
}

// MATCHED 100%
FSFileReqEx * clBtlSys::waitFreeReq() {
    FSFileReqEx * req = this->freeList;

    if (req == NULL) {
        BOOL flag;
        while (!(flag = (this->tableReqHead == NULL)) != FALSE) {
                this->pumpTableLoads();
        }

        while (req == NULL) {
            this->pumpReads();
            req = this->freeList;
        }
    }

    return req;
}

// MATCHED 100%
u32 clBtlSys::readStreamSize(u32 slot, u32 chunkIdx) {
    FSFileReqEx * freeReq = this->freeList;
    FSFileReqEx * req = (FSFileReqEx *)this->setupReq(freeReq, chunkIdx, NULL, 0, slot);
    
    u32 size = this->readStreamHeader(req, -1);

    freeReq->streamState = 0;

    return size;
}

// MATCHED 100%
void clBtlProTransMan::submit(YanaTransEntry * srcEntry) {
    this->clYanaTransMan::submit(srcEntry);
}

// MATCHED 100%
BOOL clBtlSys::isOverlayBusy() {
    if (this->ovyQueue.head != this->ovyQueue.tail || !(this->ovyState == 0) != FALSE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

// MATCHED 100%
void BtlOvyQueue::push(BtlOvySlot * slot) {
    if (this->head == this->tail) {
        btlSys->queueJob(btlOvyQueueJob, 0, 0, 0);
    }

    this->pending[this->tail] = slot;

    this->tail++;
    if (this->tail == 16) {
        this->tail = 0;
    }
}

// MATCHED 100%
BOOL BtlOvyQueue::pop() {
    this->pending[this->head]->apply();

    this->head++;
    if (this->head == 16) {
        this->head = 0;
    }

    return (this->head == this->tail);
}

// MATCHED 100%
void btlOvyQueueJob(BtlJob * job) {
    if (btlSys->ovyQueue.pop()) {
        job->fn = NULL;
    }
}

// MATCHED 100%
void BtlOvyQueue::deferUnload(s16 ovyId) {
    if (ovyId != NULL) {
        this->unloadIds[this->unloadCount++] = ovyId;
    }
}

// MATCHED 100%
BtlOvyQueue::~BtlOvyQueue() {
    while (this->unloadCount > 0) {
        this->unloadCount--;
        fsUnloadOverlay(this->unloadIds[this->unloadCount], MI_PROCESSOR_ARM9);
    }
}

// MATCHED 100%
void BtlOvySlot::request(s32 ovyId) {
    if (this->wanted != (s16)ovyId) {
        this->wanted = ovyId;

        if (this->current != (s16)ovyId) {
            btlSys->ovyQueue.push(this);
        }
    }
}

// MATCHED 100%
void BtlOvySlot::apply() {
    if (this->current != this->wanted) {
        if (this->current != 0) {
            fsUnloadOverlay(this->current, MI_PROCESSOR_ARM9);
        }

        this->current = this->wanted;

        if (this->wanted != 0) {
            btlSys->beginOverlayLoad(this->wanted, MI_PROCESSOR_ARM9);
        }
    }
}

// MATCHED 100%
void BtlOvySlot::unload() {
    if (this->current != 0) {
        fsUnloadOverlay(this->current, MI_PROCESSOR_ARM9);
    }

    this->current = 0;
    this->wanted = 0;
}

// MATCHED 100%
BtlOvySlot::~BtlOvySlot() {
    if (this->current != 0) {
        btlSys->ovyQueue.deferUnload(this->current);
    }

    this->current = 0;
    this->wanted = 0;
}

// MATCHED 100%
void btlRestartSoundJob(BtlJob * job) {
    if (sndGetActiveSeqMask() == 0) {
        sndShutdown(TRUE);
        sndInit(FALSE, 0xa0000, 0x57780, 0x80);
        sndEmitSetCmdTable();

        switch (gameState.field504_0) {
            case 1:
            case 2: {
                sndEchoMode = 0;
                break;
            }
            case 0:
            case 3: {
                if (gameState.field504_5 == 1) {
                    sndEchoMode = 1;
                } else {
                    sndEchoMode = 0;
                }
                break;
            }
        }

        switch (gameState.field504_0) {
            case 0:
            case 1: {
                u32 strmNo;
                if (gameState.field504_2 == 2) {
                    strmNo = 0x2a;
                } else if (gameState.field504_4 == 0) {
                    strmNo = 0x162;
                } else if (gameState.field504_2 == 0) {
                    strmNo = 0x6b;
                } else {
                    strmNo = 0x3d;
                }

                if (gameState.field504_5 == 1) {
                    strmNo += 0x1f4;
                }

                sndPlayStreamById(strmNo, 0, 0x7f, 0x60, -1);
                break;
            }
        }

        job->fn = NULL;
    }
}

// MATCHED 100%
void clBtlMainSys::func_ov008_02064c90() {}

// MATCHED 100%
void clBtlMainSys::vblankFlush() {
    this->texTrans.flush();
}

// MATCHED 100%
void clBtlSound::func_ov008_02064ca8() {}
