// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/fs.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "main/system.h"

// --- Extern Functions --- //

extern void func_01ff8000(u8 ** param1, u32 * param2, u32 * param3);
extern void func_01ff80a4(u8 ** param1, void ** param2);
extern void func_01ff8150(u8 ** param1, void ** param2);
extern void func_01ff8228(void * param1, u32 param2, u32 param3);

// --- Variable Definitions --- //

static ChunkSizeFn sChunkSizeFns[4] = {  // 0x02054c94
    &clFS::getChunkSize, &clFS::getChunkDataSize,
    &clFS::getChunkSize, &clFS::getChunkDataSize,
};

FSManager fsManager;  // 0x0205ba8c

// --- Function Declarations --- //

u32 fsChunkCount(FSChunkTable *); // 0x0201adbc
u8 * fsChunkPtr(FSChunkTable *, u32); // 0x0201add4
u32 fsChunkSize(FSChunkTable *, u32); // 0x0201ade4
BOOL fsInit(u32, void *, u32); // 0x0201b28c
BOOL fsLoadFileTable(void *, u32); // 0x0201b2e4

// --- Function Definitions --- //

// MATCHED 100%
clFS::clFS(u8 priority, u32 param2, u16 numSlots, u32 dmaNo, u32 param5) : clTask(priority, param2, param5) {
    this->reqTail = NULL;
    this->reqHead = NULL;
    this->tableReqTail = NULL;
    this->tableReqHead = NULL;

    this->vcountBusyStart = 0xa8;
    this->stallReset = 5;
    this->stallLimit = 2;
    this->stallCount = 0;
    this->inCallback = 0;

    this->ovyState = 0;

    if (dmaNo != 0xffff7f7f) {
        fsManager.dmaNo = dmaNo;
        FS_SetDefaultDMA(dmaNo);
    }

    this->numSlots = numSlots;

    HeapBlock * header = GET_HEADER(this);
    u16 heap = header->heapId;

    this->files = (FSFile *)heapAlloc(heap, numSlots * sizeof(FSFile), NULL, 0);
    this->tables = (FSChunkTable **)heapAlloc(heap, numSlots * sizeof(FSChunkTable **), NULL, 0);
    this->paths = (const char **)heapAlloc(heap, numSlots * sizeof(const char *), NULL, 0);
    this->tableReqs = (FSTableReq *)heapAlloc(heap, numSlots * sizeof(FSTableReq), NULL, 0);

    for (s16 i = 0; numSlots > i; i++) {
        FS_InitFile(&this->files[i]);
        this->paths[i] = NULL;
        this->tables[i] = NULL;
    }

    FS_InitFile(&this->ovyFile);
}

// MATCHED 100%
clFS::~clFS() {
    closeAllFiles();

    heapFree(this->files);
    heapFree(this->tables);
    heapFree(this->paths);
    heapFree(this->tableReqs);
}

// MATCHED 100%
void clFS::run() {
    bool active = false;

    if (systemState.frameTimer.frameSkips == 0) {
        bool idle = true;

        if (reg_GX_VCOUNT >= this->vcountBusyStart && reg_GX_VCOUNT <= (s16)GX_LCD_SIZE_Y) {
            idle = false;
        }

        if (idle) {
            active = true;
        }
    }

    if (active) {
        this->stallCount = 0;
    } else if (this->stallCount >= this->stallLimit) {
        this->stallCount = this->stallReset;
    } else if (++this->stallCount != this->stallReset+1) {
        return;
    }

    bool run = true;
    while (run) {
        BOOL flag = (this->tableReqHead == NULL);

        if (!flag) {
            this->pumpTableLoads();
        } else if (!(flag = (this->reqHead == NULL))) {
            this->pumpReads();
        } else if (this->ovyState != 0) {
            this->pumpOverlay();
        } else {
            this->stallCount = 0;
            return;
        }

        run = false;

        if (systemState.frameTimer.frameSkips == 0) {
            bool idle = true;

            if (reg_GX_VCOUNT >= this->vcountBusyStart && reg_GX_VCOUNT <= (s16)GX_LCD_SIZE_Y) {
                idle = false;
            }

            if (idle) {
                run = true;
            }
        }
    }
}

// MATCHED 100%
void clFS::shutdown() {
    closeAllFiles();
    this->disable();
}

// MATCHED 100%
u32 fsChunkCount(FSChunkTable * tbl) {
    return tbl->tailOffset ? (tbl->tailOffset - 8) / 4 : 0;
}

// MATCHED 100%
u8 * fsChunkPtr(FSChunkTable * tbl, u32 idx) {
    return (u8 *)tbl + CHUNK_OFFSET(tbl, idx);
}

