// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/texVram.h"

// --- Variable Definitions --- //

u8 * texPlttBuf;  // 0x0205c230
u32 texBankAddrs[4];  // 0x0205c234
u32 texPlttBankAddrs[6];  // 0x0205c244
