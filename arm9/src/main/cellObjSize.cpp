// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/cellObjSize.h"

// --- Variable Definitions --- //

const u8 tileSizeShift[8] = { 32, 6, 4, 5, 6, 32, 6, 7 };  // 0x0204b0ec
const u8 objHalfWidths[12] = { 4, 8, 16, 32, 8, 16, 16, 32, 4, 4, 8, 16 };  // 0x0204b0f4
const u8 objHalfHeights[12] = { 4, 8, 16, 32, 4, 4, 8, 16, 8, 16, 16, 32 };  // 0x0204b100
const u8 objTileCounts[12] = { 1, 4, 16, 64, 2, 4, 8, 32, 2, 4, 8, 32 };  // 0x0204b10c
