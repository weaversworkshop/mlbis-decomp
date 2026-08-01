// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clVCountTask.h"

extern "C" {

#include <nitro.h>

// --- Extern Functions --- //    

extern void func_01ff8a38(void);

}
// --- Variable Definitions --- //

VCountTaskManager vcountTaskManager;

// --- Function Definitions --- //

// MATCHED 100%
clVCountTask::clVCountTask(s32 priority, u8 id, u32 param3, u32 arg) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    clVCountTask * task = vcountTaskManager.next;
    if (task == NULL) {
        vcountTaskManager.next = this;
        vcountTaskManager.prev = this;
        this->prev = NULL;
        this->next = NULL;
    } else {
        while (TRUE) {
            if (task->priority > priority || (task->priority == priority && task->id > id)) {
                clVCountTask * prev = task->prev;

                if (prev == NULL) {
                    vcountTaskManager.next = this;
                    this->prev = NULL;
                } else {
                    prev->next = this;
                    this->prev = task->prev;
                }
                
                this->next = task;
                task->prev = this;
                break;
            } else {
                clVCountTask * next = task->next;

                if (next == NULL) {
                    task->next = this;
                    this->prev = task;
                    this->next = NULL;
                    break;
                }

                task = next;
            }
        }
    }

    this->arg = arg;
    this->priority = priority;
    this->field14 = 0;
    this->id = id;
    this->enabled = 0;
    this->pending = 1;
    this->field18 = 0;

    vcountTaskManager.count++;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
clVCountTask::~clVCountTask() {
    if (this->next != NULL) {
        this->next->prev = this->prev;
    }

    if (this->prev == NULL) {
        vcountTaskManager.next = this->next;
    } else {
        this->prev->next = this->next;
    } 

    vcountTaskManager.count--;

    if (vcountTaskManager.cursor == this) {
        vcountTaskManager.cursor = NULL;
    }

    if (vcountTaskManager.prev == this) {
        vcountTaskManager.prev = this->next;
    }
}

// MATCHED 100%
void clVCountTask::disable() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    this->enabled = 0;
    this->pending = 1;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void clVCountTask::enable() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    this->enabled = 1;
    this->pending = 1;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void disableVCountTasks() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    clVCountTask * next = vcountTaskManager.next;
    while (next != 0) {
        if (next->id != 0 && next->id != (u8)-1) {
            next->enabled = 0;
            next->pending = 1;
        }

        next = next->next;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void initVCountTaskMgr() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    vcountTaskManager.count = 0;
    vcountTaskManager.next = NULL;
    vcountTaskManager.cursor = NULL;
    vcountTaskManager.prev = NULL;
    vcountTaskManager.field0c = 0;

    GX_VCountEqIntr(FALSE);

    OS_DisableIrqMask(OS_IE_V_COUNT);
    OS_SetIrqFunction(OS_IE_V_COUNT, func_01ff8a38);
    GX_SetVCountEqVal(GX_LCD_SIZE_Y);

    GX_VCountEqIntr(TRUE);

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void deleteVCountTask(clVCountTask * task) {
    if (task) {
        OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);
        delete task;
        OS_EnableIrqMask(mask);
    }
}
