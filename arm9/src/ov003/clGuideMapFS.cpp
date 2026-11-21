// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clGuideMapFS.h"

extern "C" {

#include "main/system.h"

#include "data/clGuideMapFS.h"

}

// --- Function Definitions --- //

// MATCHED 100%
clGuideMapFS::clGuideMapFS(u8 priority, u16 numSlots) : clFSEx(priority, 0, 0x18, numSlots, 2, 0) {
    this->openArchive(7, "/EDataSave/mfset_EMesPlace.dat", NULL, 0, FALSE, 0, 0, 0);

    BOOL loaded = (this->tableReqHead == NULL);
    while (!loaded) {
        this->pumpTableLoads();
        loaded = (this->tableReqHead == NULL);
    }

    this->setArchive(0, "/FObjPc/FObjPc.dat", &fobjPcTable, FALSE);
    this->setArchive(1, "/FObj/FObj.dat", &fobjTable, FALSE);
    this->setArchive(2, "/FObjMon/FObjMon.dat", &fobjMonTable, FALSE);
    this->setArchive(3, "/FObjMap/FObjMap.dat", &fobjMapTable, FALSE);
    this->setArchive(4, "/FEvent/FEvent.dat", &feventTable, FALSE);
    this->setArchive(5, "/FMap/FMapData.dat", &fmapDataTable, FALSE);
    this->setArchive(6, "/FPaf/FPaf.dat", &fpafTable, FALSE);
}

// MATCHED 100%
clGuideMapFS::~clGuideMapFS() {}

// MATCHED 100%
void clGuideMapFS::run() {
    BOOL run = TRUE;
    while (run) {
        BOOL empty = (this->reqHead == NULL);
        if (!empty) {
            this->pumpReads();
        } else {
            this->stallCount = 0;
            break;
        }

        run = FALSE;

        if (systemState.frameTimer.frameSkips == 0) {
            BOOL idle = TRUE;
            if (GX_GetVCount() >= this->vcountBusyStart && GX_GetVCount() <= GX_LCD_SIZE_Y) {
                idle = FALSE;
            }

            if (idle) {
                run = TRUE;
            }
        }
    }

    this->stallCount = 0;
}
