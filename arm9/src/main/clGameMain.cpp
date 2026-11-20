// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clGameMain.h"

#include "main/btlBoot.h"
#include "main/clGlobalEvent.h"
#include "main/fs.h"
#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"
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

// --- Variable Definitions --- //

clGameMain * gameMain;  // 0x02055fe0
GameWork gameWork;  // 0x02055fe4

// --- Function Declarations --- //

#define BITSET_GET(ptr,idx) (((ptr)[idx >> 5] >> ((idx) & 0x1f)) & 1)
#define BITSET_SET(ptr,idx) ((ptr)[(idx) >> 5] |= 1 << ((idx) & 0x1f))
#define BITSET_CLR(ptr,idx) ((ptr)[(idx) >> 5] &= ~(1 << ((idx) & 0x1f)))

void initGameWork(void);  // 0x020057fc
u8 * getMessageText(u32, u32);  // 0x02005ca4
u8 * getItemNameText(s32);  // 0x02005cbc

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clGameMain::run() {
    if (this->field10 != 0) {
        if (this->field10 != 1) {
            return;
        }

        switch (this->prevGameMode) {
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

    switch (this->gameMode) {
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

            if (gameWork.field4cc_2) {
                initGameWork();
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
void clGameMain::change(u16 newMode) {
    this->prevGameMode = this->gameMode;
    this->gameMode = newMode;
    this->field10 = 1;

    this->enable();
}

// MATCHED 100%
void initGameWork() {
    cpu_fill32(gameWork.field000, 0, sizeof(gameWork.field000));
    cpu_fill32(gameWork.field040, 0, sizeof(gameWork.field040));
    cpu_fill32(gameWork.field054, 0, sizeof(gameWork.field054));
    cpu_fill32(gameWork.field05c, 0, sizeof(gameWork.field05c));
    cpu_fill32(gameWork.field0e4, 0, sizeof(gameWork.field0e4));
    MI_CpuFill8(gameWork.field2e4, 0, sizeof(gameWork.field2e4));
    cpu_fill32(gameWork.field37c, 0, sizeof(gameWork.field37c));
    cpu_fill16(gameWork.field4e8, 0, sizeof(gameWork.field4e8));

    initParty(0);

    gameWork.field4ce = 0;
    gameWork.field4d0 = 0;
}

// MATCHED 100%
u32 getRegFlag(u16 id, clGlobalEvent * sys, clLocalEvent * ctx) {
    if ((id & 0xe000) == 0xe000) {
        u32 idx = id & ~0xe000;
        return BITSET_GET(gameWork.field0e4, idx);
    }

    u32 idx = id & ID_INDEX_MASK;
    switch ((u16)(id & ID_SPACE_MASK)) {
        case 0x2000:
            return BITSET_GET(gameWork.field054, idx);
        case 0xd000:
            return BITSET_GET(gameWork.field05c, idx);
        case 0x3000:
        case 0x4000:
            return sys->getStatusReg(ctx, id);
        case 0x5000:
            return gameWork.field000[idx];
        case 0x6000:
            return gameWork.field2e4[idx];
        case 0x9000:
            return sys->vars[idx];
        case 0xa000:
            return ctx->workRegs[idx];
        case 0xb000:
            return BITSET_GET(ctx->workBitRegs, idx);
        case 0xc000:
            return gameWork.field040[idx];
        case 0x1000:
            return ctx->tempRegs[idx];
        default:
            return 0;
    }
}

// MATCHED 100%
void putRegFlag(u16 id, s32 value, clGlobalEvent * sys, clLocalEvent * ctx) {
    if ((id & 0xe000) == 0xe000) {
        u32 idx = id & ~0xe000;
        if (value != 0) {
            BITSET_SET(gameWork.field0e4, idx);
        } else {
            BITSET_CLR(gameWork.field0e4, idx);
        }
        return;
    }

    u32 idx = id & ~ID_SPACE_MASK;
    switch ((u16)(id & ID_SPACE_MASK)) {
        case 0x2000:
            if (value != 0) {
                BITSET_SET(gameWork.field054, idx);
            } else {
                BITSET_CLR(gameWork.field054, idx);
            }
            break;
        case 0xd000:
            if (value != 0) {
                BITSET_SET(gameWork.field05c, idx);
            } else {
                BITSET_CLR(gameWork.field05c, idx);
            }
            break;
        case 0x4000:
            sys->setStatusReg(ctx, id, value);
            break;
        case 0x5000:
            gameWork.field000[idx] = value;
            break;
        case 0x6000:
            gameWork.field2e4[idx] = value;
            break;
        case 0x9000:
            sys->vars[idx] = value;
            break;
        case 0xa000:
            ctx->workRegs[idx] = value;
            break;
        case 0xb000:
            if (value != 0) {
                BITSET_SET(ctx->workBitRegs, idx);
            } else {
                BITSET_CLR(ctx->workBitRegs, idx);
            }
            break;
        case 0xc000:
            gameWork.field040[idx] = value;
            break;
        case 0x1000:
            ctx->tempRegs[idx] = value;
            break;
    }
}

// MATCHED 100%
u8 * getMessageText(u32 bank, u32 idx) {
    return (u8*)gameWork.messageBanks[bank] + gameWork.messageBanks[bank][idx];
}

// MATCHED 100%
u8 * getItemNameText(s32 id) {
    switch (id & ID_SPACE_MASK) {
        case ID_KEYITEM:
            return getMessageText(0, *getNameIdPtr(id));
        case ID_ITEM:
            return getMessageText(1, *getNameIdPtr(id));
        case ID_BADGE:
            return getMessageText(2, *getNameIdPtr(id));
        case ID_GEAR:
            return getMessageText(3, *getNameIdPtr(id));
        default:
            return NULL;
    }
}
