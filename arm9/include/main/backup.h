#ifndef MAIN_BACKUP_H
#define MAIN_BACKUP_H

#include "types.h"

enum BackupStatus {
    BACKUP_IDLE,
    BACKUP_BUSY,
    BACKUP_ERROR
};

void backupInit(u32);  // 0x02017758
void eepromWriteAsync(u32, u32, u32, MIDmaCallback, void *);  // 0x02017844
void eepromReadAsync(u32, u32, u32, MIDmaCallback, void *);  // 0x0201788c
void flashWriteAsync(u32, u32, u32, MIDmaCallback, void *);  // 0x020178c0
void flashReadAsync(u32, u32, u32, MIDmaCallback, void *);  // 0x02017908
BOOL backupRead(u32, u32, u32);  // 0x0201793c
void readDone(void *);  // 0x020179a4
BOOL backupWrite(u32, u32, u32, BOOL);  // 0x020179d8
void writeDone(void *);  // 0x02017a54
BOOL lockBackup(void);  // 0x02017a88
void unlockBackup(void);  // 0x02017ac0
BOOL backupWait(void);  // 0x02017b0c
u16 calcChecksum(void *, u32);  // 0x02017b40
enum BackupStatus getBackupStatus(void);  // 0x02017b50

#endif
