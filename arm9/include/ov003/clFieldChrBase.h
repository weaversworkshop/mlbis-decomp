#ifndef OV003_CL_FIELD_CHR_BASE_H
#define OV003_CL_FIELD_CHR_BASE_H

#include "types.h"

#ifdef __cplusplus

#include "ov003/clFieldBase.h"

class clFieldChrBase;

typedef struct {
    u8 pad_00[0xac];
    struct {
        u32 fieldac_0 : 1;  // +0xac
        u32 fieldac_1 : 1;
        u32 fieldac_2 : 1;
        u32 : 29;
    };
    u8 pad_b0[0x8];
    u32 fieldb8;
    clFieldChrBase * fieldbc;
} UnkStruct_10;

typedef struct {
    u8 pad_00[0x10];
    struct {
        u32 : 1;  // +0x10
        u32 field10_1 : 1;
        u32 : 30;
    };
} UnkStruct_0206b0f4;

class clFieldChrBase {
public:
    clFieldBase * field;  // +0x4
    clFieldChrBase * field08;
    u8 pad_0c[0x4];
    UnkStruct_10 field10;
    u8 fieldd0;
    u8 pad_d1[0x2];
    union {
        u8 fieldd3;
        struct {
            u8 fieldd3_0 : 1;  // +0xd3
            u8 : 3;
            u8 fieldd3_4 : 1;
            u8 : 3;
        };
    };

    clFieldChrBase(clFieldBase *, u8, UnkStruct_0206b0f4 *, u32);  // C2 0x0206b0f4 C1 0x0206b1b4
    virtual ~clFieldChrBase();  // D1 0x0206b274 D0 0x0206b278 D2 0x0206b28c
    virtual void func_ov003_0206b290(void);  // 0x0206b290
    virtual u32 func_ov003_0206b2d8(void);  // 0x0206b2d8
    virtual void func_ov003_0206b294(void);  // 0x0206b294
    virtual void func_ov003_0206b2ac(void);  // 0x0206b2ac
    virtual void func_ov003_0206b2bc(void);  // 0x0206b2bc
    virtual void func_ov003_0206b2cc(void);  // 0x0206b2cc
    virtual void func_ov003_0206b2d4(void);  // 0x0206b2d4
};

#endif

#endif
