// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/yanaCtrl.h"

#include "dtcm/yanaWork.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

// --- Function Declarations --- //

clYanaUnitSys * findOwner(void *);  // 0x020204c8

void nullHandler(clYanaCtrl *);  // 0x020206c8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void * operator new(size_t size, clYanaUnitSys * pool) {
    return pool->acquire();
}

// MATCHED 100%
void inYanaUnit::operator delete(void * block) {
    if (block != NULL) {
        clYanaUnitSys * owner = findOwner(block);
        if (owner != NULL) {
            owner->release(block);
        }
    }
}

// MATCHED 100%
clYanaUnitSys * findOwner(void * addr) {
    clYanaUnitSys * sys = unitSysHead;
    while (TRUE) {
        if (sys == NULL) {
            return NULL;
        }

        if (sys->buffer <= addr && addr < sys->bufferEnd) {
            return sys;
        }

        sys = sys->next;
    }
}

// MATCHED 100%
void clYanaCtrlBase::moveToList(clYanaCtrlSys * param1) {
    if (param1 != NULL) {
        clYanaCtrlBase * prev = this->prev;
        if (this->prev != NULL) {
            this->prev = NULL;
            clYanaCtrlBase * next = this->next;
            if (next != NULL) {
                next->prev = prev;
            }
            prev->next = next;
        }

        clYanaCtrlBase * base = (clYanaCtrlBase *)((u8 *)param1 - 4);
        clYanaCtrlBase * next = base->next;
        if (next != NULL) {
            next->prev = this;
        }
        base->next = this;
        this->prev = base;
        this->next = next;
    }
}

// MATCHED 100%
void clYanaCtrlBase::detach() {
    clYanaCtrlBase * prev = this->prev;
    if (prev != NULL) {
        this->prev = NULL;
        clYanaCtrlBase * next = this->next;
        if (next != NULL) {
            next->prev = prev;
        }
        prev->next = next;
    }
}

// MATCHED 100%
clYanaCtrlBase::~clYanaCtrlBase() {
    clYanaCtrlBase * prev = this->prev;
    if (prev != NULL) {
        this->prev = NULL;
        clYanaCtrlBase * next = this->next;
        if (next != NULL) {
            next->prev = prev;
        }
        prev->next = next;
    }
}

// MATCHED 100%
void clYanaCtrlSys::func_02020608() {
    clYanaCtrlBase * node = this->field00;

    while (node != NULL) {
        clYanaCtrlBase * next = node->next;
        node->update();
        node = next;
    }
}

// MATCHED 100%
void clYanaCtrlSys::func_02020634() {
    clYanaCtrlBase * node = this->field00;
    while (node != NULL) {
        if (node != NULL) {
            delete node;
        }

        node = this->field00;
    }
}

// MATCHED 100%
void clYanaCtrl::update() {
    if (this->handler != NULL) {
        this->handler(this);
    }

    if (this->handler == NULL) {
        delete this;
    }
}

// MATCHED 100%
void nullHandler(clYanaCtrl *) {}
