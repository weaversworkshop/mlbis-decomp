#ifndef MAIN_CL_OBJ_CELL_ANIME_H
#define MAIN_CL_OBJ_CELL_ANIME_H

#include "types.h"

#ifdef __cplusplus

namespace nsObjSys {

class clCellAnime {
public:
    u8 * field04;
    u8 pad_08[0x1e];
    s16 field26;
    u8 pad_28[0x8];
    s32 field30;
    u8 pad_34[0xc];
    MtxFx44 field40;
    u8 pad_80[0x1];
    u8 field81;
    u8 field82;
    u8 pad_83[0x15];
    u8 field98;
    u8 field99;
    u16 field9a;

    virtual ~clCellAnime();  // D1 0x0202ac68 D0 0x0202ac90
    virtual void func_0202b24c();  // 0x0202b24c
    virtual void func_0202b2dc();  // 0x0202b2dc
    virtual void func_0202b338();  // 0x0202b338
    virtual void func_0202b658();  // 0x0202b658
    virtual void func_0202b71c();  // 0x0202b71c
    virtual void func_0202b7b4();  // 0x0202b7b4
    virtual void func_0202bac0();  // 0x0202bac0
    virtual void func_0202bb44(u32);  // 0x0202bb44
    virtual void func_0202bc64();  // 0x0202bc64
    virtual void func_0202bd24();  // 0x0202bd24
    virtual void func_0202bcc8();  // 0x0202bcc8
    virtual void func_0202c834();  // 0x0202c834
    virtual void func_0202d084();  // 0x0202d084
    virtual void func_0202d3c0();  // 0x0202d3c0
    virtual void func_0202d5d0();  // 0x0202d5d0
    virtual void func_0202dee8(s32, s32);  // 0x0202dee8
    virtual s16 func_020225dc(void) { return this->field26; }  // 0x020225dc
    virtual void func_0202bab0();  // 0x0202bab0
    virtual u8 func_020225d0(void) { return this->field04[1]; }  // 0x020225d0
    virtual void func_0202e0d0();  // 0x0202e0d0
    virtual void func_0202e0c8();  // 0x0202e0c8
    virtual void func_0202bc5c();  // 0x0202bc5c
    virtual void func_0202bab8();  // 0x0202bab8
    virtual void func_0202e0bc();  // 0x0202e0bc
    virtual void func_0202e09c();  // 0x0202e09c
    virtual void func_0202d078();  // 0x0202d078
    virtual void func_02022f38(u32 param1) {  // 0x02022f38
        this->field98 = param1;
        if (this->field82 == 0) {
            BOOL flag = (param1 != 0);
            if (flag == TRUE) {
                this->field30 = -1;
            }
        }
        this->field82 = (param1 != 0);
    };
    virtual void func_0202d800();  // 0x0202d800
};

}

#endif // __cplusplus

#endif