// MATCHED 100%
u32 fsChunkSize(FSChunkTable * tbl, u32 idx) {
    return CHUNK_OFFSET(tbl, idx+1) - CHUNK_OFFSET(tbl, idx);
}

// MATCHED 100%
u32 clFS::getTableSize(s16 slot, const char * path, BOOL hasTails) {
    this->openSlot(slot, path);

    u32 size;
    FS_ReadFile(&this->files[slot], &size, 4);

    u32 bytes;
    if (hasTails == TRUE) {
            bytes = ((size != 0) ? (size - 8) >> 2 : 0) << 1;
    } else {
        bytes = 0;
    }

    return size + bytes;
}

// MATCHED 100%
FSChunkTable * clFS::releaseSlot(u32 slot) {
    if (FS_IsFile(&this->files[(u16)slot])) {
        FS_CloseFile(&this->files[(u16)slot]);
    }

    FSChunkTable * ptr = this->tables[slot];

    this->paths[slot] = NULL;
    this->tables[slot] = NULL;

    return ptr;
}

// MATCHED 100%
void clFS::waitTableLoad(u32 slot) {
    FSTableReq * val = &this->tableReqs[slot];
    while ((val->state & FSREQ_STATE_MASK) != 0) {
        this->pumpTableLoads();
    }
}

// MATCHED 100%
u32 clFS::getChunkSize(u32 slot, u32 idx) {
    FSChunkTable * tbl = this->tables[slot];
    return CHUNK_OFFSET(tbl, idx+1) - CHUNK_OFFSET(tbl, idx);
}

// MATCHED 100%
u32 clFS::getChunkDataSize(u32 slot, u32 idx) {
    FSChunkTable * tbl = this->tables[slot];
    return (CHUNK_OFFSET(tbl, idx+1) - CHUNK_OFFSET(tbl, idx)) - CHUNK_TAIL(tbl, idx);
}

// MATCHED 100%
FSFileReq * clFS::setupReq(FSFileReq * req, u32 chunkIdx, u8 * buf, u32 bufSize, s16 slot) {
    if (slot >= 0) {
        req->slot = slot;
    }

    req->chunkIdx = chunkIdx;
    req->buf = buf;
    req->bufSize = bufSize;

    return req;
}

// MATCHED 100%
void clFS::waitReq(FSFileReq * req) {
    FSFileReq * reqNode = this->reqHead;

    while (reqNode != NULL) {
        if (reqNode == req) {
            BOOL isEmpty = (this->tableReqHead == NULL);
            while (!isEmpty) {
                this->pumpTableLoads();
                isEmpty = (this->tableReqHead == NULL);
            }

            while ((req->state & FSREQ_STATE_MASK) != 0) {
                this->pumpReads();
            }
            break;
        }

        reqNode = reqNode->next;
    }
}

// MATCHED 100%
void clFS::cancelReq(FSFileReq * req) {
    if ((req->state & FSREQ_STATE_MASK) != FSREQ_QUEUED) {
        if ((req->state & FSREQ_STATE_MASK) == FSREQ_ACTIVE) {
            FS_CancelFile(&this->files[req->slot]);

            req->remaining = 0;
            req->state |= FSREQ_CANCELED;
        }
    } else {
        FSFileReq * reqNode = this->reqHead;

        if (reqNode == req) {
            FSFileReq * next = req->next;
            this->reqHead = next;
            if (next == NULL) {
                this->reqTail = NULL;
            }
        } else {
            while (reqNode != NULL) {
                FSFileReq * next = reqNode->next;

                if (reqNode->next == req) {
                    FSFileReq * newNext = req->next;
                    reqNode->next = req->next;
                    if (newNext == NULL) {
                        this->reqTail = reqNode;
                    }
                    break;
                }

                reqNode = next;
            }
        }

        if (req->fn != NULL) {
            req->fn(req, false);
        }

        req->state = FSREQ_CANCELED;
    }
}

// MATCHED 100%
void clFS::waitAll() {
    BOOL run = (this->tableReqHead == NULL);
    while (!run) {
        this->pumpTableLoads();
        run = (this->tableReqHead == NULL);
    }

    run = (this->reqHead == 0);
    while (!run) {
        this->pumpReads();
        run = (this->reqHead == 0);
    }
}

// MATCHED 100%
BOOL fsLoadOverlay(FSOverlayID id, MIProcessor target) {
    return FS_LoadOverlay(target, id);
}

