// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clYanaPileMan.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

// --- Function Declarations --- //

u32 getBlockSize(YanaPileEntry *);  // 0x0201fe60

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
u32 getBlockSize(YanaPileEntry * entry) {
    return entry[1].offset - entry[0].offset;
}

// MATCHED 100%
clYanaPileMan::~clYanaPileMan() {
    freeBuffer();
}

// MATCHED 100%
void clYanaPileMan::init(u32 regionSize, u32 capacity, YanaPileEntry * buf, s32 heapId) {
    if (buf == NULL) {
        capacity++;
    }

    this->capacity = capacity;
    this->ownsBuffer = (buf == NULL) ? 1 : 0;
    this->entries = (buf == NULL) ? (YanaPileEntry *)heapAlloc(heapId, capacity * sizeof(YanaPileEntry), (void *)"Pile Buf", FALSE) : buf;

    this->resetPile(0);
    this->resetPile(1);

    this->entries[this->head].offset = 0;
    this->entries[this->tail].offset = regionSize;
}

// MATCHED 100%
void clYanaPileMan::freeBuffer() {
    if (this->ownsBuffer != 0 && this->entries != NULL) {
        heapFree(this->entries);
        this->entries = NULL;
    }
}

// MATCHED 100%
YanaPileEntry * clYanaPileMan::pushBlock(u32 key, u32 size, u32 pile) {
    if (size == 0) {
        return NULL;
    }

    u32 headIdx = this->head;
    u32 lowOffset = this->entries[this->head].offset;
    u32 highOffset = this->entries[this->tail].offset;

    YanaPileEntry * entry;

    if (pile != PILE_LOW) {
        this->tail--;
        entry = &this->entries[this->tail];
        entry->offset = highOffset - size;
    } else {
        this->head++;
        size += lowOffset;
        entry = &this->entries[headIdx];
        if (this->head < this->tail) {
            entry[1].offset = size;
        }
    }

    entry->key = key;
    return entry;
}

// MATCHED 100%
void clYanaPileMan::resetPile(u32 pile) {
    if (this->entries != NULL) {
        if (pile != PILE_LOW) {
            this->tail = this->capacity-1;
        } else {
            this->head = 0;
        }
    }
}

// MATCHED 100%
YanaPileEntry * clYanaPileMan::acquire(u32 key, u32 size, u32 pile) {
    if (size == 0) {
        return NULL;
    }

    YanaPileEntry * ptr1 = this->entries;
    YanaPileEntry * outPtr = NULL;
    YanaPileEntry * ptr2 = &this->entries[this->head];
    while (ptr1 < ptr2) {
        if (key == ptr1->key && key != 0) {
            ptr1->used = 1;
            ptr1->stale = 0;
            ptr1->needsLoad = 0;
            return ptr1;
        }

        if (ptr1->stale != 0 && pile == PILE_LOW && outPtr == NULL) {
            if (size == getBlockSize(ptr1)) {
                outPtr = ptr1;
            }
        }

        ptr1++;
    }

    ptr2 = &this->entries[this->tail];
    ptr1 = &this->entries[this->capacity-1];
    while (ptr2 < ptr1) {
        if (key == ptr2->key && key != 0) {
            ptr2->used = 1;
            ptr2->stale = 0;
            ptr2->needsLoad = 0;
            return ptr2;
        }

        if (ptr2->stale != 0 && pile == PILE_LOW && outPtr == NULL) {
            if (size == getBlockSize(ptr2)) {
                outPtr = ptr2;
            }
        }

        ptr2++;
    }

    if (outPtr != NULL) {
        outPtr->key = key;
    } else {
        outPtr = this->pushBlock(key, size, pile);
    }

    outPtr->used = 1;
    outPtr->stale = 0;
    outPtr->needsLoad = 1;
    return outPtr;
}


void clYanaPileMan::sweep() {
    u32 newHead;
    YanaPileEntry * entries = this->entries;
    if (entries == NULL) {
        return;
    }

    newHead = 0;

    for (s32 i = 0; i < this->head; i++) {
        if (entries->used == 0) {
            entries->stale = 1;
        } else {
            entries->used = 0;
            entries->stale = 0;
            newHead = i + 1;
        }
        entries++;
    }

    this->head = newHead;

    s32 i;
    u32 newTail = this->capacity-1;
    entries = &this->entries[newTail];
    for (i = this->capacity-1; i >= this->tail; i--, entries--) {
        if (entries->used != 0) {
            entries->used = 0;
            entries->stale = 0;
            newTail = i;
        } else {
            entries->stale = 1;
        }
    }

    this->tail = newTail;
}
