#ifndef MAIN_CL_RFX_FILE_MAN_H
#define MAIN_CL_RFX_FILE_MAN_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
    #include "main/fs.h"
}

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

#endif // __cplusplus

#endif
