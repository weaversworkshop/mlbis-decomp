#ifndef MAIN_UNK_02005DDC_H
#define MAIN_UNK_02005DDC_H

#include "types.h"

#include "structs/unk_02005ddc.h"

#define RESET_COMBO (PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START | PAD_BUTTON_SELECT)

void func_02005ddc(UnkStruct_02005ddc * param1);  // 0x02005ddc
BOOL func_02005f24(UnkStruct_02005ddc * param1);  // 0x02005f24
void func_02005f8c(UnkStruct_02005ddc * param1);  // 0x02005f8c

#endif
