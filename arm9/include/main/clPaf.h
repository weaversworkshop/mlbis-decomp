#ifndef MAIN_CL_PAF_H
#define MAIN_CL_PAF_H

#include "types.h"

#ifdef __cplusplus

#include "main/clRfx.h"

class clPaf : public clRfx {
public:
    u16 * palette;  // +0x20
    s16 id;  // +0x24

    // ~clPaf();  // implicit; D1 0x02027b0c D0 0x02027adc Thn12_D1 0x02027b44 Thn12_D0 0x02027b34
    virtual void unk_virtual1(s32, u16 *, u8);  // 0x02027360
    virtual u32 unk_virtual2();  // 0x020273f8
    virtual void beginDraw(void);  // 0x02027400

    void cmdPaletteOp(s32, u16 *);  // 0x02027448
    void cmdCrossFade(s32, u16 *);  // 0x0202777c
    void runIdTracks(s32, u16 *);  // 0x020278c8
};

#endif // __cplusplus

#endif
