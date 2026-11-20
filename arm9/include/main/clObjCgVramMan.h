#ifndef MAIN_CL_OBJ_CG_VRAM_MAN_H
#define MAIN_CL_OBJ_CG_VRAM_MAN_H

#include "types.h"

#ifdef __cplusplus

#include "main/clCellAnime.h"
#include "main/clHandleMan.h"
#include "main/clObjPltVramMan.h"

namespace nsObjSys {

class clObjCgVramMan : public clVramManByteBase {
public:

    clObjCgVramMan(u8 bank) : clVramManByteBase(bank, 0, 0) {
        u8 unitShift = (u16)((*dispcntRegs[bank] & REG_GX_DISPCNT_EXOBJ_CH_MASK) >> REG_GX_DISPCNT_EXOBJ_CH_SHIFT) + 5;
        objCharUnitShift[bank] = unitShift;

        u32 maxSize = 0x8000 << (objCharUnitShift[bank] - 5);
        u32 size = (bank == 0) ? GX_GetSizeOfOBJ(): GX_GetSizeOfSubOBJ();
        if (size > maxSize) {
            size = maxSize;
        }

        this->size = size - this->base;
    }

    virtual ~clObjCgVramMan() {}  // D1 0x0206b0f0 D0 0x0206ac54

    virtual u32 getVramAddr(VramHandle * handle) {  // 0x0206b0cc
        return objVramBase[handle->manBank] + handle->offset;
    }

    virtual VramHandle * alloc(u32 size, u32 id, u32 attr) {  // 0x02014070
        u32 base = (1 << objCharUnitShift[this->manBank]);
        return this->nsObjSys::clVramManByteBase::alloc(MATH_ROUNDUP(size, base), id, attr);
    }

    virtual VramHandle * allocFrom(u32 size, u32 id, u32 attr, u32 hint) {  // 0x0206b088
        u32 base = (1 << objCharUnitShift[this->manBank]);
        return this->clVramManByteBase::allocFrom(MATH_ROUNDUP(size, base), id, attr, hint);
    }

    virtual VramHandle * allocAt(u32 size, u32 id, u32 attr, u32 offset) {  // 0x0206b044
        u32 base = (1 << objCharUnitShift[this->manBank]);
        return this->clVramManByteBase::allocAt(MATH_ROUNDUP(size, base), id, attr, offset);
    }
};

}

#endif

#endif
