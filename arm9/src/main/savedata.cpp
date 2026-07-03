// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include <nitro/math.h>

#include "main/backup.h"
#include "main/clGlobalEvent.h"
#include "main/savedata.h"

// --- Struct Definitions --- //

typedef struct {
    u8 pad_00[0x8];
    u32 field08_0 : 4;
    u32 field08_4_7 : 4;
    u32 field08_8 : 24;
    u8 pad0c[0x4];
} SaveHeader;

typedef struct { 
    u8 pad[0xe0];
    u32 flags_e0;
    u32 flags_e4;
} UnkStruct_02059148;

typedef struct {
    u8 pad_000[0x54];
    s32 coins; // ??
    u8 pad_058[0x78 - 0x58];
    s8 field078[0x84];
    u8 pad_0fc[0x10e - 0x0fc];
    u16 field10e[10];
    u8 pad_122[0x1b4 - 0x122];
    UnkStruct_02059148 field1b4;
    u8 data[SAVE_COPY_STRIDE - 0x29c];
} SaveCopy;

typedef struct { SaveCopy copy[2]; } SaveSlot;

typedef struct {
    SaveHeader header;
    SaveCopy saves[SAVE_SLOT_COUNT * 2];
} UnkStruct_02058f84;

// --- Extern Data Variables --- //

// --- Extern Functions --- //

extern s32 func_020058bc(u16 id, clGlobalEvent * param1, UnkStruct_02015f0c_01 * param2);

// --- Variable Definitions --- // (replace with real data later)

extern UnkStruct_02058f84 data_02058f84;

// --- Function Declarations --- //

BOOL func_02017b60(u32 slot_idx);
BOOL func_02017ba4(u32 slot_idx);
s32 func_02017bec(s32 param1, u32 param2, u32 param3, BOOL is_bkup);

// --- Function Definitions --- //

// MATCHED 100%
BOOL func_02017b60(u32 slot_idx) {  // Save into primary slot
    if (slot_idx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    func_020179d8((void *)(slot_idx * SAVE_SLOT_SIZE + SAVE_SLOT_PRIMARY_OFFSET), &data_02058f84.saves[slot_idx * 2], SAVE_PAYLOAD_SIZE, 1);

    return TRUE;
}

// MATCHED 100%
BOOL func_02017ba4(u32 slot_idx) {  // Save into backup slot
    if (slot_idx >= SAVE_SLOT_COUNT) {
        return FALSE;
    }

    func_020179d8((void *)(slot_idx * SAVE_SLOT_SIZE + SAVE_SLOT_BACKUP_OFFSET), &data_02058f84.saves[slot_idx * 2], SAVE_PAYLOAD_SIZE, 1);

    return TRUE;
}

// MATCHED 100%
s32 func_02017bec(s32 param1, u32 param2, u32 param3, BOOL is_bkup) {
    u32 slot = data_02055fe4.field4cc_3_4;
    u32 r7 = 2 * slot;

    if ((data_02058f84.header.field08_4_7 & (1 << r7)) == 0) {
        return 1;
    }

    u32 n = (r7 + is_bkup);

    s32 coins = data_02058f84.saves[n].coins;
    coins += param1;
    if (coins < 0) {
        coins = 0;
    }
    
    if (coins > MAX_COINS) {
        coins = MAX_COINS;
    }
    data_02058f84.saves[n].coins = coins;

    if (param2 != 0) {
        if (data_02058f84.saves[n].field078[param2 & 0xfff] < 9) {
            data_02058f84.saves[n].field078[param2 & 0xfff]++;
        }
    }

    if (param3 != 0) {
        if (data_02058f84.saves[n].field078[param3 & 0xfff] < 9) {
            data_02058f84.saves[n].field078[param3 & 0xfff]++;
        }
    }

    for (s32 i = 0; i < 10; i++) {
        data_02058f84.saves[n].field10e[i] = data_02055fe4.field4e8[i];
    }

    UnkStruct_02059148 * rec =  &data_02058f84.saves[n].field1b4;

    if (func_020058bc(0xe713, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x80000;
    } else {
        rec->flags_e0 &= ~0x80000;
    }

    if (func_020058bc(0xe714, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x100000;
    } else {
        rec->flags_e0 &= ~0x100000;
    }

    if (func_020058bc(0xe715, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x200000;
    } else {
        rec->flags_e0 &= ~0x200000;
    }

    if (func_020058bc(0xe716, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x400000;
    } else {
        rec->flags_e0 &= ~0x400000;
    }

    if (func_020058bc(0xe717, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x800000;
    } else {
        rec->flags_e0 &= ~0x800000;
    }

    if (func_020058bc(0xe718, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x1000000;
    } else {
        rec->flags_e0 &= ~0x1000000;
    }

    if (func_020058bc(0xe719, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x2000000;
    } else {
        rec->flags_e0 &= ~0x2000000;
    }

    if (func_020058bc(0xe71a, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x4000000;
    } else {
        rec->flags_e0 &= ~0x4000000;
    }

    if (func_020058bc(0xe71b, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x8000000;
    } else {
        rec->flags_e0 &= ~0x8000000;
    }

    if (func_020058bc(0xe71c, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x10000000;
    } else {
        rec->flags_e0 &= ~0x10000000;
    }

    if (func_020058bc(0xe71d, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x20000000;
    } else {
        rec->flags_e0 &= ~0x20000000;
    }

    if (func_020058bc(0xe71e, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x40000000;
    } else {
        rec->flags_e0 &= ~0x40000000;
    }

    if (func_020058bc(0xe71f, NULL, NULL) != 0) {
        rec->flags_e0 |= 0x80000000;
    } else {
        rec->flags_e0 &= ~0x80000000;
    }


    if (func_020058bc(0xe720, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x1;
    } else {
        rec->flags_e4 &= ~0x1;
    }

    if (func_020058bc(0xe721, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x2;
    } else {
        rec->flags_e4 &= ~0x2;
    }

    if (func_020058bc(0xe722, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x4;
    } else {
        rec->flags_e4 &= ~0x4;
    }

    if (func_020058bc(0xe723, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x8;
    } else {
        rec->flags_e4 &= ~0x8;
    }

    if (func_020058bc(0xe724, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x10;
    } else {
        rec->flags_e4 &= ~0x10;
    }

    if (func_020058bc(0xe725, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x20;
    } else {
        rec->flags_e4 &= ~0x20;
    }

    if (func_020058bc(0xe726, NULL, NULL) != 0) {
        rec->flags_e4 |= 0x40;
    } else {
        rec->flags_e4 &= ~0x40;
    }

    BOOL result;
    if (is_bkup) {
        result = func_02017ba4(slot);
    } else {
        result = func_02017b60(slot);
    }

    return (result) ? 0 : 2;
}

} // extern "C"
