#ifndef MAIN_COPY_RECT_H
#define MAIN_COPY_RECT_H

#include "types.h"

void copy_rect_blk128(void * src, void * dst, s32 rows, s32 srcStride, s32 dstStride);  // 0x0201eee8
void copy_rect_blk64(void * src, void * dst, s32 rows, s32 srcStride, s32 dstStride);  // 0x0201ef30
void copy_rect_rem(void * src, void * dst, s32 rows, s32 srcStride, s32 dstStride);  // 0x0201ef68
void copy_rect(void * src, void * dst, s32 rows, s32 width, s32 srcStride, s32 dstStride);  // 0x0201efc4

#endif