// MATCHED 100%
BOOL clFS::beginOverlayLoad(FSOverlayID id, MIProcessor target) {
    BOOL success = FS_LoadOverlayInfo(&this->ovyInfo, target, id);
    if (!success) {

        return FALSE;
    }

    FS_ClearOverlayImage(&this->ovyInfo);

    if (FS_IsFile(&this->ovyFile)) {
        FS_CloseFile(&this->ovyFile);
    }

    if (!FS_OpenFileFast(&this->ovyFile, FS_GetOverlayFileID(&this->ovyInfo))) {
        return FALSE;
    }

    FS_ClearOverlayImage(&this->ovyInfo);

    this->ovyDst = this->ovyInfo.header.ram_address;

    u32 size = this->ovyInfo.header.ram_size;
    this->ovyRemaining = this->ovyInfo.header.ram_size;
    this->ovyReadSize = MATH_MIN(size, CARD_ROM_PAGE_SIZE);
    this->ovyState = 1;

    return TRUE;
}

// MATCHED 100%
u32 clFS::startOverlay() {
    if (this->ovyState != 0) {
        return 2;
    } else {
        FS_StartOverlay(&this->ovyInfo);
        return 0;
    }
}

// MATCHED 100%
BOOL fsUnloadOverlay(FSOverlayID id, MIProcessor target) {
    return FS_UnloadOverlay(target, id);
}

// MATCHED 100%
BOOL fsInit(u32 dmaNo, void * p_mem, u32 size) {
    fsManager.dmaNo = dmaNo;

    FS_Init(dmaNo);

    if (p_mem != NULL) {
        BOOL flag = (FS_TryLoadTable(p_mem, size) <= size);
        if (flag) {
            fsManager.table = p_mem;
        }
    }

    return FS_TryLoadTable(NULL, 0);
}

// MATCHED 100%
BOOL fsLoadFileTable(void * p_mem, u32 size) {
    if (fsManager.table != NULL) {
        return FALSE;
    }

    BOOL flag = (FS_TryLoadTable(p_mem, size) <= size);
    if (!flag) {
        return FALSE;
    } else {
        fsManager.table = p_mem;
        return TRUE;
    }
}

// MATCHED 100%
bool clFS::openSlot(u16 slot, const char * path) {
    if (FS_IsFile(&this->files[slot])) {
        FS_CloseFile(&this->files[slot]);
    }

    FS_OpenFile(&this->files[slot], path);

    this->paths[slot] = path;

    return true;
}

// MATCHED 100%
void clFS::closeAllFiles() {
    s32 numSlots = this->numSlots;
    for (s16 i = 0; i < numSlots; i++) {
        FSFile * file = &this->files[(u16)i];
        if (FS_IsFile(file)) {
            FS_CloseFile(file);
        }
    }

    if (FS_IsFile(&this->ovyFile)) {
        FS_CloseFile(&this->ovyFile);
    }
}

// MATCHED 100%
bool clFS::queueTableLoad(u32 slot, FSChunkTable * table, u32 size, u32 offset, void (*fn)(FSTableReq *, bool), u32 arg, u32 flags) {
    FSTableReq * tblReqs = this->tableReqs;
    FSTableReq * tblReq = &tblReqs[slot];

    tblReq->fn = fn;
    tblReq->arg = arg;

    this->tables[slot] = table;

    tblReq->dst = (u32 *)this->tables[slot];
    tblReq->slot = slot;
    tblReq->remaining = size;
    tblReq->offset = offset;
    tblReq->state = flags | 1;
    tblReqs[slot].next = 0;

    if (flags != 0) {
        size = MATH_MIN(size, CARD_ROM_PAGE_SIZE);
    }

    tblReq->readSize = size;

    if (this->tableReqTail == NULL) {
        this->tableReqHead = tblReq;
        this->tableReqTail = tblReq;
    } else {
        this->tableReqTail->next = tblReq;
        this->tableReqTail = this->tableReqTail->next;
    }

    return true;
}

// MATCHED 100%
bool clFS::pumpTableLoads() {
    FSTableReq * req = this->tableReqHead;
    FSFile * file = &this->files[req->slot];

    if (FS_IsBusy(file)) {
        return false;
    }

    if ((req->state & FSREQ_STATE_MASK) == FSREQ_ACTIVE) {
        if ((req->state & FSREQ_PAGED) && req->remaining > CARD_ROM_PAGE_SIZE) {
            req->dst += CARD_ROM_PAGE_SIZE / sizeof(u32);
            req->offset += CARD_ROM_PAGE_SIZE;
            req->remaining -= CARD_ROM_PAGE_SIZE;
            if (req->remaining < CARD_ROM_PAGE_SIZE) {
                req->readSize = req->remaining;
            }
        } else {
            if (req->fn != NULL) {
                this->inCallback = 1;
                req->fn(req, (req->state & FSREQ_CANCELED) == 0);
                this->inCallback = 0;
            }

            req->state &= FSREQ_CANCELED;
            this->tableReqHead = req->next;
            req->next = NULL;

            req = this->tableReqHead;
            if (req == NULL) {
                this->tableReqTail = NULL;
                return true;
            }

            file = &this->files[req->slot];
            goto start;
        }

        if (!FS_IsFile(file)) {
            this->openSlot(req->slot, this->paths[req->slot]);
            FS_SeekFile(file, req->offset, FS_SEEK_SET);
        }
    } else {
start:
        req->state = (req->state & ~FSREQ_STATE_MASK) | FSREQ_ACTIVE;
        if (!FS_IsFile(file)) {
            this->openSlot(req->slot, this->paths[req->slot]);
        }
        FS_SeekFile(file, req->offset, FS_SEEK_SET);
    }
    
    FS_ReadFile(file, req->dst, req->readSize);
    return true;
}

