// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro.h>

#include "main/backup.h"

// --- Struct Definitions --- //

typedef struct {
    u8 initialized;  // +0x0
    u8 pad_01[0x3];
    CARDResult result;  // +0x4
    enum BackupStatus status;
    s32 lockId;  // +0xc
    void (*write)(u32, const void *, u32, MIDmaCallback, void *);  // +0x10
    void (*read)(u32, void *, u32, MIDmaCallback, void *);  // +0x14
} BackupWork;

// --- Variable Definitions --- //

BackupWork backup;  // 0x02058f6c

// --- Function Definitions --- //

// MATCHED 100%
void backupInit(u32 type) {
    if (!CARD_IsAvailable()) {
        CARD_Init();
    }

    if (backup.initialized == 0) {
        backup.lockId = OS_LOCK_ID_ERROR;

        lockBackup();

        switch (type) {
            case 0:
                CARD_IdentifyBackup(CARD_BACKUP_TYPE_EEPROM_4KBITS);
                backup.write = &CARD_WriteAndVerifyEepromAsync;
                backup.read = &CARD_ReadEepromAsync;
                break;
            case 1:
                CARD_IdentifyBackup(CARD_BACKUP_TYPE_EEPROM_64KBITS);
                backup.write = &CARD_WriteAndVerifyEepromAsync;
                backup.read = &CARD_ReadEepromAsync;
                break;
            case 2:
                CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_2MBITS);
                backup.write = &CARD_WriteAndVerifyFlashAsync;
                backup.read = &CARD_ReadFlashAsync;
        }

        unlockBackup();

        backup.status = BACKUP_IDLE;
        backup.result = CARD_RESULT_SUCCESS;
        backup.initialized = 1;
    }
}

// MATCHED 100%
BOOL backupRead(u32 src, void * dst, u32 len) {
    if  (!lockBackup()) {
        return FALSE;
    }

    backup.read(src, dst, len, &readDone, NULL);

    backup.status = BACKUP_BUSY;

    return TRUE;
}

// MATCHED 100%
void readDone(void * param1) {
    backup.result = CARD_GetResultCode();

    unlockBackup();

    if (backup.result != CARD_RESULT_SUCCESS) {
        backup.status = BACKUP_ERROR;
    } else {
        backup.status = BACKUP_IDLE;
    }
}

// MATCHED 100%
BOOL backupWrite(u32 dst, void * src, u32 len, BOOL addChecksum) {
    if (!lockBackup()) {
        return FALSE;
    }

    if (addChecksum) {
        u16 * footer = (u16 *)((u8 *)src + len);
        footer[-1] = calcChecksum(src, len);
    }

    backup.write(dst, src, len, &writeDone, NULL);
    backup.status = BACKUP_BUSY;

    return TRUE;
}

// MATCHED 100%
void writeDone(void * param1) {
    backup.result = CARD_GetResultCode();

    unlockBackup();

    if (backup.result != CARD_RESULT_SUCCESS) {
        backup.status = BACKUP_ERROR;
    } else {
        backup.status = BACKUP_IDLE;
    }
}

// MATCHED 100%
BOOL lockBackup() {

    s32 lock = OS_GetLockID();
    backup.lockId = lock;

    if (lock == OS_LOCK_ID_ERROR) {
        return FALSE;
    }

    CARD_LockBackup(lock);

    return TRUE;
}

// MATCHED 100%
void unlockBackup() {
    if (backup.lockId == OS_LOCK_ID_ERROR) {
        return;
    }

    CARD_UnlockBackup(backup.lockId);

    OS_ReleaseLockID(backup.lockId);

    backup.lockId = OS_LOCK_ID_ERROR;
}

// MATCHED 100%
BOOL backupWait() {
    if (CARD_WaitBackupAsync()) {
        return (backup.status != BACKUP_ERROR);
    }

    return FALSE;
}

// MATCHED 100%
u16 calcChecksum(void * data, u32 len) {
    return MATH_CalcChecksum16(data, len-2);
}

// MATCHED 100%
enum BackupStatus getBackupStatus() {
    return backup.status;
}
