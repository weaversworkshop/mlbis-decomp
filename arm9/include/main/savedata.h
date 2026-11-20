#ifndef MAIN_SAVEDATA_H
#define MAIN_SAVEDATA_H

#include "types.h"

#include "main/clGameMain.h"

#define SAVE_OK         0
#define SAVE_DISABLED   1
#define SAVE_FAILED     2

#define MAX_COINS          999999 // Not sure

#define SAVE_COPY_STRIDE    0x7ec
#define SAVE_PAYLOAD_SIZE   0x5f4
#define SAVE_SLOT_COUNT     2
#define SAVE_NUM_COPIES     2
#define SAVE_COPY_COUNT     (SAVE_SLOT_COUNT * SAVE_NUM_COPIES)

#define SAVE_SLOT_SIZE      (SAVE_COPY_STRIDE * 2)

#define SAVE_SLOT_PRIMARY_OFFSET  0x10
#define SAVE_SLOT_BACKUP_OFFSET  (SAVE_SLOT_PRIMARY_OFFSET + SAVE_COPY_STRIDE)

#define SAVE_HEADER_MAGIC_SIZE   6

#define EVENT_FLAG_COUNT    128
#define BYTE_VAR_COUNT      152

#define EVENT_FLAG_WORD(id) (((id) & 0xfff) >> 5)
#define EVENT_FLAG_BIT(id)  (1u << ((id) & 0x1f))

#define GVAR_SPACE_6000    0x6000

typedef struct {
    u8 magic[SAVE_HEADER_MAGIC_SIZE];  // +0x0
    u8 version;  // +0x6
    u8 pad_07;
} SaveHeader;

typedef struct {
    u8 pad_000[0x54];
    s32 coins;  // +0x54
    u8 pad_058[0x78 - 0x58];
    s8 gearCounts[0x84];  // +0x78
    u32 field0fc;
    u8 pad_100[0x10e - 0x100];
    u16 field10e[10];
    u8 pad_122[0x1b4 - 0x122];
    u32 eventFlags[EVENT_FLAG_COUNT];  // +0x1b4
    u8 byteVars[BYTE_VAR_COUNT];  // +0x3b4
    u8 pad_44c[0x45c - 0x44c];
    void * field45c;
    u8 data[SAVE_COPY_STRIDE - 0x460];  // +0x460
} SaveCopy;

typedef struct {
    SaveHeader header;  // +0x0
    struct {
        u32 : 4;
        u32 copyInUse : 4;
        u32 : 24;
    } info;
    u32 field0c;
    SaveCopy saves[SAVE_COPY_COUNT];  // +0x10
    u8 pad_1fc0[0x40];
} SaveData;

BOOL writeSavePrimary(u32);  // 0x02017b60
BOOL writeSaveBackup(u32);  // 0x02017ba4
s32 commitSave(s32, u32, u32, BOOL);  // 0x02017bec

static inline void snapshotEventFlag(u32 * eventFlags, u16 id) {
    if (getRegFlag(id, NULL, NULL) != 0) {
        eventFlags[EVENT_FLAG_WORD(id)] |=  EVENT_FLAG_BIT(id);
    } else {
        eventFlags[EVENT_FLAG_WORD(id)] &= ~EVENT_FLAG_BIT(id);
    }
}

static inline void snapshotByteVar(u8 * byteVars, u16 id) {
    byteVars[id - GVAR_SPACE_6000] = getRegFlag(id, NULL, NULL);
}

extern const SaveHeader saveHeaderTemplate;
extern SaveData saveData;

#endif
