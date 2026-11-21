// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clGuideMapChr.h"

// --- Function Definitions --- //

// MATCHED 100%
clGuideMapChr::clGuideMapChr(clFieldBase * field, u8 param2, UnkStruct_0206b0f4 * param3, u32 param4) : clFieldChrBase(field, param2, param3, param4) {
    this->fieldd4 = 0;

    this->posX = 0;
    this->posY = 0;

    if (param3 == NULL) {
        return;
    }

    this->posX = param3->x << FX32_SHIFT;
    this->posY = param3->y << FX32_SHIFT;
}

// MATCHED 100%
clGuideMapChr::~clGuideMapChr() {
    this->releaseCellAnime();
}

// MATCHED 100%
void clGuideMapChr::releaseCellAnime() {
    delete this->fieldd4;
    this->fieldd4 = NULL;
}
