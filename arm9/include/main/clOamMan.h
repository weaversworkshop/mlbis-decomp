#ifndef MAIN_CL_OAM_MAN_H
#define MAIN_CL_OAM_MAN_H

#include "types.h"

#ifdef __cplusplus

extern "C" {
#include <nitro.h>

typedef struct {
    union {
        struct {
            s16 pa;  // +0x0
            s16 pb;  // +0x2
            s16 pc;  // +0x4
            s16 pd;  // +0x6
        };
        struct {
            u32 w0;  // +0x0
            u32 w1;  // +0x4
        };
    };
} OamAffine;

typedef struct {
    u32 key;  // +0x0
    union {
        u32 posw;  // +0x4
        struct {
            u8 start;  // +0x4
            u8 count;  // +0x5
            u8 pad_06[0x2];
        };
    };
} OamRun;

typedef struct {
    OamAffine affine[32];  // +0x0
    OamRun runs[128];  // +0x100
} OamPool;
}

namespace nsObjSys {

class clOamMan {
public:
    GXOamAttr * oam;  // +0x4
    OamRun * runs;  // +0x8
    OamAffine * affine;  // +0xc
    OamRun * runEnd;  // +0x10
    GXOamAttr * oamFixedEnd;  // +0x14
    GXOamAttr * oamEnd;  // +0x18
    OamAffine * affineFixedEnd;  // +0x1c
    OamAffine * affineEnd;  // +0x20
    u8 screen;  // +0x24
    u8 ownsPool;  // +0x25

    clOamMan(u8);  // C1 0x0202a608
    virtual ~clOamMan();  // D1 0x0202a6ac D0 0x0202a6fc
    virtual void reset(u32, u32);  // 0x0202a754
    virtual OamRun * addRun(s32, s32, s32);  // 0x0202a780
    virtual void sortRuns(u32);  // 0x0202a7a4
    virtual void flush(GXOamAttr *, u32);  // 0x0202a960
    virtual s32 getAffineIndex(s32, s32, s32, s32);  // 0x0202aaac

    void init(u8, GXOamAttr *, OamAffine *, OamRun *);  // 0x0202a668
};

}

extern nsObjSys::clOamMan * oamManTable[2];

#endif // __cplusplus

#endif
