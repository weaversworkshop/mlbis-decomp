#ifndef MAIN_FS_H
#define MAIN_FS_H

#include "types.h"

#include <nitro/fs.h>

#ifdef __cplusplus

#include "main/task.h"

#define FSREQ_STATE_MASK    0x0003
#define FSREQ_QUEUED        0x0001
#define FSREQ_ACTIVE        0x0002
#define FSREQ_CANCELED      0x0010
#define FSREQ_PAGED         0x8000

#define DPP_IDLE        0
#define DPP_PREPARED    1
#define DPP_QUEUED      2
#define DPP_COPYING     3

typedef struct {
    u32 tailOffset;
    u32 offsets[1];
    // u16 tails[]     trailing dynamic sized u16 array
} FSChunkTable;

#define CHUNK_OFFSET(t, i)  ((t)->offsets[(i)])
#define CHUNK_TAIL(t, i)    ((u16 *)((u8 *)(t) + (t)->tailOffset))[(i)]

typedef struct {
    u8 dmaNo;
    u8 pad_01[0x3];
    void * table;
    u8 pad_08[0x2c];
} FSManager;

extern FSManager fsManager;

typedef struct FSFileReq {
    FSFileReq * next;
    void (*fn)(FSFileReq *, bool);
    u32 arg;
    u8 * dst;
    u8 * buf;
    u32 bufSize;
    s32 remaining;
    s32 readSize;
    u32 offset;
    u16 slot;
    u16 chunkIdx;
    u16 state;
    u16 streamState;
} FSFileReq;

struct FSFileReqEx : FSFileReq {
    void * dstBase;
    void * dstCur;
    u32 totalSize;
    u16 recordsLeft;
};

typedef struct FSTableReq {
    FSTableReq * next;
    void (*fn)(FSTableReq *, bool);
    u32 arg;
    u32 * dst;
    u32 remaining;
    u32 readSize;
    u32 offset;
    u16 slot;
    u16 state;
} FSTableReq;

class clFS : public clTask {
public:
    u8 pad_18[0x10];
    FSFile ovyFile;
    FSOverlayInfo ovyInfo;
    u8 * ovyDst;
    u32 ovyRemaining;
    u32 ovyReadSize;
    u16 ovyState;
    u8 pad_aa[0x2];
    FSFileReq * reqHead;
    FSFileReq * reqTail;
    FSFile * files;
    const char ** paths;
    FSChunkTable ** tables;
    u32 vcountBusyStart;
    u16 stallReset;
    u16 stallLimit;
    u16 stallCount;
    u8 numSlots : 7;
    u8 inCallback : 1;
    u8 pad_cb[0x1];
    FSTableReq * tableReqs;     // size [numSlots]
    FSTableReq * tableReqHead;
    FSTableReq * tableReqTail;

    clFS(u8, u32, u16, u32, u32);  // C2 0x0201a9e0
    virtual ~clFS();  // D1 0x0201ab4c D0 0x0201ab94 D2 0x0201abe4

    virtual void run();  // 0x0201ac2c
    void shutdown();  // 0x0201ada4
    u32 getTableSize(s16, const char *, BOOL);  // 0x0201adf8
    FSChunkTable * releaseSlot(u32);  // 0x0201ae68
    void waitTableLoad(u32);  // 0x0201aecc
    u32 getChunkSize(u32, u32);  // 0x0201af08
    u32 clFS::getChunkDataSize(u32, u32);  // 0x0201af24
    FSFileReq * setupReq(FSFileReq *, u32, u8 *, u32, s16); // 0x0201af54
    void clFS::waitReq(FSFileReq *);  // 0x0201af78
    void clFS::cancelReq(FSFileReq *);  // 0x0201b01c
    void waitAll();  // 0x0201b0e4
    BOOL beginOverlayLoad(FSOverlayID, MIProcessor);  // 0x0201b198
    u32 startOverlay();  // 0x0201b250
    bool openSlot(u16, const char *);  // 0x0201b330
    void closeAllFiles();  // 0x0201b390
    virtual bool queueTableLoad(u32, FSChunkTable *, u32, u32, void (*)(FSTableReq *, bool), u32, u32);  // 0x0201b420
    virtual bool pumpTableLoads();  // 0x0201b4b4
    virtual bool queueRead(FSFileReq *, u32, s32, void (*)(FSFileReq *, bool), u32, u32);  // 0x0201b668
    virtual bool pumpReads();  // 0x0201b724
    virtual bool pumpOverlay();  // 0x0201b910
};

