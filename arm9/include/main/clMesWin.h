#ifndef MAIN_CL_MES_WIN_H
#define MAIN_CL_MES_WIN_H

#include "types.h"

#ifdef __cplusplus

#include "main/clObjPltVramMan.h"

#include "main/sndEmitter.h"

extern "C" {
    #include "main/uiUtil.h"
    #include "main/unk_tu_0200b8bc.h"
}

template <typename T>
struct ListLink {
    T * prev;
    T * next;
};

typedef struct MesIcon {
    ListLink<MesIcon> link;  // +0x0
    struct {
        u32 screen : 1;  // +0x8
        u32 x : 9;
        u32 y : 9;
        u32 flip : 2;
        u32 priority : 2;
        u32 : 1;
        u32 hidden : 1;
        u32 fresh : 1;
        u32 active : 1;
        u32 : 5;
    };
    u8 anim;  // +0xc
    u8 frame;  // +0xd
    u8 timer;  // +0xe
    u8 pad_0f[0x1];
    u32 depth;  // +0x10
} MesIcon;

typedef struct {
    struct {
        u32 screen : 1;  // +0x0
        u32 style : 4;
        u32 width : 5;
        u32 height : 5;
        u32 mode : 3;
        u32 : 14;
    };
    struct {
        u32 tailKind : 4;  // +0x4
        u32 tailAlt : 1;
        u32 tailUp : 3;
        u32 tailFlipH : 1;
        u32 tailX : 8;
        u32 textId : 11;
        u32 field04_28 : 4;
    };
    struct {
        u32 persistent : 1;  // +0x8
        u32 x : 9;
        u32 y : 9;
        u32 indent : 8;
        u32 fixedSize : 1;
        u32 : 4;
    };
    s32 * numbers;  // +0xc
    UnkStruct_0200b8bc_10 ** font;  // +0x10
    u32 * text;  // +0x14
    s32 * result;  // +0x18
    s32 voice;  // +0x1c
    s16 voiceX;  // +0x20
    u8 pad_22[0x2];
    union {
        u32 field24;
        struct {
            u32 field24_0 : 7;  // +0x24
            u32 field24_7 : 1;
            u32 : 24;
        };
    };
} MesWinParams;

typedef struct {
    char buf[8];  // +0x0
    char * pos;  // +0x8
    bool ready;  // +0xc
    u8 pad_0d[0x3];
} MesNumStr;

typedef struct {
    u16 cornerW;  // +0x0
    u16 topH;  // +0x2
    u16 bottomH;  // +0x4
    u8 pad_06[0x2];
    u32 fill;  // +0x8
    const u16 * topLeft;  // +0xc
    const u16 * topRight;  // +0x10
    const u16 * bottomLeft;  // +0x14
    const u16 * bottomRight;  // +0x18
    const u16 * edgeH;  // +0x1c
    const u16 * edgeV;  // +0x20
} MesFrame;

typedef struct {
    const MesFrame * frame;  // +0x0
    u8 maxH;  // +0x4
    u8 pad_05[0x3];
} MesFrameEntry;

typedef struct MesWinSlot {
    ListLink<MesWinSlot> link;  // +0x0
    MesWinParams params;  // +0x8
    UnkStruct_0200b8bc text;  // +0x30
    MesNumStr nums[4];  // +0x60
    struct {
        u32 textX : 8;  // +0xa0
        u32 textY : 8;
        u32 waitCount : 8;
        u32 oddFrame : 1;
        u32 promptMode : 4;
        u32 paused : 1;
        u32 : 2;
    };
    struct {
        u32 dirty : 1;  // +0xa4
        u32 newPage : 1;
        u32 closeReq : 1;
        u32 clearStep : 3;
        u32 tileBase : 15;
        u32 slot : 5;
        u32 closed : 1;
        u32 copyPending : 1;
        u32 voiceRestart : 2;
        u32 : 2;
    };
    struct {
        u32 charSize : 16;  // +0xa8
        u32 voiceState : 3;
        u32 waitKey : 1;
        u32 hold : 1;
        u32 line : 5;
        u32 skip : 1;
        u32 numSlot : 4;
        u32 : 1;
    };
    u8 cols;  // +0xac
    u8 rows;  // +0xad
    u8 pad_ae[0x2];
    u32 * chars;  // +0xb0
    u32 * charsBack;  // +0xb4
    u32 depth;  // +0xb8
    MesIcon cursor;  // +0xbc
    MesIcon tail;  // +0xd0
    const MesFrameEntry * frame;  // +0xe4
    u8 choicePos[10][2];  // +0xe8
    struct {
        u32 choiceCount : 8;  // +0xfc
        u32 choice : 8;
        u32 lineWidth : 16;
    };
    SndEmitter voice;  // +0x100
    u8 pad_146[0xc];
} MesWinSlot;