// MATCHED 100%
bool clFS::queueRead(FSFileReq * req, u32 offset, s32 slot, void (*fn)(FSFileReq *, bool), u32 arg, u32 flags) {
    if (slot >= 0) {
        req->slot = slot;
    }

    req->fn = fn;
    req->arg = arg;
    req->state = flags | 1;
    req->offset = offset;
    req->dst = req->buf;
    req->next = NULL;

    s32 size = this->getChunkSize((s16)req->slot, req->chunkIdx) - offset;
    if (req->bufSize != 0) {
        size = req->bufSize;
    }

    req->remaining = size;
    if (size == 0) {
        return false;
    }

    if (flags != 0) {
        size = MATH_MIN(size, CARD_ROM_PAGE_SIZE);
    }

    req->readSize = size;

    if (this->reqTail == NULL) {
        this->reqHead = req;
        this->reqTail = req;
    } else {
        this->reqTail->next = req;
        this->reqTail = this->reqTail->next;
    }

    return true;
}

// MATCHED 100%
bool clFS::pumpReads() {
    FSFileReq * req = this->reqHead;
    FSFile * file = &this->files[req->slot];

    if (FS_IsBusy(file)) {
        return false;
    }

    if ((req->state & FSREQ_STATE_MASK) == FSREQ_ACTIVE) { 
        if ((req->state & FSREQ_PAGED) && req->remaining > CARD_ROM_PAGE_SIZE) {
            req->dst += CARD_ROM_PAGE_SIZE;
            req->offset += CARD_ROM_PAGE_SIZE;
            req->remaining -= CARD_ROM_PAGE_SIZE;
            if (req->remaining < CARD_ROM_PAGE_SIZE) {
                req->readSize = req->remaining;
            }
        } else {
            if (req->fn != NULL) {
                this->inCallback = 1;
                req->fn(req, (req->state & FSREQ_CANCELED) == 0);
                this->inCallback = 0;
            }

            req->state &= FSREQ_CANCELED;
            this->reqHead = req->next;
            req->next = NULL;

            req = this->reqHead;
            if (req == NULL) {
                this->reqTail = NULL;
                return true;
            }

            file = &this->files[req->slot];
            goto start;
        }

        if (!FS_IsFile(file)) {
            this->openSlot(req->slot, this->paths[req->slot]);
            FS_SeekFile(file, req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx), FS_SEEK_SET);
        }
    } else {
start:
        req->state = (req->state & ~FSREQ_STATE_MASK) | FSREQ_ACTIVE;

        if (!FS_IsFile(file)) {
            this->openSlot(req->slot, this->paths[req->slot]);
        }
        FS_SeekFile(file, req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx), FS_SEEK_SET);
    }

    FS_ReadFile(file, req->dst, req->readSize);
    return true;
}

// MATCHED 100%
bool clFS::pumpOverlay() {
    if (FS_IsBusy(&this->ovyFile)) {
        return false;
    }

    if (this->ovyState == 2) {
        if (this->ovyRemaining <= CARD_ROM_PAGE_SIZE) {
            if (FS_IsFile(&this->ovyFile)) {
                FS_CloseFile(&this->ovyFile);
            }

            this->ovyState = 0;

            return true;
        }

        this->ovyDst += CARD_ROM_PAGE_SIZE;
        this->ovyRemaining -= CARD_ROM_PAGE_SIZE;
        if (this->ovyRemaining < CARD_ROM_PAGE_SIZE) {
            this->ovyReadSize = this->ovyRemaining;
        }
    } else {
        this->ovyState = 2;
    }

    if (!FS_IsFile(&this->ovyFile)) {
        if (!FS_OpenFileFast(&this->ovyFile, FS_GetOverlayFileID(&this->ovyInfo))) {
            return false;
        }

        FS_SeekFile(&this->ovyFile, this->ovyInfo.header.ram_size - this->ovyRemaining, FS_SEEK_SET);
    }

    FS_ReadFile(&this->ovyFile, this->ovyDst, this->ovyReadSize);

    return true;
}

