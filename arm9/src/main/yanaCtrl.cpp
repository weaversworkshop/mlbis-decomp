// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/yanaCtrl.h"

extern "C" {

#include <nitro.h>

#include "main/heap.h"

// --- Extern Data Variables --- //

extern clYanaUnitSys * data_027e00e8;

// --- Function Declarations --- //

clYanaUnitSys * findOwner(void *);  // 0x020204c8
void func_02020608(inYanaUnit **);  // 0x02020608
void func_02020634(inYanaUnit **);  // 0x02020634
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
    clYanaUnitSys * sys = data_027e00e8;
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
void inYanaUnit::moveToList(inYanaUnit ** head) {
    if (head != NULL) {
        inYanaUnit * prev = this->prev;
        if (this->prev != NULL) {
            this->prev = NULL;
            inYanaUnit * next = this->next;
            if (next != NULL) {
                next->prev = prev;
            }
            prev->next = next;
        }

        inYanaUnit * base = (inYanaUnit *)((u8 *)head - 4);
        inYanaUnit * next = base->next;
        if (next != NULL) {
            next->prev = this;
        }
        base->next = this;
        this->prev = base;
        this->next = next;
    }
}

// MATCHED 100%
void inYanaUnit::detach() {
    inYanaUnit * prev = this->prev;
    if (prev != NULL) {
        this->prev = NULL;
        inYanaUnit * next = this->next;
        if (next != NULL) {
            next->prev = prev;
        }
        prev->next = next;
    }
}

// MATCHED 100%
clYanaCtrlBase::~clYanaCtrlBase() {
    inYanaUnit * prev = this->prev;
    if (prev != NULL) {
        this->prev = NULL;
        inYanaUnit * next = this->next;
        if (next != NULL) {
            next->prev = prev;
        }
        prev->next = next;
    }
}

// MATCHED 100%
void func_02020608(inYanaUnit ** head) {
    inYanaUnit * node = *head;

    while (node != NULL) {
        inYanaUnit * next = node->next;
        node->func_0202066c();
        node = next;
    }
}

// MATCHED 100%
void func_02020634(inYanaUnit ** head) {
    inYanaUnit * node = *head;
    while (node != NULL) {
        if (node != NULL) {
            delete node;
        }

        node = *head;
    }
}

// MATCHED 100%
void clYanaCtrl::func_0202066c() {
    if (this->handler != NULL) {
        this->handler(this);
    }

    if (this->handler == NULL) {
        delete this;
    }
}

// MATCHED 100%
void nullHandler(clYanaCtrl *) {}
