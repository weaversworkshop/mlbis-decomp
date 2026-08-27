#ifndef MAIN_CL_VCOUNT_TASK_H
#define MAIN_CL_VCOUNT_TASK_H

#include "types.h"

#ifdef __cplusplus

class clVCountTask {
public:
    u32 arg;  // +0x4
    clVCountTask * prev;  // +0x8
    clVCountTask * next;  // +0xc
    s32 priority;  // +0x10
    u8 field14;
    u8 id;  // +0x15
    u8 enabled : 1;  // +0x16
    u8 pending : 1;
    u8 : 6;
    u8 pad_17[0x1];
    u32 field18;

    clVCountTask(s32 priority, u8 id, u32 param3, u32 arg);  // C1 0x0201e5ec
    virtual ~clVCountTask();  // D1 0x0201e700 D0 0x0201e76c D2 0x0201e7ec

    void disable();  // 0x0201e858
    void enable();  // 0x0201e884
};

extern "C" {
#else

typedef struct clVCountTask {
public:
    void * vtable;
    u32 arg;  // +0x4
    clVCountTask * prev;  // +0x8
    clVCountTask * next;  // +0xc
    s32 priority;  // +0x10
    u8 field14;
    u8 id;  // +0x15
    u8 enabled : 1;  // +0x16
    u8 pending : 1;
    u8 : 6;
    u8 pad_17[0x1];
    u32 field18;
} clVCountTask;

#endif // __cplusplus

typedef struct {
    u8 count;  // +0x0
    u8 pad_01[0x3];
    clVCountTask * prev;  // +0x4
    clVCountTask * next;  // +0x8
    u32 field0c;
    u8 pad_10[0x4];
    clVCountTask * cursor;  // +0x14
} VCountTaskManager;

extern VCountTaskManager vcountTaskManager;  // 0x0205bad4

void disableVCountTasks();  // 0x0201e8b4
void initVCountTaskMgr();  // 0x0201e90c
void deleteVCountTask(clVCountTask * task);  // 0x0201e98c

#ifdef __cplusplus
} // extern "C"
#endif

#endif
