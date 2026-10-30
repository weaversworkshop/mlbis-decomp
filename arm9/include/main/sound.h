#ifndef MAIN_SOUND_H
#define MAIN_SOUND_H

#include "types.h"

#define SND_TRACK_ALL           0xffff
#define SND_ID_SEQ_MASK         0xfff
#define SND_ID_ECHO_MASK        0xc000
#define SND_ID_MATCH_MASK       0xff0fff
#define SND_ID_PAN_ONLY         0x1000000
#define SND_ID_FORCE_CENTER_Y   0x8000000
#define SND_ID_FORCE_CENTER_X   0x10000000
#define SND_ID_EXCLUSIVE        0x20000000
#define SND_ID_STREAM           0x40000000
#define SND_ID_ALT              0x80000000
#define SND_ID_TYPE_MASK        0xc0000000

#define SND_ID_ARC_SHIFT        0x10

#define SND_PRIO_DEFAULT        0xffff

#define SND_ECHO_DELAY_FRAMES   0xc

#define SND_TEMPO_RATIO_UNITY   0x100

#define SND_LOAD_COUNT          8
#define SND_BANK_COUNT          8
#define SND_REQ_COUNT           16
#define SND_VOICE_COUNT         20

#include <nnsys.h>

typedef struct SndVoiceList SndVoiceList;

typedef struct SndVoice {
    struct SndVoice * next;  // +0x0
    struct SndVoice * prev;  // +0x4
    struct SndVoice * chainNext;  // +0x8
    struct SndVoice * chainPrev;  // +0xc
    union {
        NNSSndHandle handle;  // +0x10
        NNSSndStrmHandle strmHandle;  // +0x10
    };
    SndVoiceList * owner;  // +0x14
    u32 id;  // +0x18
    s8 volume;  // +0x1c
    u8 idx;  // +0x1d
    s16 delay;  // +0x1e
    s16 echoDelay;  // +0x20
    s16 elapsed;  // +0x22
    u16 trackCount;  // +0x24
    s16 x;  // +0x26
    s16 y;  // +0x28
    u8 stackIdx;  // +0x2a
    u8 pad_2b;
    union {
        u16 field2c;
        struct {
            u16 inUse : 1;  // +0x2c
            u16 echoIdx : 7;
            u16 stopOthers : 1;
            u16 stopped : 1;
            u16 field2c_10 : 1;
            u16 streamIdx : 2;
            u16 forceCenterX : 1;
            u16 forceCenterY : 1;
            u16 field2c_15 : 1;
        };
    };
    union {
        u16 dualPrio;  // +0x2e
        struct {
            s8 channelPrio;  // +0x2e
            s8 playerPrio;  // +0x2f
        };
    };
    s16 duration;  // +0x30
    u8 pad_32[0x2];
} SndVoice;

struct SndVoiceList {
    SndVoice * head;  // +0x0
};

#ifdef __cplusplus
extern "C" {
#endif

void sndInit(BOOL, u32, u32, u32);  // 0x02005fe4
void sndShutdown(BOOL);  // 0x02006274
void sndPlaySeqNoStop(u32, u8, u32);  // 0x02006464
void sndRestartSlot(u32, BOOL, BOOL);  // 0x02006594
void sndStartFadeSlot(u32, s32, u32);  // 0x0200677c
u32 sndGetSlotStatus(u32);  // 0x02006864
void sndPlayById(u32, s16, s16, s16, s8, s16);  // 0x02006964
BOOL sndPlayByIdOwned(SndVoiceList *, u32, s16, s16, s16, s8, s16);  // 0x02006a74
void sndStopOwned(SndVoiceList *, u32);  // 0x02006bc0
void sndSetListPosition(SndVoiceList *, s16, s16);  // 0x02006cf8
void sndLoadGroup(u32, u32);  // 0x02006d14

#ifdef __cplusplus
} // extern "C"
#endif

#endif
