#ifndef OV003_CL_GUIDE_MAP_CHR_H
#define OV003_CL_GUIDE_MAP_CHR_H

#include "types.h"

#ifdef __cplusplus

#include "ov003/clFieldCellAnime.h"
#include "ov003/clFieldChrBase.h"

class clGuideMapChr : public clFieldChrBase {
public:
    clFieldCellAnime * fieldd4;
    u8 pad_d8[0x28];
    fx32 posX;  // +0x100
    u8 pad_104[0x4];
    fx32 posY;  // +0x108

    clGuideMapChr(clFieldBase *, u8, UnkStruct_0206b0f4 *, u32);  // C2 0x0206b838 C1 0x0206b898
    virtual ~clGuideMapChr();  // D1 0x0206b8f8 D0 0x0206b928 D2 0x0206b960
    virtual void func_ov007_02115bf4();  // ov007 0x02115bf4
    virtual void releaseCellAnime(void);  // 0x0206b990
    virtual void func_ov007_02115c74();  // ov007 0x02115c74
    virtual void func_ov007_02115d54();  // ov007 0x02115d54
    virtual void func_ov007_02115d88();  // ov007 0x02115d88
};

#endif

#endif
