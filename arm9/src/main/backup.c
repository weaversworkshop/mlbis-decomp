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
    void (*write)(u32, u32, u32, MIDmaCallback, void *);
    void (*read)(u32, u32, u32, MIDmaCallback, void *);
} BackupWork;

// --- Extern Functions --- //

extern s16 func_02038708(void); // OS_GetLockID?
extern void func_02038770(u16); // OS_ReleaseLockID?
extern BOOL func_020421e8(void);

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
                backup.write = &eepromWriteAsync;
                backup.read = &eepromReadAsync;
                break;
            case 1:
                CARD_IdentifyBackup(CARD_BACKUP_TYPE_EEPROM_64KBITS);
                backup.write = &eepromWriteAsync;
                backup.read = &eepromReadAsync;
                break;
            case 2:
                CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_2MBITS);
                backup.write = &flashWriteAsync;
                backup.read = &flashReadAsync;
        }

        unlockBackup();

        backup.status = BACKUP_IDLE;
        backup.result = CARD_RESULT_SUCCESS;
        backup.initialized = 1;
    }
}

// MATCHED 100% -- may return bool
void eepromWriteAsync(u32 dst, u32 src, u32 len, MIDmaCallback callback, void * arg) {
    CARDi_RequestStreamCommand(src, dst, len, callback, arg, TRUE, CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX, CARD_REQUEST_MODE_SEND_VERIFY);
    // CARDi_RequestStreamCommand
}

// MATCHED 100%
void eepromReadAsync(u32 src, u32 dst, u32 len, MIDmaCallback callback, void * arg) {
    CARDi_RequestStreamCommand(src, dst, len, callback, arg, TRUE, CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

// MATCHED 100%
void flashWriteAsync(u32 dst, u32 src, u32 len, MIDmaCallback callback, void * arg) {
    CARDi_RequestStreamCommand(src, dst, len, callback, arg, TRUE, CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX, CARD_REQUEST_MODE_SEND_VERIFY);
}

// MATCHED 100%
void flashReadAsync(u32 src, u32 dst, u32 len, MIDmaCallback callback, void * arg) {
    CARDi_RequestStreamCommand(src, dst, len, callback, arg, TRUE, CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

// MATCHED 100%
BOOL backupRead(u32 src, u32 dst, u32 len) {
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
BOOL backupWrite(u32 dst, u32 src, u32 len, BOOL addChecksum) {
    if (!lockBackup()) {
        return FALSE;
    }

    if (addChecksum) {
        u16 * footer = (u16 *)(src + len);
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

    s16 lock = func_02038708();
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

    func_02038770(backup.lockId);

    backup.lockId = OS_LOCK_ID_ERROR;
}

// MATCHED 100%
BOOL backupWait() {
    if (func_020421e8()) {
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
