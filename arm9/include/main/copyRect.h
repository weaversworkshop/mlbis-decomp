#ifndef MAIN_COPY_RECT_H
#define MAIN_COPY_RECT_H

#include "types.h"

void copy_rect_blk128(void *, void *, s32, s32, s32);  // 0x0201eee8
void copy_rect_blk64(void *, void *, s32, s32, s32);  // 0x0201ef30
void copy_rect_rem(void *, void *, s32, s32, s32);  // 0x0201ef68
void copy_rect(void *, void *, s32, s32, s32, s32);  // 0x0201efc4

#endif
