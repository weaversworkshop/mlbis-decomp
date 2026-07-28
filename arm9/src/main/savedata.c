// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/savedata.h"

#include <nitro/math.h>

#include "main/backup.h"
#include "main/clGlobalEvent.h"

// --- Struct Definitions --- //

#define SAVE_OK         0
#define SAVE_DISABLED   1
#define SAVE_FAILED     2

#define EVENT_FLAG_WORD(id) (((id) & 0xfff) >> 5)
#define EVENT_FLAG_BIT(id)  (1u << ((id) & 0x1f))


typedef struct {
    u8 pad_00[0x8];
    u32 field08_0 : 4;
    u32 slotValid : 4;
    u32 field08_8 : 24;
    u8 pad0c[0x4];
} SaveHeader;

typedef struct { 
    u32 eventFlags[0x3a];
} SaveEventFlags;

typedef struct {
    u8 pad_000[0x54];
    s32 coins;
    u8 pad_058[0x78 - 0x58];
    s8 gearCounts[0x84];
    u8 pad_0fc[0x10e - 0x0fc];
    u16 field10e[10];
    u8 pad_122[0x1b4 - 0x122];
    SaveEventFlags eventFlags;
    u8 data[SAVE_COPY_STRIDE - 0x29c];
} SaveCopy;

typedef struct {
    SaveHeader header;
    SaveCopy saves[SAVE_SLOT_COUNT * 2];
    u8 pad_1fc0[0x40];
} SaveData;

// --- Extern Functions --- //

extern s32 func_020058bc(u16, void *, void *);

// --- Variable Definitions --- // (replace with real data later)

SaveData saveData;  // 0x02058f84

// --- Function Definitions --- //

// MATCHED 100%
BOOL writeSavePrimary(u32 slotIdx) {  // Save into primary slot
    if (slotIdx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    backupWrite((void *)(slotIdx * SAVE_SLOT_SIZE + SAVE_SLOT_PRIMARY_OFFSET), &saveData.saves[slotIdx * 2], SAVE_PAYLOAD_SIZE, 1);

    return TRUE;
}

// MATCHED 100%
BOOL writeSaveBackup(u32 slotIdx) {  // Save into backup slot
    if (slotIdx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    backupWrite((void *)(slotIdx * SAVE_SLOT_SIZE + SAVE_SLOT_BACKUP_OFFSET), &saveData.saves[slotIdx * 2], SAVE_PAYLOAD_SIZE, 1);

    return TRUE;
}

// Helper inline function for `commitSave()`
static inline void snapshotEventFlag(SaveEventFlags * record, u16 id) {
    if (func_020058bc(id, NULL, NULL) != 0) {
        record->eventFlags[EVENT_FLAG_WORD(id)] |=  EVENT_FLAG_BIT(id);
    } else {
        record->eventFlags[EVENT_FLAG_WORD(id)] &= ~EVENT_FLAG_BIT(id);
    }
}

// MATCHED 100%
s32 commitSave(s32 coinDelta, u32 gearId1, u32 gearId2, BOOL useBackup) {
    u32 slot = data_02055fe4.field4cc_3_4;
    u32 slotBase = 2 * slot;

    if ((saveData.header.slotValid & (1 << slotBase)) == 0) {
        return SAVE_DISABLED;
    }

    u32 copyIdx = (slotBase + useBackup);

    s32 coins = saveData.saves[copyIdx].coins;
    coins += coinDelta;
    if (coins < 0) {
        coins = 0;
    }
    
    if (coins > MAX_COINS) {
        coins = MAX_COINS;
    }
    saveData.saves[copyIdx].coins = coins;

    if (gearId1 != 0) {
        if (saveData.saves[copyIdx].gearCounts[gearId1 & 0xfff] < 9) {
            saveData.saves[copyIdx].gearCounts[gearId1 & 0xfff]++;
        }
    }

    if (gearId2 != 0) {
        if (saveData.saves[copyIdx].gearCounts[gearId2 & 0xfff] < 9) {
            saveData.saves[copyIdx].gearCounts[gearId2 & 0xfff]++;
        }
    }

    for (s32 i = 0; i < 10; i++) {
        saveData.saves[copyIdx].field10e[i] = data_02055fe4.field4e8[i];
    }

    SaveEventFlags * record = &saveData.saves[copyIdx].eventFlags;

    snapshotEventFlag(record, 0xe713);
    snapshotEventFlag(record, 0xe714);
    snapshotEventFlag(record, 0xe715);
    snapshotEventFlag(record, 0xe716);
    snapshotEventFlag(record, 0xe717);
    snapshotEventFlag(record, 0xe718);
    snapshotEventFlag(record, 0xe719);
    snapshotEventFlag(record, 0xe71a);
    snapshotEventFlag(record, 0xe71b);
    snapshotEventFlag(record, 0xe71c);
    snapshotEventFlag(record, 0xe71d);
    snapshotEventFlag(record, 0xe71e);
    snapshotEventFlag(record, 0xe71f);
    snapshotEventFlag(record, 0xe720);
    snapshotEventFlag(record, 0xe721);
    snapshotEventFlag(record, 0xe722);
    snapshotEventFlag(record, 0xe723);
    snapshotEventFlag(record, 0xe724);
    snapshotEventFlag(record, 0xe725);
    snapshotEventFlag(record, 0xe726);

    BOOL result;
    if (useBackup) {
        result = writeSaveBackup(slot);
    } else {
        result = writeSavePrimary(slot);
    }

    return (result) ? SAVE_OK : SAVE_FAILED;
}
