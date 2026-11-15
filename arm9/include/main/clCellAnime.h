#ifndef MAIN_CL_OBJ_CELL_ANIME_H
#define MAIN_CL_OBJ_CELL_ANIME_H

#include "types.h"

#ifdef __cplusplus

#include "main/clHandleMan.h"
#include "main/clOamMan.h"

extern "C" {
    #include <nitro.h>

    #include "main/uiUtil.h"
}

typedef struct {
    s16 m00;  // +0x0
    s16 m01;  // +0x2
    s16 m10;  // +0x4
    s16 m11;  // +0x6
} YanaResMtx;

#define ANIM_PTR(hdr, off)      ((u8 *)(hdr) + (off))
#define ANIM_REC(hdr, i)        ((YanaResAnim *)ANIM_PTR(hdr, (hdr)->animOffset + (i) * sizeof(YanaResAnim)))
#define ANIM_ENTRIES(hdr, rec)  ((YanaResFrame *)ANIM_PTR(hdr, (rec)->frames))
#define ANIM_ENTRY(hdr, rec, i) ((YanaResFrame *)ANIM_PTR(hdr, (rec)->frames + (i) * sizeof(YanaResFrame)))
#define ANIM_ITEMS(hdr)         ((YanaResObj *)ANIM_PTR(hdr, (hdr)->objOffset))
#define ANIM_ITEM(hdr, i)       ((YanaResObj *)ANIM_PTR(hdr, (hdr)->objOffset + (i) * sizeof(YanaResObj)))
#define ANIM_TBL(hdr, i)        ((YanaResEntry *)ANIM_PTR(hdr, RES_HDR_SIZE + (i) * sizeof(YanaResEntry)))
#define ANIM_TBL_DATA(hdr, tbl) ((u16 *)ANIM_PTR(hdr, (tbl)->offset))

namespace nsObjSys {

class clCellAnime {
public:
    YanaRes * res;  // +0x4
    u32 * charData;  // +0x8
    u32 * plttData;  // +0xc
    u16 * pafData;  // +0x10
    VramHandle * charHandle;  // +0x14
    u32 charOffset;  // +0x18
    VramHandle * plttHandle;  // +0x1c
    u32 plttOffset;  // +0x20
    u8 charEntry;  // +0x24
    u8 pad_25[0x1];
    s16 anim;  // +0x26
    s16 frame;  // +0x28
    s16 speed;  // +0x2a
    u8 ended;  // +0x2c
    u8 pad_2d[0x3];
    s32 loadedCell;  // +0x30
    u32 cell;  // +0x34
    u32 frameTimer;  // +0x38
    u32 animTime;  // +0x3c
    MtxFx44 mtx;  // +0x40
    union {
        u32 field80w;
        struct {
            u8 screen;  // +0x80
            u8 mode;  // +0x81
            u8 visible;  // +0x82
            u8 paused;  // +0x83
        };
    };
    u8 endMode;  // +0x84
    s8 playCount;  // +0x85
    u8 perFrame;  // +0x86
    s8 state;  // +0x87
    union {
        u32 field88w;
        u8 priority[4];  // +0x88
    };
    union {
        u32 field8cw;
        struct {
            u8 charDirty;  // +0x8c
            u8 plttDirty;  // +0x8d
            u8 plttBuffered;  // +0x8e
            struct {
                u8 ownsChar : 1;  // +0x8f
                u8 ownsPltt : 1;
                u8 skipped : 1;
                u8 : 5;
            };
        };
    };
    void (*onEnd)(u32, clCellAnime *);
    u32 onEndArg;  // +0x94
    union {
        u32 field98w;
        u16 field98h;
        struct {
            u8 alpha;  // +0x98
            u8 polygonId;  // +0x99
            u16 color;  // +0x9a
        };
    };
    union {
        u32 field9cw;
        struct {
            s16 depthScale;  // +0x9c
            s16 depthStep;  // +0x9e
        };
    };
    clCellAnime * nextCharLoad;  // +0xa0
    clCellAnime * nextPlttLoad;  // +0xa4
    clCellAnime * next;  // +0xa8
    clCellAnime * prev;  // +0xac
    s32 pafState;  // +0xb0

