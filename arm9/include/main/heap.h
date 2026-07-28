#ifndef MAIN_HEAP_H
#define MAIN_HEAP_H

#include "types.h"

#define DTCM_STACK_RESERVE  0x1000

typedef struct HeapBlock {
    struct HeapBlock * prev;
    struct HeapBlock * next;
    union {
        struct {
            u32 size : 25;
            u32 free : 1;
            u32 heapId : 6;
        };
        struct {
            u32 sizeFree : 26;
        };
    };
} HeapBlock;

#define GET_HEADER(ptr) ((HeapBlock*)((u8*)ptr - 0xc))

typedef struct {
    HeapBlock * bounds[2][2];
    u32 size : 27;
    u32 slot : 5;
} Heap;

extern Heap heaps[32];

#ifdef __cplusplus
extern "C" {
#endif

void initHeaps(void);  // 0x02019c90
void initHeapBlocks(Heap *);  // 0x02019e10
void * heapAlloc(s32, u32, void *, BOOL);  // 0x02019eac
void heapFree(void *);  // 0x0201a058
void mergeBlocks(HeapBlock *, HeapBlock *);  // 0x0201a180
s32 createHeap(s32, u32, void *, BOOL);  // 0x0201a1c0
void destroyHeap(s32);  // 0x0201a258
void setMainHeapBase(void *);  // 0x0201a2bc
void rebaseHeap(u32, void *);  // 0x0201a2e0

#ifdef __cplusplus
}

void * operator new(size_t, u32, void *, u32);
void * operator new[](size_t, u32, void *, u32);

#endif

#endif
