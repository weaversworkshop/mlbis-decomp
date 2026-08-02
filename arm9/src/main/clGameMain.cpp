// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clGameMain.h"

#include "main/btlBoot.h"
#include "main/clGlobalEvent.h"
#include "main/fs.h"
#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "main/cpu_mem16.h"
#include "main/savedata.h"

// --- Struct Definitions --- //

typedef struct {
    u8 pad_00[0x10c8];
} UnkStruct_ov004_020cad58;

typedef struct {
    u8 pad_00[0x668];
} UnkStruct_ov007_0210fdc0;

typedef struct {
    u8 pad_00[0x1bd7c];
} UnkStruct_ov123_020935e4;

typedef struct {
    u8 pad_00[0x10968];
} UnkStruct_ov124_02093818;

typedef struct {
    u8 pad_00[0x4284];
} UnkStruct_ov125_02076f64;

typedef struct {
    u8 pad_00[0x6750];
} UnkStruct_ov125_0207c410;

typedef struct {
    u8 pad_00[0x1bbb8];
} UnkStruct_ov126_0208125c;

typedef struct {
    u8 pad_00[0x90];
} UnkStruct_ov128_0206ea20;

typedef struct {
    u8 pad_00[0x8bc];
} UnkStruct_ov129_0206e3f0;

typedef struct {
    u8 pad_00[0x8ac];
} UnkStruct_ov129_02070ff0;

typedef struct {
    u8 pad_00[0x148];
} UnkStruct_ov130_0206db80;

typedef struct {
    u8 pad_00[0x2e4];
} UnkStruct_ov131_02069d60;

typedef struct {
    u8 pad_00[0x428c];
} UnkStruct_ov132_020776c8;

// --- Extern Data Variables --- //

FS_EXTERN_OVERLAY(OVY_0);
FS_EXTERN_OVERLAY(OVY_2);
FS_EXTERN_OVERLAY(OVY_3);
FS_EXTERN_OVERLAY(OVY_4);
FS_EXTERN_OVERLAY(OVY_5);
FS_EXTERN_OVERLAY(OVY_6);
FS_EXTERN_OVERLAY(OVY_7);
FS_EXTERN_OVERLAY(OVY_8);

FS_EXTERN_OVERLAY(OVY_122);
FS_EXTERN_OVERLAY(OVY_123);
FS_EXTERN_OVERLAY(OVY_124);
FS_EXTERN_OVERLAY(OVY_125);
FS_EXTERN_OVERLAY(OVY_126);
FS_EXTERN_OVERLAY(OVY_127);
FS_EXTERN_OVERLAY(OVY_128);
FS_EXTERN_OVERLAY(OVY_129);
FS_EXTERN_OVERLAY(OVY_130);
FS_EXTERN_OVERLAY(OVY_131);
FS_EXTERN_OVERLAY(OVY_132);

FS_EXTERN_OVERLAY(OVY_138);
FS_EXTERN_OVERLAY(OVY_139);

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

extern void func_ov004_020cad58(UnkStruct_ov004_020cad58 *);
extern void func_ov007_0210fdc0(UnkStruct_ov007_0210fdc0 *);
extern void func_ov123_020935e4(UnkStruct_ov123_020935e4 *, u32, u32, u32);
extern void func_ov124_02093818(UnkStruct_ov124_02093818 *, u32, u32, u32);
extern void func_ov125_02076f64(UnkStruct_ov125_02076f64 *, u32, u32, u32);
extern void func_ov125_0207c410(UnkStruct_ov125_0207c410 *, u32, u32, u32);
extern void func_ov126_0208125c(UnkStruct_ov126_0208125c *);
extern void func_ov128_0206ea20(UnkStruct_ov128_0206ea20 *, u32, u32, u32, u32);
extern void func_ov129_0206e3f0(UnkStruct_ov129_0206e3f0 *, u32, u32);
extern void func_ov129_02070ff0(UnkStruct_ov129_02070ff0 *, u32, u32);
extern void func_ov130_0206db80(UnkStruct_ov130_0206db80 *, u32, u32);
extern void func_ov131_02069d60(UnkStruct_ov131_02069d60 *);
extern void func_ov132_020776c8(UnkStruct_ov132_020776c8 *, u32, u32, u32);

// --- Function Declarations --- //

#define BITSET_GET(ptr,idx) (((ptr)[idx >> 5] >> ((idx) & 0x1f)) & 1)
#define BITSET_SET(ptr,idx) ((ptr)[(idx) >> 5] |= 1 << ((idx) & 0x1f))
#define BITSET_CLR(ptr,idx) ((ptr)[(idx) >> 5] &= ~(1 << ((idx) & 0x1f)))

void initGameState(void);  // 0x020057fc
u32 getGameVar(u32, clGlobalEvent *, EventContext *);  // 0x020058bc
void setGameVar(u32, s32, clGlobalEvent *, EventContext *);  // 0x02005a78

