#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "types.h"

#ifdef __cplusplus

class clTask {
public:
    u32 field04;
    clTask * next;  // +0x8
    clTask * prev;  // +0xc
    u8 field10;
    u8 priority;  // +0x11
    u16 enabled : 1;  // +0x12
    u16 hasRan : 1;
    s16 rate : 4;
    u16 counter : 4;
    u32 numRuns;  // +0x14

    clTask(u8, u32, u32);  // C2 0x0201a39c
    virtual ~clTask();  // D1 0x0201a49c D0 0x0201a508 D2 0x0201a588

    void disable(void);  // 0x0201a5f4
    void enable(void);  // 0x0201a610
};

class clVBlankTask {
public:
    u32 field04;
    clVBlankTask * next;  // +0x8
    clVBlankTask * prev;  // +0xc
    u8 field10;
    u8 priority;  // +0x11
    u8 enabled : 1;  // +0x12
    u8 hasRan : 1;
    u8 pad_13[0x1];
    u32 numRuns;  // +0x14

    clVBlankTask(u8, u32, u32);  // C1 0x0201a69c
    virtual ~clVBlankTask();  // D1 0x0201a794 D0 0x0201a800 D2 0x0201a880

    void enable(void);  // 0x0201a8ec
};

typedef struct {
    u8 vblCount;  // +0x0
    u8 taskCount;  // +0x1
    u8 pad_02[0x2];
    clTask * taskCur;  // +0x4
    clTask * taskNext;  // +0x8
    clVBlankTask * vblCur;  // +0xc
    clVBlankTask * vblNext;  // +0x10
    clTask * taskHead;  // +0x14
    clVBlankTask * vblHead;  // +0x18
} TaskManager;

extern TaskManager taskMgr;

extern "C" {
#else
typedef struct clVBlankTask clVBlankTask;
#endif // __cplusplus

void disableAllTasks(void);  // 0x0201a630
void resetTasks(void);  // 0x0201a67c
void disableAllVBlankTasks(void);  // 0x0201a91c
void resetVBlankTasks(void);  // 0x0201a974
void deleteVBlankTask(clVBlankTask *);  // 0x0201a9a4

#ifdef __cplusplus
} // extern "C"
#endif

#endif
