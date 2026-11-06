#ifndef MAIN_CL_HANDLE_MAN_H
#define MAIN_CL_HANDLE_MAN_H

#include "types.h"

#ifdef __cplusplus

extern "C" {

typedef struct VramHandle {
    VramHandle * next;  // +0x0
    VramHandle * prev;  // +0x4
    u32 offset;  // +0x8
    u32 size;  // +0xc
    u32 id;  // +0x10
    s16 refCount;  // +0x14
    union {
        u16 attr;  // +0x16
        struct {
            u8 field16b;
            u8 moved;  // +0x17
        };
        struct {
            u8 kind : 2;  // +0x16
            u8 manBank : 1;
            u8 manType : 3;
            u8 manIndex : 2;
        };
    };
} VramHandle;

}

namespace nsObjSys { 
    class clVramManBase;
    class clHandleMan;
}

extern nsObjSys::clHandleMan * handleMan;
extern nsObjSys::clVramManBase * vramManTable[2][4][4];

namespace nsObjSys {

class clVramManBase {
public:
    u8 manBank;  // +0x4
    u8 manType;  // +0x5
    u8 manIndex;  // +0x6
    u8 pad_07[0x1];

    clVramManBase(u8 bank, u8 type, u32 index) {
        this->manBank = bank;
        this->manType = type;
        this->manIndex = index;
        if (index < 4) {
            vramManTable[bank][type][index] = this;
        }
    };

    virtual ~clVramManBase() {};  // D1 0x0202ac4c D0 0x0202ac38
    virtual u32 getVramAddr(VramHandle *) = 0;
    virtual VramHandle * alloc(u32, u32, u32) = 0;
    virtual BOOL release(VramHandle *, s32, u32) = 0;
    virtual u32 compact(u32) = 0;
};

class clHandleMan {
public:
    VramHandle * pool;  // +0x4
    VramHandle * freeList;  // +0x8
    u32 used;  // +0xc

    clHandleMan(u32, u32);  // C1 0x02028820
    virtual ~clHandleMan();  // D1 0x02028878 D0 0x020288bc
    virtual VramHandle * alloc();  // 0x02028908
    virtual void free(VramHandle *);  // 0x02028940
};

class clVramManByteBase : public clVramManBase {
public:
    VramHandle * head[2];  // +0x8
    u32 base;  // +0x10
    u32 size;  // +0x14

    virtual ~clVramManByteBase() {};  // D1 0x020296c0 D0 0x020296ac
    virtual VramHandle * alloc(u32, u32, u32);  // 0x02028950
    virtual BOOL release(VramHandle *, s32, u32);  // 0x0202946c
    virtual u32 compact(u32);  // 0x020295ec
    virtual VramHandle * allocFrom(u32, u32, u32, u32);  // 0x02028c70
    virtual VramHandle * allocAt(u32, u32, u32, u32);  // 0x020291bc
    virtual VramHandle * findShared(u32, u16, u32, u32, s16);  // 0x020294ec

    VramHandle * insert(VramHandle *, VramHandle *, u32, u32, u32, u16);
};

}

#endif // __cplusplus

#endif
