#ifndef MAIN_CL_GLOBAL_EVENT_H
#define MAIN_CL_GLOBAL_EVENT_H
// 0x02018538–0x02019628 //

#include "types.h"

#ifdef __cplusplus

// --- Script engine (clScriptSystem / clGlobalEvent) --- //

typedef struct {
    u8 field00;
    u8 opcodes[0xf];
} UnkStruct_02015f0c_00_04;

typedef struct UnkStruct_02015f0c_01 {
    u8 * field00;
    u8 pad_04[0x20];
    u32 field24[16];
    union {
        u8 * field64[17];
        struct {
            u8 pad_64[4];
            u8 * field68[16];
        };
    };
    u16 fielda8;
    u16 fieldaa;
} UnkStruct_02015f0c_01;

typedef struct UnkStruct_02015f0c_02 {
    u16 field00;
    u16 field02;
    u32 field04;
    s32 field08[24];
} UnkStruct_02015f0c_02;

class clScriptSystem {
public:
    UnkStruct_02015f0c_00_04 * field04;
    u32 field08;
    u8 pad_0c[0x14];
    u32 field20;
    u32 field24;

    clScriptSystem();

    virtual ~clScriptSystem();

    virtual void m08(UnkStruct_02015f0c_00_04 * param);
};

class clGlobalEvent : public clScriptSystem {
public:
    u8 pad_04[0x20];
    u32 field24[16];
    union {
        u8 * field64[17];
        struct {
            u8 pad_64[4];
            u8 * field68[16];
        };
    };
    u16 fielda8;
    u16 fieldaa;
    
    clGlobalEvent();

    virtual ~clGlobalEvent();
    
    virtual s32  fn_0c(UnkStruct_02015f0c_01 *, u32);
    virtual u32  fn_10(UnkStruct_02015f0c_01 *, UnkStruct_02015f0c_02 *);
    virtual u32  fn_14();
    virtual void fn_18();
};

#endif // __cplusplus

u16 * func_02018538(s32 param1);

#endif