// MATCHED 100%
clFSEx::clFSEx(u8 priority, u32 param2, u16 count, u16 numSlots, u32 dmaNo, u32 param6) : clDPP::clDPP(priority, param2, numSlots, dmaNo, param6) {
    u32 heap = GET_HEADER(this)->heapId;
    this->reqPool = (FSFileReqEx *)heapAlloc(heap, count * sizeof(FSFileReqEx), NULL, 0);
    this->sizeFns = (ChunkSizeFn *)heapAlloc(heap, numSlots * sizeof(ChunkSizeFn), NULL, 0);;
    this->freeList = NULL;
    this->reqPoolEnd = (u8*)this->reqPool + count * sizeof(FSFileReqEx) * sizeof(FSFileReqEx);

    func_01ff8228(this->reqPool, 0, count * sizeof(FSFileReqEx));
    func_01ff8228(&this->externTable, 0, 2 * sizeof(u32));

    FSFileReqEx * ptr = this->reqPool;
    while (count != 0) {
        ptr->next = this->freeList;
        this->freeList = ptr;
        count--;
        ptr++;
    }
}

// MATCHED 100%
clFSEx::~clFSEx() {
    s16 size = this->numSlots;
    for (s16 r7 = 0; r7 < size; r7++) {
        if ((this->externTable[r7 >> 5] & (1 << (r7 & 0x1f))) != 0) {
            this->releaseSlot(r7);
            this->externTable[r7 >> 5] &= ~(1 << (r7 & 0x1f));
        } else {
            heapFree(this->releaseSlot(r7));
        }
    }

    heapFree(this->reqPool);
    heapFree(this->sizeFns);
}

// MATCHED 100%
bool clFSEx::openArchive(s16 slot, const char * path, void (*fn)(FSTableReq *, bool), u32 arg, BOOL hasTails, u32 heapId, u32 flags, u32 align) {
    if (this->tables[slot] != NULL) {
        if ((this->externTable[slot >> 5] & 1 << (slot & 0x1f)) != 0) {
            this->releaseSlot(slot);
            this->externTable[slot >> 5] &= ~(1 << (slot & 0x1f));
        } else {
            heapFree(this->releaseSlot(slot));
        }
        this->tables[slot] = NULL;
    }

    u32 size = this->getTableSize(slot, path, hasTails);
    this->sizeFns[slot] = (hasTails == 0) ? sChunkSizeFns[2] : sChunkSizeFns[1];
    
    FSChunkTable * tbl = (FSChunkTable *)heapAlloc(heapId, size, NULL, align);
    this->queueTableLoad(slot, tbl, size, 0, fn, arg, flags);

    return true;
}

// MATCHED 100%
void clFSEx::setArchive(s16 slot, const char * path, FSChunkTable * table, BOOL hasTails) {
    if (this->tables[slot] != NULL) {
        if ((this->externTable[slot >> 5] & 1 << (slot & 0x1f)) != 0) {
            this->releaseSlot(slot);
            this->externTable[slot >> 5] &= ~(1 << (slot & 0x1f));
        } else {
            heapFree(this->releaseSlot(slot));
        }
        this->tables[slot] = NULL;
    }

    this->openSlot(slot, path);

    this->sizeFns[slot] = (hasTails == FALSE) ? sChunkSizeFns[0] : sChunkSizeFns[3];
    this->tables[slot] = table;
    this->externTable[slot >> 5] |= (1 << (slot & 0x1f));
}

// MATCHED 100%
u8 * clFSEx::readChunk(s16 slot, u32 chunkIdx, u32 offset, u8 * dst, s32 * pSize, void (*fn)(FSFileReq *, bool), u32 arg, u32 flags, u32 heapId, u32 align) {
    u32 size = (this->*this->sizeFns[slot])(slot, chunkIdx);
    if (size == 0) {
        if (pSize != NULL) {
            *pSize = 0;
        }
        return NULL;
    }

    FSFileReqEx * reqEx = this->allocReq();
    if (reqEx == NULL) {
        if (pSize != NULL) {
            *pSize = -1;
        }
        return NULL;
    }

    u32 oldSize = size;
    if (dst == 0) {
        dst = (u8 *)heapAlloc(heapId, size, NULL, align);
    } else if (pSize != NULL && *pSize != 0) {
        size = *pSize;
    }

    if (pSize != NULL) {
        *pSize = oldSize;
    }

    if (dst == NULL) {
        return NULL;
    }

    reqEx->dstBase = dst;

    FSFileReq * req = this->setupReq(reqEx, chunkIdx, dst, size, -1);
    this->clDPP::queueRead(req, offset, slot, fn, arg, flags);

    return dst;
}

// MATCHED 100%
void clFSEx::cancelAll() {
    FSFileReq * req = this->reqHead;
    while (req != NULL) {
        this->cancelStream(req);

        BOOL flag = (req->state & FSREQ_STATE_MASK) == 0;
        if (!flag) {
            req = req->next;
        } else {
            FSFileReq * oldReq = req;
            req = req->next;
            oldReq->next = this->freeList;
            this->freeList = (FSFileReqEx *)oldReq;
        }
    }
}

