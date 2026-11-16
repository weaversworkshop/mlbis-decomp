// objdiff-target: build/delinks/_dsd_gap@dtcm_1.o

#include "globals.h"

#include "dtcm/yanaWork.h"

// --- Variable Definitions --- //

MtxFx33 identityMtx = { 
    FX32_ONE, 0, 0, 
    0, FX32_ONE, 0, 0, 
    0, FX32_ONE 
};  // 0x027e0000

GXVRamTex savedTexBank;  // 0x027e00e0
clYanaUnitSys * unitSysHead;  // 0x027e00e8
GXVRamTexPltt savedTexPlttBank;  // 0x027e00e4
RfxKeySeg keySeg;  // 0x027e00ec
u8 rfxNodeBuf[0x1000];  // 0x027e00fc
