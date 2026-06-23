// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"
#include "main/unk_02019a2c.h"
#include "main/unk_02019a74.h"

#include <nitro/ioreg.h>
#include <nitro/pad.h>

// MATCHED 100%
void func_02019a2c(void) {
    func_02019a74(&data_0205afa8, PAD_Read());
}
