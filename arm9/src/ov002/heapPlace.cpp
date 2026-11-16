// objdiff-target: build/delinks/_dsd_gap@ov002_0.o

#include "ov002/heapPlace.h"

#include "main/heap.h"

extern "C" {

// --- Function Declarations --- //

HeapBlock * heapAllocAt(s32, u32, HeapBlock *, BOOL);  // 0x02069820
HeapBlock * heapClaimBlock(HeapBlock *, s32, u32, BOOL);  // 0x02069a88
void heapSetFrontier(HeapBlock *);  // 0x02069bdc
void heapResetFrontier(s32);  // 0x02069c98

} // extern "C"

void * operator new(size_t, HeapBlock *, s32, BOOL);  // ov002 0x02069bbc

// --- Function Definitions --- //

// MATCHED 100%
#pragma optimization_level 2
HeapBlock * heapAllocAt(s32 heapId, u32 size, HeapBlock * ptr, BOOL flag) {
    if (size != 0) {
        s32 dir = heapId & 1;
        size = (size + 3) & ~3;

        HeapBlock * block = heaps[heapId >> 1].bounds[1][0];
        HeapBlock * frontier = block+1;

        if (frontier < ptr && (u8 *)heaps[heapId >> 1].bounds[1][1] > (u8 *)ptr + size) {
            frontier = heaps[heapId >> 1].bounds[1][dir];

            if (dir == 0) {
                heapClaimBlock(frontier, heapId, (u8 *)ptr - (u8 *)frontier - 2 * sizeof(HeapBlock), 0);
                frontier->free = 1;
                return heapClaimBlock(frontier->next, heapId, size, flag);
            } else {
                heapClaimBlock(frontier->prev, heapId, (u8 *)frontier - (u8 *)ptr - size - sizeof(HeapBlock), 0);
                HeapBlock * prev = frontier->prev;
                prev->free = 1;
                return heapClaimBlock(prev->prev, heapId, size, flag);
            }
        }

        if (ptr == frontier) {
            if (block->free != 0 && size <= block->size) {
                return heapClaimBlock(block, heapId & ~1, size, flag);
            }
        } else {
            u8 * end = (u8 *)ptr + size;
            frontier = heaps[heapId >> 1].bounds[1][1];

            if ((u8 *)frontier == end) {
                block = frontier->prev;

                if (block->free != 0 && size <= block->size) {
                    return heapClaimBlock(block, heapId | 1, size, flag);
                }
            } else {
                HeapBlock * node = heaps[heapId >> 1].bounds[0][dir];
                if (dir != 0) {
                    node = node->prev;
                }

                BOOL searchFwd = (dir == 0);
                HeapBlock * header = GET_HEADER(ptr);
                do {
                    u32 blockSize = node->size;
                    if (header > node) {
                        if (end < (u8 *)node->next) {
                            if (node->free == 0 || blockSize < size) {
                                break;
                            }

                            if (dir == 0) {
                                heapClaimBlock(node, heapId, (u8 *)ptr - (u8 *)node - 2 * sizeof(HeapBlock), 0);
                                node->free = 1;
                                return heapClaimBlock(node->next, heapId, size, flag);
                            } else {
                                heapClaimBlock(node, heapId, (u8 *)node->next - (u8 *)ptr - size - sizeof(HeapBlock), 0);
                                node->next->free = 1;
                                return heapClaimBlock(node, heapId, size, flag);
                            }
                        }
                    }

                    node = (searchFwd) ? node->next : node->prev;
                } while (node != NULL);
            }
        }
    }

    return NULL;
}
#pragma optimization_level reset

// MATCHED 100%
HeapBlock * heapClaimBlock(HeapBlock * block, s32 heapId, u32 size, BOOL unused) {
    s32 slot = heapId >> 1;
    u32 blockSize = block->size;

    if (size + sizeof(HeapBlock) < block->size) {
        if ((heapId & 1) == 0) {
            HeapBlock * rest = (HeapBlock *)((u8 *)block + sizeof(HeapBlock) + size);

            rest->sizeFree = (blockSize - (size + sizeof(HeapBlock))) | 0x2000000;
            rest->prev = block;
            HeapBlock * next = block->next;
            rest->next = next;

            if (next != NULL) {
                next->prev = rest;
            }

            block->sizeFree = size;
            block->next = rest;

            if (heaps[slot].bounds[1][0] < rest) {
                heaps[slot].bounds[1][0] = rest;
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

            base->sizeFree = (blockSize - (size + sizeof(HeapBlock))) | 0x2000000;
            base->next = block;

            if (heaps[slot].bounds[1][1] > block) {
                heaps[slot].bounds[1][1] = block;
            }
        }
    } else {
        block->sizeFree = blockSize;
    }

    block->heapId = heapId;

    return block+1;
}

// MATCHED 100%
void * operator new(size_t size, HeapBlock * ptr, s32 heapId, BOOL flag) {
    return heapAllocAt(heapId, size, ptr, flag);
}

// MATCHED 100%
void heapSetFrontier(HeapBlock * ptr) {
    if (ptr != NULL) {
        HeapBlock * block = ptr-1;
        Heap * heap = &heaps[block->slot];

        if (block->dir) {
            heap->bounds[1][1] = block;
            heap->bounds[1][0]->next = block;

            heap->bounds[1][0]->sizeFree = ((u32)heap->bounds[1][1] - (u32)heap->bounds[1][0] - sizeof(HeapBlock)) | 0x2000000;
            heap->bounds[1][1]->prev = heap->bounds[1][0];
        } else {
            HeapBlock * next = block->next;
            heap->bounds[1][0] = next;
            heap->bounds[1][1]->prev = next;

            heap->bounds[1][0]->sizeFree = ((u32)heap->bounds[1][1] - (u32)heap->bounds[1][0] - sizeof(HeapBlock)) | 0x2000000;
            heap->bounds[1][0]->next = heap->bounds[1][1];
        }
    }
}

// MATCHED 100%
void heapResetFrontier(s32 heapId) {
    Heap * heap = &heaps[heapId >> 1];

    if (heapId & 1) {
        HeapBlock * bound = heap->bounds[0][1];
        heap->bounds[1][1] = bound;
        heap->bounds[1][0]->next = bound;

        heap->bounds[1][0]->sizeFree = ((u32)heap->bounds[1][1] - (u32)heap->bounds[1][0] - sizeof(HeapBlock)) | 0x2000000;
        heap->bounds[1][1]->prev = heap->bounds[1][0];
    } else {
        HeapBlock * bound = heap->bounds[0][0];
        heap->bounds[1][0] = bound;
        heap->bounds[1][1]->prev = bound;

        heap->bounds[1][0]->sizeFree = ((u32)heap->bounds[1][1] - (u32)heap->bounds[1][0] - sizeof(HeapBlock)) | 0x2000000;
        heap->bounds[1][0]->next = heap->bounds[1][1];
    }
}
