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

BOOL writeSavePrimary(u32);  // 0x02017b60
BOOL writeSaveBackup(u32);  // 0x02017ba4
s32 commitSave(s32, u32, u32, BOOL);  // 0x02017bec

#endif
