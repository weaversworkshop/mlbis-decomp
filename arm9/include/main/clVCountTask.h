#ifndef MAIN_CL_VCOUNT_TASK_H
#define MAIN_CL_VCOUNT_TASK_H

#include "types.h"

#ifdef __cplusplus

class clVCountTask {
public:
    u32 arg;  // +0x4
    clVCountTask * prev;  // +0x8
    clVCountTask * next;  // +0xc
    s32 line;  // +0x10
    u8 field14;
    u8 priority;  // +0x15
    struct {
        u8 enabled : 1;  // +0x16
        u8 hasRan : 1;
        u8 : 6;
    };
    u8 pad_17[0x1];
    u32 numRuns;  // +0x18

    clVCountTask(s32 priority, u8 id, u32 param3, u32 arg);  // C1 0x0201e5ec
    virtual ~clVCountTask();  // D1 0x0201e700 D0 0x0201e76c D2 0x0201e7ec
    virtual void func_ov003_02072638(void) = 0;

    void disable(void);  // 0x0201e858
    void enable(void);  // 0x0201e884
};

extern "C" {
#else

typedef struct clVCountTask {
public:
    void * vtable;
    u32 arg;  // +0x4
    clVCountTask * prev;  // +0x8
    clVCountTask * next;  // +0xc
    s32 priority;  // +0x15
    u8 field14;
    u8 id;  // +0x15
    struct {
        u8 enabled : 1;  // +0x16
        u8 hasRan : 1;
        u8 : 6;
    };
    u8 pad_17[0x1];
    u32 field18;
} clVCountTask;

#endif // __cplusplus

extern u8 vcountCount;
extern clVCountTask * vcountNext;
extern clVCountTask * vcountHead;
extern void (*vcountHook)(void);
extern s32 vcountHookLine;
extern clVCountTask * vcountCur;

void disableVCountTasks(void);  // 0x0201e8b4
void initVCountTaskMgmt(void);  // 0x0201e90c
void deleteVCountTask(clVCountTask * task);  // 0x0201e98c

#ifdef __cplusplus
} // extern "C"
#endif

#endif
