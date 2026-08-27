#ifndef MAIN_SND_EMITTER_H
#define MAIN_SND_EMITTER_H

#include "types.h"

#define SND_ID_SCRIPT   0x2000000
#define SND_ID_REQUEUE  0x4000000
#define SND_ID_NO_DUP   0x2000

typedef struct {
    s32 id;  // +0x0
    s16 arg;  // +0x4
    s16 volume;  // +0x6
    s16 duration;  // +0x8
    s16 life;  // +0xa
} SndEmitterCmd;


class SndEmitter {
public:
    SndEmitter * next;  // +0x0
    SndEmitter * prev;  // +0x4
    void (*tick)(SndEmitter *);
    SndVoiceList voices[8];  // +0xc
    s16 * xSrc;  // +0x2c
    s16 * ySrc;  // +0x30
    u32 id;  // +0x34
    s16 x;  // +0x38
    s16 y;  // +0x3a
    s16 timer;  // +0x3c
    s16 period;  // +0x3e
    s16 repeats;  // +0x40
    s8 volume;  // +0x42
    union {
        u8 field43;
        struct {
            u8 noPosUpdate : 1;  // +0x43
            u8 noStopOnEnd : 1;
            u8 voiceIdx : 3;
        };
    };
    u8 step;  // +0x44
    s16 repeatCount;  // +0x46

    SndEmitter();  // C2 0x0200a424 C1 0x0200a444

    void start(s32, s16, s16, s16, s16, s16, s16, s16);  // 0x0200a464
    void stop(u32);  // 0x0200a5d4
    void getPosition(s16 *, s16 *);  // 0x0200a69c
    void playVoice(u32, s16, BOOL, s16);  // 0x0200a6c8
    void advance(void);  // 0x0200a8d0
    void link(void);  // 0x0200ac10
    void unlink(void);  // 0x0200ac44
    void reset(void);  // 0x0200ac7c
    SndEmitterCmd * resolveCmd(void);  // 0x0200ad94
    void enqueue(s32, u32, u32, s16, s16, s8, u8);  // 0x0200b814
};


class SndEmitterSlot : public SndEmitter {
public:
    u8 slot;  // +0x48

    // Implicit ctor // C1 0x0200b8a8
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
