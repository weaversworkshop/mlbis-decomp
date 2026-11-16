#ifndef ITCM_UNCOMP_H
#define ITCM_UNCOMP_H

#include "types.h"

void uncompReadHeader(u8 **, u32 *, u32 *);  // 0x01ff8000
void uncompRecord8(u8 **, void **);  // 0x01ff80a4
void uncompRecord16(u8 **, void **);  // 0x01ff8150

#endif