// --- Variable Definitions --- //

clGameMain * gameMain;  // 0x02055fe0
GameState gameState;  // 0x02055fe4

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clGameMain::run() {
    if (this->field10 != 0) {
        if (this->field10 != 1) {
            return;
        }

        switch (this->prevMode) {
            case 0:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_2), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_3), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_4), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_6), MI_PROCESSOR_ARM9);
                break;
            case 1:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_2), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_3), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_7), MI_PROCESSOR_ARM9);
                break;
            case 2:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_8), MI_PROCESSOR_ARM9);
                break;
            case 3:
            case 4:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_125), MI_PROCESSOR_ARM9);
                break;
            case 5:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_123), MI_PROCESSOR_ARM9);
                break;
            case 6:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_124), MI_PROCESSOR_ARM9);
                break;
            case 7:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_128), MI_PROCESSOR_ARM9);
                break;
            case 8:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_2), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_131), MI_PROCESSOR_ARM9);
                break;
            case 9:
            case 10:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_129), MI_PROCESSOR_ARM9);
                break;
            case 11:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_130), MI_PROCESSOR_ARM9);
                break;
            case 12:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_126), MI_PROCESSOR_ARM9);
                break;
            case 13:
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
                fsUnloadOverlay(FS_OVERLAY_ID(OVY_132), MI_PROCESSOR_ARM9);
        }
    }

    switch (this->mode) {
        case 0: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_6))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_138));
            FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_139));
            FS_UnloadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_139));
            FS_UnloadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_138));

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_2), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_3), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_4), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_5), MI_PROCESSOR_ARM9);

            if (gameState.field4cc_2) {
                initGameState();
            }

            UnkStruct_ov004_020cad58 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov004_020cad58;
            if (ptr != NULL) {
                func_ov004_020cad58(ptr);
            }
            break;
        }
        case 1: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_7))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_2), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_3), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_7), MI_PROCESSOR_ARM9);

            UnkStruct_ov007_0210fdc0 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov007_0210fdc0;
            if (ptr != NULL) {
                func_ov007_0210fdc0(ptr);
            }
            break;
        }
        case 2: {
            btlEnter(0, 0);
            break;
        }
        case 3: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_125))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_125), MI_PROCESSOR_ARM9);

            UnkStruct_ov125_02076f64 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov125_02076f64;
            if (ptr != NULL) {
                func_ov125_02076f64(ptr, 8, 0, 0);
            }
            break;
        }
        case 4: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_125))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_125), MI_PROCESSOR_ARM9);

            UnkStruct_ov125_0207c410 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov125_0207c410;
            if (ptr != NULL) {
                func_ov125_0207c410(ptr, 8, 0, 0);
            }
            break;
        }
        case 5: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_123))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_123), MI_PROCESSOR_ARM9);

            UnkStruct_ov123_020935e4 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov123_020935e4;
            if (ptr != NULL) {
                func_ov123_020935e4(ptr, 8, 0, 0);
            }
            break;
        }
        case 6: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_124))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_124), MI_PROCESSOR_ARM9);

            UnkStruct_ov124_02093818 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov124_02093818;
            if (ptr != NULL) {
                func_ov124_02093818(ptr, 8, 0, 0);
            }
            break;
        }
        case 7: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_128))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_128), MI_PROCESSOR_ARM9);

            UnkStruct_ov128_0206ea20 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov128_0206ea20;
            if (ptr != NULL) {
                func_ov128_0206ea20(ptr, 8, 0, 0, 0);
            }
            break;
        }
        case 8: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_131))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }

            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_2), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_131), MI_PROCESSOR_ARM9);

            UnkStruct_ov131_02069d60 * ptr = new(HEAP_MAIN_HI, NULL, FALSE) UnkStruct_ov131_02069d60;
            if (ptr != NULL) {
                func_ov131_02069d60(ptr);
            }
            break;
        }
        case 9: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_129))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_129), MI_PROCESSOR_ARM9);

            UnkStruct_ov129_02070ff0 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov129_02070ff0;
            if (ptr != NULL) {
                func_ov129_02070ff0(ptr, 8, 0);
            }
            break;
        }
        case 10: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_129))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_129), MI_PROCESSOR_ARM9);

            UnkStruct_ov129_0206e3f0 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov129_0206e3f0;
            if (ptr != NULL) {
                func_ov129_0206e3f0(ptr, 8, 0);
            }
            break;
        }
        case 11: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_130))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_127), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_130), MI_PROCESSOR_ARM9);

            UnkStruct_ov130_0206db80 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov130_0206db80;
            if (ptr != NULL) {
                func_ov130_0206db80(ptr, 8, 0);
            }
            break;
        }
        case 12: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_126))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_126), MI_PROCESSOR_ARM9);

            UnkStruct_ov126_0208125c * ptr = new(HEAP_MAIN_LO, NULL, FALSE) UnkStruct_ov126_0208125c;
            if (ptr != NULL) {
                func_ov126_0208125c(ptr);
            }
            break;
        }
        case 13: {
            FSOverlayInfo info;
            void * heapBase;
            if (FS_LoadOverlayInfo(&info, MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_132))) {
                heapBase = info.header.ram_address + (info.header.ram_size + info.header.bss_size);
            } else {
                heapBase = NULL;
            }
            setMainHeapBase(heapBase);

            FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_138));
            FS_LoadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_139));
            FS_UnloadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_139));
            FS_UnloadOverlay(MI_PROCESSOR_ARM9, FS_OVERLAY_ID(OVY_138));

            fsLoadOverlay(FS_OVERLAY_ID(OVY_0), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_122), MI_PROCESSOR_ARM9);
            fsLoadOverlay(FS_OVERLAY_ID(OVY_132), MI_PROCESSOR_ARM9);

            UnkStruct_ov132_020776c8 * ptr = new(HEAP_MAIN_LO, NULL, TRUE) UnkStruct_ov132_020776c8;
            if (ptr != NULL) {
                func_ov132_020776c8(ptr, 8, 0, 0);
            }
            break;
        }
    }

    this->disable();
}

