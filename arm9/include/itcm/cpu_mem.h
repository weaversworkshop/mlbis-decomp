#ifndef ITCM_CPU_MEM_H
#define ITCM_CPU_MEM_H

#include "types.h"

void cpu_fill32(void * param1, u32 param2, u32 param3);  // 0x01ff8228
void cpu_copy32_fwd(void * src, void * dst, u32 size);  // 0x01ff82ac
void cpu_copy32_rev(void * src, void * dst, u32 size);  // 0x01ff8380
void cpu_copy16_fwd(u16 * param1, u16 * param2, u32 param3);  // 0x01ff845c
void cpu_copy16_rev(u16 * param1, u16 * param2, u32 param3);  // 0x01ff84fc

#endif
