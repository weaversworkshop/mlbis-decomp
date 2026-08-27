#ifndef MAIN_CL_YANA_PILE_MANAGER_H
#define MAIN_CL_YANA_PILE_MANAGER_H

#include "types.h"

#ifdef __cplusplus

#define PILE_LOW 0
#define PILE_HIGH 1

typedef struct {
    u32 key;  // +0x0
    u32 offset : 29;  // +0x4
    u32 used : 1;
    u32 stale : 1;
    u32 needsLoad : 1;
} YanaPileEntry;

class clYanaPileMan {
public:
    YanaPileEntry * entries;  // +0x4
    u16 head;  // +0x8
    u16 tail;  // +0xa
    u16 capacity;  // +0xc
    u16 ownsBuffer : 1;  // +0xe

    virtual ~clYanaPileMan();  // D1 0x0201fe7c D0 0x0201fe9c

    void init(u32, u32, YanaPileEntry *, s32);  // 0x0201fec4
    void freeBuffer(void);  // 0x0201ff80
    YanaPileEntry * pushBlock(u32, u32, u32);  // 0x0201ffb0
    void resetPile(u32);  // 0x0202005c
    YanaPileEntry * acquire(u32, u32, u32);  // 0x02020088
    void sweep(void);  // 0x020201f0
};

#endif
#endif
