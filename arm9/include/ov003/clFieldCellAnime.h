#ifndef OV003_CL_FIELD_CELL_ANIME_H
#define OV003_CL_FIELD_CELL_ANIME_H

#include "types.h"

#ifdef __cplusplus

#include "main/clCellAnimeML3.h"

typedef struct {
    u16 field00;
    u16 field02;
    u16 field04;
    u16 field06;
    u16 field08;
} FieldAnimeRes;

class clFieldCellAnime : public nsObjSys::clCellAnimeML3 {
public:
    u32 ofsZ;  // +0xb8
    union {
        u32 basePriorityW;  // +0xbc
        u8 basePriority[4];  // +0xbc
    };
    union {
        u16 fieldc0;
        struct {
            u16 : 6;  // +0xc0
            u16 fieldc0_6 : 2;
            u16 : 8;
        };
        struct {
            struct {
                u8 : 4;  // +0xc0
                u8 flipX : 1;
                u8 flipY : 1;
                u8 mtxDirty : 1;
                u8 fieldc0_7 : 1;
            };
            u8 usePlttBuf;  // +0xc1
        };
    };
    u16 rot;  // +0xc2
    u32 scaleX;  // +0xc4
    u32 scaleY;  // +0xc8
    u32 ofsX;  // +0xcc
    u32 ofsY;  // +0xd0
    u32 fieldd4;

    clFieldCellAnime();  // C2 0x02069ddc C1 0x02069e38
    virtual ~clFieldCellAnime();  // D1 0x02069e94 D0 0x02069ea8 D2 0x02069ec4
    virtual void initWithHandles(VramHandle *, VramHandle *, u32, YanaRes *, u32 *, u32 *, u16 *);  // 0x0206a914
    virtual void initAlloc(u32, u32, nsObjSys::clVramManBase *, nsObjSys::clVramManBase *, YanaRes *, u32 *, u32 *, u16 *);  // 0x0206a4b8
    virtual void update(void);  // 0x0206a888
    virtual void setPriority(u32, u8);  // 0x0206a8e4
    virtual void setAllPriority(u32);  // 0x0206a8b8
    virtual void setAlpha(u32);  // 0x0206a90c
    virtual u32 getPolygonAttrMisc(void);  // 0x0206a8b0
    virtual void reset(void);  // 0x02069ed8
    virtual void updateXform(void);  // 0x02069f10
    virtual void func_ov003_0206a720(s16, s16);  // 0x0206a720
    virtual void setBasePriority(u32, u8);  // 0x0206a8f8
    virtual void setBasePriorityWord(u32);  // 0x0206a904
    virtual void setAllBasePriority(u32);  // 0x0206a8cc
    virtual void copyState(clFieldCellAnime *);  // 0x0206a68c

    void initAllocFront(u32, nsObjSys::clVramManBase *, nsObjSys::clVramManBase *, YanaRes *, u32 *, u32 *, u16 *);  // 0x0206a4fc
    void initAllocEx(u32, u32, u32, nsObjSys::clVramManBase *, nsObjSys::clVramManBase *, YanaRes *, u32 *, u32 *, u16 *);  // 0x0206a53c
};

class clFieldSysCellAnime : public clFieldCellAnime {
public:
    u16 lifeTimer;  // +0xd8
    struct {
        u16 hideReq : 1;  // +0xda
        u16 fieldda_1 : 1;
        u16 releaseOnHide : 1;
        u16 : 13;
    };

    clFieldSysCellAnime();  // C1 0x0206a72c
    virtual ~clFieldSysCellAnime();  // D1 0x0206a778 D0 0x0206a78c
    virtual void reset(void);  // 0x0206a7a8
    virtual void updateXform(void);  // 0x0206a7e4
};

extern "C" {

FieldAnimeRes * getFieldAnimeRes(u32);  // ov003 0x0206a048
u32 * func_ov003_0206a0e8(u32);  // ov003 0x0206a0e8
void parkScreen(u32);  // ov003 0x0206a218
void parkCount(nsObjSys::clCellAnime *, u32);  // ov003 0x0206a2ec
void unparkAll(void);  // ov003 0x0206a344
void flushLoads(u32, u32, nsObjSys::clCellAnime *);  // ov003 0x0206a3c4

}

extern CellAnimeList parkedCellAnimeList;

#endif

#endif