// MATCHED 100%
void clGameMain::setMode(u16 newMode) {
    this->prevMode = this->mode;
    this->mode = newMode;
    this->field10 = 1;

    this->enable();
}

// MATCHED 100%
void initGameState() {
    func_01ff8228(gameState.field000, 0, sizeof(gameState.field000));
    func_01ff8228(gameState.field040, 0, sizeof(gameState.field040));
    func_01ff8228(gameState.field054, 0, sizeof(gameState.field054));
    func_01ff8228(gameState.field05c, 0, sizeof(gameState.field05c));
    func_01ff8228(gameState.field0e4, 0, sizeof(gameState.field0e4));
    MI_CpuFill8(gameState.field2e4, 0, sizeof(gameState.field2e4));
    func_01ff8228(gameState.field37c, 0, sizeof(gameState.field37c));
    cpu_fill16(gameState.field4e8, 0, sizeof(gameState.field4e8));

    initParty(0);

    gameState.field4ce = 0;
    gameState.field4d0 = 0;
}

// MATCHED 100%
u32 getGameVar(u32 id, clGlobalEvent * sys, EventContext * ctx) {
    if ((id & 0xe000) == 0xe000) {
        u32 idx = id & ~0xe000;
        return BITSET_GET(gameState.field0e4, idx);
    }

    u32 idx = id & ID_INDEX_MASK;
    switch ((u16)(id & ID_SPACE_MASK)) {
        case 0x2000:
            return BITSET_GET(gameState.field054, idx);
        case 0xd000:
            return BITSET_GET(gameState.field05c, idx);
        case 0x3000:
        case 0x4000:
            return sys->getCustomVariable(ctx, id);
        case 0x5000:
            return gameState.field000[idx];
        case 0x6000:
            return gameState.field2e4[idx];
        case 0x9000:
            return sys->vars[idx];
        case 0xa000:
            return ctx->locals[idx];
        case 0xb000:
            return BITSET_GET(ctx->words, idx);
        case 0xc000:
            return gameState.field040[idx];
        case 0x1000:
            return ctx->args[idx];
        default:
            return 0;
    }
}

// MATCHED 100%
void setGameVar(u32 id, s32 value, clGlobalEvent * sys, EventContext * ctx) {
    if ((id & 0xe000) == 0xe000) {
        u32 idx = id & ~0xe000;
        if (value != 0) {
            BITSET_SET(gameState.field0e4, idx);
        } else {
            BITSET_CLR(gameState.field0e4, idx);
        }
        return;
    }

    u32 idx = id & ~ID_SPACE_MASK;
    switch ((u16)(id & ID_SPACE_MASK)) {
        case 0x2000:
            if (value != 0) {
                BITSET_SET(gameState.field054, idx);
            } else {
                BITSET_CLR(gameState.field054, idx);
            }
            break;
        case 0xd000:
            if (value != 0) {
                BITSET_SET(gameState.field05c, idx);
            } else {
                BITSET_CLR(gameState.field05c, idx);
            }
            break;
        case 0x4000:
            sys->setCustomVariable(ctx, id, value);
            break;
        case 0x5000:
            gameState.field000[idx] = value;
            break;
        case 0x6000:
            gameState.field2e4[idx] = value;
            break;
        case 0x9000:
            sys->vars[idx] = value;
            break;
        case 0xa000:
            ctx->locals[idx] = value;
            break;
        case 0xb000:
            if (value != 0) {
                BITSET_SET(ctx->words, idx);
            } else {
                BITSET_CLR(ctx->words, idx);
            }
            break;
        case 0xc000:
            gameState.field040[idx] = value;
            break;
        case 0x1000:
            ctx->args[idx] = value;
            break;
    }
}
