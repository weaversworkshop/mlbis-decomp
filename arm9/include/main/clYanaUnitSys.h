#ifndef MAIN_CL_YANA_UNIT_SYS_H
#define MAIN_CL_YANA_UNIT_SYS_H

#include "types.h"

#ifdef __cplusplus

class clYanaUnitSys {
public:
    clYanaUnitSys * next;  // +0x4
    clYanaUnitSys * prev;  // +0x8
    u16 unitSize;  // +0xc
    u16 ownsBuffer : 1;  // +0xe
    u16 field0e_01 : 15;
    void * freeList;  // +0x10
    void * buffer;  // +0x14
    void * bufferEnd;  // +0x18

    virtual ~clYanaUnitSys();  // D1 0x020202b8 D0 0x020202d8 D2 0x02020300

    void init(u32, s32, void *, s32);  // 0x02020320
    void link();  // 0x020203f0
    void destroy();  // 0x02020418
    void * acquire();  // 0x0202046c
    void release(void *);  // 0x02020480
};

#endif
#endif
