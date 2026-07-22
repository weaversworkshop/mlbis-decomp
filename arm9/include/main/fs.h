#ifndef MAIN_FS_H
#define MAIN_FS_H

#include "types.h"

#include <nitro/fs.h>

#ifdef __cplusplus

#include "main/task.h"

typedef struct {
    u32 sizesArrOffset;
    u32 offsets[1];
    // u16 sizes[]     trailing dynamic sized u16 array
} FSChunkTable;

#define CHUNK_OFFSET(t, i)  ((t)->offsets[(i)])
#define CHUNK_SIZE(t, i)    ((u16 *)((u8 *)(t) + (t)->sizesArrOffset))[(i)]

typedef struct {
    u8 dmaNo;
    u8 pad_01[0x3];
    void * table;
} FSManager;
extern FSManager data_0205ba8c;

typedef struct FSFileReq {
    FSFileReq * field00;
    void (*fn)(FSFileReq *, bool);
    u32 field08;
    u8 * field0c;
    u8 * field10;
    u32 field14;
    s32 field18;
    s32 field1c;
    u32 field20;
    u16 field24;
    u16 field26;
    u16 field28;
    u16 field2a;
} FSFileReq;

struct FSFileReqEx : FSFileReq {
    void * field2c;
    void * field30;
    u32 field34;
    u16 field38;
};

typedef struct UnkStruct_FieldCC {
    UnkStruct_FieldCC * field00;
    void (*fn)(UnkStruct_FieldCC *, bool);
    u32 field08;
    u32 * field0c;
    u32 field10;
    u32 field14;
    u32 field18;
    u16 field1c;
    u16 field1e;
} UnkStruct_FieldCC;

class clFS : public clTask {
public:
    u8 pad_18[0x10];
    FSFile field28;
    FSOverlayInfo field70;
    u8 * field9c;
    u32 fielda0;
    u32 fielda4;
    u16 fielda8;
    u8 pad_aa[0x2];
    FSFileReq * fieldac;
    FSFileReq * fieldb0;
    FSFile * fieldb4;
    const char ** fieldb8;
    FSChunkTable ** fieldbc;
    u32 fieldc0;
    u16 fieldc4;
    u16 fieldc6;
    u16 fieldc8;
    u8 fieldca_00 : 7;
    u8 fieldca_07 : 1;
    u8 pad_cb[0x1];
    UnkStruct_FieldCC * fieldcc;
    UnkStruct_FieldCC * fieldd0;
    UnkStruct_FieldCC * fieldd4;

    clFS(u8, u32, u16, u32, u32);  // C2 0x0201a9e0
    virtual ~clFS();  // D1 0x0201ab4c D0 0x0201ab94 D2 0x0201abe4

    virtual void run();  // 0x0201ac2c
    void func_0201ada4();  // 0x0201ada4
    u32 func_0201adf8(s16 param1, const char * param2, u32 param3);  // 0x0201adf8
    FSChunkTable * func_0201ae68(u32 param1);  // 0x0201ae68
    void func_0201aecc(u32 param1);  // 0x0201aecc
    u32 func_0201af08(u32 param1, u32 param2);  // 0x0201af08
    u32 clFS::func_0201af24(u32 param1, u32 param2);  // 0x0201af24
    FSFileReq * func_0201af54(FSFileReq * param1, u32 param2, u8 * param3, u32 param4, s16 param5); // 0x0201af54
    void clFS::func_0201af78(FSFileReq * param1);  // 0x0201af78
    void clFS::func_0201b01c(FSFileReq * param1);  // 0x0201b01c
    void func_0201b0e4();  // 0x0201b0e4
    BOOL func_0201b198(FSOverlayID id, MIProcessor target);  // 0x0201b198
    u32 func_0201b250();  // 0x0201b250
    bool func_0201b330(u16 param1, const char * param2);  // 0x0201b330
    void func_0201b390();  // 0x0201b390
    virtual bool queueTableLoad(u32 param1, FSChunkTable * param2, u32 param3, u32 param4, void (*param5)(UnkStruct_FieldCC *, bool), u32 param6, u32 param7);  // 0x0201b420
    virtual bool pumpTableLoads();  // 0x0201b4b4
    virtual bool queueRead(FSFileReq * param1, u32 param2, s32 param3, void (*param4)(FSFileReq *, bool), u32 param5, u32 param6);  // 0x0201b668
    virtual bool pumpReads();  // 0x0201b724
    virtual bool pumpOverlay();  // 0x0201b910
};

