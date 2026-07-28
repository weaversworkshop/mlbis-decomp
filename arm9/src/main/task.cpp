// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/task.h"

#include <nitro/os.h>

// --- Variable Definitions --- //

TaskManager taskMgr;  // 0x0205ba70

// --- Function Definitions --- //

// MATCHED 100%
clTask::clTask(u8 priority, u32 param2, u32 param3) {
    clTask * task = taskMgr.taskHead;

    if (task == NULL) {
        taskMgr.taskHead = this;
        this->next = NULL;
        this->prev = NULL;
    } else {
        while (TRUE) {
            if (task->priority > priority) {
                if (task->next == NULL) {
                    taskMgr.taskHead = this;
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
    this->priority = priority;
    this->enabled = 1;
    this->hasRan = 1;
    this->rate = 1;
    this->counter = 0;
    this->numRuns = 0;

    taskMgr.taskCount++;
}

// MATCHED 100%
clTask::~clTask() {
    if (this->prev != NULL) {
        this->prev->next = this->next;
    }

    if (this->next == NULL) {
        taskMgr.taskHead = this->prev;
    } else {
        this->next->prev = this->prev;
    }

    taskMgr.taskCount--;

    if (taskMgr.taskCur == this) {
        taskMgr.taskCur = NULL;
    }

    if (taskMgr.taskNext == this) {
        taskMgr.taskNext = this->prev;
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
    clTask * task = taskMgr.taskHead;

    while (task != NULL) {
        u8 r0 = task->priority;
        if (task->priority != 0 && task->priority != 0xff) {
            task->enabled = 0;
            task->hasRan = 1;
        }
        task = task->prev;
    }
}

// MATCHED 100%
void resetTasks() {
    taskMgr.taskCount = 0;
    taskMgr.taskHead = NULL;
    taskMgr.taskCur = NULL;
    taskMgr.taskNext = NULL;
}

// MATCHED 100%
clVBlankTask::clVBlankTask(u8 param1, u32 param2, u32 param3) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    clVBlankTask * task = taskMgr.vblHead;

    if (task == NULL) {
        taskMgr.vblHead = this;
        this->next = NULL;
        this->prev = NULL;
    } else {
        while (TRUE) {
            if (task->priority > param1) {
                if (task->next == NULL) {
                    taskMgr.vblHead = this;
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
    this->enabled = 0;
    this->hasRan = 1;
    this->numRuns = 0;

    taskMgr.vblCount++;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
clVBlankTask::~clVBlankTask() {
    if (this->prev != NULL) {
        this->prev->next = this->next;
    }

    if (this->next == NULL) {
        taskMgr.vblHead = this->prev;
    } else {
        this->next->prev = this->prev;
    }

    taskMgr.vblCount--;

    if (taskMgr.vblCur == this) {
        taskMgr.vblCur = NULL;
    }

    if (taskMgr.vblNext == this) {
        taskMgr.vblNext = this->prev;
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
    clVBlankTask * task = taskMgr.vblHead;

    while (task != NULL) {
        if (task->priority != 0 &&  task->priority != 0xff) {
            task->enabled = 0;
            task->hasRan = 1;
        }
        task = task->prev;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void resetVBlankTasks() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_BLANK);

    taskMgr.vblCount = 0;
    taskMgr.vblHead = NULL;
    taskMgr.vblCur = NULL;
    taskMgr.vblNext = NULL;

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