    clCellAnime();  // C1 0x0202ac50
    virtual ~clCellAnime();  // D1 0x0202ac68 D0 0x0202ac90 D2 0x0202acc0
    virtual void initWithHandles(VramHandle *, VramHandle *, u32, YanaRes *, u32 *, u32 *, u16 *);  // 0x0202b24c
    virtual void initAt(u32, u32, u32, u32, u32, YanaRes *, u32 *, u32 *, u16 *);  // 0x0202b2dc
    virtual void initAlloc(u32, u32, clVramManBase *, clVramManBase *, YanaRes *, u32 *, u32 *, u16 *);  // 0x0202b338
    virtual void setRes(YanaRes *, u32 *, u32 *);  // 0x0202b658
    virtual void release(void);  // 0x0202b71c
    virtual void update(void);  // 0x0202b7b4
    virtual void updatePaf(void);  // 0x0202bac0
    virtual u32 draw(clOamMan *);  // 0x0202bb44
    virtual u32 drawImpl(GXOamAttr *, GXOamAttr **, OamAffine *, OamAffine **);  // 0x0202bc64
    virtual u32 drawOam(GXOamAttr *, GXOamAttr **, OamAffine *, OamAffine **);  // 0x0202bd24
    virtual void mtxToAffine(YanaResXform *, MtxFx44 *);  // 0x0202bcc8
    virtual u32 draw3d(void);  // 0x0202c834
    virtual u32 func_0202d084(void);  // 0x0202d084
    virtual void loadChars(void);  // 0x0202d3c0
    virtual void loadPltt(void);  // 0x0202d5d0
    virtual void setAnim(s32, s32);  // 0x0202dee8
    virtual s16 getAnim(void) { return this->anim; }  // 0x020225dc
    virtual s16 getFrame(void);  // 0x0202bab0
    virtual u8 getNumAnims(void) { return this->res->numAnims; }  // 0x020225d0
    virtual u16 getNumFrames(s16);  // 0x0202e0d0
    virtual void setDepth(fx32);  // 0x0202e0c8
    virtual u32 getDepth(void);  // 0x0202bc5c
    virtual void setPaused(u8);  // 0x0202bab8
    virtual void setPriority(u32, u8);  // 0x0202e0bc
    virtual void setAllPriority(u32);  // 0x0202e09c
    virtual u8 getPriority(u32);  // 0x0202d078
    virtual void setAlpha(u32 alpha) {  // 0x02022f38
        this->alpha = alpha;
        if (this->visible == 0) {
            BOOL flag = (alpha != 0);
            if (flag == TRUE) {
                this->loadedCell = -1;
            }
        }
        this->visible = (alpha != 0);
    };
    virtual u32 getPlttSize(void);  // 0x0202d800

    void setup(u32, u32, u32, VramHandle *, u32, VramHandle *, u32, YanaRes *, u32 *, u32 *, u16 *);  // 0x0202b474

    u16 * getPlttBuf(void);  // 0x0202d848

    void link(void);  // 0x0202dc54
    void unlink(void);  // 0x0202dce8
    u16 * getCharMap(void);  // 0x0202dd5c
    BOOL needsCharLoad(void);  // 0x0202de74
};

}

extern "C" {

s32 findCharEntry(u32, u32, u32, YanaRes *);  // 0x0202af48
u32 getResCharSize(u32, u32, u32, YanaRes *);  // 0x0202afbc
u32 getObjPlttUnits(YanaRes *, u32 *);  // 0x0202b170
void transferChars(u32, u32, YanaRes *, u32 *, u16 *, u32, u32, u32, u8);  // 0x0202d10c
void queuePush(nsObjSys::clCellAnime *, nsObjSys::clCellAnime * &, nsObjSys::clCellAnime * &, u32);  // 0x0202dbf0

}

extern nsObjSys::clCellAnime * charLoadQueue[2];
extern nsObjSys::clCellAnime * plttLoadQueue[2];

extern const u32 objVramBase[2];
extern vu32 * const dispcntRegs[2];
extern const u32 texSizeParams[12];
extern const u32 quadVtx[12][4];
extern const u32 quadTexCoords[12][4][4];

#endif // __cplusplus

#endif