// MATCHED 100%
u8 * clFSEx::readChunkStream(s16 slot, u32 chunkIdx, u8 * headerBuf, u32 headerSize, u8 * dst, s32 * pSize, void (*fn)(FSFileReq *, bool), u32 arg, s32 heapId, u32 align) {
    if (this->getChunkSize(slot, chunkIdx) == 0) {
        if (pSize != NULL) {
            *pSize = 0;
        }
        return NULL;
    }

    FSFileReqEx * req = this->allocReq();
    if (req == NULL) {
        if (pSize != NULL) {
            *pSize = -1;
        }
        return NULL;
    }

    headerSize = this->readStreamHeader((FSFileReqEx *)this->setupReq(req, chunkIdx, headerBuf, headerSize, slot), -1);

    if (pSize != NULL) {
        *pSize = headerSize;
    }

    if (dst == NULL) {
        dst = (u8 *)heapAlloc(heapId, headerSize, NULL, align);
    }

    if (dst == NULL) {
        return NULL;
    }

    this->queueStream(req, (u32*)dst, fn, arg);

    return dst;
}

// MATCHED 100%
bool clFSEx::pumpReads() {
    FSFileReqEx * req = (FSFileReqEx *)this->reqHead;
    if (!this->clDPP::pumpReads()) {
        return false;
    }

    if (req != this->reqHead && req >= this->reqPool && req < this->reqPoolEnd) {
        req->next = this->freeList;
        this->freeList = req;
    }

    return true;
}

// MATCHED 100%
FSFileReqEx * clFSEx::allocReq() {
    FSFileReqEx * req = this->freeList;
    if (req == NULL) {
        return NULL;
    } else {
        this->freeList = (FSFileReqEx *)req->next;
        return req;
    }
}

// MATCHED 100%
clDAP::clDAP(u8 priority, u32 param2, u32 param3) : clTask::clTask(priority, param2, param3) {
    this->tail = NULL;
    this->head = NULL;
    this->vcountBusyStart = 0xa8;
    this->stallReset = 5;
    this->stallLimit = 2;
    this->stallCount = 0;
}

// MATCHED 100%
void clDAP::run() {
    bool active = false;
    if (systemState.frameTimer.frameSkips == 0) {
        bool idle = true;

        if (reg_GX_VCOUNT >= this->vcountBusyStart && reg_GX_VCOUNT <= (s16)GX_LCD_SIZE_Y) {
            idle = false;
        }

        if (idle) {
            active = true;
        }
    }

    if (active) {
        this->stallCount = 0;
    } else if (this->stallCount >= this->stallLimit) {
        this->stallCount = this->stallReset;
    } else if (++this->stallCount != this->stallReset+1) {
        return;
    }

    bool run = true;
    while (run) {
        BOOL flag = (this->head == NULL) ? TRUE : FALSE;
        if (flag) {
            this->stallCount = 0;
            break;
        }

        this->pumpJobs();

        run = false;

        if (systemState.frameTimer.frameSkips == 0) {
            bool ready = true;

            if (reg_GX_VCOUNT >= this->vcountBusyStart && reg_GX_VCOUNT <= (s16)GX_LCD_SIZE_Y) {
                ready = false;
            }

            if (ready) {
                run = true;
            }
        }
    }
}

// MATCHED 100%
u32 clDAP::beginJob(FSFileReqEx * req) {
    req->dst = req->buf;

    u32 local;
    func_01ff8000(&req->dst, &local, &req->totalSize);

    req->recordsLeft = local;
    req->streamState = DPP_PREPARED;
    return req->totalSize;
}

// MATCHED 100%
void clDAP::queueJob(FSFileReqEx * req, u32 * dst) {
    req->dstBase = dst;
    req->dstCur = dst;
    req->streamState = DPP_QUEUED;
    req->next = NULL;

    if (this->tail != NULL) {
        this->tail->next = req;
        this->tail = (FSFileReqEx *)this->tail->next;
    } else {
        this->head = req;
        this->tail = req;
    } 
}

// MATCHED 100%
void clDAP::waitJob(FSFileReq * req) {
    FSFileReq * reqNode = this->head;
    while (reqNode != NULL) {
        if (reqNode == req) {
            while (req->streamState != DPP_IDLE) {
                this->pumpJobs();
            }
            break;
        }
        reqNode = reqNode->next;
    }
}

