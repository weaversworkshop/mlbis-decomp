#ifndef OV004_CL_FIELD_H
#define OV004_CL_FIELD_H

#include "types.h"

#ifdef __cplusplus

#include "ov003/clFieldBase.h"

class clField : public clFieldBase {
public:

    virtual void init(void);  // ov005 0x02112338
    virtual void unk_virtual_01(void);  // 0x02095694
    virtual void func_ov003_0206a9f8(void);  // 0x020875d4
    virtual void unk_virtual_02(void);  // 0x02087814
    virtual void createObjMans(void);  // 0x02087a5c
    virtual void destroyObjMans(void);  // 0x02087adc
    virtual void unk_virtual_03(void);  // 0x0208a744
    virtual void unk_virtual_05(void);  // 0x0208d3d8
    virtual void unk_virtual_06(void);  // 0x0208d3e0
    virtual void unk_virtual_07(void);  // 0x0208d3e8
    virtual s32 unk_virtual_14(void) = 0;
    virtual void func_ov004_0208b42c(void);  // 0x0208b42c
    virtual void func_ov004_02088e4c(void);  // 0x02088e4c
    virtual void func_ov004_020926bc(void);  // 0x020926bc
    virtual void func_ov004_020875b0(void);  // 0x020875b0
    virtual void unk_virtual_15(void) = 0;
    virtual void func_ov004_0208db9c(void);  // 0x0208db9c
    virtual void func_ov004_0208aa24(void);  // 0x0208aa24
    virtual void func_ov004_0208ac50(void);  // 0x0208ac50
    virtual void func_ov004_0208adb4(void);  // 0x0208adb4
    virtual void unk_virtual_16(void) = 0;
    virtual void unk_virtual_17(void) = 0;
    virtual void unk_virtual_18(void) = 0;
    virtual void unk_virtual_19(void) = 0;
    virtual void unk_virtual_20(void) = 0;
    virtual void func_ov004_0208bd28(void);  // 0x0208bd28
    virtual void func_ov004_0208d930(void);  // 0x0208d930
    virtual void unk_virtual_21(void) = 0;
    virtual void func_ov004_0208da00(void);  // 0x0208da00
    virtual void func_ov004_0208dc98(void);  // 0x0208dc98
    virtual void func_ov004_0208e604(void);  // 0x0208e604
    virtual void func_ov004_0208e630(void);  // 0x0208e630
    virtual void unk_virtual_22(void) = 0;
    virtual void func_ov004_0208d8cc(void);  // 0x0208d8cc
    virtual void func_ov004_0208d8b0(void);  // 0x0208d8b0
    virtual void func_ov004_0208f4cc(void);  // 0x0208f4cc
    virtual void func_ov004_02091d10(void);  // 0x02091d10
};

#endif

#endif