class clDPP : public clFS {
public:
    void (*copyFn)(u8 **, void **);
    u8 buffer[0xb20];
    u8 * alignedBuf;
    u8 * cursor;    // +0xc00
    u16 page;
    u8 pad_c06[0x2];
    u32 remaining;

    clDPP(u8, u32, u16, u32, u32);  // C2 0x0201c66c
    virtual ~clDPP() {};  // D1 0x0201c6bc D0 0x0201c6d0 D2 0x0201c6ec

    u32 readStreamHeader(FSFileReqEx *, s16);  // 0x0201c700
    void queueStream(FSFileReqEx *, u32 *, void (*)(FSFileReq *, bool), u32);  // 0x0201c7a0
    void cancelStream(FSFileReq *);  // 0x0201c890
    virtual bool queueRead(FSFileReq *, u32, s32, void (*)(FSFileReq *, bool), u32, u32);  // 0x0201c8a4
    void readSync(FSFileReqEx *, u32);  // 0x0201c8d8
    virtual bool pumpReads();  // 0x0201c9ac
    void selectCopyFn(FSFileReqEx *);  // 0x0201ced8
};

typedef u32 (clFS::*ChunkSizeFn)(u32, u32);

class clFSEx : public clDPP {
public:
    ChunkSizeFn * sizeFns;
    FSFileReqEx * reqPool;
    FSFileReqEx * freeList;
    void * reqPoolEnd;
    u32 externTable[2];
    
    clFSEx(u8, u32, u16, u16, u32, u32);  // C2 0x0201ba2c C1 0x0201bb14
    virtual ~clFSEx();  // D1 0x0201bbfc D0 0x0201bca4 D2 0x0201bd54

    bool openArchive(s16, const char *, void (*)(FSTableReq *, bool), u32, BOOL, u32, u32, u32);  // 0x0201bdfc
    void setArchive(s16, const char *, FSChunkTable *, BOOL);  // 0x0201bf34
    u8 * readChunk(s16, u32, u32, u8 *, s32 *, void (*)(FSFileReq *, bool), u32, u32, u32, u32);  // 0x0201c044
    void cancelAll();  // 0x0201c194
    u8 * readChunkStream(s16, u32, u8 *, u32, u8 *, s32 *, void (*)(FSFileReq *, bool), u32, s32, u32);  // 0x0201c1f8
    virtual bool pumpReads();  // 0x0201c2f4
    FSFileReqEx * allocReq();  // 0x0201c344
};

class clDAP : public clTask {
public:
    u8 pad_18[0x10];
    FSFileReqEx * head;
    FSFileReqEx * tail;
    u32 vcountBusyStart;
    u16 stallReset;
    u16 stallLimit;
    u16 stallCount;
    
    clDAP(u8, u32, u32);  // C1 0x0201c360
    virtual ~clDAP() {};  // D1 0x0201c3a8 D0 0x0201c3bc

    virtual void run();  // 0x0201c3d8
    u32 beginJob(FSFileReqEx *);  // 0x0201c4f4
    void queueJob(FSFileReqEx *, u32 *);  // 0x0201c534
    void waitJob(FSFileReq *);  // 0x0201c574
    void pumpJobs();  // 0x0201c5c8
};

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
