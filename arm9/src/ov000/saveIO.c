// objdiff-target: build/delinks/_dsd_gap@ov000_2.o

#include "ov000/saveIO.h"

#include <nitro.h>
#include <stddef.h>

#include "itcm/cpu_mem.h"

#include "main/clGameMain.h"
#include "main/backup.h"
#include "main/heap.h"
#include "main/savedata.h"

// --- Function Declarations --- //

BOOL verifyChecksum(void *, u32);  // 0x02066a28
BOOL backupClear(u32, u32, BOOL);  // 0x02066a50
BOOL writeSaveHeader(void);  // 0x02066aa8
BOOL readSaveHeader(void);  // 0x02066ad8
BOOL isSaveHeaderValid(void);  // 0x02066b04
BOOL writeSaveInfoWait(void);  // 0x02066b58
BOOL writeSaveInfo(void);  // 0x02066b88
BOOL readSaveInfo(void);  // 0x02066ba8
BOOL verifySaveInfo(void);  // 0x02066bd4
BOOL func_ov000_02066c8c(void);  // 0x02066c8c
BOOL func_ov000_02066cb8(void);  // 0x02066cb8
BOOL readSaveCopyWait(u8);  // 0x02066cd0
BOOL readSaveCopy(u8);  // 0x02066d0c
BOOL isSaveCopyValid(u8);  // 0x02066d38
BOOL verifySaveCopy(SaveCopy *);  // 0x02066d58
BOOL formatSaveData(void);  // 0x02066d6c
BOOL clearSaveInfo(void);  // 0x02066e10
BOOL func_ov000_02066e2c(void);  // 0x02066e2c
BOOL clearSaveCopy(u8);  // 0x02066e48
BOOL copySaveCopy(u8, u8);  // 0x02066e70
void * func_ov000_02066f18(void);  // 0x02066f18
u32 commitGauntletResult(s32, BOOL);  // 0x02066f48
BOOL rebuildSaveInfo(void);  // 0x0206726c

// --- Function Definitions --- //

// MATCHED 100%
BOOL verifyChecksum(void * data, u32 len) {
    u16 checksum = calcChecksum(data, len);
    return (checksum == ((u16*)((u8*)data + len))[-1]);
}

// MATCHED 100%
BOOL backupClear(u32 dst, u32 len, BOOL addChecksum) {
    void * buf = heapAlloc(0, len, NULL, FALSE);
    cpu_fill32(buf, 0, len);

    BOOL result = backupWrite(dst, buf, len, addChecksum);
    heapFree(buf);

    return result;
}

// MATCHED 100%
BOOL writeSaveHeader() {
    if (!backupWrite(offsetof(SaveData, header), &saveHeaderTemplate, sizeof(saveHeaderTemplate), FALSE)) {
        return FALSE;
    }
    return backupWait();
}

// MATCHED 100%
BOOL readSaveHeader() {
    if (!backupRead(offsetof(SaveData, header), &saveData.header, sizeof(saveData.header))) {
        return FALSE;
    }
    return backupWait();
}

// MATCHED 100%
BOOL isSaveHeaderValid() {
    for (s32 i = 0; i < SAVE_HEADER_MAGIC_SIZE; i++) {
        if (saveData.header.magic[i] != saveHeaderTemplate.magic[i]) {
            return FALSE;
        }
    }

    return (saveData.header.version == 1);
}

// MATCHED 100%
BOOL writeSaveInfoWait() {
    if (!backupWrite(offsetof(SaveData, info), &saveData.info, sizeof(saveData.info), TRUE)) {
        return FALSE;
    }
    return backupWait();
}

// MATCHED 100%
BOOL writeSaveInfo() {
    return backupWrite(offsetof(SaveData, info), &saveData.info, sizeof(saveData.info), TRUE);
}

// MATCHED 100%
BOOL readSaveInfo() {
    if (!backupRead(offsetof(SaveData, info), &saveData.info, sizeof(saveData.info))) {
        return FALSE;
    }
    return backupWait();
}

// MATCHED 100%
BOOL verifySaveInfo() {
    if (verifyChecksum(&saveData.info, sizeof(saveData.info))) {
        for (s32 copyIdx = 0; copyIdx < SAVE_COPY_COUNT; copyIdx++) {
            if (!isSaveCopyValid(copyIdx) || saveData.saves[copyIdx].field0fc != 0) {
                saveData.info.copyInUse |= (1 << copyIdx);
            } else {
                saveData.info.copyInUse &= ~(1 << copyIdx);
            }
        }

        return TRUE;
    }

    return FALSE;
}

// MATCHED 100%
BOOL func_ov000_02066c8c() {
    if (!backupRead(offsetof(SaveData, field0c), &saveData.field0c, sizeof(saveData.field0c))) {
        return FALSE;
    }
    return backupWait();
}

// MATCHED 100%
BOOL func_ov000_02066cb8() {
    return verifyChecksum(&saveData.field0c, sizeof(saveData.field0c));
}

// MATCHED 100%
BOOL readSaveCopyWait(u8 copyIdx) {
    if (!backupRead(offsetof(SaveData, saves[copyIdx]), &saveData.saves[copyIdx], SAVE_PAYLOAD_SIZE)) {
        return FALSE;
    }
    return backupWait();
}

// MATCHED 100%
BOOL readSaveCopy(u8 copyIdx) {
    return backupRead(offsetof(SaveData, saves[copyIdx]), &saveData.saves[copyIdx], SAVE_PAYLOAD_SIZE);
}

