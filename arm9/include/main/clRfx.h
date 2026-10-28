#ifndef MAIN_CL_RFX_H
#define MAIN_CL_RFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaAnim.h"
#include "main/yanaCtrl.h"

typedef struct {
    s16 field00;
    union {
        s16 field02;
        struct {
            u16 cull : 2;  // +0x2
            u16 triStrip : 1;
            s16 cols : 4;
            u16 : 9;
        };
    };
    s16 field04;
    s16 r;  // +0x6
    s16 g;  // +0x8
    s16 b;  // +0xa
    s16 field0c;
    s16 field0e;
    s16 w;  // +0x10
    s16 h;  // +0x12
    s16 field14;
    s16 transparency;  // +0x16
} RfxNode_30;

typedef struct {
    union {
        MtxFx43 mtx;  // +0x0
        struct {
            VecFx32 axisX;  // +0x0
            VecFx32 axisY;  // +0xc
            VecFx32 axisZ;  // +0x18
            VecFx32 pos;  // +0x24
        };
    };
    RfxNode_30 field30;
    u8 pad_48[0x18];
} RfxNode;

typedef struct {
    u8 pad_00[0x20];
    RfxNode * cur;  // +0x20
} RfxStack;

typedef struct {
    s32 key;  // +0x0
    s32 nextKey;  // +0x4
    s32 t;  // +0x8
    s32 dur;  // +0xc
} RfxKeySeg;

class clRfx : public clYanaCtrlBase, public clYanaAnim {
public:
    u16 * script;  // +0x18
    u32 * handle;  // +0x1c

    virtual ~clRfx() { this->script = NULL; }  // D1 0x02021e60 D0 0x02020b68 Thn12_D1 0x02021ea8 Thn12_D0 0x02021e88
    virtual void update(void);  // 0x02020a5c
    virtual void exec(void);  // 0x02020a90
    virtual void onEnd(void);  // 0x02020b18 Thn12 0x02021e98
    virtual void unk_virtual1(s32, u16 *, u8) = 0;
    virtual u32 unk_virtual2() = 0;
    virtual u16 * func_02020ba0(s32);  // 0x02020ba0
    virtual u32 func_02020b98(void);  // 0x02020b98
    virtual void beginDraw();  // 0x020213a8

    void start(inYanaUnit **, u16 *);  // 0x02020998
    void finish(void);  // 0x02020a3c

    void runTrackLooped(s32, u32, s32);  // 0x02021290
    void runTrack(s32, u32);  // 0x02021324
    void runSubTracks(s32, u16 *);  // 0x020213ac
    void runSubTrackSeg(s32, u16 *);  // 0x02021570
    void runExtTracks(s32, u16 *);  // 0x020217c4
};

s32 evalRow(u16 *, u32, u32, u32, u32);  // 0x02020ba8
void applyRows(RfxNode *, u16 *, s32, s32, s32, s32, s32);  // 0x02020d90
void applyKeyedRows(s32, u16 *);  // 0x02021b80
BOOL findKeySeg(RfxKeySeg *, s32, u16 *);  // 0x02021c48
void vecMulAdd16(s32, VecFx32 *, VecFx32 *);  // 0x02021cd0
void copyNode(RfxNode *, RfxNode *);  // 0x02021de0
void copy64(void *, void *);  // 0x02021df8
void copy36(void *, void *);  // 0x02021e30

#endif // __cplusplus

#endif
