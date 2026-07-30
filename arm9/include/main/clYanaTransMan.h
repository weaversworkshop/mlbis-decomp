#ifndef MAIN_CL_YANA_TRANSFER_MANAGER_H
#define MAIN_CL_YANA_TRANSFER_MANAGER_H

#include "types.h"

#ifdef __cplusplus

typedef struct YanaTransEntry YanaTransEntry;
typedef void (*YanaTransFn)(YanaTransEntry *);

struct YanaTransEntry {
    YanaTransFn fn;
    void * src;
    void * dst;
    union {
        struct { 
            u32 size;
            u32 arg;
        } raw;
        struct { 
            u16 rowBytes : 12;
            u16 rows : 4;
            u16 srcStride;
            u16 dstStride;
        } strided;
        struct {
            u16 widthTiles;
            u16 rows;
            u16 dstStrideTiles;
            u16 srcStrideTiles;
        } tiles;
        struct {
            u32 size;
            u16 sub : 1;
            u16 : 15;
        } pltt;
    };
};

class clYanaTransMan {
public:
    YanaTransEntry * entries;
    u16 head;
    u16 tail;
    u16 capacity;
    vu16 texMode : 1;
    vu16 ownsBuffer : 1;
    vu16 locked : 1;
    vu16 flushBlocked : 1;
    vu16 : 12;

    virtual ~clYanaTransMan();  // D1 0x0201f5ac D0 0x0201f5cc D2 0x0201f5f4

    void init(u32, u16, YanaTransEntry *, s32);  // 0x0201f614
    void destroy();  // 0x0201f6cc
    void flush();  // 0x0201f708
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
    YanaTransEntry * beginEntry();  // 0x0201fb20
    void endEntry();  // 0x0201fb58

    virtual void submit(YanaTransEntry *);  // 0x0201fb68
};

#endif
#endif
