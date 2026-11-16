#ifndef DTCM_YANA_WORK_H
#define DTCM_YANA_WORK_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaUnitSys.h"

extern "C" {
    #include <nitro.h>
}

typedef struct {
    s32 key;  // +0x0
    s32 nextKey;  // +0x4
    s32 t;  // +0x8
    s32 dur;  // +0xc
} RfxKeySeg;

extern MtxFx33 identityMtx;

extern GXVRamTex savedTexBank;
extern GXVRamTexPltt savedTexPlttBank;
extern clYanaUnitSys * unitSysHead;
extern RfxKeySeg keySeg;
extern u8 rfxNodeBuf[0x1000];

#endif // __cplusplus

#endif
