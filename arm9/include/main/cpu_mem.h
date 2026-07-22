#ifndef MAIN_CPU_MEM_H
#define MAIN_CPU_MEM_H

#include "types.h"

void cpu_clear32(u32 data, void *destp, u32 size);  // 0x0203a8dc
void cpu_copy32(const void *srcp, void *destp, u32 size);  // 0x0203a8f8

#endif
