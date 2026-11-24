#ifndef OV003_CL_FIELD_MAP_H
#define OV003_CL_FIELD_MAP_H

#include "types.h"

#ifdef __cplusplus

#include "main/clPaf.h"
#include "main/clVCountTask.h"
#include "main/fs.h"
#include "main/task.h"

#include "ov003/clFPaf.h"

class clFieldMap;

typedef struct {
    void * src;  // +0x0
    void * dst;  // +0x4
    struct {
        u32 size : 31;  // +0x8
        u32 bytewise : 1;
    };
} VramCopy;

typedef struct FieldMapVBlankWork {
    u8 dmaCount;  // +0x0
    struct {
        u8 blendPending : 1;  // +0x1
        u8 palDirty : 4;
        u8 blendBg0 : 1;
        u8 : 2;
    };
    u8 pad_02[0x2];
    u32 palLoadOffset;  // +0x4
    u16 palLoadSize;  // +0x8
    u8 pad_0a[0x2];
    u16 field0c;
    u16 field0e;
    u16 field10;
    u16 field12;
    u8 pad_14[0x4];
    struct {
        u32 : 2;  // +0x18
        u32 field18_2 : 1;
        u32 : 1;
        u32 field18_4 : 8;
        u32 field18_12 : 5;
        u32 : 15;
    };
    u32 offsetsSet;  // +0x1c
    void * dmaSrc[16];  // +0x20
    void * dmaDst[16];  // +0x60
    u32 dmaSize[16];  // +0xa0
    u32 blendPlane1;  // +0xe0
    u32 blendPlane2;  // +0xe4
    u32 blendEva;  // +0xe8
    u32 blendEvb;  // +0xec
    VramCopy copies[32];  // +0xf0
    u8 copyHead;  // +0x270
    u8 copyTail;  // +0x271
    u8 pad_272[0x2];
} FieldMapVBlankWork;

class clFieldMapVBlank : public clVCountTask {
public:
    u8 pad_1c[0x10];
    FieldMapVBlankWork work;  // +0x2c

    clFieldMapVBlank(u8, u32, clFieldMap *);  // C1 0x02072530
    virtual ~clFieldMapVBlank();  // D1 0x02072608 D0 0x0207261c
    virtual void onVCount(void);  // 0x02072638
    virtual void flushScreens(void);  // 0x02072b64
    virtual void loadScreen(u8);  // 0x02072c20
    virtual void copyScreenEdges(u8, u16, u16, u16 *, u16 *);  // 0x02072d28
    virtual void setBgOffset(u32, u32, u32);  // 0x02072f78
    virtual void loadBgPalette(void);  // 0x0207309c
    virtual u16 * getScreenAddr(u8);  // 0x020730e0
    virtual void queueDmaCopy(void *, void *, u32);  // 0x02073104
    virtual void flushDmaCopies(void);  // 0x0207316c
    virtual void queueBlend(u32, u32, u32, u32);  // 0x020731e0
    virtual void applyBlend(void);  // 0x02073208
    virtual void queueCopy(void *, void *, u32);  // 0x020732b8
};

typedef struct {
    s16 kind;  // +0x0
    u16 color;  // +0x2
    u16 rowMask[4];  // +0x4
    s32 dur;  // +0xc
    s32 time;  // +0x10
    s16 speed;  // +0x14
    s8 endMode;  // +0x16
    u8 done;  // +0x17
} PalEffect;

typedef struct {
    u16 value;  // +0x0
    struct {
        u16 layerMask : 3;  // +0x2
        u16 : 13;
    };
    s16 x0;  // +0x4
    s16 x1;  // +0x6
    s16 y0;  // +0x8
    s16 y1;  // +0xa
} ZRect;

typedef struct {
    u32 idx[5];  // +0x0
} MapChunkIdx;

typedef struct {
    struct {
        u32 last : 1;  // +0x0
        u32 idx : 31;
    };
    ZRect rect;  // +0x4
} MapPatchRect;

typedef struct {
    u16 * tiles;  // +0x0
    u8 * data;  // +0x4
    MapPatchRect * rects;  // +0x8
    u16 x;  // +0xc
    u16 y;  // +0xe
    u16 w;  // +0x10
    u16 h;  // +0x12
    u16 setFlag;  // +0x14
    u16 clearFlag;  // +0x16
} MapPatch;

