// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/sound.h"
#include "main/unk_0200b718.h"

// MATCHED 100%
void func_0200b718(void) {
    sndShutdown(TRUE);
    sndInit(FALSE, 0xa0000, 0x8ace0, 0x80);
    sndLoadGroup(0x47, 0xc00);
    sndPlaySeqNoStop(0x5f, 0, 0xc00);
}
