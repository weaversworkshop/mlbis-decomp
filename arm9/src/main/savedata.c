// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/savedata.h"

#include <nitro/math.h>

#include "main/backup.h"
#include "main/clGameMain.h"
#include "main/clGlobalEvent.h"

// --- Struct Definitions --- //

#define SAVE_OK         0
#define SAVE_DISABLED   1
#define SAVE_FAILED     2

#define EVENT_FLAG_WORD(id) (((id) & 0xfff) >> 5)
#define EVENT_FLAG_BIT(id)  (1u << ((id) & 0x1f))

// --- Variable Definitions --- //

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
    if (getGameVar(id, NULL, NULL) != 0) {
        record->eventFlags[EVENT_FLAG_WORD(id)] |=  EVENT_FLAG_BIT(id);
    } else {
        record->eventFlags[EVENT_FLAG_WORD(id)] &= ~EVENT_FLAG_BIT(id);
    }
}

// MATCHED 100%
s32 commitSave(s32 coinDelta, u32 gearId1, u32 gearId2, BOOL useBackup) {
    u32 slot = gameState.field4cc_3;
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
        saveData.saves[copyIdx].field10e[i] = gameState.field4e8[i];
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