class clMesIcon {
public:
    UnkStruct_02029b08 * charAllocMan;  // +0x0
    UnkStruct_02029b08 * charAllocSub;  // +0x4
    UnkStruct_02029b08 * plttAllocMain;  // +0x8
    UnkStruct_02029b08 * plttAllocSub;  // +0xc
    nsObjSys::clObjPltVramMan * objManMain[3];  // +0x10
    nsObjSys::clObjPltVramMan * objManSub[3];  // +0x1c
    nsObjSys::clObjPltVramMan * texMan[2];  // +0x28
    YanaResEntry * tileRes[2];  // +0x30
    u32 scale;  // +0x38
    u8 plttIdx[2];  // +0x3c
    u8 pad_3e[0x2];
    s32 texPlttAddr;  // +0x40
    u32 charBase[2];  // +0x44  unclear boundary
    u8 pad_4c[0x8];
    BOOL useTex;  // +0x54
    ListLink<MesIcon> list;  // +0x58
    ListLink<MesIcon> listEnd;  // +0x60

    clMesIcon(BOOL);  // C1 0x0201465c C2 0x0201470c
    ~clMesIcon();  // D1 0x020147bc D2 0x0201480c

    void update(void);  // 0x02013dc8

    void setObjMan(BOOL, nsObjSys::clObjPltVramMan *, nsObjSys::clObjPltVramMan *, nsObjSys::clObjPltVramMan *);  // 0x02013ec4
    void setTexMan(nsObjSys::clObjPltVramMan *, nsObjSys::clObjPltVramMan *);  // 0x02013f00
    void loadChars(u32, u32);  // 0x02013f14
    void loadObjChars(u32, u32);  // 0x02013f3c
    
    void loadTex(u32);  // 0x020140a4
    u32 getCharSize(u32);  // 0x02014200
    void loadPltt(u32, u32);  // 0x02014220
    void loadObjPltt(u32, u32);  // 0x02014264
    void loadTexPltt(u32);  // 0x020143ac
    void add(MesIcon *, u8, s8, u16, u16, u8, u8, s32);  // 0x02014410
    void freeChars(BOOL);  // 0x0201450c
    void freePltt(BOOL);  // 0x020145b4

    void drawOam(MesIcon *);  // 0x0201485c
    void drawTex(MesIcon *);  // 0x02014c28
};

typedef union {
    u32 word;  // +0x0
    struct {
        s16 x;  // +0x0
        s16 y;  // +0x2
    };
} MesScroll;

class clMesWin {
public:
    clMesIcon * icons;  // +0x4
    MesScroll scrollMain;  // +0x8
    MesScroll scrollSub;  // +0xc
    u16 decideKeys;  // +0x10
    u16 field12;
    u8 * backBuf[2];  // +0x14
    void * screenMain;  // +0x1c
    void * screenSub;  // +0x20
    u8 * charPoolMain;  // +0x24
    u8 * charPoolSub;  // +0x28
    u32 charPoolSizeMain;  // +0x2c
    u32 charPoolSizeSub;  // +0x30
    u16 screenSizeMain;  // +0x34
    u16 screenSizeSub;  // +0x36
    u8 singleBuf[2];  // +0x38
    u8 pad_40[0x2];
    union {
        u32 flags;  // +0x3c
        struct {
            u32 active : 1;  // +0x3c
            u32 screenDirtyMain : 1;
            u32 screenDirtySub : 1;
            u32 charsDirty : 1;
            u32 winCountMain : 5;
            u32 winCountSub : 5;
            u32 bgMain : 2;
            u32 bgSub : 2;
            u32 priorityMain : 2;
            u32 prioritySub : 2;
            u32 plttMain : 4;
            u32 plttSub : 4;
            u32 : 2;
        };
    };
    union {
        u32 state;  // +0x40
        struct {
            u32 screenMask : 3;  // +0x40
            u32 scrollLockMain : 1;
            u32 scrollLockSub : 1;
            u32 curSlot : 5;
            u32 updated : 1;
            u32 ownsBuffers : 1;
            u32 ownsIcons : 1;
            u32 : 19;
        };
    };
    u32 * frameTiles;  // +0x44
    void * palette;  // +0x48
    MesWinSlot * windows;  // +0x4c
    u16 windowCount;  // +0x50
    u8 pad_52[0x2];
    u32 bufSize;  // +0x54
    ListLink<MesWinSlot> listMain;  // +0x58
    ListLink<MesWinSlot> listMainEnd;  // +0x60
    ListLink<MesWinSlot> listSub;  // +0x68
    ListLink<MesWinSlot> listSubEnd;  // +0x70
    nsObjSys::clObjPltVramMan * texMan;  // +0x78
    nsObjSys::clObjPltVramMan * texPlttMan;  // +0x7c
    UnkStruct_02029b08 * texAlloc;  // +0x80
    UnkStruct_02029b08 * texPlttAlloc;  // +0x84
    u32 texAddr;  // +0x88
    u32 texPlttAddr;  // +0x8c
    u32 baseDepth;  // +0x90
    u32 scale;  // +0x94
    MesScroll scrollMainCur;  // +0x98
    MesScroll scrollSubCur;  // +0x9c
    UnkStruct_0200b8bc_2c textCallbacks[31];  // +0xa0

