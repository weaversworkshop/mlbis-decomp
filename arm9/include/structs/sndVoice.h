#ifndef STRUCTS_SND_VOICE_H
    #define STRUCTS_SND_VOICE_H

    #include "types.h"

    #include <nnsys/snd.h>

    typedef struct SndVoice {
        struct SndVoice * next;
        struct SndVoice * prev;
        struct SndVoice * chainNext;
        struct SndVoice * chainPrev;
        union {
            NNSSndHandle handle;
            NNSSndStrmHandle strmHandle;
        };
        struct SndVoice * owner;
        u32 id;
        s8 volume;
        u8 idx;
        s16 delay;
        s16 echoDelay;
        s16 elapsed;
        u16 trackCount;
        s16 x;
        s16 y;
        u8 stackIdx;
        u8 pad_2b;
        union {
            u16 field2c;
            struct {
                u16 inUse : 1;
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
            u16 dualPrio;
            struct {
                s8 channelPrio;
                s8 playerPrio;
            };
        };
        s16 duration;
        u8 pad_32[0x2];
    } SndVoice;

    typedef struct {
        SndVoice * head;
    } SndVoiceList;
#endif
