#ifndef MAIN_CL_RFX_H
#define MAIN_CL_RFX_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaAnim.h"
#include "main/yanaCtrl.h"

#include "structs/rfxNode.h"

extern "C" {
    #include <nitro.h>
}

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
    virtual u16 * extScript(s32);  // 0x02020ba0
    virtual u32 extScriptCount(void);  // 0x02020b98
    virtual void beginDraw(void);  // 0x020213a8

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