// MATCHED 100%
BOOL isSaveCopyValid(u8 copyIdx) {
    return verifySaveCopy(&saveData.saves[copyIdx]);
}

// MATCHED 100%
BOOL verifySaveCopy(SaveCopy * save) {
    return verifyChecksum(save, SAVE_PAYLOAD_SIZE);
}

// MATCHED 100%
BOOL formatSaveData() {
    if (!writeSaveHeader()) {
        return FALSE;
    }
    if (!readSaveHeader()) {
        return FALSE;
    }
    if (!clearSaveInfo()) {
        return FALSE;
    }
    if (!readSaveInfo()) {
        return FALSE;
    }
    if (!func_ov000_02066e2c()) {
        return FALSE;
    }
    if (!func_ov000_02066c8c()) {
        return FALSE;
    }

    for (s32 copyIdx = 0; copyIdx < SAVE_COPY_COUNT; copyIdx++) {
        if (!clearSaveCopy(copyIdx)) {
            return FALSE;
        }
        if (!readSaveCopyWait(copyIdx)) {
            return FALSE;
        }
    }

    return TRUE;
}

// MATCHED 100%
BOOL clearSaveInfo() {
    backupClear(offsetof(SaveData, info), sizeof(saveData.info), TRUE);
    return backupWait();
}

// MATCHED 100%
BOOL func_ov000_02066e2c() {
    backupClear(offsetof(SaveData, field0c), sizeof(saveData.field0c), TRUE);
    return backupWait();
}

// MATCHED 100%
BOOL clearSaveCopy(u8 copyIdx) {
    backupClear(offsetof(SaveData, saves[copyIdx]), SAVE_PAYLOAD_SIZE, TRUE);
    return backupWait();
}

// MATCHED 100%
BOOL copySaveCopy(u8 srcIdx, u8 dstIdx) {
    SaveCopy copy;

    backupRead(srcIdx * sizeof(SaveCopy) + offsetof(SaveData, saves), &copy, SAVE_PAYLOAD_SIZE);
    if (!backupWait()) {
        return FALSE;
    }

    backupWrite(dstIdx * sizeof(SaveCopy) + offsetof(SaveData, saves), &copy, SAVE_PAYLOAD_SIZE, FALSE);
    if (!backupWait()) {
        return FALSE;
    }

    cpu_copy16(&saveData.saves[srcIdx], &saveData.saves[dstIdx], SAVE_PAYLOAD_SIZE);
    return TRUE;
}

// MATCHED 100%
void * func_ov000_02066f18() {
    return &saveData.saves[gameState.field4cc_3 * SAVE_NUM_COPIES].field45c;
}

// MATCHED 100%
u32 commitGauntletResult(s32 coinDelta, BOOL useBackup) {
    u32 slot = gameState.field4cc_3;
    u32 slotBase = slot * SAVE_NUM_COPIES;

    if ((saveData.info.copyInUse & (1 << slotBase)) == 0) {
        return SAVE_DISABLED;
    }

    u32 copyIdx = slotBase + useBackup;

    s32 coins = saveData.saves[copyIdx].coins;
    coins += coinDelta;
    if (coins < 0) {
        coins = 0;
    }
    if (coins > MAX_COINS) {
        coins = MAX_COINS;
    }
    saveData.saves[copyIdx].coins = coins;

    u32 * eventFlags = saveData.saves[copyIdx].eventFlags;
    u8 * byteVars = saveData.saves[copyIdx].byteVars;

    snapshotEventFlag(eventFlags, 0xed59);
    snapshotEventFlag(eventFlags, 0xed5a);
    snapshotEventFlag(eventFlags, 0xed5b);
    snapshotEventFlag(eventFlags, 0xed5c);
    snapshotEventFlag(eventFlags, 0xed5d);
    snapshotEventFlag(eventFlags, 0xed5e);
    snapshotEventFlag(eventFlags, 0xed5f);

    snapshotByteVar(byteVars, 0x6088);
    snapshotByteVar(byteVars, 0x6089);
    snapshotByteVar(byteVars, 0x608a);
    snapshotByteVar(byteVars, 0x608b);
    snapshotByteVar(byteVars, 0x608c);
    snapshotByteVar(byteVars, 0x608d);
    snapshotByteVar(byteVars, 0x608e);
    snapshotByteVar(byteVars, 0x608f);
    snapshotByteVar(byteVars, 0x6090);
    snapshotByteVar(byteVars, 0x6091);
    snapshotByteVar(byteVars, 0x6092);
    snapshotByteVar(byteVars, 0x6093);
    snapshotByteVar(byteVars, 0x6094);
    snapshotByteVar(byteVars, 0x6095);

    BOOL result;
    if (useBackup) {
        result = writeSaveBackup(slot);
    } else {
        result = writeSavePrimary(slot);
    }

    return (result) ? SAVE_OK : SAVE_FAILED;
}

// MATCHED 100%
BOOL rebuildSaveInfo() {
    cpu_fill32(&saveData.info, 0, sizeof(saveData.info));

    for (s32 copyIdx = 0; copyIdx < SAVE_COPY_COUNT; copyIdx++) {
        if (!isSaveCopyValid(copyIdx) || saveData.saves[copyIdx].field0fc) {
            saveData.info.copyInUse |= (1 << copyIdx);
        }
    }

    return writeSaveInfoWait();
}