// MATCHED 100%
void clDAP::pumpJobs() {
    this->head->dst += 2;

    if (this->head->dstCur >= (void *)HW_PLTT && this->head->dstCur < (void *)HW_OAM_END) {
        func_01ff8150(&this->head->dst, &this->head->dstCur);
    } else {
        func_01ff80a4(&this->head->dst, &this->head->dstCur);
    }

    this->head->streamState = DPP_COPYING;

    if (--this->head->recordsLeft == 0) {
        FSFileReqEx * req = this->head;
        req->streamState = DPP_IDLE;
        this->head = (FSFileReqEx *)req->next;
        req->next = NULL;
        if (this->head == NULL) {
            this->tail = NULL;
        }
    }
}

// MATCHED 100%
clDPP::clDPP(u8 priority, u32 param2, u16 numSlots, u32 dmaNo, u32 param5) : clFS::clFS(priority, param2, numSlots, dmaNo, param5) {
    this->alignedBuf = (u8*)((u32)(this->buffer + 0x1f) & ~0x1f);
    this->page = 0;
}

// MATCHED 100%
u32 clDPP::readStreamHeader(FSFileReqEx * req, s16 slot) {
    if (slot >= 0) {
        req->slot = slot;
    }

    u32 size = this->getChunkSize((s16)req->slot, req->chunkIdx);
    if (size == 0) {
        return 0;
    }

    u8 arr[0x8];
    req->dst = arr;
    req->readSize = 8;
    req->offset = 0;

    this->readSync(req, 0);

    u32 recordsLeft;
    func_01ff8000(&req->dst, &recordsLeft, &req->totalSize);

    req->offset = req->dst - (u8*)arr;
    req->remaining = size - req->offset;
    req->streamState = DPP_PREPARED;
    req->recordsLeft = recordsLeft;
    return req->totalSize;
}

// MATCHED 100%
void clDPP::queueStream(FSFileReqEx * req, u32 * dst, void (*fn)(FSFileReq *, bool), u32 arg) {
    req->fn = fn;
    req->arg = arg;
    req->dstBase = dst;
    req->dstCur = dst;
    req->state = FSREQ_QUEUED;
    req->streamState = DPP_QUEUED;
    req->next = NULL;

    if (req->buf == NULL) {
        req->buf = this->alignedBuf;
        req->bufSize = 0xb00;
        req->readSize = CARD_ROM_PAGE_SIZE;
    } else {
        if (fsManager.dmaNo < 4) {
            u32 alignment = ((u32)req->buf & 0x1f);
            if (((u32)req->buf & 0x1f) != 0) {
                req->bufSize -= 32 - alignment;
            }
            req->buf = (u8*)(((u32)req->buf + 0x1f) & ~0x1f);
            req->readSize = -CARD_ROM_PAGE_SIZE & (req->bufSize / 6);
        } else {
            req->readSize = req->bufSize / 6;
        }
    }

    if (this->reqTail != NULL) {
        this->reqTail->next = req;
        this->reqTail = this->reqTail->next;
    } else {
        this->reqHead = req;
        this->reqTail = req;
    }
}

// MATCHED 100%
void clDPP::cancelStream(FSFileReq * req) {
    req->streamState = DPP_IDLE;
    this->cancelReq(req);
}

// MATCHED 100%
bool clDPP::queueRead(FSFileReq * req, u32 offset, s32 slot, void (*fn)(FSFileReq *, bool), u32 arg, u32 flags) {
    req->streamState = DPP_IDLE;
    return clFS::queueRead(req, offset, slot, fn, arg, flags);
}

// MATCHED 100%
void clDPP::readSync(FSFileReqEx * req, u32 param2) {
    FSFile * file = &this->files[req->slot];

    if (FS_IsBusy(file)) {
        FS_WaitAsync(file);
    }

    if (!FS_IsFile(file)) {
        this->openSlot(req->slot, this->paths[req->slot]);
    }

    FSFile newFile;
    FS_InitFile(&newFile);
    FS_CreateFileFromRom(&newFile, file->prop.file.top, file->prop.file.bottom - file->prop.file.top);
    FS_SeekFile(&newFile, req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx), FS_SEEK_SET);
    FS_ReadFile(&newFile, req->dst, req->readSize);
    FS_CloseFile(&newFile);
}