typedef struct {
    u16 width;  // +0x0
    u16 height;  // +0x2
    struct {
        u32 : 8;  // +0x4
        u32 color256Mask : 3;
        u32 : 2;
        u32 texLayerMask : 3;
        u32 : 16;
    };
    struct {
        u32 priorityOrder : 2;  // +0x8
        u32 clipMask : 3;
        u32 fixedRateMask : 3;
        u32 rateX0 : 4;
        u32 rateY0 : 4;
        u32 rateX1 : 4;
        u32 rateY1 : 4;
        u32 rateX2 : 4;
        u32 rateY2 : 4;
    };
} MapInfo;

typedef struct {
    u32 count;  // +0x0
    ZRect rects[];  // +0x4
} ZRectList;

typedef struct {
    u32 size;  // +0x0
    void * data;  // +0x4
} MapSection;

typedef struct {
    u32 size;  // +0x0
    u16 * data;  // +0x4
} MapSectionU16;

typedef struct {
    u32 size;  // +0x0
    MapInfo * data;  // +0x4
} MapSectionInfo;

typedef struct {
    u32 size;  // +0x0
    u32 * data;  // +0x4
} MapSectionU32;

typedef struct {
    u32 size;  // +0x0
    ZRectList * data;  // +0x4
} MapSectionZRects;

typedef struct {
    u16 srcFrame;  // +0x0
    u16 duration;  // +0x2
} TileAnimFrame;

typedef struct {
    struct {
        u32 layer : 2;  // +0x0
        u32 mode : 2;
        u32 dstTile : 10;
        u32 tileCount : 10;
        u32 : 8;
    };
    u16 gfxId;  // +0x4
    u16 frameCount;  // +0x6
    TileAnimFrame frames[];  // +0x8
} TileAnimDef;

typedef struct {
    TileAnimDef * def;  // +0x0
    s32 timer;  // +0x4
    struct {
        u32 active : 8;  // +0x8
        u32 frame : 16;
        u32 : 8;
    };
} TileAnim;

typedef struct {
    struct {
        u16 eva : 5;  // +0x0
        u16 evb : 5;
        u16 : 6;
    };
    u16 duration;  // +0x2
} BlendAnimStep;

typedef struct {
    u16 stepCount;  // +0x0
    struct {
        u16 once : 1;  // +0x2
        u16 manual : 1;
        u16 plane1 : 5;
        u16 plane2 : 5;
        u16 : 4;
    };
    BlendAnimStep steps[];  // +0x4
} BlendAnim;

typedef struct {
    u8 active;  // +0x0
    u8 pad_01[0x3];
    u32 dist;  // +0x4
    u16 frame;  // +0x8
    u16 frames;  // +0xa
    s32 curX;  // +0xc
    s32 curY;  // +0x10
    s32 startX;  // +0x14
    s32 startY;  // +0x18
    s32 dstX;  // +0x1c
    s32 dstY;  // +0x20
    u32 speedStart;  // +0x24
    u32 speedEnd;  // +0x28
    u32 accel;  // +0x2c
} LayerMove;

typedef struct {
    u8 pad_00[0x14];
    u8 cache[0x674];  // +0x14
    void * map;  // +0x688
    u16 * zTbl;  // +0x68c
    u16 w;  // +0x690
    u16 h;  // +0x692
    u16 tileBase;  // +0x694
    u16 palBase;  // +0x696
    struct {
        u16 color256 : 1;  // +0x698
        u16 : 1;
        u16 field698_2 : 1;
        u16 hidden : 1;
        u16 : 12;
    };
    u16 layer;  // +0x69a
    s16 scrollX;  // +0x69c
    s16 scrollY;  // +0x69e
} TexLayer;

typedef struct {
    FSFileReqEx fs;  // +0x0
    u8 pad_3c[0x4];
} MapFileReq;

typedef struct {
    u32 hasTiles : 1;  // +0x0
    u32 hasData : 1;
    u32 hasRects : 1;
    u32 : 3;
    u32 setFlag : 13;
    u32 clearFlag : 13;
} MapPatchHdr;

