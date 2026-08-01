// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/heap.h"

#include <nitro/os.h>

extern "C" {

#include "main/cpu_mem32.h"

// --- Extern Data Variables --- //

extern void SDK_MAIN_ARENA_LO(void);
extern void SDK_IRQ_STACKSIZE(void);
extern void SDK_SYS_STACKSIZE(void);
extern void SDK_AUTOLOAD_ITCM_END(void);
extern void SDK_AUTOLOAD_DTCM_END(void);

// --- Variable Definitions --- //

Heap heaps[32];  // 0x0205b7f0

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void initHeaps() {
    cpu_clear32(0, &heaps, 32 * sizeof(Heap));

    u32 mainLo = ((u32)SDK_MAIN_ARENA_LO + 0x1f) & ~0x1f;
    u32 mainTail = HW_MAIN_MEM_MAIN_END - sizeof(HeapBlock);
    heaps[0].bounds[0][0] = (HeapBlock *)mainLo;
    heaps[0].bounds[0][1] = (HeapBlock *)mainTail;
    
    u32 itcmLo = (u32)SDK_AUTOLOAD_ITCM_END;
    u32 itcmPayloadEnd = HW_ITCM_END - 2 * sizeof(HeapBlock);
    u32 stackBottom = HW_DTCM_IRQ_STACK_END - (u32)SDK_IRQ_STACKSIZE - (u32)SDK_SYS_STACKSIZE;
    u32 itcmSize = itcmPayloadEnd - itcmLo;
    u32 mainSize = mainTail - sizeof(HeapBlock) - mainLo;
    heaps[1].bounds[0][0] = (HeapBlock *)itcmLo;
    u32 itcmTail = HW_ITCM_END - sizeof(HeapBlock);
    heaps[1].bounds[0][1] = (HeapBlock *)itcmTail;

    u32 dtcmLo = (u32)SDK_AUTOLOAD_DTCM_END;
    u32 dtcmTail = stackBottom - DTCM_STACK_RESERVE - sizeof(HeapBlock);
    u32 dtcmSize = stackBottom - DTCM_STACK_RESERVE - dtcmLo;
    heaps[2].bounds[0][0] = (HeapBlock *)dtcmLo;
    heaps[2].bounds[0][1] = (HeapBlock *)dtcmTail;

    u32 sharedLo = HW_MAIN_MEM_SHARED;
    u32 sharedTail = HW_MAIN_MEM_SYSTEM - sizeof(HeapBlock);
    heaps[3].bounds[0][0] = (HeapBlock *)sharedLo;
    heaps[3].bounds[0][1] = (HeapBlock *)sharedTail;

    heaps[0].bounds[1][0] = (HeapBlock *)mainLo;
    heaps[0].bounds[1][1] = (HeapBlock *)mainTail;
    heaps[1].bounds[1][0] = (HeapBlock *)itcmLo;
    heaps[1].bounds[1][1] = (HeapBlock *)itcmTail;
    heaps[2].bounds[1][0] = (HeapBlock *)dtcmLo;
    heaps[2].bounds[1][1] = (HeapBlock *)dtcmTail;
    heaps[3].bounds[1][0] = (HeapBlock *)sharedLo;
    heaps[3].bounds[1][1] = (HeapBlock *)sharedTail;

    heaps[0].size = mainSize;
    heaps[1].size = itcmSize;
    heaps[2].size = dtcmSize - 2 * sizeof(HeapBlock);
    heaps[3].size = HW_MAIN_MEM_SYSTEM - HW_MAIN_MEM_SHARED - 2 * sizeof(HeapBlock);

    heaps[0].slot = 0;
    heaps[1].slot = 1;
    heaps[2].slot = 2;
    heaps[3].slot = 3;

    Heap * d = heaps;
    for (s32 i = 4; i > 0; i--) {
        initHeapBlocks(d);
        d++;
    }
}

// MATCHED 100%
void initHeapBlocks(Heap * heap) {
    HeapBlock * node = heap->bounds[0][0];
    node->prev = NULL;
    node->next = heap->bounds[0][1];
    node->sizeFree = heap->size | 0x2000000;
    node->heapId = heap->slot * 2;

    node = heap->bounds[0][1];
    node->prev = heap->bounds[0][0];
    node->next = NULL;
    node->sizeFree = heap->size;
    node->heapId = heap->slot * 2 + 1;
}

// MATCHED 100%
void * heapAlloc(s32 heapId, u32 size, void * ptr, BOOL useFrontier) {
    s32 dir;
    u32 blockSize;
    s32 tmp;
    u32 searchFwd;
    HeapBlock * block;

    if (size != 0) {
        tmp = heapId / 2;
        dir = heapId & 1;
        if (useFrontier == TRUE) {
            block = heaps[tmp].bounds[1][dir];
        } else {
            block = heaps[tmp].bounds[0][dir];
        }

        if (dir != 0) {
            block = block->prev;
        }

        size = (size + 3) & ~3;

        searchFwd = (dir == 0);
        do {
            blockSize = block->size;
            if (block->free != 0 && blockSize >= size) {
                tmp = size + sizeof(HeapBlock);
                if (blockSize > tmp) {
                    if (dir == 0) {

                        HeapBlock * rest = (HeapBlock *)((u8 *)block + sizeof(HeapBlock) + size);
                        rest->sizeFree = (blockSize - tmp) | 0x2000000;
                        rest->prev = block;

                        HeapBlock * next = block->next;
                        rest->next = next;

                        if (next != NULL) {
                            next->prev = rest;
                        }

                        block->sizeFree = size;
                        block->next = rest;

                        if (heaps[heapId / 2].bounds[1][0] < rest) {
                            heaps[heapId / 2].bounds[1][0] = rest;
                        }
                    } else {
                        HeapBlock * base = block;
                        block = (HeapBlock *)((u8 *)block + (blockSize - size));
                        block->sizeFree = size;
                        block->prev = base;

                        HeapBlock * neighbor = base->next;
                        block->next = neighbor;

                        if (neighbor != NULL) {
                            neighbor->prev = block;
                        }

                        size = blockSize - (size + sizeof(HeapBlock));

                        base->sizeFree = size | 0x2000000;
                        base->next = block;

                        if (heaps[heapId / 2].bounds[1][1] > block) {
                            heaps[heapId / 2].bounds[1][1] = block;
                        }
                    }
                } else {
                    block->sizeFree = blockSize;
                }

                block->free = 0;
                block->heapId = heapId;

                return (void *)((u8 *)block + sizeof(HeapBlock));
            }
            block = (searchFwd != 0) ? block->next : block->prev;
        } while (block != NULL);
    }

    return NULL;
}

// MATCHED 100%
void heapFree(void * ptr) {
    if (ptr == NULL) {
        return;
    }

    HeapBlock * block = GET_HEADER(ptr);
    HeapBlock * prev = block->prev;
    HeapBlock * next = block->next;
    s32 heapId = block->heapId;

    block->free = 1;

    s32 slot = heapId / 2;
    s32 dir = heapId & 1;

    if (next != NULL && next->free != 0) {
        mergeBlocks(block, next);

        if (dir == 0) {
            if (heaps[slot].bounds[1][0] == next) {
                heaps[slot].bounds[1][0] = block;
            }
        }
    }

    if (prev != NULL && prev->free != 0) {
        mergeBlocks(prev, block);

        if (dir == 0) {
            if (heaps[slot].bounds[1][0] == block) {
                heaps[slot].bounds[1][0] = prev;
            }
        } else {
            if (heaps[slot].bounds[1][1] == block) {
                heaps[slot].bounds[1][1] = block->next;
            }
        }
    }
}

// MATCHED 100%
void * operator new(size_t size, u32 heap, void * ptr, u32 flag) {
    return heapAlloc(heap, size, ptr, flag);
}

// MATCHED 100%
void * operator new[](size_t size, u32 heap, void * ptr, u32 flag) {
    return heapAlloc(heap, size, ptr, flag);
}

// MATCHED 100%
void operator delete(void * param1) {
    heapFree(param1);
}

// MATCHED 100%
void operator delete[](void * param1) {
    heapFree(param1);
}

// MATCHED 100%
void mergeBlocks(HeapBlock * block1, HeapBlock * block2) {
    block1->size += block2->size + sizeof(HeapBlock);

    HeapBlock * node = block2->next;
    block1->next = node;
    if (node != NULL) {
        node->prev = block1;
    }
}

// MATCHED 100%
s32 createHeap(s32 heapId, u32 size, void * ptr, BOOL useFrontier) {
    void * region = heapAlloc(heapId, size + 2 * sizeof(HeapBlock), NULL, useFrontier);
    if (region == NULL) {
        return -1;
    }

    Heap * heap = &heaps[4];
    for (s32 slot = 4; slot < 32; slot++) {
        if (heap->bounds[0][0] == NULL) {
            heap->bounds[0][0] = (HeapBlock *)region;
            heap->bounds[1][0] = (HeapBlock *)region;
            heap->bounds[0][1] = (HeapBlock *)((u32)region + size + sizeof(HeapBlock));
            heap->bounds[1][1] = (HeapBlock *)((u32)region + size + sizeof(HeapBlock));

            heap->size = size;
            heap->slot = slot;

            initHeapBlocks(heap);

            return slot*2;
        }

        heap++;
    }

    heapFree(region);
    return -1;
}

// MATCHED 100%
void destroyHeap(s32 heapId) {
    heapFree(heaps[heapId / 2].bounds[0][0]);

    heaps[heapId / 2].bounds[0][0] = NULL;
    heaps[heapId / 2].bounds[0][1] = NULL;
    heaps[heapId / 2].bounds[1][0] = NULL;
    heaps[heapId / 2].bounds[1][1] = NULL;

    heaps[heapId / 2].size = 0;
}

// MATCHED 100%
void setMainHeapBase(void * base) {
    if (base == NULL) {
        base = (void *)(((u32)SDK_MAIN_ARENA_LO + 0x1f) & ~0x1f);
    }

    rebaseHeap(0, base);
}

// MATCHED 100%
void rebaseHeap(u32 slot, void * base) {
    u32 r7 = ((u32)base + 0x1f) & ~0x1f;
    HeapBlock * r6 = heaps[slot].bounds[0][0];
    
    HeapBlock r5 = *r6;

    heaps[slot].bounds[0][0] = (HeapBlock *)r7;
    heaps[slot].bounds[1][0] = (HeapBlock *)r7;

    heaps[slot].size += (u32)r6 - r7;

    *heaps[slot].bounds[0][0] = r5;

    heaps[slot].bounds[0][0]->size += (u32)r6 - r7;
    heaps[slot].bounds[1][1]->prev = heaps[slot].bounds[0][0];
}
