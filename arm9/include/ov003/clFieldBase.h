#ifndef OV003_CL_FIELD_BASE_H
#define OV003_CL_FIELD_BASE_H

#include "types.h"

#ifdef __cplusplus

#include "main/clOamMan.h"
#include "main/clObjCgVramMan.h"
#include "main/clObjPltVramMan.h"

#include "ov003/clFieldMap.h"

class clFieldBase {
public:
    u8 pad_04[0x8];
    clFieldMap * map;  // +0xc
    nsObjSys::clObjCgVramMan * objCgVramMan;  // +0x10
    nsObjSys::clObjPltVramMan * objPltVramMan;  // +0x14
    nsObjSys::clOamMan * oamMan;  // +0x18
    void * field1c;
    void * field20[32];
    u32 fielda0;
    u8 pad_a4[0x4];
    u32 fielda8;
    u8 pad_ac[0xc];
    u32 fieldb8;
    u32 fieldbc;
    u16 fieldc0;
    u16 fadeFrames;  // +0xc2
    fx32 brightness;  // +0xc4
    s32 fadeStep;  // +0xc8
    union {
        u32 fieldccw;
        struct {
            u16 fadeFramesLeft;  // +0xcc
            struct {
                u16 fadeActive : 1;  // +0xce
                u16 fadePaused : 1;
                s16 fadeTarget : 6;
                s16 fadeStart : 6;
                u16 : 2;
            };
        };
    };
    union {
        u32 fieldd0;
        struct {
            u8 pad_d0;
            u8 fieldd1;
        };
    };

    virtual void init(void);  // 0x0206a948
    virtual void reset(void);  // 0x0206a974
    virtual void unk_virtual_01() = 0;
    virtual void func_ov003_0206a9f8(void);  // 0x0206a9f8
    virtual void unk_virtual_02() = 0;
    virtual void createObjMans(void);  // 0x0206aa14
    virtual void destroyObjMans(void);  // 0x0206abc8
    virtual void unk_virtual_03() = 0;
    virtual void drawCellAnimes(void);  // 0x0206ac68
    virtual void flushOam(void);  // 0x0206ad14
    virtual u8 unk_virtual_04() = 0;
    virtual void unk_virtual_05() = 0;
    virtual void unk_virtual_06() = 0;
    virtual void unk_virtual_07() = 0;
    virtual void unk_virtual_08(s16) = 0;
    virtual void unk_virtual_09() = 0;
    virtual u32 unk_virtual_10() = 0;
    virtual void unk_virtual_11(u32) = 0;
    virtual void unk_virtual_12() = 0;
    virtual u32 unk_virtual_13() = 0;

    void startFade(s16, s16, u16);  // 0x0206ad4c
    void reverseFade(void);  // 0x0206ade4
    void updateFade(void);  // 0x0206ae54
    u8 * func_ov003_0206aee0(u8 *);  // 0x0206aee0

    // dead-stripped function required for clFieldMap inline getter placement
    void unk_stripped_func();  // stripped
};

#endif

#endif
