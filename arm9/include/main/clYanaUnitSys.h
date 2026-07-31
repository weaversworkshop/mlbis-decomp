#ifndef MAIN_CL_YANA_UNIT_SYS_H
#define MAIN_CL_YANA_UNIT_SYS_H

#include "types.h"

#ifdef __cplusplus

class clYanaUnitSys {
public:
    clYanaUnitSys * next;
    clYanaUnitSys * prev;
    u16 unitSize;
    u16 ownsBuffer : 1;
    u16 field0e_01 : 15;
    void * freeList;
    void * buffer;
    void * bufferEnd;

    virtual ~clYanaUnitSys();  // D1 0x020202b8 D0 0x020202d8 D2 0x02020300

    void init(u32, s32, void *, s32);  // 0x02020320
    void link();  // 0x020203f0
    void destroy();  // 0x02020418
    void * acquire();  // 0x0202046c
    void release(void *);  // 0x02020480
};

#endif
#endif
