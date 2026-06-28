// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_02031be4.h"

#include <nnsys/snd.h>

// MATCHED 100%
void func_02031be4(NNSSndStrmHandle *handle) {
    handle->player = NULL;
}
