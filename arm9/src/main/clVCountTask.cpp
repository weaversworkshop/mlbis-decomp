// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clVCountTask.h"

#include "itcm/mainLoop.h"

extern "C" {

#include <nitro.h>

}

// --- Variable Definitions --- //

u8 vcountCount;  // 0x0205bad4
clVCountTask * vcountHead;  // 0x0205badc
clVCountTask * vcountCur;  // 0x0205bae8
clVCountTask * vcountNext;  // 0x0205bad8
void (*vcountHook)(void);  // 0x0205bae0
s32 vcountHookLine;  // 0x0205bae4

// --- Function Definitions --- //

// MATCHED 100%
clVCountTask::clVCountTask(s32 line, u8 priority, u32 param3, void * arg) {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    clVCountTask * task = vcountHead;
    if (task == NULL) {
        vcountHead = this;
        vcountNext = this;
        this->prev = NULL;
        this->next = NULL;
    } else {
        while (TRUE) {
            if (task->line > line || (task->line == line && task->priority > priority)) {
                clVCountTask * prev = task->prev;

                if (prev == NULL) {
                    vcountHead = this;
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
    this->line = line;
    this->field14 = 0;
    this->priority = priority;
    this->enabled = 0;
    this->hasRan = 1;
    this->numRuns = 0;

    vcountCount++;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
clVCountTask::~clVCountTask() {
    if (this->next != NULL) {
        this->next->prev = this->prev;
    }

    if (this->prev == NULL) {
        vcountHead = this->next;
    } else {
        this->prev->next = this->next;
    } 

    vcountCount--;

    if (vcountCur == this) {
        vcountCur = NULL;
    }

    if (vcountNext == this) {
        vcountNext = this->next;
    }
}

// MATCHED 100%
void clVCountTask::disable() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    this->enabled = 0;
    this->hasRan = 1;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void clVCountTask::enable() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    this->enabled = 1;
    this->hasRan = 1;

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void disableVCountTasks() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    clVCountTask * next = vcountHead;
    while (next != 0) {
        if (next->priority != 0 && next->priority != (u8)-1) {
            next->enabled = 0;
            next->hasRan = 1;
        }

        next = next->next;
    }

    OS_EnableIrqMask(mask);
}

// MATCHED 100%
void initVCountTaskMgmt() {
    OSIrqMask mask = OS_DisableIrqMask(OS_IE_V_COUNT);

    vcountCount = 0;
    vcountHead = NULL;
    vcountCur = NULL;
    vcountNext = NULL;
    vcountHook = 0;

    GX_VCountEqIntr(FALSE);

    OS_DisableIrqMask(OS_IE_V_COUNT);
    OS_SetIrqFunction(OS_IE_V_COUNT, vcountIntr);
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

// not in ROM; seems required to keep vcountHookLine pooled
void setVcountHook(void (*hook)(void), s32 line) {  // stripped
    vcountHook = hook;
    vcountHookLine = line;
}
