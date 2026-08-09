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

#ifdef __cplusplus
extern "C" {
#endif

void sndInit(BOOL, u32, u32, u32);  // 0x02005fe4
void sndShutdown(BOOL);  // 0x02006274
void sndPlaySeqNoStop(u32, u8, u32);  // 0x02006464
void sndLoadGroup(u32, u32);  // 0x02006d14

#ifdef __cplusplus
} // extern "C"
#endif

#endif
