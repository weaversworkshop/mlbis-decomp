#ifndef MAIN_HEAP_H
#define MAIN_HEAP_H

#include "types.h"

#define DTCM_STACK_RESERVE  0x1000

#define HEAP_MAIN_LO  0
#define HEAP_MAIN_HI  1

typedef struct HeapBlock {
    struct HeapBlock * prev;  // +0x0
    struct HeapBlock * next;  // +0x4
    union {
        struct {
            u32 size : 25;  // +0x8
            u32 free : 1;
            u32 heapId : 6;
        };
        struct {
            u32 sizeFree : 26;  // +0x8
        };
    };
} HeapBlock;

#define GET_HEADER(ptr) ((HeapBlock*)((u8*)ptr - 0xc))

typedef struct {
    HeapBlock * bounds[2][2];  // +0x0
    u32 size : 27;  // +0x10
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
#include <nitro.h>
}

void * operator new(size_t, u32, void *, BOOL);  // 0x0201a138
void * operator new[](size_t, u32, void *, BOOL);  // 0x0201a150
void operator delete(void *);  // 0x0201a168
void operator delete[](void *);  // 0x0201a174

#endif

#endif
