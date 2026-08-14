#ifndef MAIN_SND_EMITTER_H
#define MAIN_SND_EMITTER_H

#include "types.h"

#define SND_ID_SCRIPT   0x2000000
#define SND_ID_REQUEUE  0x4000000
#define SND_ID_NO_DUP   0x2000

typedef struct {
    s32 id;
    s16 arg;
    s16 volume;
    s16 duration;
    s16 life;
} SndEmitterCmd;


class SndEmitter {
public:
    SndEmitter * next;
    SndEmitter * prev;
    void (*tick)(SndEmitter *);
    SndVoiceList voices[8];
    s16 * xSrc;
    s16 * ySrc;
    u32 id;
    s16 x;
    s16 y;
    s16 timer;
    s16 period;
    s16 repeats;
    s8 volume;
    union {
        u8 field43;
        struct {
            u8 noPosUpdate : 1;
            u8 noStopOnEnd : 1;
            u8 voiceIdx : 3;
        };
    };
    u8 step;
    s16 repeatCount;

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
    u8 slot;

    // Implicit ctor // C1 0x0200b8a8
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif
