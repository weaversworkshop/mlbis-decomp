#ifndef MAIN_SAVEDATA_H
#define MAIN_SAVEDATA_H

#include "types.h"

#define MAX_COINS           999999 // Not sure

#define SAVE_COPY_STRIDE    0x7ec
#define SAVE_PAYLOAD_SIZE   0x5f4
#define SAVE_SLOT_COUNT     2

#define SAVE_SLOT_SIZE      (SAVE_COPY_STRIDE * 2)

#define SAVE_SLOT_PRIMARY_OFFSET  0x10
#define SAVE_SLOT_BACKUP_OFFSET  (SAVE_SLOT_PRIMARY_OFFSET + SAVE_COPY_STRIDE)

typedef struct {
    u8 pad_00[0x8];
    u32 field08_0 : 4;  // +0x8
    u32 slotValid : 4;
    u32 field08_8 : 24;
    u8 pad_0c[0x4];
} SaveHeader;

typedef struct { 
    u32 eventFlags[0x3a];  // +0x0
} SaveEventFlags;

typedef struct {
    u8 pad_000[0x54];
    s32 coins;  // +0x54
    u8 pad_058[0x78 - 0x58];
    s8 gearCounts[0x84];  // +0x78
    u8 pad_0fc[0x10e - 0x0fc];
    u16 field10e[10];
    u8 pad_122[0x1b4 - 0x122];
    SaveEventFlags eventFlags;  // +0x1b4
    u8 data[SAVE_COPY_STRIDE - 0x29c];  // +0x29c
} SaveCopy;

typedef struct {
    SaveHeader header;  // +0x0
    SaveCopy saves[SAVE_SLOT_COUNT * 2];  // +0x10
    u8 pad_1fc0[0x40];
} SaveData;

BOOL writeSavePrimary(u32);  // 0x02017b60
BOOL writeSaveBackup(u32);  // 0x02017ba4
s32 commitSave(s32, u32, u32, BOOL);  // 0x02017bec

extern SaveData saveData;

#endif
