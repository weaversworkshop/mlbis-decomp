// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/savedata.h"

#include <nitro/math.h>

#include "main/backup.h"
#include "main/clGameMain.h"
#include "main/clGlobalEvent.h"

// --- Variable Definitions --- //

const SaveHeader saveHeaderTemplate = { "MLRPG3", 1 };  // 0x0204ac38
SaveData saveData;  // 0x02058f84

// --- Function Definitions --- //

// MATCHED 100%
BOOL writeSavePrimary(u32 slotIdx) {  // Save into primary slot
    if (slotIdx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    backupWrite(slotIdx * SAVE_SLOT_SIZE + SAVE_SLOT_PRIMARY_OFFSET, &saveData.saves[slotIdx * SAVE_NUM_COPIES], SAVE_PAYLOAD_SIZE, TRUE);

    return TRUE;
}

// MATCHED 100%
BOOL writeSaveBackup(u32 slotIdx) {  // Save into backup slot
    if (slotIdx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    backupWrite(slotIdx * SAVE_SLOT_SIZE + SAVE_SLOT_BACKUP_OFFSET, &saveData.saves[slotIdx * SAVE_NUM_COPIES], SAVE_PAYLOAD_SIZE, TRUE);

    return TRUE;
}

// MATCHED 100%
s32 commitSave(s32 coinDelta, u32 gearId1, u32 gearId2, BOOL useBackup) {
    u32 slot = gameWork.field4cc_3;
    u32 slotBase = slot * SAVE_NUM_COPIES;

    if ((saveData.info.copyInUse & (1 << slotBase)) == 0) {
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
        saveData.saves[copyIdx].field10e[i] = gameWork.field4e8[i];
    }

    u32 * eventFlags = saveData.saves[copyIdx].eventFlags;

    snapshotEventFlag(eventFlags, 0xe713);
    snapshotEventFlag(eventFlags, 0xe714);
    snapshotEventFlag(eventFlags, 0xe715);
    snapshotEventFlag(eventFlags, 0xe716);
    snapshotEventFlag(eventFlags, 0xe717);
    snapshotEventFlag(eventFlags, 0xe718);
    snapshotEventFlag(eventFlags, 0xe719);
    snapshotEventFlag(eventFlags, 0xe71a);
    snapshotEventFlag(eventFlags, 0xe71b);
    snapshotEventFlag(eventFlags, 0xe71c);
    snapshotEventFlag(eventFlags, 0xe71d);
    snapshotEventFlag(eventFlags, 0xe71e);
    snapshotEventFlag(eventFlags, 0xe71f);
    snapshotEventFlag(eventFlags, 0xe720);
    snapshotEventFlag(eventFlags, 0xe721);
    snapshotEventFlag(eventFlags, 0xe722);
    snapshotEventFlag(eventFlags, 0xe723);
    snapshotEventFlag(eventFlags, 0xe724);
    snapshotEventFlag(eventFlags, 0xe725);
    snapshotEventFlag(eventFlags, 0xe726);

    BOOL result;
    if (useBackup) {
        result = writeSaveBackup(slot);
    } else {
        result = writeSavePrimary(slot);
    }

    return (result) ? SAVE_OK : SAVE_FAILED;
}