// MATCHED 100%
bool clDPP::pumpReads() {
    FSFileReqEx * doneReq;
    FSFile * file;
    FSFileReqEx * req;
    s32 remaining;
    u8 * ptr;
    u8 * cursor;
    
    req = (FSFileReqEx *)this->reqHead;
    cursor = NULL;
    file = &this->files[req->slot];

    if (FS_IsBusy(file)) {
        return false;
    }

    if ((req->state & FSREQ_STATE_MASK) == FSREQ_ACTIVE) {
        if (fsManager.dmaNo < 4 &&
            (req->streamState != DPP_IDLE || ((req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx)) & 0x1ff) == 0) &&
            ((u32)req->dst & 0x1f) == 0 && 
            (req->readSize & 0x1ff) == 0) {
                    DC_InvalidateRange(req->dst, req->readSize);
        }

        if (req->streamState != DPP_IDLE) {
            this->selectCopyFn(req);
            doneReq = req;
            cursor = this->cursor;
            remaining = this->remaining;
            if (req->remaining > 0) {
                req->dst = ((u8*)req->buf + req->readSize * this->page);
                req->remaining -= req->readSize;
                req->offset += req->readSize;
                if (++this->page >= 4) {
                    this->page = 0;
                }
            } else {
                goto pop;
            }
        } else if ((req->state & FSREQ_PAGED) && req->remaining > CARD_ROM_PAGE_SIZE) {
            req->dst += CARD_ROM_PAGE_SIZE;
            req->offset += CARD_ROM_PAGE_SIZE;
            req->remaining -= CARD_ROM_PAGE_SIZE;
            if (req->remaining < CARD_ROM_PAGE_SIZE) {
                req->readSize = req->remaining;
            }
        } else {
            if (req->fn != NULL) {
                this->inCallback = 1;
                req->fn(req, (req->state & FSREQ_CANCELED) == 0);
                this->inCallback = 0;
            }
pop:
            this->reqHead = req->next;
            req->next = NULL;
            req->state &= FSREQ_CANCELED;
            req = (FSFileReqEx *)this->reqHead;
            if (req == NULL) {
                this->reqTail = NULL;
                goto drain;
            }
            file = &this->files[req->slot];
            goto start;
        }

        if (!FS_IsFile(file)) {
            this->openSlot(req->slot, this->paths[req->slot]);
            FS_SeekFile(file, req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx), FS_SEEK_SET);
        }
    } else {
start:
        if (req->streamState != DPP_IDLE) {
            u32 align = (req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx)) & 0x1ff;
            this->remaining = req->readSize - align;
            req->remaining -= this->remaining;
            ptr = req->buf;
            req->dst = ptr + req->readSize * this->page;
            req->offset -= align;
            this->cursor = req->dst + align;
            this->page++;
            if (this->page >= 4) {
                this->page = 0;
            }
        }

        req->state = (req->state & ~FSREQ_STATE_MASK) | FSREQ_ACTIVE;

        if (!FS_IsFile(file)) {
            this->openSlot(req->slot, this->paths[req->slot]);
        }
        FS_SeekFile(file, req->offset + CHUNK_OFFSET(this->tables[req->slot], req->chunkIdx), FS_SEEK_SET);
    }
    FS_ReadFileAsync(file, req->dst, req->readSize);

drain:
    if (cursor != NULL && doneReq->recordsLeft != 0) {
        ptr = &doneReq->buf[doneReq->readSize * 4];
        if (cursor == ptr) {
            cursor = doneReq->buf;
        }

        u16 recLen = (cursor[0] | cursor[1] << 8) + 2;
        if (&cursor[2] > ptr) {
            recLen = (remaining >= 2) ? (cursor[0] | doneReq->buf[0] << 8) + 2 : 0xffff;
        }

        u8 * payload;
        while (remaining > 1 && recLen <= remaining) {
            payload = cursor + 2;
            if (cursor + recLen > ptr) {
                u32 size = (u32)(cursor + ((u8*)recLen - ptr));
                MI_CpuCopy8(doneReq->buf, ptr, size);
                cursor = doneReq->buf + size;
            } else {
                cursor += (s32)recLen;
            }

            this->copyFn(&payload, &doneReq->dstCur);
            if (--doneReq->recordsLeft == 0) {
                if (doneReq->fn != NULL) {
                    this->inCallback = 1;
                    doneReq->fn(doneReq, (doneReq->state & FSREQ_CANCELED) == 0);
                    this->inCallback = 0;
                }
                doneReq->streamState = DPP_IDLE;
                goto done;
            }

            remaining -= recLen;

            recLen = (cursor[0] | (u16)(cursor[1] << 8)) + 2;
            if (&cursor[2] > ptr) {
                recLen = (remaining >= 2) ? (cursor[0] | (u16)(doneReq->buf[0] << 8)) + 2 : 0xffff;
            }
        }

        this->cursor = cursor;
        this->remaining = remaining + doneReq->readSize;
    }
done:

    if (FS_IsBusy(file)) {
        FS_WaitAsync(file);
    }

    return true;
}

// MATCHED 100%
void clDPP::selectCopyFn(FSFileReqEx * req) {
    bool inGraphicsRAM = false;
    if (req->dstCur >= (void *)HW_PLTT && req->dstCur < (void *)HW_OAM_END) {
        inGraphicsRAM = true;
    }

    this->copyFn = (inGraphicsRAM) ? func_01ff8150 : func_01ff80a4;
    req->streamState = DPP_COPYING;
}

} // extern "C"
