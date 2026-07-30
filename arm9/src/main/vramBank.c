// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/vramBank.h"

// --- Function Definitions --- //

// MATCHED 100%
u32 getTexVramAddr(GXVRamTex bank) {
    switch (bank) {
        case GX_VRAM_TEX_0_A:
        case GX_VRAM_TEX_01_AB:
        case GX_VRAM_TEX_01_AC:
        case GX_VRAM_TEX_012_ABC:
        case GX_VRAM_TEX_01_AD:
        case GX_VRAM_TEX_012_ABD:
        case GX_VRAM_TEX_012_ACD:
        case GX_VRAM_TEX_0123_ABCD:
            return (u32)HW_LCDC_VRAM_A;
        case GX_VRAM_TEX_0_B:
        case GX_VRAM_TEX_01_BC:
        case GX_VRAM_TEX_01_BD:
        case GX_VRAM_TEX_012_BCD:
            return (u32)HW_LCDC_VRAM_B;
        case GX_VRAM_TEX_0_C:
        case GX_VRAM_TEX_01_CD:
            return (u32)HW_LCDC_VRAM_C;
        case GX_VRAM_TEX_0_D:
            return (u32)HW_LCDC_VRAM_D;
        default:
            return 0;
    }
}

// MATCHED 100%
u32 getTexPlttVramAddr(GXVRamTexPltt bank) {
    switch (bank) {
        case GX_VRAM_TEXPLTT_0123_E:
        case GX_VRAM_TEXPLTT_01234_EF:
        case GX_VRAM_TEXPLTT_012345_EFG:
            return (u32)HW_LCDC_VRAM_E;
        case GX_VRAM_TEXPLTT_0_F:
        case GX_VRAM_TEXPLTT_01_FG:
            return (u32)HW_LCDC_VRAM_F;
        case GX_VRAM_TEXPLTT_0_G:
            return (u32)HW_LCDC_VRAM_G;
        case GX_VRAM_TEXPLTT_NONE:
        default:
            return 0;
    }
}

// MATCHED 100%
u32 func_0201f1b4(BOOL param1) {
    if (param1 == FALSE) {
        switch (GX_GetBankForOBJExtPltt()) {
            case GX_VRAM_OBJEXTPLTT_0_F:
                return HW_LCDC_VRAM_F;
            case GX_VRAM_OBJEXTPLTT_0_G:
                return HW_LCDC_VRAM_G;
            default:
                return HW_OBJ_PLTT;
        }
    } else {
        return (GX_GetBankForSubOBJExtPltt() == GX_VRAM_SUB_OBJEXTPLTT_0_I) ? HW_LCDC_VRAM_I : HW_DB_OBJ_PLTT;
    }
}
