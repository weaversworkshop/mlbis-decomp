// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clFieldBase.h"

#include "main/heap.h"

#include "ov003/clFieldCellAnime.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"

// --- Function Declarations --- //

void initMainDisplay(void);  // 0x0206af18
void initSubDisplay(void);  // 0x0206afdc

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void clFieldBase::init() {
    this->reset();
    this->unk_virtual_11(16);
}

// MATCHED 100%
void clFieldBase::reset() {
    cpu_fill32(this->field20, 0, sizeof(this->field20));

    this->map = NULL;
    this->field1c = NULL;
    
    this->fieldd1 = 0;

    this->fielda8 = 0;
    this->fielda0 = 0;

    this->fieldbc = 0;
    this->fieldb8 = 0;

    this->fieldd0 = 0;
    this->fieldccw = 0;

    this->brightness = this->unk_virtual_10() << FX32_SHIFT;

    this->fadeActive = this->fadePaused = 0;
}

// MATCHED 100%
void clFieldBase::func_ov003_0206a9f8() {
    this->map->work.vblank->onVCount();
}

// MATCHED 100%
void clFieldBase::createObjMans() {
    this->objCgVramMan = new(this->unk_virtual_13(), NULL, FALSE) nsObjSys::clObjCgVramMan(this->unk_virtual_04());
    this->objPltVramMan = new(this->unk_virtual_13(), NULL, FALSE) nsObjSys::clObjPltVramMan(this->unk_virtual_04(), 0, 0, 0);
    this->oamMan = new(this->unk_virtual_13(), NULL, FALSE) nsObjSys::clOamMan(this->unk_virtual_04());
}

// MATCHED 100%
void clFieldBase::destroyObjMans() {
    if (this->field1c != NULL) {

        u32 idx = this->unk_virtual_04();
        objPlttBufs[idx].ext = NULL;
        objPlttBufs[idx].std = NULL;

        this->field1c = NULL;

        delete this->objCgVramMan;
        delete this->objPltVramMan;
        delete this->oamMan;
    }
}

// MATCHED 100%
void clFieldBase::drawCellAnimes() {
    if (this->field1c != NULL) {
        clFieldCellAnime * anim = (clFieldCellAnime *)cellAnimeList.head;
        while (anim != NULL) {
            if (anim->screen == this->unk_virtual_04()) {
                anim->updatePaf();
                anim->draw(this->oamMan);

                if (anim->fieldc0_7) {
                    MTX_Identity44(&anim->mtx);
                } else {
                    anim->mtx._12 = 0;
                    anim->mtx._22 = 0x1000;
                    anim->mtx._30 = 0;
                    anim->mtx._31 = 0;
                    anim->mtx._32 = 0;
                }
            }

            anim = (clFieldCellAnime *)anim->next;
        }
    }
}

// MATCHED 100%
void clFieldBase::flushOam() {
    this->oamMan->sortRuns(0);
    this->oamMan->flush(NULL, 0);
}

// MATCHED 100%
void clFieldBase::startFade(s16 from, s16 to, u16 frames) {
    if (frames == 0) {
        this->unk_virtual_08(to);
        return;
    }

    if (from != 0x7f) {
        this->brightness = from << FX32_SHIFT;
    }

    this->fadeActive = 1;
    this->fadeStart = this->brightness >> FX32_SHIFT;
    this->fadeTarget = to;
    this->fadeFramesLeft = frames;
    this->fadeFrames = this->fadeFramesLeft;
    this->fadeStep = ((to << FX32_SHIFT) - this->brightness) / frames;
}

// MATCHED 100%
void clFieldBase::reverseFade() {
    if (this->fadeActive) {
        this->fadePaused = 0;

        this->fadeStep = -this->fadeStep;
        this->fadeFramesLeft = this->fadeFrames - this->fadeFramesLeft;

        s16 temp = this->fadeTarget;
        this->fadeTarget = this->fadeStart;
        this->fadeStart = temp;
    }
}

// MATCHED 100%
void clFieldBase::updateFade() {
    if (this->fadeActive && !this->fadePaused) {
        if (--this->fadeFramesLeft == 0) {
            this->fadeActive = 0;
            this->brightness = this->fadeTarget << FX32_SHIFT;
        } else {
            this->brightness += this->fadeStep;
        }

        this->unk_virtual_09();
    }
}

// Dead-stripped; required to emit clFieldMap's inline getters at 0x0206aec8/0x0206aed4
void clFieldBase::unk_stripped_func() {
    this->map->getWidthPx();
    this->map->getHeightPx();
}

// MATCHED 100%
u8 * clFieldBase::func_ov003_0206aee0(u8 * param1) {
    param1[0] = 0xff;
    param1[1] = 3;
    param1[2] = 0xff;
    param1[3] = 0x35;
    param1[4] = 0xff;
    param1[5] = 11;
    param1[6] = 1;

    return param1+7;
}

// MATCHED 100%
void initMainDisplay() {
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_32K);
    GX_SetBGCharOffset(GX_BGCHAROFFSET_0x00000);
    GX_SetBankForBG(GX_VRAM_BG_64_E);
    GX_SetBankForOBJ(GX_VRAM_OBJ_16_F);
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_NONE);
    GX_SetBankForOBJExtPltt(GX_VRAM_OBJEXTPLTT_NONE);
    GX_SetBankForTex(GX_VRAM_TEX_01_AB);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0_G);

    G2_SetBlendAlpha(1, 63, 16, 16);

    G3X_AlphaBlend(TRUE);
    GX_SetVisiblePlane(GX_PLANEMASK_BG0 | GX_PLANEMASK_OBJ);
    G3_ViewPort(0, 0, 255, 191);
}

// MATCHED 100%
void initSubDisplay() {
    GXS_SetGraphicsMode(GX_BGMODE_0);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_128K);

    GX_SetBankForSubBG(GX_VRAM_SUB_BG_128_C);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_0123_H);
    GX_SetBankForSubOBJExtPltt(GX_VRAM_SUB_OBJEXTPLTT_0_I);

    GXS_SetVisiblePlane(GX_PLANEMASK_OBJ);
}
