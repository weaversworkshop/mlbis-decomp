#ifndef STRUCTS_SND_BANK_SLOT_H
    #define STRUCTS_SND_BANK_SLOT_H

    #include "types.h"

    #include <nnsys/snd.h>

    typedef struct {
        const NNSSndArcBankInfo * bankInfo;
        void * bankData;
        u32 seqIdx;
        u32 inUse;
  } SndBankSlot;
#endif
