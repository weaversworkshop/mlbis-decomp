// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/task.h"

#include <nitro/os.h>

// --- Function Declarations --- //

void func_0201a630();
void func_0201a67c();  // 0x0201a67c
void func_0201a974();
void func_0201a9a4(clVBlankTask *task);

// --- Function Definitions --- //

// MATCHED 100%
clTask::clTask(u8 param1, u32 param2, u32 param3) {
    clTask * task = data_0205ba70.taskHead;

    if (task == NULL) {
        data_0205ba70.taskHead = this;
        this->next = NULL;
        this->prev = NULL;
    } else {
        while (TRUE) {
            if (task->priority > param1) {
                if (task->next == NULL) {
                    data_0205ba70.taskHead = this;
                    this->next = NULL;
                } else {
                    task->next->prev = this;
                    this->next = task->next;
                }

                this->prev = task;
                task->next = this;
                break;
            }

            if (task->prev == NULL) {
                task->prev = this;
                this->next = task;
                this->prev = NULL;
                break;
            }

            task = task->prev;
        }
    }

    this->field04 = param3;
    this->field10 = 0;
    this->priority = param1;
    this->field12_00 = 1;
    this->field12_01 = 1;
    this->field12_02 = 1;
    this->field12_06 = 0;
    this->field14 = 0;

    data_0205ba70.taskCount++;
}

// MATCHED 100%
clTask::~clTask() {
    if (this->prev != NULL) {
        this->prev->next = this->next;
    }

    if (this->next == NULL) {
        data_0205ba70.taskHead = this->prev;
    } else {
        this->next->prev = this->prev;
    }

    data_0205ba70.taskCount--;

    if (data_0205ba70.field04 == this) {
        data_0205ba70.field04 = NULL;
    }

    if (data_0205ba70.field08 == this) {
        data_0205ba70.field08 = this->prev;
    }
}

// MATCHED 100%
void clTask::func_0201a5f4() {
    this->field12_00 = 0;
    this->field12_01 = 1;
}

// MATCHED 100%
void clTask::func_0201a610() {
    this->field12_00 = 1;
    this->field12_01 = 1;
}

// MATCHED 100%
void func_0201a630() {
    clTask * task = data_0205ba70.taskHead;

    while (task != NULL) {
        u8 r0 = task->priority;
        if (task->priority != 0 && task->priority != 0xff) {
            task->field12_00 = 0;
            task->field12_01 = 1;
        }
        task = task->prev;
    }
}

// MATCHED 100%
void func_0201a67c() {
    data_0205ba70.taskCount = 0;
    data_0205ba70.taskHead = NULL;
    data_0205ba70.field04 = NULL;
    data_0205ba70.field08 = NULL;
}

// MATCHED 100%
clVBlankTask::clVBlankTask(u8 param1, u32 param2, u32 param3) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    clVBlankTask * task = data_0205ba70.vblHead;

    if (task == NULL) {
        data_0205ba70.vblHead = this;
        this->next = NULL;
        this->prev = NULL;
    } else {
        while (TRUE) {
            if (task->priority > param1) {
                if (task->next == NULL) {
                    data_0205ba70.vblHead = this;
                    this->next = NULL;
                } else {
                    task->next->prev = this;
                    this->next = task->next;
                }

                this->prev = task;
                task->next = this;
                break;
            }

            if (task->prev == NULL) {
                task->prev = this;
                this->next = task;
                this->prev = NULL;
                break;
            }

            task = task->prev;
        }
    }

    this->field04 = param3;
    this->field10 = 0;
    this->priority = param1;
    this->field12_00 = 0;
    this->field12_01 = 1;
    this->field14 = 0;

    data_0205ba70.vblCount++;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
clVBlankTask::~clVBlankTask() {
    if (this->prev != NULL) {
        this->prev->next = this->next;
    }

    if (this->next == NULL) {
        data_0205ba70.vblHead = this->prev;
    } else {
        this->next->prev = this->prev;
    }

    data_0205ba70.vblCount--;

    if (data_0205ba70.field0c == this) {
        data_0205ba70.field0c = NULL;
    }

    if (data_0205ba70.field10 == this) {
        data_0205ba70.field10 = this->prev;
    }
}

// MATCHED 100%
void clVBlankTask::func_0201a8ec() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    this->field12_00 = 1;
    this->field12_01 = 1;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void func_0201a91c() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);
    clVBlankTask * task = data_0205ba70.vblHead;

    while (task != NULL) {
        if (task->priority != 0 &&  task->priority != 0xff) {
            task->field12_00 = 0;
            task->field12_01 = 1;
        }
        task = task->prev;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void func_0201a974() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    data_0205ba70.vblCount = 0;
    data_0205ba70.vblHead = NULL;
    data_0205ba70.field0c = NULL;
    data_0205ba70.field10 = NULL;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void func_0201a9a4(clVBlankTask * task) {
    if (task != NULL) {
        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

        delete task;

        OS_EnableIrqMask(mask);
    }
}

} // extern "C"
