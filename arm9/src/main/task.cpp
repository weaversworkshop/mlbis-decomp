// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/task.h"

#include <nitro.h>

// --- Variable Definitions --- //

u8 vblCount;  // 0x0205ba70
u8 taskCount;  // 0x0205ba71
clTask * taskCur;  // 0x0205ba74
clTask * taskHead;  // 0x0205ba84
clTask * taskNext;  // 0x0205ba78
clVBlankTask * vblHead;  // 0x0205ba88
clVBlankTask * vblCur;  // 0x0205ba7c
clVBlankTask * vblNext;  // 0x0205ba80

// --- Function Definitions --- //

// MATCHED 100%
clTask::clTask(u8 priority, u32 param2, u32 param3) {
    clTask * task = taskHead;

    if (task == NULL) {
        taskHead = this;
        this->prev = NULL;
        this->next = NULL;
    } else {
        while (TRUE) {
            if (task->priority > priority) {
                if (task->prev == NULL) {
                    taskHead = this;
                    this->prev = NULL;
                } else {
                    task->prev->next = this;
                    this->prev = task->prev;
                }

                this->next = task;
                task->prev = this;
                break;
            }

            if (task->next == NULL) {
                task->next = this;
                this->prev = task;
                this->next = NULL;
                break;
            }

            task = task->next;
        }
    }

    this->field04 = param3;
    this->field10 = 0;
    this->priority = priority;
    this->enabled = 1;
    this->hasRan = 1;
    this->rate = 1;
    this->counter = 0;
    this->numRuns = 0;

    taskCount++;
}

// MATCHED 100%
clTask::~clTask() {
    if (this->next != NULL) {
        this->next->prev = this->prev;
    }

    if (this->prev == NULL) {
        taskHead = this->next;
    } else {
        this->prev->next = this->next;
    }

    taskCount--;

    if (taskCur == this) {
        taskCur = NULL;
    }

    if (taskNext == this) {
        taskNext = this->next;
    }
}

// MATCHED 100%
void clTask::disable() {
    this->enabled = 0;
    this->hasRan = 1;
}

// MATCHED 100%
void clTask::enable() {
    this->enabled = 1;
    this->hasRan = 1;
}

// MATCHED 100%
void disableAllTasks() {
    clTask * task = taskHead;

    while (task != NULL) {
        u8 r0 = task->priority;
        if (task->priority != 0 && task->priority != 0xff) {
            task->enabled = 0;
            task->hasRan = 1;
        }
        task = task->next;
    }
}

// MATCHED 100%
void resetTasks() {
    taskCount = 0;
    taskHead = NULL;
    taskCur = NULL;
    taskNext = NULL;
}

// MATCHED 100%
clVBlankTask::clVBlankTask(u8 param1, u32 param2, void * param3) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    clVBlankTask * task = vblHead;

    if (task == NULL) {
        vblHead = this;
        this->prev = NULL;
        this->next = NULL;
    } else {
        while (TRUE) {
            if (task->priority > param1) {
                if (task->prev == NULL) {
                    vblHead = this;
                    this->prev = NULL;
                } else {
                    task->prev->next = this;
                    this->prev = task->prev;
                }

                this->next = task;
                task->prev = this;
                break;
            }

            if (task->next == NULL) {
                task->next = this;
                this->prev = task;
                this->next = NULL;
                break;
            }

            task = task->next;
        }
    }

    this->field04 = param3;
    this->field10 = 0;
    this->priority = param1;
    this->enabled = 0;
    this->hasRan = 1;
    this->numRuns = 0;

    vblCount++;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
clVBlankTask::~clVBlankTask() {
    if (this->next != NULL) {
        this->next->prev = this->prev;
    }

    if (this->prev == NULL) {
        vblHead = this->next;
    } else {
        this->prev->next = this->next;
    }

    vblCount--;

    if (vblCur == this) {
        vblCur = NULL;
    }

    if (vblNext == this) {
        vblNext = this->next;
    }
}

// MATCHED 100%
void clVBlankTask::enable() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    this->enabled = 1;
    this->hasRan = 1;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void disableAllVBlankTasks() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);
    clVBlankTask * task = vblHead;

    while (task != NULL) {
        if (task->priority != 0 &&  task->priority != 0xff) {
            task->enabled = 0;
            task->hasRan = 1;
        }
        task = task->next;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void resetVBlankTasks() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    vblCount = 0;
    vblHead = NULL;
    vblCur = NULL;
    vblNext = NULL;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void deleteVBlankTask(clVBlankTask * task) {
    if (task != NULL) {
        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

        delete task;

        OS_EnableIrqMask(mask);
    }
}

} // extern "C"
