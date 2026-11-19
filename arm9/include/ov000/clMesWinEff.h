#ifndef OV000_CL_MES_WIN_EFF_H
#define OV000_CL_MES_WIN_EFF_H

#include "types.h"

#ifdef __cplusplus

#include "main/clMesWin.h"

typedef union {
    u16 raw;  // +0x0
    struct {
        u16 maskOfs : 15;  // +0x0
        u16 spawn : 1;
    };
} MesEffSeq;

typedef struct {
    const MesEffSeq * fwd;  // +0x0
    const MesEffSeq * back;  // +0x4
} MesEffSeqPair;

typedef struct {
    struct {
        u8 active : 1;  // +0x0
        u8 last : 1;
        u8 field00_2 : 1;
        u8 skip : 1;
        u8 : 4;
    };
    u8 frame;  // +0x1
    u8 timer;  // +0x2
    u8 pad_03;
    s16 x;  // +0x4
    s16 y;  // +0x6
} MesEffWork;

class clMesWinEff : public clMesWin {
public:
    MesEffWork * effWork;  // +0x214
    u32 effWorkOffset;  // +0x218
    struct {
        u32 subStep : 4;  // +0x21c
        u32 screenFlagged : 1;
        u32 replaced : 1;
        u32 : 26;
    };

    clMesWinEff(u32, u32, u32, u8, u8, u32, clMesIcon *);  // C1 0x02063c84 C2 0x02063cc8
    virtual void open(MesWinParams *, s16);  // 0x02066a1c
    virtual void close(s16);  // 0x02066a00
    virtual void updateList(MesWinSlot *);  // 0x02063d0c
    virtual void drawFrame(MesWinSlot *, BOOL);  // 0x0206698c

    void allocEffBuffers(u32, u16, u32, u32);  // 0x02063b00
    u32 calcEffBufferSize(u16, u32, u32);  // 0x02063b4c
    void assignEffBuffers(MesWinSlot *);  // 0x02063b88
    s16 createEff(MesWinParams *, s16);  // ov000 0x02063ba8
    void startOpen(MesWinSlot *);  // 0x02063ea0
    void updateEffect(MesWinSlot *);  // 0x02063ffc
    void startNone(MesWinSlot *);  // 0x02064380
    void startFade(MesWinSlot *, const u16 *);  // 0x02064560
    void startDiag(MesWinSlot *, const MesEffSeq *, u32);  // 0x02064758
    void startDiagPair(MesWinSlot *, MesEffSeqPair *, u32);  // 0x02064b30
    void startWipeH(MesWinSlot *, const u16 *);  // 0x02065264
    void startWipeV(MesWinSlot *, const u16 *);  // 0x0206566c
    void startSplitV(MesWinSlot *);  // 0x02065a64
    void startSplitH(MesWinSlot *);  // 0x02065dd8
    
};


#endif

#endif
