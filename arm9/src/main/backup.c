// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include <nitro/card.h>
#include <nitro/math.h>

#include "main/backup.h"

// --- Struct Definitions --- //

typedef struct {
    union {
        u32 field00;
        struct {
            u8 field00_b;
            u8 pad_01[0x3];
        };
    };
    CARDResult field04;
    u32 field08;
    s32 field0c;
    void (*fn_10)(void *, void *, u32, MIDmaCallback, void *);
    void (*fn_14)(void *, void *, u32, MIDmaCallback, void *);
} UnkGlobal_02058f6c;

// --- Extern Data --- //

extern UnkGlobal_02058f6c data_02058f6c;

// --- Extern Functions --- //

extern s16 func_02038708(void);
extern void func_02038770(u16);
extern BOOL func_020421e8(void);
extern void func_02041fb4(void * src, void * dst, u32 len, MIDmaCallback callback, void *arg, BOOL is_async, CARDRequest req, int retry_ct, CARDRequestMode mode); // FORK of CARDi_ProgramAndVerifyBackup
extern void func_020420b0(CARDBackupType type);

// --- Function Declarations --- //

void func_02017758(u32 param1);
void func_02017844(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg); // FORK of CARD_ProgramAndVerifyBackupAsync ??
void func_0201788c(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg);
void func_020178c0(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg);
void func_02017908(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg);
BOOL func_0201793c(void * dst, void * src, u32 len);
void func_020179a4(void * param1);
BOOL func_020179d8(void * dst, void * src, u32 len, u32 param4);
void func_02017a54(void * param1);
BOOL func_02017a88(void);
void func_02017ac0(void);
BOOL func_02017b0c(void);
u16 func_02017b40(void * data, u32 len);
u32 func_02017b50(void);
BOOL func_02017b60(u32 slot_idx);
BOOL func_02017ba4(u32 slot_idx);

// --- Function Definitions --- //

// MATCHED 100%
void func_02017758(u32 param1) {
    if (!CARD_IsAvailable()) {
        CARD_Init();
    }

    if (data_02058f6c.field00_b == 0) {
        data_02058f6c.field0c = OS_LOCK_ID_ERROR;

        func_02017a88();

        switch (param1) {
            case 0:
                func_020420b0(CARD_BACKUP_TYPE_EEPROM_4KBITS);
                data_02058f6c.fn_10 = &func_02017844;
                data_02058f6c.fn_14 = &func_0201788c;
                break;
            case 1:
                func_020420b0(CARD_BACKUP_TYPE_EEPROM_64KBITS);
                data_02058f6c.fn_10 = &func_02017844;
                data_02058f6c.fn_14 = &func_0201788c;
                break;
            case 2:
                func_020420b0(CARD_BACKUP_TYPE_FLASH_2MBITS);
                data_02058f6c.fn_10 = &func_020178c0;
                data_02058f6c.fn_14 = &func_02017908;
        }

        func_02017ac0();

        data_02058f6c.field08 = 0;
        data_02058f6c.field04 = CARD_RESULT_SUCCESS;
        data_02058f6c.field00_b = 1;
    }
}

// MATCHED 100% -- may return bool
void func_02017844(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg) {
    func_02041fb4(src, dst, len, callback, arg, TRUE, CARD_REQ_PROGRAM_BACKUP, CARD_RETRY_COUNT_MAX, CARD_REQUEST_MODE_SEND_VERIFY);
}

// MATCHED 100%
void func_0201788c(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg) {
    func_02041fb4(dst, src, len, callback, arg, TRUE, CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

// MATCHED 100%
void func_020178c0(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg) {
    func_02041fb4(src, dst, len, callback, arg, TRUE, CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX, CARD_REQUEST_MODE_SEND_VERIFY);
}

// MATCHED 100%
void func_02017908(void * dst, void * src, u32 len, MIDmaCallback callback, void *arg) {
    func_02041fb4(dst, src, len, callback, arg, TRUE, CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

// MATCHED 100%
BOOL func_0201793c(void * dst, void * src, u32 len) {
    if  (!func_02017a88()) {
        return FALSE;
    }

    data_02058f6c.fn_14(dst, src, len, &func_020179a4, 0);

    data_02058f6c.field08 = 1;

    return TRUE;
}

// MATCHED 100%
void func_020179a4(void * param1) {
    data_02058f6c.field04 = CARD_GetResultCode();

    func_02017ac0();

    if (data_02058f6c.field04 != CARD_RESULT_SUCCESS) {
        data_02058f6c.field08 = 2;
    } else {
        data_02058f6c.field08 = 0;
    }
}

// MATCHED 100%
BOOL func_020179d8(void * dst, void * src, u32 len, u32 param4) {
    if (!func_02017a88()) {
        return FALSE;
    }

    if (param4 != 0) {
        u16 *footer = (u16 *)((u8 *)src + len);
        footer[-1] = func_02017b40(src, len);
    }

    data_02058f6c.fn_10(dst, src, len, &func_02017a54, NULL);
    data_02058f6c.field08 = 1;

    return TRUE;
}

// MATCHED 100%
void func_02017a54(void * param1) {
    data_02058f6c.field04 = CARD_GetResultCode();

    func_02017ac0();

    if (data_02058f6c.field04 != CARD_RESULT_SUCCESS) {
        data_02058f6c.field08 = 2;
    } else {
        data_02058f6c.field08 = 0;
    }
}

// MATCHED 100%
BOOL func_02017a88() {

    s16 lock = func_02038708();
    data_02058f6c.field0c = lock;

    if (lock == OS_LOCK_ID_ERROR) {
        return FALSE;
    }

    CARD_LockBackup(lock);

    return TRUE;
}

// MATCHED 100%
void func_02017ac0() {
    if (data_02058f6c.field0c == OS_LOCK_ID_ERROR) {
        return;
    }

    CARD_UnlockBackup(data_02058f6c.field0c);

    func_02038770(data_02058f6c.field0c);

    data_02058f6c.field0c = OS_LOCK_ID_ERROR;
}

// MATCHED 100%
BOOL func_02017b0c() {
    if (func_020421e8()) {
        return (data_02058f6c.field08 != 2);
    }

    return FALSE;
}

// MATCHED 100%
u16 func_02017b40(void * data, u32 len) {
    return MATH_CalcChecksum16(data, len-2);
}

// MATCHED 100%
u32 func_02017b50() {
    return data_02058f6c.field08;
}
