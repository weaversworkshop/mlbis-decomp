// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0200a024.h"
#include "main/unk_0200a0c8.h"

#include <nnsys/snd.h>

// MATCHED 100%
extern void func_0200a024(void * mem, u32 size, u32 data1, u32 data2) {
    func_0200a0c8(mem, data1, data2);
    SND_InvalidateBankData(mem, mem + size);
    SND_DestroyBank(mem);
}
