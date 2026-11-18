#ifndef OV008_CL_BTL_SYS_H
#define OV008_CL_BTL_SYS_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaTransMan.h"
#include "main/clYanaUnitSys.h"
#include "main/fs.h"
#include "main/task.h"
#include "main/yanaCtrl.h"

#define BTL_JOB_COUNT 48
#define TEX_TRANS_ENTRY_COUNT 128
#define VBLANK_TRANS_ENTRY_COUNT 32
#define PRO_TRANS_ENTRY_COUNT 8

typedef struct BtlJob {
    void (*fn)(BtlJob *);  // +0x0
    u32 arg0;  // +0x4
    u32 arg1;  // +0x8
    u32 arg2;  // +0xc
} BtlJob;

class clBtlProTransMan : public clYanaTransMan {
public:

    ~clBtlProTransMan();  // D1 0x02063d44 D0 0x02064cac
    virtual void submit(YanaTransEntry *);  // 0x020648c8
};

class clBtlSysVBlank : public clVBlankTask {
public:
    u8 pad_18[0x10];

    clBtlSysVBlank(u8 priority, u32 param2, void * owner) : clVBlankTask(priority, param2, owner) {};
    virtual void run(void);  // 0x02064558
};

class BtlOvySlot {
public:
    s16 current;  // +0x0
    s16 wanted;  // +0x2

    ~BtlOvySlot();  // D1 0x02064b18
    void request(s32);  // 0x02064a58
    void apply(void);  // 0x02064a98
    void unload(void);  // 0x02064aec
};

class BtlOvyQueue {
public:
    BtlOvySlot * pending[16];  // +0x0
    s16 head;  // +0x40
    s16 tail;  // +0x42
    s16 unloadIds[16];  // +0x44
    s16 unloadCount;  // +0x64

    BtlOvyQueue() { this->head = this->tail = this->unloadCount = 0; }
    ~BtlOvyQueue();  // D1 0x02064a0c

    void push(BtlOvySlot *);  // 0x02064910
    BOOL pop(void);  // 0x02064978
    void deferUnload(s16);  // 0x020649ec
};

class clBtlSys : public clFSEx {
public:
    clYanaTransMan texTrans;  // +0xc24
    clYanaTransMan vblankTrans;  // +0xc34
    clBtlProTransMan proTrans;  // +0xc44
    void (*frameFn)(void);  // +0xc54
    void (*exitFn)(void);  // +0xc58
    u32 frameCt;  // +0xc5c
    BtlOvyQueue ovyQueue;  // +0xc60
    YanaTransEntry texTransEntries[TEX_TRANS_ENTRY_COUNT];  // +0xcc8
    YanaTransEntry vblankTransEntries[VBLANK_TRANS_ENTRY_COUNT];  // +0x16c8
    YanaTransEntry proTransEntries[PRO_TRANS_ENTRY_COUNT];  // +0x1948
    BtlJob jobs[BTL_JOB_COUNT];  // +0x19e8
    u16 jobHead;  // +0x1ce8
    u16 jobTail;  // +0x1cea
    u16 field1cec;
    struct {
        u16 use3D : 1;  // +0x1cee
        u16 field1cee_1 : 1;
        u16 vblankDone : 1;
        u16 inTransfer : 1;
        u16 : 12;
    };
    clVBlankTask * vblankTask;  // +0x1cf0

    clBtlSys(u16, u16);  // C1 0x02063b00
    virtual ~clBtlSys();  // D1 0x02063d58 D0 0x02063e5c D2 0x02063f68
    virtual void run(void);  // 0x02064198
    virtual void vblankFlush(void);  // 0x02064614

    void init3D(void);  // 0x020640a0
    void onVBlank(void);  // 0x0206457c
    BtlJob * queueJob(void (*)(BtlJob *), u32, u32, u32);  // 0x02064638
    u8 * readChunkWait(s16, u16, u8 *, s32 *, void (*)(FSFileReq *, s32), u32, u32, u32);  // 0x0206467c
    u8 * readChunkStreamWait(s16, u16, u8 *, s32 *, void (*)(FSFileReq *, s32), u32, u32);  // 0x02064728
    FSFileReqEx * waitFreeReq(void);  // 0x020647cc
    u32 readStreamSize(u32, u32);  // 0x02064874
    BOOL isOverlayBusy(void);  // 0x020648d4
};

class clBtlMainSys : public clBtlSys {
public:
    s16 field1cf4;
    s16 field1cf6;

    virtual void vblankFlush(void);  // 0x02064c94
    virtual void func_ov008_02064c90(void);  // 0x02064c90
    virtual ~clBtlMainSys();
};

class Unk_Class_28 : public clYanaCtrlBase {
public:
    u8 pad_0c[0x50];

    ~Unk_Class_28();
    virtual void update(void);
};

class clBtlSound : public clYanaCtrlSys, public clYanaUnitSys {
public:
    u8 pad_24[0x4];
    Unk_Class_28 field28[16];

    virtual void func_ov008_02064ca8(void);  // 0x02064ca8
};

void btlDisableVramBanks(void);  // ov008 0x0206406c

extern clBtlMainSys * btlMainSys;

#endif

#endif
