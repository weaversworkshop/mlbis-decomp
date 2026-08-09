#ifndef STRUCTS_SND_FADE_H
    #define STRUCTS_SND_FADE_H

    #include "types.h"

    typedef struct {
        fx32 maxVolume;
        fx32 volume;
        fx32 step;
        NNSSndSeqMute state;
    } SndFade;
#endif