class clDPP : public clFS {
public:
    void (*fn_d8)(u8 **, void **);
    u8 buffer[0xb20];
    u8 * alignedBuf;
    u8 * cursor;    // +0xc00
    u16 bufIdx;
    u8 pad_c06[0x2];
    u32 remaining;

    clDPP(u8, u32, u16, u32, u32);  // C2 0x0201c66c
    virtual ~clDPP() {};  // D1 0x0201c6bc D0 0x0201c6d0 D2 0x0201c6ec

    u32 func_0201c700(FSFileReqEx * param1, s16 param2);  // 0x0201c700
    void func_0201c7a0(FSFileReqEx * param1, u32 * param2, void (*param3)(FSFileReq *, bool), u32 param4);  // 0x0201c7a0
    void func_0201c890(FSFileReq * param1);  // 0x0201c890
    virtual bool queueRead(FSFileReq * param1, u32 param2, s32 param3, void (*param4)(FSFileReq *, bool), u32 param5, u32 param6);  // 0x0201c8a4
    void func_0201c8d8(FSFileReqEx * param1, u32 param2);  // 0x0201c8d8
    virtual bool pumpReads();  // 0x0201c9ac
    void func_0201ced8(FSFileReqEx * param1);  // 0x0201ced8
};

typedef u32 (clFS::*ChunkSizeFn)(u32, u32);
static const ChunkSizeFn data_02054c94[4] = {
    &clFS::func_0201af08,
    &clFS::func_0201af24,
    &clFS::func_0201af08,
    &clFS::func_0201af24,
};

class clFSEx : public clDPP {
public:
    ChunkSizeFn * fieldc0c;
    FSFileReqEx * fieldc10;
    FSFileReqEx * fieldc14;
    void * fieldc18;
    u32 fieldc1c[2];
    
    clFSEx(u8, u32, u16, u16, u32, u32);  // C2 0x0201ba2c C1 0x0201bb14
    virtual ~clFSEx();  // D1 0x0201bbfc D0 0x0201bca4 D2 0x0201bd54

    bool func_0201bdfc(s16 param1, const char * param2, void (*param3)(UnkStruct_FieldCC *, bool), u32 param4, u32 param5, u32 param6, u32 param7, u32 param8);  // 0x0201bdfc
    void func_0201bf34(s16 param1, const char * param2, FSChunkTable * param3, u32 param4);  // 0x0201bf34
    u8 * func_0201c044(s16 param1, u32 param2, u32 param3, u8 * param4, s32 * param5, void (*param6)(FSFileReq *, bool), u32 param7, u32 param8, u32 param9, u32 param10);  // 0x0201c044
    void func_0201c194();  // 0x0201c194
    u8 * func_0201c1f8(s16 param1, u32 param2, u8 * param3, u32 param4, u8 * param5, s32 * param6, void (*param7)(FSFileReq *, bool), u32 param8, s32 param9, u32 param10);  // 0x0201c1f8
    virtual bool pumpReads();  // 0x0201c2f4
    FSFileReqEx * func_0201c344();  // 0x0201c344
};

// TODO: Continue decomp
class clDAP : public clTask {
public:
    u8 pad_18[0x10];
    FSFileReqEx * field28;
    FSFileReqEx * field2c;
    u32 field30;
    u16 field34;
    u16 field36;
    u16 field38;
    
    clDAP(u8, u32, u32);  // C1 0x0201c360
    virtual ~clDAP() {};  // D1 0x0201c3a8 D0 0x0201c3bc

    virtual void run();  // 0x0201c3d8
    u32 func_0201c4f4(FSFileReqEx * param1);  // 0x0201c4f4
    void func_0201c534(FSFileReqEx * param1, u32 * param2);  // 0x0201c534
    void func_0201c574(FSFileReq * param1);  // 0x0201c574
    void func_0201c5c8();  // 0x0201c5c8
};

#endif // __cplusplus

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
