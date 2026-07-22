#ifndef MAIN_TILE_CONVERT_H
#define MAIN_TILE_CONVERT_H

#include "types.h"

void LinearToTiles4bpp(u32 * src, u32 * dst, u32 lineBytes, u32 lineCount);  // 0x0201d0bc
void LinearToTiles8bpp(u32 * src, u32 * dst, u32 lineBytes, u32 lineCount);  // 0x0201d138
void TilesToLinear4bpp(u32 * src, u32 * dst, u32 lineBytes, u32 lineCount);  // 0x0201d1b8
void TilesToLinear8bpp(u32 * src, u32 * dst, u32 lineBytes, u32 lineCount);  // 0x0201d234

#endif
