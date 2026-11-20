#ifndef OV003_CL_FIELD_BASE_SYSTEM_H
#define OV003_CL_FIELD_BASE_SYSTEM_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaUnitSys.h"
#include "main/task.h"

class clFieldBaseSystem;

typedef void (clFieldBaseSystem::*Func_28)(void);

class clFieldBaseSystem : clTask {
public:
    u8 pad_18[0x10];
    Func_28 field28;
    u8 pad_30[0x4];
    u32 field34;
    u32 field38;
    u8 pad_3c[0x4];
    clYanaUnitSys field40;
    u32 field5c;
    u8 pad_60[0x8];
    u32 field68;
    
    clFieldBaseSystem();  // C2 0x0206b2e0
    virtual ~clFieldBaseSystem();  // D1 0x0206b354 D0 0x0206b374 D2 0x0206b39c
    virtual void run(void);  // 0x0206b3bc
};

#endif

#endif
