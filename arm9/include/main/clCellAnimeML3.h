#ifndef MAIN_CL_OBJ_CELL_ANIME_ML3_H
#define MAIN_CL_OBJ_CELL_ANIME_ML3_H

#include "types.h"

#ifdef __cplusplus

#include "main/clCellAnime.h"

extern "C" {
    #include <nitro.h>
}

namespace nsObjSys {

class clCellAnimeML3 : public clCellAnime {
public:
    u8 prevEnded;  // +0xb4

    virtual ~clCellAnimeML3();  // D1 0x0202e8fc D0 0x0202e910
    virtual void update(void);  // 0x0202e8e8
    virtual u32 draw3d(void);  // 0x0202e0f4
    virtual void getDrawMtx(MtxFx43 *);  // 0x0202e8b0
    virtual u32 getPolygonAttrMisc(void);  // 0x0202e8a8
};

}

#endif // __cplusplus

#endif
