#ifndef MAIN_CL_YANA_TRANSFER_MANAGER_H
#define MAIN_CL_YANA_TRANSFER_MANAGER_H

#include "types.h"

#ifdef __cplusplus

typedef struct YanaTransEntry YanaTransEntry;
typedef void (*YanaTransFn)(YanaTransEntry *);

struct YanaTransEntry {
    YanaTransFn fn;  // +0x0
    void * src;  // +0x4
    void * dst;  // +0x8
    union {
        struct { 
            u32 size;  // +0xc
            u32 arg;  // +0x10
        } raw;
        struct { 
            u16 rowBytes : 12;
            u16 rows : 4;
            u16 srcStride;  // +0xe
            u16 dstStride;  // +0x10
        } strided;
        struct {
            u16 widthTiles;  // +0xc
            u16 rows;  // +0xe
            u16 dstStrideTiles;  // +0x10
            u16 srcStrideTiles;  // +0x12
        } tiles;
        struct {
            u32 size;  // +0xc
            u16 sub : 1;
            u16 : 15;
        } pltt;
    };
};

class clYanaTransMan {
public:
    YanaTransEntry * entries;  // +0x4
    u16 head;  // +0x8
    u16 tail;  // +0xa
    u16 capacity;  // +0xc
    vu16 texMode : 1;  // +0xe
    vu16 ownsBuffer : 1;
    vu16 locked : 1;
    vu16 flushBlocked : 1;
    vu16 : 12;

    virtual ~clYanaTransMan();  // D1 0x0201f5ac D0 0x0201f5cc D2 0x0201f5f4

    void init(u32, u16, YanaTransEntry *, s32);  // 0x0201f614
    void destroy(void);  // 0x0201f6cc
    void flush(void);  // 0x0201f708
    void post(void (*)(YanaTransEntry *));  // 0x0201f7ec
    void post1(void (*)(YanaTransEntry *), void *);  // 0x0201f810
    void post2(void (*)(YanaTransEntry *), void *, void *);  // 0x0201f838
    void post3(void (*)(YanaTransEntry *), void *, void *, u32);  // 0x0201f864
    void post4(void (*)(YanaTransEntry *), void *, void *, u32, u32);  // 0x0201f898
    void postCopy(void *, void *, u32);  // 0x0201f8d4
    void postCopyStrided(void *, void *, u16, u16, u16, u16);  // 0x0201f90c
    void postTilesToLinear4bpp(void *, void *, u16, u16, u16, u16);  // 0x0201f984
    void postTilesToLinear8bpp(void *, void *, u16, u16, u16, u16);  // 0x0201f9d4
    void postLinearToTiles4bpp(void *, void *, u16, u16, u16, u16);  // 0x0201fa24
    void postResChunk(void *, void *, u32, u16);  // 0x0201fa74
    void postObjExtPltt(void *, void *, u32, u32);  // 0x0201fac8
    YanaTransEntry * beginEntry(void);  // 0x0201fb20
    void endEntry(void);  // 0x0201fb58

    virtual void submit(YanaTransEntry *);  // 0x0201fb68
};

#endif
#endif