typedef struct UnkStruct_F808 {
    u8 pad_00[0x4];
    u32 field04;
    u8 pad_08[0xa08];
    u16 fielda10;
    u8 pad_a12[0x2];
    u16 fielda14;
    u8 pad_a16[0x2];

    UnkStruct_F808() : field04(0), fielda10(0) {}
} UnkStruct_F808;

typedef struct FieldMapWork {
    clFieldMapVBlank * vblank;  // +0x0
    clDPP * dpp;  // +0x4
    u32 * tileAnimGfx[16];  // +0x8
    u16 * screenBuf[3];  // +0x48
    u16 * layerPal[3];  // +0x54
    u16 * basePal;  // +0x60
    u16 * basePalSrc;  // +0x64
    u8 pad_68[0x30];
    PalEffect * palEffects;  // +0x98
    MapPatch * mapPatches;  // +0x9c
    FSFileReqEx mapReq;  // +0xa0
    u32 * mapData;  // +0xdc
    MapFileReq layerGfxReqs[3];  // +0xe0
    MapFileReq tileAnimReqs[16];  // +0x1a0
    union {
        MapSection sections[17];  // +0x5a0
        struct {
            MapSectionU16 sectionsU16[6];  // +0x5a0
            MapSectionInfo infoSection;  // +0x5d0
            MapSection pad_5d8[4];
            MapSectionU32 blendSection;  // +0x5f8
            MapSection pad_600[1];
            MapSectionZRects zRectSection;  // +0x608
            MapSection pad_610[3];
        };
    };
    TileAnim tileAnims[16];  // +0x628
    BlendAnim * blendAnim;  // +0x6e8
    s32 blendTimer;  // +0x6ec
    u16 blendStep;  // +0x6f0
    u8 blendAnimIdx;  // +0x6f2
    u8 blendPaused;  // +0x6f3
    s32 layerX[3];  // +0x6f4
    s32 layerY[3];  // +0x700
    s32 scrollX;  // +0x70c
    s32 scrollY;  // +0x710
    void * field714;
    void * field718;
    u32 timeStep;  // +0x71c
    u16 mapId;  // +0x720
    struct {
        u16 : 5;  // +0x722
        u16 extPafMask : 4;
        u16 prioritiesDirty : 1;
        u16 layerPriorities : 6;
    };
    s32 scrollDX;  // +0x724
    s32 scrollDY;  // +0x728
    u16 width;  // +0x72c
    u16 height;  // +0x72e
    u16 widthPx;  // +0x730
    u16 heightPx;  // +0x732
    s32 wrapW;  // +0x734
    s32 wrapH;  // +0x738
    u8 screenDirty[3];  // +0x73c
    u8 charBlock[3];  // +0x73f
    u8 isSub;  // +0x742
    u8 color256Mask;  // +0x743
    u8 heapId;  // +0x744
    u8 tmpHeapId;  // +0x745
    u8 clipMask;  // +0x746
    u8 mapPatchCount;  // +0x747
    u8 tileAnimCount;  // +0x748
    u8 blendAnimCount;  // +0x749
    u8 pad_74a[0x3];
    u8 palEffectCount;  // +0x74d
    struct {
        u8 palAnimState : 2;  // +0x74e
        u8 palEffectBusy : 2;
        u8 basePalTaken : 1;
        u8 dirtyRectSet : 1;
        u8 gfxToRam : 1;
        u8 async : 1;
    };
    struct {
        u8 palDirty : 4;  // +0x74f
        u8 palAnimMask : 4;
    };
    s16 fsSlot;  // +0x750
    u8 pad_752[0x2];
    u32 layerGfxSize[3];  // +0x754
    u8 * layerGfxBuf[3];  // +0x760
    u16 layerGfxOwned;  // +0x76c
    u8 pad_76e[0x2];
    LayerMove moves[3];  // +0x770
    s16 dirtyX0;  // +0x800
    s16 dirtyY0;  // +0x802
    s16 dirtyX1;  // +0x804
    s16 dirtyY1;  // +0x806
    UnkStruct_F808 field808;
    TexLayer texLayers[3];  // +0x1220
    void * field2600;
    u16 texTileBase[3];  // +0x2604
    u16 texPalBase[3];  // +0x260a
    u8 pad_2610[0x2];
    s8 texPal16Layer;  // +0x2612
    s8 lastBgLayer;  // +0x2613
    struct {
        u32 texMode : 1;  // +0x2614
        u32 texVisibleMask : 3;
        u32 : 28;
    };
    u8 * texVram;  // +0x2618
    GXVRamTex savedTexBank;  // +0x261c
    u8 state;  // +0x2620
    u8 pad_2621[0x3];
    clFPaf ** extPaf[4];  // +0x2624
} FieldMapWork;

