// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clYanaUnitSys.h"

extern "C" {

#include <nitro.h>

#include "main/heap.h"

// --- Extern Data Variables --- //

extern clYanaUnitSys * data_027e00e8;  // 0x027e00e8

// --- Function Declarations --- //

void func_020204a4(void *);  // 0x020204a4
clYanaUnitSys * findOwner(void *);  // 0x020204c8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
clYanaUnitSys::~clYanaUnitSys() {
    destroy();
}

// MATCHED 100%
void clYanaUnitSys::init(u32 unitSize, s32 unitCount, void * buf, s32 heapId) {
    this->link();

    u32 totalSize;
    u32 stride = (unitSize + 3) & ~3;
    totalSize = stride * unitCount;

    this->ownsBuffer = (buf == NULL) ? 1 : 0;
    this->buffer = (buf == NULL) ? heapAlloc(heapId, totalSize, (void *)"UnitSys Buf", FALSE) : buf;
    this->bufferEnd = (u8 *)this->buffer + totalSize;
    this->unitSize = unitSize;
    this->freeList = this->buffer;
    this->field0e_01 = 0;

    void * ptr = this->freeList;
    for (s32 i = 1; i < unitCount; i++) {
        void * nextBlock = (u8 *)ptr + stride;
        *(void **)ptr = nextBlock;
        ptr = nextBlock;
    }

    *(void **)ptr = NULL;
}

// MATCHED 100%
void clYanaUnitSys::link() {
    clYanaUnitSys * sentinel = (clYanaUnitSys *)((u8 *)&data_027e00e8 - 4);
    clYanaUnitSys * oldHead = sentinel->next;
    if (oldHead != NULL) {
        oldHead->prev = this;
    }
    sentinel->next = this;
    this->prev = sentinel;
    this->next = oldHead;
}

// MATCHED 100%
void clYanaUnitSys::destroy() {
    clYanaUnitSys * prev = this->prev;
    if (prev != NULL) {
        this->prev = NULL;
        clYanaUnitSys * next = this->next;
        if (next != NULL) {
            next->prev = prev;
        }
        prev->next = next;
    }

    if (this->ownsBuffer != 0 && this->buffer != NULL) {
        heapFree(this->buffer);
        this->buffer = NULL;
    }
}

// MATCHED 100%
void * clYanaUnitSys::acquire() {
    void * block = this->freeList;
    this->freeList = *(void **)block;
    return block;
}

// MATCHED 100%
void clYanaUnitSys::release(void * block) {
    if (block != NULL) {
        *(void **)block = this->freeList;
        this->freeList = block;
    }
}

// MATCHED 100%
void * operator new(size_t size, clYanaUnitSys * param2) {
    return param2->acquire();
}

// MATCHED 100%
void func_020204a4(void * block) {
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