    clMesWin(u32, u32, u32, u8, u8, u32, clMesIcon *);  // C1 0x0200e370 C2 0x0200e5f8
    ~clMesWin();  // D1 0x0200e880 D2 0x0200e964

    virtual void open(MesWinParams *, s16);  // 0x0200dd9c
    virtual void close(s16);  // 0x0200de40
    virtual void updateList(MesWinSlot *);  // 0x0200ea48
    virtual void drawFrame(MesWinSlot *, BOOL);  // 0x02010c00
    virtual void drawQuad(MesWinSlot *);  // 0x020126d8
    virtual void loadChars(void);  // 0x02012c24

    inline void updateWindow(MesWinSlot *);
    void update(void);  // 0x0200db0c
    void flush(void);  // 0x0200dc38
    void initParams(MesWinParams *);  // 0x0200dcdc
    void allocBuffers(u32, u16, u32, u32, u8, u8);  // 0x0200de80
    u32 calcBufferSize(u16, u32, u32, u8, u8);  // 0x0200dee0
    void assignBuffers(MesWinSlot *);  // 0x0200dfac
    void release(void);  // 0x0200e0bc
    u32 measureText(const char *, UnkStruct_0200b8bc_10 **);  // 0x0200e12c
    u16 getBorderWidth(u8, u8);  // 0x0200e1bc
    void attachTexMan(nsObjSys::clObjPltVramMan *);  // 0x0200e1d0
    void loadPalette(BOOL, u32, void *);  // 0x0200e24c
    void attachTexPlttMan(nsObjSys::clObjPltVramMan *, u32);  // 0x0200e2d8

    void scrollIcon(MesIcon *);  // 0x0200ec8c
    BOOL checkClose(MesWinSlot *);  // 0x0200ed74
    void stepClear(MesWinSlot *);  // 0x0200ee10
    void advanceText(MesWinSlot *);  // 0x0200ef50
    BOOL execCode(MesWinSlot *, UnkStruct_0200b8bc *, UnkStruct_0200bce8_24);  // 0x0200f428
    void setVoice(MesWinSlot *, BOOL);  // 0x0200fa40
    void playCursorSound(void);  // 0x0200fb90
    void playDecideSound(void);  // 0x0200fbf4
    void alignLine(MesWinSlot *, UnkStruct_0200b8bc *, u32);  // 0x0200fc58
    void addIcon(MesWinSlot *, s16, s16, u16, u8, MesIcon *);  // 0x0200fe38
    void copyRect(MesWinSlot *, u8, u8, u8, u8);  // 0x0200ff3c
    s16 create(MesWinParams *, s16);  // 0x0201008c

    void layoutText(const char *, UnkStruct_0200b8bc_10 **, UnkStruct_0200b8bc_24 *, u8);  // 0x020106fc
    bool destroy(s16);  // 0x020107a0

    void rebuildScreen(u8);  // 0x02010930
    MesWinSlot * allocWindow(u8, u32, s16);  // 0x020109d0
    const MesFrameEntry * findFrame(u8, u8);  // 0x02010bd4

    void drawFrameTex(MesWinSlot *, u32 *);  // 0x02010d34

    void drawFrameBG(MesWinSlot *, u32 *);  // 0x02011370
    void drawFrameBGShifted(MesWinSlot *, u32 *);  // 0x02011980
    void writeScreen(MesWinSlot *);  // 0x020125c0

    void setBGBase(GXBGScrBase, GXBGCharBase);  // 0x0201291c
    void setBGBaseSub(GXBGScrBase, GXBGCharBase);  // 0x02012a24
    void loadScreen(void);  // 0x02012b2c
    void loadScreenSub(void);  // 0x02012ba8

    void loadWinChars(MesWinSlot *);  // 0x02012db4
    void applyScroll(void);  // 0x02012f54

    u32 calcSize(u8, u8, u8, u8 *, u8);  // 0x02013164
    void addTail(MesWinSlot *);  // 0x020133ac
    void formatNumber(MesWinSlot *, u8, u8, BOOL);  // 0x02013540
    BOOL printNumber(MesWinSlot *);  // 0x02013640
    static u32 callbackNumber(clMesWin *, UnkStruct_0200b8bc *, BOOL);  // 0x020139fc
    static u32 callbackNumberPadded(clMesWin *, UnkStruct_0200b8bc *, BOOL);  // 0x02013a84
    void measureString(UnkStruct_0200b8bc *, MesNumStr *);  // 0x02013b0c
};

// extern "C" {
#endif // __cplusplus

// #ifdef __cplusplus
// } // extern "C"
// #endif

#endif
