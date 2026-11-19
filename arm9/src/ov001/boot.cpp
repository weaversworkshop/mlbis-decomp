// objdiff-target: build/delinks/_dsd_gap@ov001_1.o

#include "ov001/boot.h"

#include "itcm/mainLoop.h"

#include "main/clGameMain.h"
#include "main/clVCountTask.h"
#include "main/fs.h"
#include "main/heap.h"
#include "main/sndEmitter.h"
#include "main/softReset.h"
#include "main/sound.h"

extern "C" {

#include "itcm/cpu_mem.h"

#include "main/backup.h"
#include "main/frameSync.h"
#include "main/rng.h"
#include "main/savedata.h"

// --- Extern Data Variables --- //

extern const u32 data_02047d3c;

// --- Function Declarations --- //

clFSEx * openNameArchives(void);  // 0x02069978
void loadNameMessages(void);  // 0x02069ac8
void initSoftResetInput(SoftResetState *);  // 0x02069bf8
BOOL loadSaveData(void);  // 0x02069cf8

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void bootInit(u32 fileTableSize) {
    initVCountTaskMgmt();

    OS_DisableIrqMask(OS_IE_V_BLANK);
    OS_EnableIrqMask(OS_IE_V_COUNT);

    initSoftResetInput(&softResetState);

    loadSaveData();

    CARD_SetCacheFlushThreshold(0x500, 0x2400);

    u8 * pool = (u8 *)::operator new[](fileTableSize, 5, NULL, FALSE);
    fsLoadFileTable(pool, fileTableSize);

    TP_Init();

    consoleInfo.hasTouchCalib = TP_GetUserInfo(&consoleInfo.touchCalib);

    if (consoleInfo.hasTouchCalib) {
        TP_SetCalibrateParam(&consoleInfo.touchCalib);
    }

    OS_GetOwnerInfo(&consoleInfo.owner);
    OS_GetMacAddress(consoleInfo.macAddress);

    consoleInfo.rtcOffset = OS_GetOwnerRtcOffset();

    RTCTime time;
    RTC_GetTime(&time);
    srand((time.hour << 16) + (time.minute << 8) +  (time.second));
    OS_InitTick();
    OS_SetTick((time.minute * 60) + (time.hour * 3600) + (time.second));

    loadNameMessages();

    sndInit(FALSE, 0xa0000, 0x2bc00, 0x2bc00);
    sndEmitClearQueue();

    irqTasksEnabled = 1;

    gameMain = new(HEAP_MAIN_HI, NULL, FALSE) clGameMain;
}

// MATCHED 100%
clFSEx * openNameArchives() {
    clFSEx * fs = new(HEAP_MAIN_LO, NULL, TRUE) clFSEx(8, 0, 4, 4, -1, 0);
    fs->openArchive(0, "/BData/mfset_AItmN.dat", NULL, 0, FALSE, 0, 0, 0);
    fs->openArchive(1, "/BData/mfset_UItmN.dat", NULL, 0, FALSE, 0, 0, 0);
    fs->openArchive(2, "/BData/mfset_BadgeN.dat", NULL, 0, FALSE, 0, 0, 0);
    fs->openArchive(3, "/BData/mfset_WearN.dat", NULL, 0, FALSE, 0, 0, 0);

    BOOL done = (fs->tableReqHead == NULL);
    while (!done) {
        fs->pumpTableLoads();
        done = (fs->tableReqHead == NULL);
    }

    return fs;
}

// MATCHED 100%
void loadNameMessages() {
    u32 slot;
    clFSEx * fs;
    u8 lang;

    cpu_fill32(&gameState, 0, sizeof(gameState));
    gameState.field4d4 = &data_02047d3c;

    fs = openNameArchives();

    lang = consoleInfo.owner.language;
    if (lang != OS_LANGUAGE_ENGLISH && lang != OS_LANGUAGE_FRENCH && lang != OS_LANGUAGE_SPANISH) {
        lang = OS_LANGUAGE_ENGLISH;
    }

    gameState.language = lang;

    for (s32 i = 0; i < 4; i++) {
        u32 maxSize = 0;
        slot = (s16)i;

        for (s32 chunk = 1; chunk <= 5; chunk++) {
            u32 size = fs->getChunkSize(slot, (u16)chunk);
            if (size > maxSize) {
                maxSize = size;
            }
        }

        if (maxSize != 0) {
            u8 * ptr = (u8 *)heapAlloc(1, maxSize, NULL, FALSE);
            gameState.messageBanks[i] = (u32 *)fs->readChunk(i, lang, 0, ptr, NULL, NULL, 0, 0, 1, FALSE);
        }
    }

    fs->waitAll();

    if (fs != NULL) {
        delete fs;
    }
}

// MATCHED 100%
void initSoftResetInput(SoftResetState * state) {
    MI_CpuFill8(&state->input, 0, sizeof(state->input));
    state->input.repeatDelay = 10;
    state->input.repeatRate = 2;

    readKeypad();
    decodeKeypad(&state->input, PAD_Read());

    if (reg_OS_IE & REG_OS_IE_VB_MASK) {
        waitVBlankSync();
    } else {
        waitFrameSync();
    }

    readKeypad();
    decodeKeypad(&state->input, PAD_Read());
}

// MATCHED 100%
clGameMain::clGameMain() : clTask(8, 0, 0) {
    mode = 0xd;
    prevMode = 0xffff;
}

// MATCHED 100%
BOOL loadSaveData() {
    cpu_fill32(&saveData, 0, sizeof(saveData));

    backupInit(1);

    if (backupRead(0, &saveData, sizeof(saveData)) == FALSE) {
        return FALSE;
    }

    return backupWait();
}
