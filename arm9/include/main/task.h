#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include "types.h"

#ifdef __cplusplus

class clTask {
public:
    u32 field04;
    clTask * next;
    clTask * prev;
    u8 field10;
    u8 priority;
    u16 field12_00 : 1;
    u16 field12_01 : 1;
    s16 field12_02 : 4;
    u16 field12_06 : 4;
    u32 field14;

    clTask(u8 param1, u32 param2, u32 param3);
    virtual ~clTask();

    void func_0201a5f4();
    void func_0201a610();
};

class clVBlankTask {
public:
    u32 field04;
    clVBlankTask * next;
    clVBlankTask * prev;
    u8 field10;
    u8 priority;
    u8 field12_00 : 1;
    u8 field12_01 : 1;
    u8 pad_13[0x1];
    u32 field14;

    clVBlankTask(u8 param1, u32 param2, u32 param3);
    virtual ~clVBlankTask();

    void func_0201a8ec();
};

typedef struct {
    u8 vblCount;
    u8 taskCount;
    u8 pad_02[0x2];
    clTask * field04;
    clTask * field08;
    clVBlankTask * field0c;
    clVBlankTask * field10;
    clTask * taskHead;
    clVBlankTask * vblHead;
} UnkStruct_0205ba70;
extern UnkStruct_0205ba70 data_0205ba70;

#else
typedef struct clVBlankTask clVBlankTask;
#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

void func_0201a630(void);
void func_0201a91c(void);
void func_0201a974(void);
void func_0201a9a4(clVBlankTask * task);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
