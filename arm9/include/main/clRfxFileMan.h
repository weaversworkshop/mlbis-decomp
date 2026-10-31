#ifndef MAIN_CL_RFX_FILE_MAN_H
#define MAIN_CL_RFX_FILE_MAN_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
    #include "main/fs.h"
}

typedef struct {
    u8 wBlocks;  // +0x0
    u8 hBlocks;  // +0x1
    u8 bpp;  // +0x2
    u8 fmt;  // +0x3
} RfxTexHeader;

typedef struct {
    u8 pad_00[0x4];
    u8 data[];  // +0x4
} RfxPlttHeader;

typedef struct {
    void * data;  // +0x0
    u16 id;  // +0x4
    u16 owned;  // +0x6
} GenFileEntry;

class clRfxFileMan {
public:
    FSChunkTable * dataTable;  // +0x4
    FSChunkTable * scriptTable;  // +0x8
    struct {
        u32 ownsDataTable : 1;  // +0xc
        u32 ownsScriptTable : 1;
        u32 : 30;
    };

    virtual ~clRfxFileMan();  // D1 0x02021eb8 D0 0x02021f24 D2 0x02021f98

    u16 getScriptLen(u32);  // 0x02022004
};

class clGenFileMan {
public:
    GenFileEntry * entries;  // +0x4
    u16 capacity;  // +0x8
    u16 ownsTable;  // +0xa
    
    virtual ~clGenFileMan();  // D1 0x02022034 D0 0x02022054

    void init(GenFileEntry *, u32, u16);  // 0x0202207c
    void release(void);  // 0x020220c8
    void add(u16, void *, u16);  // 0x02022104
    void * find(u32);  // 0x0202214c
    void clear(void);  // 0x020221a4
};

typedef struct {
    clGenFileMan managerA;  // +0x0
    clGenFileMan managerB;  // +0xc
    GenFileEntry entriesA[128];  // +0x18
    GenFileEntry entriesB[128];  // +0x418
    u16 dirtyA;  // +0x818
    u16 dirtyB;  // +0x81a
} RfxGenPools;

class clTexFileMan {
public:
    RfxGenPools pools;  // +0x0
};

RfxTexHeader * func_020222a4(RfxGenPools *, u32);  // 0x020222a4
RfxPlttHeader * func_020223b0(RfxGenPools *, u32);  // 0x020223b0

#endif // __cplusplus

#endif