class clFieldMap : public clTask {
public:
    u8 pad_18[0x10];
    FieldMapWork work;  // +0x28
    clPaf palAnims[4];  // +0x265c

    clFieldMap(u8, u32, u32, u8, u16, u32, u32, clDPP *, u16, s32, s32, u8);  // C1 0x0206bdf0
    clFieldMap(u8, u32, u32, u16, u32, u32, clDPP *, u32, u32, u16, s32, s32, u8);  // C1 0x0206be9c

    virtual ~clFieldMap();  // D1 0x0206bf64 D0 0x0206c198
    virtual void run(void);  // 0x0206c3d4
    virtual void setupBg(u32);  // 0x0206e694
    virtual void setScroll(s32, s32);  // 0x0206ebf0
    virtual void setLayerScroll(u8, u32, u32);  // 0x0206ecec
    virtual void redrawLayer(u8, u32, u32);  // 0x0206ed00
    virtual void scrollLayer(u8, s32, s32);  // 0x0206f164
    virtual void setScrollDelta(s32, s32);  // 0x0206f884
    virtual void scrollTo(s32, s32);  // 0x0206f890
    virtual void setPlanesVisible(s8, s8, s8, s8, s8, s8, s8);  // 0x0206fa44
    virtual void moveLayerTo(u32, s32, s32, s32);  // 0x0206fc80
    virtual void moveLayerBy(u32, s32, s32, s32);  // 0x0206fca8
    virtual void moveLayerToFrames(u32, s32, s32, u16);  // 0x0206fce0
    virtual void moveLayerByFrames(u32, s32, s32, u16);  // 0x0206fdc8
    virtual void moveLayerToAccel(u32, s32, s32, s32, s32);  // 0x0206fe00
    virtual void moveLayerByAccel(u32, s32, s32, s32, s32);  // 0x0206ff10
    virtual BOOL isLayerMoving(u32);  // 0x020735cc
    virtual void getScroll(s32 *, s32 *);  // 0x0206ff50
    virtual void syncLayerScroll(s32);  // 0x0206ff64
    virtual BOOL isReady(void);  // 0x0206ff98
    virtual u32 getLayerChunkIdx(u32);  // 0x0206ffb0
    virtual MapInfo * getMapInfo(void);  // 0x0206ffd0
    virtual void * func_ov003_0206ffd8(void);  // 0x0206ffd8
    virtual ZRectList * getZRects(void);  // 0x0206ffec
    virtual void * func_ov003_02070000(void);  // 0x02070000
    virtual u32 func_ov003_02070014(void);  // 0x02070014
    virtual void * func_ov003_0207001c(void);  // 0x0207001c
    virtual u16 * getLayerPaletteSrc(u32);  // 0x020735c0
    virtual void * getSection(u32);  // 0x020735b4
    virtual u8 * getLayerCharBuf(u32);  // 0x020735a8
    virtual u16 * getLayerScreenBuf(u32);  // 0x0207359c
    virtual u16 * getLayerPalette(u32);  // 0x02073574
    virtual u8 * applyMapPatch(s16);  // 0x02070030
    virtual void patchTilemap(u32);  // 0x02070334
    virtual u8 * getMapPatchData(u32);  // 0x020704f0
    virtual u8 getMapPatchCount(void);  // 0x0207356c
    virtual BOOL startTileAnim(u32);  // 0x02070504
    virtual void restartTileAnim(u32);  // 0x02070554
    virtual void updateTileAnims(void);  // 0x02070730
    virtual BOOL stopTileAnim(u32);  // 0x020709a0
    virtual BOOL resumeTileAnim(u32);  // 0x020709d0
    virtual u8 getTileAnimCount(void);  // 0x02073564
    virtual BOOL isPalAnimActive(void);  // 0x02073548
    virtual void resumePalAnims(void);  // 0x0206e630
    virtual void pausePalAnims(void);  // 0x020734e4
    virtual void reloadPalettes(u8);  // 0x02070a0c
    virtual void restorePalettes(u8);  // 0x02070a64
    virtual void allocPalettes(void);  // 0x02070dbc
    virtual void uploadPalettes(void);  // 0x02070b04
    virtual void flushPalettes(void);  // 0x02070ec8
    virtual void attachPaf(u32, clFPaf **);  // 0x02070edc
    virtual BOOL hasPaf(u32);  // 0x02070f6c
    virtual BOOL startBlendAnim(u8);  // 0x02070fa0
    virtual BOOL stopBlendAnim(void);  // 0x02071034
    virtual BOOL pauseBlendAnim(void);  // 0x02071054
    virtual BOOL resumeBlendAnim(u32);  // 0x02071064
    virtual void updateBlendAnim(void);  // 0x020710a8
    virtual u8 getBlendAnimCount(void);  // 0x020734dc
    virtual void setupTexLayers(void);  // 0x020711a4
    virtual void buildTexTiles(void);  // 0x020712a0
    virtual u16 func_ov003_020715fc(u32, u32);  // 0x020715fc
    virtual u16 func_ov003_020734d0(void);  // 0x020734d0
    virtual void func_ov003_020734b4(u32);  // 0x020734b4
    virtual u32 func_ov003_020734a0(void);  // 0x020734a0
    virtual void func_ov003_02073484(u32);  // 0x02073484
    virtual u32 func_ov003_02073470(void);  // 0x02073470
    virtual BOOL func_ov003_02071734(u8, u16);  // 0x02071734
    virtual void fillZRect(ZRect *);  // 0x020718d8
    virtual void addDirtyRect(s32, s32, s32, s32);  // 0x02071a10
    virtual void resetDirtyRect(void);  // 0x02071a68
    virtual u16 * getBgCharAddr(u32);  // 0x02071a90
    virtual void getLayerScrollRate(u8, s32 *, s32 *);  // 0x02071ac4
    virtual u32 calcWrapSize(u32);  // 0x02071bcc
    virtual u16 getMapId(void);  // 0x02073464
    virtual u16 getWidthPx(void) { return this->work.widthPx; }  // 0x0206aec8
    virtual u16 getHeightPx(void) { return this->work.heightPx; }  // 0x0206aed4
    virtual void setLayerPriority(u32, u32);  // 0x02073418
    virtual void setLayerPriorities(u32, u32, u32);  // 0x0206ebb8
    virtual u32 getLayerPriority(u32);  // 0x02072b44
    virtual void setBlendBg0(u8);  // 0x02073350
    virtual BOOL getBlendBg0(void);  // 0x02073334
    virtual u32 isTexMode(void);  // 0x02073320
    virtual void setPalEffects(PalEffect *, u8, u32);  // 0x02071bf8
    virtual void startPalEffect(u32, u16, u16, u16, u16, s16, u16, u32, u8, s16, u32, u8);  // 0x02071c48
    virtual void clearPalEffect(u8);  // 0x02071ccc
    virtual void updatePalEffects(void);  // 0x02071d18
    virtual void reversePalEffect(u32);  // 0x020720f0
    virtual u16 * getBasePalette(void);  // 0x02072120
    virtual void queuePaletteLoad(u32, u16);  // 0x02072134
    virtual void markPaletteDirty(u32);  // 0x0207217c
    virtual void markAllPalettesDirty(void);  // 0x020721c8
    virtual u32 getTileSize(u32);  // 0x02070718
    virtual void uploadTexPalettes(void);  // 0x020721dc

    void init(u8, u32, u16, u32, u32, clDPP *, u16, s32, s32, u8);  // 0x02072440
};

#endif

#endif
