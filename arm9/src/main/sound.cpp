// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/sound.h"

#include <nnsys.h>

#include "main/heap.h"

extern "C" {

#include "structs/sndAltVoiceInfo.h"

#include "main/unk_0202f1b4.h"
#include "main/unk_02031be4.h"

// --- Struct Definitions --- //

enum SndStreamState { SND_STREAM_IDLE, SND_STREAM_PLAYING };

// --- Extern Data Variables --- //

extern s8 sndDriverInfoIdx;  // 0x01ff9438
extern u8 sndForceCenterX;  // 0x01ff943c
extern u8 sndAltIdx;  // 0x01ff9444
extern u8 sndForceCenterY;  // 0x01ff9448
extern u32 sndHeapSlot0Size;  // 0x01ff944c
extern u32 sndHeapSlot1Size;  // 0x01ff9450
extern s16 sndAltVoiceId[2];  // 0x01ff9454
extern s16 sndSeqNo[2];  // 0x1ff9460
extern u32 sndHeapSysSize;  // 0x01ff9458
extern SndRequestHead sndRequestHead;  // 0x01ff9464
extern SndHeap sndHeapSys;  // 0x01ff9468
extern u32 sndCommandTag;  // 0x01ff946c
extern SndVoiceList sndVoiceList;  // 0x01ff9470
extern SndHeap sndHeapSlot0;  // 0x01ff9474
extern SndLoadNodeHead sndLoadNodeHead;  // 0x01ff9478
extern char * sndHeapBuffer;  // 0x01ff947c
extern SndHeap sndHeapSlot1;  // 0x01ff9480
extern u32 sndDriverInfoDirty;  // 0x01ff9484
extern u32 sndLoadBudget;  // 0x01ff9488
extern SndLoadNodeTail sndLoadNodeTail;  // 0x01ff948c
extern SndRequestTail sndRequestTail;  // 0x01ff9494
extern SndCurrent sndCurrent;  // 0x01ff949c
extern NNSSndHandle sndSeqHandle[2];  // 0x01ff94a4
extern u32 sndTempoRatio[2];  // 0x01ff94ac
extern SndFade sndFade[2];  // 0x01ff94b4
extern SndLoadedData sndLoadedData[2];  // 0x01ff94d4
extern SndLoader sndLoader;  // 0x01ff9504
extern SndLoadSlot sndLoadSlots[SND_LOAD_COUNT];  // 0x01ff9538
extern SndBankSlot sndBankSlots[SND_BANK_COUNT];  // 0x01ff9598

extern NNSSndArc sndArc;  // 0x01ff9618

extern SndRequest sndRequests[SND_REQ_COUNT];  // 0x01ff96ac

extern SndVoice sndVoices[SND_VOICE_COUNT];  // 0x01ff986c

extern NNSSndStrmThread sndStrmThread;  // 0x01ff9c7c

extern SNDDriverInfo sndDriverInfo[];  // 0x02056560

// --- Extern Functions --- //

extern void func_0200a600(void);

extern void func_0202f388(u8);

// --- Variable Definitions --- //

const SndAltVoiceInfo sndAltVoiceInfo[2] = { {0, 15}, {1, 14} };  // 0x02050040
const s16 sndStackVolumeDrop[32] = { 27, 27, 40, 60, 80, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90,
                                     90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90 };  // 0x02050048
char sndHeapName[12] = "SoundHeap";  // 0x02050088
const char sndArcPath[24] = "sound/sound_data.sdat";  // 0x02050094

static SNDWaveArc sndWaveArcHeader;  // 0x02056520

// --- Function Declarations --- //

void sndWaitLoading(void);  // 0x02006368
u8 sndIsLoading(void);  // 0x02006388
void sndPlaySeq(u32, u8, u32);  // 0x02006398
void sndRestartSeq(u32, BOOL);  // 0x0200650c
void sndStop(u32);  // 0x02006624
void sndStopSlot(u32, u8);  // 0x02006688
void sndPauseSlot(BOOL, u32);  // 0x020066e0
void sndStartFade(u32, s32);  // 0x02006700
u32 sndGetStatus(void);  // 0x02006810
u32 sndGetSeqNos(void);  // 0x020068ac
void sndClearSlot(u32);  // 0x020068c4
void sndSetTempoRatio(u32, s32);  // 0x02006914
void sndPlayById(u32, s16, s16, s16, s8, s16);  // 0x02006964
void sndStopAllVoices(void);  // 0x02006c34
u32 sndGetActiveSeqMask(void);  // 0x02006c90
void sndHeapLoadState(void);  // 0x02006d68
void sndLoadSeqArc(u32, u32);  // 0x02006d84
void sndPlayAlt(s16, s16, s16, s16, s8, s16);  // 0x02006e84
void sndPlayAltOwned(SndVoiceList *, s16, s16, s16, s16, s8, s16);  // 0x02006ed0
void sndLoadPrograms(u16 *, BOOL, u32);  // 0x02006f4c
void sndStopAlt(s16);  // 0x0200709c
void sndPlayStreamById(u16, s16, s16, s16, s8);  // 0x02007108
void sndPlayStreamOwned(SndVoiceList *, u16, s16, s16, s16, s8);  // 0x0200711c
void sndStopStreamById(s8, u32);  // 0x02007158
void sndStopVoiceStream(SndVoiceList *, u32);  // 0x02007164
u8 sndGetStreamState(u32);  // 0x02007190
s16 sndReadGlobalVar(u32);  // 0x020071c0
u32 sndCountActiveTracks();  // 0x020071d4
SndVoice * sndPlayStream(u16, s16, s16, s16, s8);  // 0x02007244
SndVoice * sndFindStream(u32);  // 0x02007344
void sndStopStream(s8, u32);  // 0x02007398
void sndApplyPosition(SndVoice *, s16, s16, u32);  // 0x020073d8
void sndCalcPanVolume(s16, s32, s16 *, s16 *, u8, u8, bool);  // 0x020077b8
void sndInitVoices(void);  // 0x02007900
SndVoice * sndAllocVoice(SndVoice *);  // 0x020079bc
void sndLinkVoice(SndVoice *);  // 0x02007ab4
SndVoice * sndFreeVoice(SndVoice *);  // 0x02007b00
void sndUpdateVoices();  // 0x02007bf0
void sndScheduleVoice(SndVoice *, u32, s16, u32, s16, s16, u8, u8, u16, s16);  // 0x02007e0c
BOOL sndStopVoices(SndVoiceList *, s32, void(*)(SndVoice *, u32), u32);  // 0x02007e8c
void sndStopVoice(SndVoice *, u32);  // 0x02007f40
void sndReleaseVoice(SndVoice *, u32);  // 0x02007f60
void sndUpdateEchoDelays(SndVoice *);  // 0x02007f98  
BOOL sndStartVoice(SndVoice *, u32, s8, s16, s16, u8, u8, s32, s16);  // 0x02008008
s32 sndCountVoices(u32);  // 0x0200894c
void sndApplyVoiceStacking(SndVoice *, u32, s16 *, s8 *);  // 0x020089d0
BOOL sndReserveChannels(s32, s8, s8);  // 0x02008ae4
void sndLoadBank(SndLoadNode *);  // 0x02008c6c
void sndLoadWaveArc(SndLoadNode *);  // 0x02008d5c
void sndFinishWaveArcLoad(SndLoadNode *);  // 0x02008e48
void * sndAllocBankSlot(const NNSSndArcBankInfo *, void *, u32);  // 0x02008f2c
void sndStartAlt(SndVoice *, s16, s16, s16, s16, s8, s16);  // 0x02008f68
void sndUpdateFades(void);  // 0x0200907c
void sndLoadAndStartSeq(u8, s16, BOOL, u32);  // 0x020091e4
void sndStartSeqOnSlot(u32, u8, BOOL);  // 0x020092e0
BOOL sndLoadSeqData(NNSSndHeapHandle, u32, SndLoadedData *);  // 0x02009490
void sndLoadSeqWaves(u32, u8);  // 0x02009668
void * sndOpenWaveArc(u32, NNSSndHeapHandle, BOOL, BOOL, BOOL, void (*)(SndLoadNode *), u32);  // 0x02009714
void sndFixWaveOffsets(SndLoadNode *);  // 0x02009880
void * sndAllocLoadSlot(u32, u32);  // 0x020098c8
void sndLoadInstrumentWaves(SNDWaveArc *, SNDBankData *, u32, u16, NNSSndHeapHandle);  // 0x0200990c
BOOL sndLoadWave(SNDWaveArc *, s32, u32, NNSSndHeapHandle, BOOL);  // 0x020099b4
void * sndHeapAlloc(NNSSndHeapHandle, u32, NNSSndHeapDisposeCallback, void *, u32);  // 0x02009a9c
void sndInitNodeList(void);  // 0x02009ab8
void sndQueueNode(void *, u32, u32, u32, void (*)(SndLoadNode *), void *);  // 0x02009ad8
void sndBeginLoad(u8, u32, u8, u32, u8);  // 0x02009b10
void sndInitRequests();  // 0x02009b90
void sndQueueRequest(SndRequest *);  // 0x02009bcc
void sndRunNextRequest(void);  // 0x02009c24
void sndUpdateLoader(void);  // 0x02009cc4
void sndHeapSaveState(SndLoadNode *);  // 0x02009e2c
BOOL sndProcessLoadQueue(void);  // 0x02009e7c
void sndDisposeSeq(void *, u32, u32, u32);  // 0x02009fcc
void sndDisposeWaveArc(void *, u32, u32, u32);  // 0x02009ff4
void sndDisposeBank(void *, u32, u32, u32);  // 0x0200a024
void sndDisposeWaveArcOnly(void *, u32, u32, u32);  // 0x0200a054
void sndDisposeWaveData(void *, u32, u32, u32);  // 0x0200a074
void sndClearFileAddress(void *, u32, u32);  // 0x0200a0c8
BOOL sndReadDriverInfo();  // 0x0200a11c
u32 sndCountPlayerTracks(NNSSndHandle * );  // 0x0200a20c
BOOL sndGetDefaultPrio(u32, s8 *, s8 *);  // 0x0200a2b0
BOOL sndGetStreamPrio(u32, s8 *);  // 0x0200a334
u16 sndGetTrackCount(u32);  // 0x0200a358

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void sndInit(BOOL loadSymbols, u32 totalSize, u32 slot0Size, u32 slot1Size) {
    NNS_SndInit();
    func_0200a600();

    sndDriverInfoIdx = -1;
    sndDriverInfoDirty = 1;

    if (sndHeapBuffer == NULL) {
        sndHeapBuffer = new(1, &sndHeapName, FALSE) char[totalSize];
    }

    sndHeapSlot0Size = slot0Size;

    u32 offset = 0;

    if (slot0Size != 0) {
        sndHeapSlot0.heap = NNS_SndHeapCreate(sndHeapBuffer, slot0Size);
        offset += sndHeapSlot0Size;
    }

    sndHeapSlot1Size = slot1Size;

    if (slot1Size != 0) {
        sndHeapSlot1.heap = NNS_SndHeapCreate((u8*)sndHeapBuffer + offset, slot1Size);
        offset += sndHeapSlot1Size;
    }

    sndHeapSysSize = totalSize - offset;

    if (totalSize - offset != 0) {
        sndHeapSys.heap = NNS_SndHeapCreate((u8*)sndHeapBuffer + offset, totalSize - offset);
    }

    NNS_SndArcInit(&sndArc, sndArcPath, sndHeapSys.heap, loadSymbols);

    NNS_SndArcPlayerSetup(sndHeapSys.heap);

    NNS_SndArcStrmInit(10, sndHeapSys.heap);

    NNS_SndArcStrmCreatePrepareThread(&sndStrmThread, 17);

    for (s16 i = 1; i >= 0; i--) {
        func_0202f1b4(&sndSeqHandle[i]);
    }

    sndInitVoices();

    MI_CpuFill8(&sndLoader, 0, sizeof(sndLoader));
    sndInitNodeList();

    MI_CpuFill8(&sndBankSlots, 0, sizeof(sndBankSlots));
    MI_CpuFill8(&sndLoadSlots, 0, sizeof(sndLoadSlots));

    sndInitRequests();

    sndSeqNo[0] = -1;
    sndSeqNo[1] = -1;

    sndAltIdx = 0;

    sndAltVoiceId[0] = 0;
    sndAltVoiceId[1] = 0;

    for (s16 i = 1; i >= 0; i--) {
        sndFade[i].maxVolume = 0x7f000;
        sndFade[i].volume = 0x7f000;
        sndFade[i].state = NNS_SND_SEQ_MUTE_OFF;
        sndFade[i].step = 0;
    }

    sndCurrent.field00 = 0;
    sndCurrent.field04 = 0;

    sndTempoRatio[0] = SND_TEMPO_RATIO_UNITY;
    sndTempoRatio[1] = SND_TEMPO_RATIO_UNITY;

    NNS_SndHeapSaveState(sndHeapSys.heap);

    sndEchoMode = 0;

    NNS_SndSetMasterVolume(97);
}

// MATCHED 100%
void sndShutdown(BOOL keepBuffer) {
    sndStopAllVoices();

    sndStopStream(0, 0);
    sndStopStream(1, 0);

    sndClearSlot(0);
    sndClearSlot(1);

    if (sndHeapSys.heap != NULL) {
        NNS_SndHeapClear(sndHeapSys.heap);
        NNS_SndHeapDestroy(sndHeapSys.heap);
        sndHeapSys.heap = NULL;
    }

    if (sndHeapSlot0.heap != NULL) {
        NNS_SndHeapClear(sndHeapSlot0.heap);
        NNS_SndHeapDestroy(sndHeapSlot0.heap);
        sndHeapSlot0.heap = NULL;
    }

    if (sndHeapSlot1.heap != NULL) {
        NNS_SndHeapClear(sndHeapSlot1.heap);
        NNS_SndHeapDestroy(sndHeapSlot1.heap);
        sndHeapSlot1.heap = NULL;
    }

    if (sndHeapBuffer != NULL && !keepBuffer) {
        delete[] sndHeapBuffer;
        sndHeapBuffer = NULL;
    }
}

// MATCHED 100%
void sndWaitLoading() {
    if (sndLoader.busy) {
        sndUpdateLoader();
    }
}

// MATCHED 100%
u8 sndIsLoading() {
    return sndLoader.busy;
}

// MATCHED 100%
void sndPlaySeq(u32 seqNo, u8 slot, u32 budget) {
    if (seqNo == 0) {
        sndStopSlot(0, slot);
    } else if (sndLoader.busy != 0) {
        SndRequest req;
        req.argForm = 0;
        req.field0c = (void (*)())&sndPlaySeq;
        req.form0.arg1 = seqNo;
        req.form0.arg2 = slot;
        req.form0.arg3 = budget;
        sndQueueRequest(&req);
    } else {
        if (sndSeqNo[slot] != -1 && seqNo != sndSeqNo[slot]) {
            sndStopSlot(0, slot);
        }

        sndStopSlot(0, (slot == 0 ? 1 : 0));

        sndLoadAndStartSeq(slot, seqNo, TRUE, budget);
    }
}

// MATCHED 100%
void sndPlaySeqNoStop(u32 seqNo, u8 slot, u32 budget) {
    if (seqNo == 0)
        return;

    if (sndLoader.busy != 0) {
        SndRequest req;
        req.argForm = 0;
        req.field0c = (void (*)())&sndPlaySeqNoStop;
        req.form0.arg1 = seqNo;
        req.form0.arg2 = slot;
        req.form0.arg3 = budget;
        sndQueueRequest(&req);
    } else {
        if (sndSeqNo[slot] != -1 && seqNo != sndSeqNo[slot]) {
            sndStopSlot(0, slot);
        }
        sndLoadAndStartSeq(slot, seqNo, FALSE, budget);
    }
}

// MATCHED 100%
void sndRestartSeq(u32 seqNo, BOOL stopOther) {
    if (seqNo == sndSeqNo[0]) {
        if (stopOther) {
            sndStopSlot(0, 1);
        }
        sndStartSeqOnSlot(seqNo, 0, FALSE);
    } else if (seqNo == sndSeqNo[1]) {
        if (stopOther) {
            sndStopSlot(0, 0);
        }
        sndStartSeqOnSlot(seqNo, 1, FALSE);
    } else {
        sndPlaySeq(seqNo, 0, 0xc00);
    }
}

// MATCHED 100%
void sndRestartSlot(u32 slot, BOOL stopOther, BOOL startMuted) {
    u32 val = sndGetSeqNos();

    if (slot == 0) {
        if (stopOther) {
            sndStopSlot(0, 1);
        }

        if ((u16)val != 0xffff) {
            sndStartSeqOnSlot((u16)val, 0, startMuted);
        }
    } else {
        if (stopOther) {
            sndStopSlot(0, 0);
        }

        if (val >> 16 != 0xffff) {
            sndStartSeqOnSlot(val >> 16, 1, startMuted);
        }
    }
}

// MATCHED 100%
void sndStop(u32 fadeFrames) {

    NNS_SndPlayerStopSeq(&sndSeqHandle[sndCurrent.field04], fadeFrames == 0 ? 8 : fadeFrames);

    u8 slot = sndCurrent.field04;
    SndFade * fade = &sndFade[slot];
    fade->volume = fade->maxVolume;
    fade->state = NNS_SND_SEQ_MUTE_OFF;

    NNS_SndPlayerSetTrackMuteEx(&sndSeqHandle[slot], SND_TRACK_ALL, NNS_SND_SEQ_MUTE_OFF);
}

// MATCHED 100%
void sndStopSlot(u32 fadeFrames, u8 slot) {
    if (fadeFrames == 0) {
        fadeFrames = 8;
    }

    NNS_SndPlayerStopSeq(&sndSeqHandle[slot], fadeFrames);

    SndFade * fade = &sndFade[slot];
    fade->volume = fade->maxVolume;
    fade->state = NNS_SND_SEQ_MUTE_OFF;

    NNS_SndPlayerSetTrackMuteEx(&sndSeqHandle[slot], SND_TRACK_ALL, NNS_SND_SEQ_MUTE_OFF);
}

// MATCHED 100%
void sndPauseSlot(BOOL pause, u32 slot) {
    NNS_SndPlayerPause(&sndSeqHandle[slot], pause);
}

// MATCHED 100%
void sndStartFade(u32 fadeOut, s32 frames) {
    SndFade * fade = &sndFade[sndCurrent.field04];
    s32 resolvedFrames;
    if (fadeOut != 0) {
        fade->state = NNS_SND_SEQ_MUTE_RELEASE;
        fade->volume = fade->maxVolume;
        if (frames == -1) {
            resolvedFrames = 20;
        }
    } else {
        fade->state = NNS_SND_SEQ_MUTE_NO_STOP;
        fade->volume = 0;
        if (frames == -1) {
            resolvedFrames = 60;
        }
    }

    if (resolvedFrames == 0) {
        resolvedFrames = 1;
    }

    fade->step = FX_Div(fade->maxVolume, resolvedFrames << FX32_SHIFT);
}

// MATCHED 100%
void sndStartFadeSlot(u32 fadeOut, s32 frames, u32 slot) {
    if (fadeOut != 0) {
        sndFade[slot].state = NNS_SND_SEQ_MUTE_RELEASE;
        sndFade[slot].volume = sndFade[slot].maxVolume;
        if (frames == -1) {
            frames = 20;
        }
    } else {
        sndFade[slot].state = NNS_SND_SEQ_MUTE_NO_STOP;
        sndFade[slot].volume = 0;
        if (frames == -1) {
            frames = 60;
        }
    }

    if (frames == 0) {
        frames = 1;
    }

    sndFade[slot].step = FX_Div(sndFade[slot].maxVolume, frames << 12);
}

// MATCHED 100%
u32 sndGetStatus() {
    u8 slot = sndCurrent.field04;
    if (!NNS_SndHandleIsValid(&sndSeqHandle[slot])) {
        return 0;
    }

    return (sndFade[slot].state != NNS_SND_SEQ_MUTE_OFF) ? NNS_SND_SEQ_MUTE_STOP : sndSeqHandle[slot].player->status;
}

// MATCHED 100%
u32 sndGetSlotStatus(u32 slot) {
    if (!NNS_SndHandleIsValid(&sndSeqHandle[slot])) {
        return 0;
    }

    return (sndFade[slot].state != NNS_SND_SEQ_MUTE_OFF) ? NNS_SND_SEQ_MUTE_STOP : sndSeqHandle[slot].player->status;
}

// MATCHED 100%
u32 sndGetSeqNos() {
    return (u16)sndSeqNo[0] | (sndSeqNo[1] << 16);
}

// MATCHED 100%
void sndClearSlot(u32 slot) {
    NNS_SndPlayerStopSeq(&sndSeqHandle[slot], 0);

    NNSSndHeapHandle heap = (slot == 0) ? sndHeapSlot0.heap : sndHeapSlot1.heap;
    NNS_SndHeapClear(heap);

    sndSeqNo[slot] = -1;
}

// MATCHED 100%
void sndSetTempoRatio(u32 slot, s32 ratio) {
    if (!NNS_SndHandleIsValid(&sndSeqHandle[slot])) {
        sndTempoRatio[slot] = ratio;
    } else {
        NNS_SndPlayerSetTempoRatio(&sndSeqHandle[slot], ratio);
        sndTempoRatio[slot] = SND_TEMPO_RATIO_UNITY;
    }
}

// MATCHED 100%
void sndPlayById(u32 id, s16 delay, s16 x, s16 y, s8 volume, s16 duration) {
    switch (id & SND_ID_TYPE_MASK) {
        case 0:
            if (delay != 0) {
                sndScheduleVoice(sndAllocVoice(NULL), id, delay, volume, x, y, 1, sndEchoMode, SND_PRIO_DEFAULT, duration);
            } else {
                sndStartVoice(sndAllocVoice(NULL), id, volume, x, y, 1, sndEchoMode, SND_PRIO_DEFAULT, duration);
            }
            break;
        case SND_ID_ALT:
            sndPlayAlt(id, delay, x, y, volume, 0);
            break;
        case SND_ID_STREAM:
            sndPlayStreamById(id, delay, x, y, -1);
    }
}

// MATCHED 100%
BOOL sndPlayByIdOwned(SndVoiceList * owner, u32 id, s16 delay, s16 x, s16 y, s8 volume, s16 duration) {
    switch (id & SND_ID_TYPE_MASK) {
        case 0:
            sndStopVoices(owner, -1, sndReleaseVoice, 0);

            SndVoice * voice = sndAllocVoice(NULL);
            if (voice == NULL) {
                return FALSE;
            }

            if (delay == 0) {
                if (sndStartVoice(voice, id, volume, x, y, 0, sndEchoMode, SND_PRIO_DEFAULT, duration) == 0) {
                    return FALSE;
                }
            } else {
                sndScheduleVoice(voice, id, delay, volume, x, y, 0, sndEchoMode, SND_PRIO_DEFAULT, duration);
            }

            voice->owner = owner;
            owner->head = voice;
            break;
        case SND_ID_ALT:
            sndPlayAltOwned(owner, id, delay, x, y, volume, 0);
            break;
        case SND_ID_STREAM:
            sndPlayStreamOwned(owner, id, delay, x, y, -1);
    }

    return TRUE;
}

// MATCHED 100%
void sndStopOwned(SndVoiceList * owner, u32 fadeFrames) {
    if (owner->head == NULL) {
        return;
    }

    switch (owner->head->id & SND_ID_TYPE_MASK) {
        case SND_ID_STREAM:
            if (owner == NULL) {
                sndStopStreamById(owner->head->id & SND_ID_MATCH_MASK, fadeFrames);
            } else {
                sndStopVoiceStream(owner, fadeFrames);
            }
            break;
        case 0:
        case SND_ID_ALT:
            sndStopVoices(owner, owner->head->id, sndStopVoice, fadeFrames);
    }
}

// MATCHED 100%
void sndStopAllVoices() {
    SndVoice * ptr = sndVoiceList.head;

    while (ptr != 0) {
        if ((ptr->id & SND_ID_TYPE_MASK) != SND_ID_STREAM) {
            sndStopVoice(ptr, 0);
            NNS_SndHandleReleaseSeq(&ptr->handle);
            func_0202f1b4(&ptr->handle);
        }
        ptr = ptr->next;
    }

    sndInitVoices();
}

// MATCHED 100%
u32 sndGetActiveSeqMask() {
    u32 mask = 0;

    for (s16 i = 19; i >= 0; i--) {
        if ((sndVoices[i].id & SND_ID_TYPE_MASK) != SND_ID_STREAM) {
            if (NNS_SndHandleIsValid(&sndVoices[i].handle) && sndVoices[i].handle.player->seqNo != 0) {
                mask |= 1 << i;
            }
        }
    }

    return mask;
}

// MATCHED 100%
void sndSetListPosition(SndVoiceList * list, s16 x, s16 y) {
    if (list->head != NULL) {
        sndApplyPosition(list->head, x, y, 0);
    }
}

// MATCHED 100%
void sndLoadGroup(u32 groupNo, u32 budget) {
    const NNSSndArcGroupInfo * groupInfo = NNS_SndArcGetGroupInfo(groupNo);

    for (u32 i = 0; i < groupInfo->count; i++) {
        const NNSSndArcGroupItem * item = &groupInfo->item[i];

        if (item->type == 3) {
            sndLoadSeqArc(item->idx, budget);
        }
    }
}

// MATCHED 100%
void sndHeapLoadState() {
    NNS_SndHeapLoadState(sndHeapSys.heap, 1);
}

// MATCHED 100%
void sndLoadSeqArc(u32 seqArcNo, u32 budget) {
    if (sndLoader.busy != 0) {
        SndRequest req;
        req.argForm = 2;
        req.field0c = (void (*)())&sndLoadSeqArc;
        req.form2.arg1 = seqArcNo;
        req.form2.arg2 = budget;

        sndQueueRequest(&req);
        return;
    }

    const NNSSndArcSeqArcInfo * arcInfo = NNS_SndArcGetSeqArcInfo(seqArcNo);
    if (NNS_SndArcGetFileAddress(arcInfo->fileId) != NULL) {
        return;
    }

    u32 size = NNS_SndArcGetFileSize(arcInfo->fileId);
    void * mem = sndHeapAlloc(sndHeapSys.heap, size, &sndDisposeSeq, &sndArc, arcInfo->fileId);

    sndQueueNode(mem, size, arcInfo->fileId, 0, &sndLoadBank, mem);

    NNS_SndArcSetFileAddress(arcInfo->fileId, mem);

    sndLoadBudget = budget;

    sndBeginLoad(0, seqArcNo, 0xff, 0, 1);
}

// MATCHED 100%
void sndPlayAlt(s16 id, s16 delay, s16 x, s16 y, s8 volume, s16 duration) {
    sndStartAlt(sndAllocVoice(NULL), id, delay, x, y, volume, duration);
}

// MATCHED 100%
void sndPlayAltOwned(SndVoiceList * owner, s16 id, s16 delay, s16 x, s16 y, s8 volume, s16 duration) {
    if (owner->head != NULL) {
        sndStopVoices(owner, -1, sndReleaseVoice, 0);
    }

    SndVoice * voice = sndAllocVoice(NULL);
    sndStartAlt(voice, delay, id, x, y, volume, duration);

    voice->owner = owner;
    owner->head = voice;
}

// MATCHED 100%
void sndLoadPrograms(u16 * programs, BOOL deferred, u32 budget) {
    u16 * prgNos = programs;
    u16 successes;
    const NNSSndArcWaveArcInfo * waveArc;
    const NNSSndArcBankInfo * bankInfo;
    SNDWaveArc * arcAddr;
    void * bankData;

    if (sndLoader.busy) {
        SndRequest req;
        req.argForm = 1;
        req.field0c = (void (*)())&sndLoadPrograms;
        req.form1.programs = programs;
        req.form1.arg2 = deferred;
        req.form1.arg3 = budget;
        sndQueueRequest(&req);
    } else {
        successes = 0;
        bankInfo = NNS_SndArcGetBankInfo(0);
        waveArc = NNS_SndArcGetWaveArcInfo(bankInfo->waveArcNo[0]);
        arcAddr = (SNDWaveArc *)NNS_SndArcGetFileAddress(waveArc->fileId);
        bankData = NNS_SndArcGetFileAddress(bankInfo->fileId);

        for (s16 i = *prgNos++; i > 0; i--) {
            if (*prgNos != 0) {
                SNDInstData inst;
                SND_ReadInstData((const SNDBankData *)bankData, *prgNos, 0, &inst);

                successes += sndLoadWave(arcAddr, inst.param.wave[0], waveArc->fileId, sndHeapSys.heap, deferred);
            }

            prgNos++;
        }

        if (successes != 0 && deferred) {
            sndLoadBudget = budget;
            sndBeginLoad(0, 0, -1, 0, 1);
        }
    }
}

// MATCHED 100%
void sndStopAlt(s16 id) {
    if (sndAltVoiceId[0] == id || id < 0) {
        sndStopVoices(NULL, 0, sndStopVoice, 0);
    }

    if (sndAltVoiceId[1] == id || id < 0) {
        sndStopVoices(NULL, 1, sndStopVoice, 0);
    }
}

// MATCHED 100%
void sndPlayStreamById(u16 strmNo, s16 delay, s16 x, s16 y, s8 strmIdx) {
    sndPlayStream(strmNo, delay, x, y, strmIdx);
}

// MATCHED 100%
void sndPlayStreamOwned(SndVoiceList * owner, u16 strmNo, s16 delay, s16 x, s16 y, s8 strmIdx) {
    SndVoice * voice = sndPlayStream(strmNo, delay, x, y, strmIdx);
    if (voice != NULL) {
        voice->owner = owner;
        owner->head = voice;
    }
}

// MATCHED 100%
void sndStopStreamById(s8 streamIdx, u32 fadeFrames) {
    sndStopStream(streamIdx, fadeFrames);
}

// MATCHED 100%
void sndStopVoiceStream(SndVoiceList * owner, u32 fadeFrames) {
    if (owner->head != NULL) {
        sndStopStream(owner->head->streamIdx, fadeFrames);
    }
}

// MATCHED 100%
u8 sndGetStreamState(u32 streamIdx) {
    SndVoice * voice = sndFindStream(streamIdx);

    if (voice != NULL) {
        return voice->handle.player == NULL ? SND_STREAM_IDLE : SND_STREAM_PLAYING;
    }

    return 0;
}

// MATCHED 100%
s16 sndReadGlobalVar(u32 varNo) {
    s16 var;
    NNS_SndPlayerReadGlobalVariable(varNo, &var);
    return var;
}

// MATCHED 100%
u32 sndCountActiveTracks() {
    u32 count = 0;
    count += sndCountPlayerTracks(&sndSeqHandle[0]);
    count += sndCountPlayerTracks(&sndSeqHandle[1]);

    SndVoice * cur = sndVoiceList.head;
    while (cur != NULL) {
        if (NNS_SndHandleIsValid(&cur->handle)) {
            count += cur->trackCount;
        }
        cur = cur->next;
    }
    return count;
}

// MATCHED 100%
SndVoice * sndPlayStream(u16 strmNo, s16 delay, s16 x, s16 y, s8 strmIdx) {
    if (strmIdx < 0) {
        if (sndFindStream(0) == NULL) {
            strmIdx = 0;
        } else if (sndFindStream(1) == NULL) {
            strmIdx = 1;
        } else {
            return NULL;
        }
    } else {
        sndStopStream(strmIdx, 2);
    }

    SndVoice * ptr = sndAllocVoice(NULL);
    ptr->streamIdx = strmIdx;

    if (delay == 0) {
        sndStartVoice(ptr, strmNo | SND_ID_STREAM, 127, x, y, 0, 0, SND_PRIO_DEFAULT, 0);
    } else {
        sndScheduleVoice(ptr, strmNo | SND_ID_STREAM, delay, 127, x, y, 0, 0, SND_PRIO_DEFAULT, 0);
    }

    return ptr;
}

// MATCHED 100%
SndVoice * sndFindStream(u32 streamIdx) {
    SndVoice * cur = sndVoiceList.head;
    while (cur != NULL) {
        if ((cur->id & SND_ID_TYPE_MASK) == SND_ID_STREAM) {
            if (streamIdx == -1 || streamIdx == cur->streamIdx) {
                return cur;
            }
        }

        cur = cur->next;
    }

    return NULL;
}

// MATCHED 100%
void sndStopStream(s8 streamIdx, u32 fadeFrames) {
    SndVoice * voice = sndFindStream(streamIdx);
    if (voice != NULL) {
        NNS_SndArcStrmStop(&voice->strmHandle, fadeFrames);
        NNS_SndStrmHandleRelease(&voice->strmHandle);
        func_02031be4(&voice->strmHandle);

        voice->stopped = 1;
    }
}

// MATCHED 100%
void sndApplyPosition(SndVoice * voice, s16 x, s16 y, u32 frames) {
    if (NNS_SndHandleIsValid(&voice->handle) == FALSE) {
        return;
    }

    s16 volumeAdj = 0;
    switch (voice->id & SND_ID_TYPE_MASK) {
        case SND_ID_STREAM: {
            voice->x = x;
            voice->y = y;

            s32 channels = NNS_SndArcStrmGetChannelCount(&voice->strmHandle);
            if (channels == 2) {
                if (!(voice->id & SND_ID_PAN_ONLY)) {
                    sndCalcPanVolume(x, y, NULL, &volumeAdj, voice->forceCenterY, voice->forceCenterX, 0);
                }

                s16 panLeft;
                sndCalcPanVolume(x-127, y, &panLeft, NULL, voice->forceCenterY, voice->forceCenterX, 1);

                s16 panRight;
                sndCalcPanVolume(x+128, y, &panRight, NULL, voice->forceCenterY, voice->forceCenterX, 1);

                NNS_SndArcStrmSetChannelPan(&voice->strmHandle, 0, panLeft);
                NNS_SndArcStrmSetChannelPan(&voice->strmHandle, 1, panRight);
            } else {
                s16 pan;
                if (voice->id & SND_ID_PAN_ONLY) {
                    sndCalcPanVolume(x, y, &pan, NULL, voice->forceCenterY, voice->forceCenterX, 1);
                } else {
                    sndCalcPanVolume(x, y, &pan, &volumeAdj, voice->forceCenterY, voice->forceCenterX, 1);
                }

                NNS_SndArcStrmSetChannelPan(&voice->strmHandle, 0, pan);
            }

            volumeAdj += 127;
            if (volumeAdj > 127) {
                volumeAdj = 127;
            } else if (volumeAdj < 0) {
                volumeAdj = 0;
            }

            NNS_SndArcStrmSetChannelVolume(&voice->strmHandle, 0, volumeAdj);
            NNS_SndArcStrmSetChannelVolume(&voice->strmHandle, 1, volumeAdj);
            break;
        }
        case 0:
        case SND_ID_ALT: {
            s16 pan;
            if (voice->id & SND_ID_PAN_ONLY) {
                sndCalcPanVolume(x, y, &pan, NULL, voice->forceCenterY, voice->forceCenterX, 0);
            } else {
                sndCalcPanVolume(x, y, &pan, &volumeAdj, voice->forceCenterY, voice->forceCenterX, 0);
            }

            while (voice != NULL) {
                voice->x = x;
                voice->y = y;

                s16 volume = voice->volume + volumeAdj;
                if (volume < 0) {
                    volume = 0;
                }

                if (voice->elapsed >= voice->duration) {
                    if (NNS_SndHandleIsValid(&voice->handle)) {
                        NNS_SndPlayerMoveVolume(&voice->handle, SND_CALC_DECIBEL_SCALE_MAX, 1);
                        NNS_SndPlayerSetVolume(&voice->handle, volume);
                    }
                } else if (voice->field2c_15 == 0) {
                    voice->field2c_15 = 1;

                    NNS_SndPlayerMoveVolume(&voice->handle, 0, 0);
                    NNS_SndPlayerMoveVolume(&voice->handle, volume, frames);
                }

                NNS_SndPlayerSetTrackPan(&voice->handle, SND_TRACK_ALL, pan);

                voice = voice->chainNext;
            }
        }
    }
}

// MATCHED 100%
void sndCalcPanVolume(s16 x, s32 y, s16 * outPan, s16 * outVolume, u8 forceCenterY, u8 forceCenterX, bool unsignedPan) {
    if (forceCenterY != 0 || sndForceCenterY != 0) {
        y = HW_LCD_HEIGHT/2;
    }
    if (forceCenterX != 0 || sndForceCenterX != 0) {
        x = HW_LCD_WIDTH/2-1;
    }

    fx32 dx = ((HW_LCD_WIDTH/2-1 - x) << FX32_SHIFT) >> 2;
    fx32 dy = ((HW_LCD_HEIGHT/2 - y) * FX32_CONST(1.15f)) >> 2;
    fx32 dist = FX_Sqrt(FX_Mul(dx,dx) + FX_Mul(dy,dy)) << 2;

    if (dist > FX32_CONST(90.0f) && outVolume != NULL) {
        *outVolume = FX_Whole(FX_Mul(FX32_CONST(90.0f) - dist, FX32_CONST(0.4f)));
    }

    if (outPan != NULL) {
        s16 offsetX = x - (HW_LCD_WIDTH/2-1);
        if (unsignedPan) {
            x = FX_Whole(offsetX * 0x986);
        } else {
            x = FX_Whole(offsetX * FX32_CONST(0.35f));
        }
        
        if (x > 127) {
            x = 127;
        } else if (x < -128) {
            x = -128;
        }

        if (unsignedPan) {
            x = (x + 128) >> 1;
        }

        *outPan = x;
    }
}

// MATCHED 100%
void sndInitVoices() {
    s32 i;
    SndVoice * voice = sndVoices;

    sndVoiceList.head = NULL;

    for (i = 0; i < SND_VOICE_COUNT; i++) {
        voice->next = NULL;
        voice->prev = NULL;
        voice->chainNext = NULL;
        voice->chainPrev = NULL;

        if (voice->owner != NULL) {
            voice->owner->head = NULL;
            voice->owner = NULL;
        }
        voice->id = 0;
        voice->volume = -1;
        voice->delay = 0;
        voice->echoDelay = 0;
        voice->x = 0x7f;
        voice->dualPrio = -1;
        voice->field2c = 0;
        voice->elapsed = 0;
        voice->trackCount = 0;
        voice->stackIdx = 0;

        voice->field2c_15 = 0;
        voice->forceCenterY = 0;
        voice->forceCenterX = 0;

        voice->idx = i;

        func_0202f1b4(&voice->handle);

        voice++;
    }
}

// MATCHED 100%
SndVoice * sndAllocVoice(SndVoice * exclude) {
    SndVoice * voice = sndVoices;
    s32 slotsLeft = SND_VOICE_COUNT-1;

    while (slotsLeft >= 0) {
        if (voice->inUse == 0) {
            break;
        }

        slotsLeft--;
        voice++;
    }

    if (slotsLeft < 0) {
        SndVoice * cur = sndVoiceList.head;
        u8 minPrio = 0xff;

        voice = NULL;

        while (cur != NULL) {
            if (cur->dualPrio != SND_PRIO_DEFAULT) {
                if (cur->channelPrio + cur->playerPrio < minPrio && cur != exclude) {
                    voice = cur;
                    minPrio = cur->channelPrio + cur->playerPrio;
                }
            }
            cur = cur->next;
        }

        if (voice == NULL) {
            return NULL;
        }

        if ((voice->id & SND_ID_TYPE_MASK) == SND_ID_STREAM) {
            sndStopStream(voice->streamIdx, 2);
        } else {
            sndStopVoice(voice, 0);
            NNS_SndHandleReleaseSeq(&voice->handle);
            func_0202f1b4(&voice->handle);
        }

        sndFreeVoice(voice);
    }

    return voice;
}

// MATCHED 100%
void sndLinkVoice(SndVoice * voice) {
    if (voice->inUse == 0) {
        if (sndVoiceList.head != NULL) {
            voice->next = sndVoiceList.head;
            sndVoiceList.head->prev = voice;
        }
        sndVoiceList.head = voice;
        voice->prev = (SndVoice *)&sndVoiceList;
        voice->inUse = 1;
    }
}

// MATCHED 100%
SndVoice * sndFreeVoice(SndVoice * voice) {
    SndVoice * next = voice->next;

    voice->prev->next = voice->next;
    if (voice->next != NULL) {
        voice->next->prev = voice->prev;
    }

    if (voice->chainPrev != NULL) {
        voice->chainPrev->chainNext = voice->chainNext;
        if (voice->chainNext != NULL) {
            voice->chainNext->chainPrev = voice->chainPrev;
        }
    }

    if ((voice->id & SND_ID_STREAM) == 0) {
        NNS_SndHandleReleaseSeq(&voice->handle);
        func_0202f1b4(&voice->handle);
    }

    voice->next = NULL;
    voice->prev = NULL;
    voice->chainNext = NULL;
    voice->chainPrev = NULL;

    if (voice->owner != NULL) {
        voice->owner->head = NULL;
        voice->owner = NULL;
    }

    voice->id = 0;
    voice->volume = -1;
    voice->delay = 0;
    voice->echoDelay = 0;
    voice->x = 0x7f;
    voice->dualPrio = -1;
    voice->field2c = 0;
    voice->elapsed = 0;
    voice->trackCount = 0;
    voice->stackIdx = 0;

    voice->field2c_15 = 0;
    voice->forceCenterY = 0;
    voice->forceCenterX = 0;

    return next;
}

// MATCHED 100%
void sndUpdateVoices() {
    SndVoice * cur = sndVoiceList.head;

    while (cur != NULL) {
        if (cur->field2c_10 == 0 && cur->stopped != 0 && cur->chainPrev == NULL 
                && cur->chainNext != NULL && !NNS_SndHandleIsValid(&cur->handle)) {
            cur->field2c_10 = 1;
            sndUpdateEchoDelays(cur);
        }

        BOOL expired = FALSE;
        switch(cur->id & SND_ID_TYPE_MASK) {
            case SND_ID_STREAM:
                if (!NNS_SndHandleIsValid(&cur->handle) && cur->stopped != 0) {
                    expired = TRUE;
                }
                break;
            case 0:
            case SND_ID_ALT:
                if (!NNS_SndHandleIsValid(&cur->handle) && cur->stopped != 0 
                        && (cur->chainNext == 0 || cur->echoIdx >= 2)) {
                    expired = TRUE;
                }
        }

        if (expired) {
            cur = sndFreeVoice(cur);
        } else {
            if (cur->stopped != 0) {
                cur->elapsed++;
            }

            if (cur->delay != 0) {
                if (--cur->delay <= 0) {
                    sndStartVoice(cur, cur->id, cur->volume, cur->x, cur->y, cur->stopOthers, cur->echoIdx, cur->dualPrio, cur->duration);
                }
            } else if (cur->echoDelay != 0) {
                if (--cur->echoDelay <= 0) {
                    sndStopVoice(cur, 0);
                }
            }

            cur = cur->next;
        }
    }
}

// MATCHED 100%
void sndScheduleVoice(SndVoice * voice, u32 id, s16 delay, u32 volume, s16 x, s16 y, u8 stopOthers, u8 echoIdx, u16 prio, s16 duration) {
    sndLinkVoice(voice);

    voice->id = id;
    voice->volume = volume;
    voice->delay = delay;
    voice->echoDelay = 0;
    voice->x = x;
    voice->y = y;
    voice->stopOthers = stopOthers;
    voice->echoIdx = echoIdx;
    voice->dualPrio = prio;
    voice->duration = duration;
}

// MATCHED 100%
BOOL sndStopVoices(SndVoiceList * owner, s32 id, void(*fn)(SndVoice *, u32), u32 fadeFrames) {
    SndVoice * cur = sndVoiceList.head;

    if ((id & SND_ID_TYPE_MASK) == SND_ID_STREAM) {
        return FALSE;
    }
    id &= SND_ID_MATCH_MASK;

    BOOL found = FALSE;
    while (cur != NULL) {
        if ((cur->id & SND_ID_TYPE_MASK) != SND_ID_STREAM && cur->echoIdx < 2
            && ((cur->id & SND_ID_MATCH_MASK) == id || id == SND_ID_MATCH_MASK)
            && (owner == NULL || owner == cur->owner)) {

            if (fn != NULL) {
                fn(cur, fadeFrames);
            }

            found = TRUE;
        }
        cur = cur->next;
    }

    return found;
}

// MATCHED 100%
void sndStopVoice(SndVoice * voice, u32 fadeFrames) {
    NNS_SndPlayerStopSeq(&voice->handle, fadeFrames);
    voice->stopped = 1;
}

// MATCHED 100%
void sndReleaseVoice(SndVoice * voice, u32 fadeFrames) {
    NNS_SndPlayerStopSeq(&voice->handle, fadeFrames);
    NNS_SndHandleReleaseSeq(&voice->handle);

    func_0202f1b4(&voice->handle);

    voice->stopped = 1;
    voice->owner = NULL;
}

// MATCHED 100%
void sndUpdateEchoDelays(SndVoice * voice) {
    SndVoice * echo = voice->chainNext;
    while (echo != NULL) {
        if (echo->delay == 0) {
            echo->echoDelay = (echo->echoIdx-1) * SND_ECHO_DELAY_FRAMES;
        } else {
            if (voice->elapsed >= SND_ECHO_DELAY_FRAMES) {
                echo->echoDelay = SND_ECHO_DELAY_FRAMES;
            } else if (voice->elapsed == 0) {
                echo->echoDelay = 1;
            } else {
                echo->echoDelay = voice->elapsed;
            }
        }

        echo = echo->chainNext;
    }
}


static inline void sndCancelVoice(SndVoice * voice) {
    if (voice->inUse == 0) {
        voice->next = 0;
        voice->prev = 0;
        voice->chainNext = 0;
        voice->chainPrev = 0;
        if (voice->owner != 0) {
            voice->owner->head = 0;
            voice->owner = 0;
        }
        voice->id = 0;
        voice->volume = -1;
        voice->delay = 0;
        voice->echoDelay = 0;
        voice->x = 0x7f;
        voice->dualPrio = SND_PRIO_DEFAULT;
        voice->field2c = 0;
        voice->elapsed = 0;
        voice->trackCount = 0;
        voice->stackIdx = 0;
        voice->field2c_15 = 0;
        voice->forceCenterY = 0;
        voice->forceCenterX = 0;
    }
}

// MATCHED 100%
BOOL sndStartVoice(SndVoice * voice, u32 id, s8 volume, s16 x, s16 y, u8 stopOthers, u8 echoIdx, s32 prio, s16 duration) {
    u16 seqNo;

    if (voice == 0) {
        return FALSE;
    }

    u16 trackCount = 0;
    s8 channelPrio = 0;
    s8 playerPrio = 0;
    seqNo = (id & SND_ID_SEQ_MASK);
    voice->stopped = 1;

    if (id & 0x1000) {
        BOOL culled = (x < -0x60 || x > 0x160 || y < -0x60 || y > 0x120);
        if (culled) {
            sndCancelVoice(voice);
            return FALSE;
        }
    }

    if (id & SND_ID_FORCE_CENTER_Y) {
        voice->forceCenterY = 1;
    }

    if (id & SND_ID_FORCE_CENTER_X) {
        voice->forceCenterX = 1;
    }

    if ((id & SND_ID_ECHO_MASK) && echoIdx < 2) {
        echoIdx = ((id & SND_ID_ECHO_MASK) >> 0xe) - 1;
    }

    switch (id & SND_ID_TYPE_MASK) {
        case SND_ID_STREAM: {
            SndVoice * stream = sndFindStream(0);
            if (stream != NULL && NNS_SndHandleIsValid(&stream->handle)) {
                if ((stream->id & SND_ID_MATCH_MASK) == seqNo) {
                    sndStopStream(0, 2);
                }
            } else {
                stream = sndFindStream(1);
                if (stream != NULL && NNS_SndHandleIsValid(&stream->handle) && (stream->id & SND_ID_MATCH_MASK) == seqNo) {
                    sndStopStream(1, 2);
                }
            }

            if (prio == SND_PRIO_DEFAULT) {
                if (sndGetStreamPrio(seqNo, &playerPrio) == 0) {
                    sndCancelVoice(voice);
                    return FALSE;
                }
                channelPrio = 0x7f;
            } else {
                channelPrio = prio;
                playerPrio = prio >> 8;
            }

            NNS_SndArcStrmPrepareEx(&voice->strmHandle, voice->streamIdx, -1, seqNo, 0);
            NNS_SndArcStrmStartPrepared(&voice->strmHandle);
            break;
        }
        case 0:
        case SND_ID_ALT: {
            s16 pitch = 0;

            if (sndCountVoices(id) >= 4) {
                sndCancelVoice(voice);
                return FALSE;
            }

            trackCount = sndGetTrackCount(id);

            if (trackCount == 0) {
                sndCancelVoice(voice);
                return FALSE;
            }

            if (prio == SND_PRIO_DEFAULT) {
                if (sndGetDefaultPrio(id, &playerPrio, &channelPrio) == 0) {
                    sndCancelVoice(voice);
                    return FALSE;
                }
            } else {
                channelPrio = prio;
                playerPrio = prio >> 8;
            }

            if (stopOthers != 0 || ((id & SND_ID_EXCLUSIVE) && (echoIdx < 2))) {
                switch (id & SND_ID_TYPE_MASK) {
                    case SND_ID_STREAM:
                        sndStopStreamById(id & SND_ID_MATCH_MASK, 0);
                        break;
                    case 0:
                    case SND_ID_ALT:
                        sndStopVoices(NULL, id, sndStopVoice, 0);
                }
            } else if (echoIdx < 2) {
                sndApplyVoiceStacking(voice, id, &pitch, &volume);
            }
            if (sndReserveChannels(trackCount, playerPrio, channelPrio) == 0) {
                sndCancelVoice(voice);
                return FALSE;
            }

            if (!NNS_SndArcPlayerStartSeqArcEx(&voice->handle, -1, -1, playerPrio, (u16)((id & SND_ID_MATCH_MASK) >> SND_ID_ARC_SHIFT), seqNo)) {
                sndCancelVoice(voice);
                return FALSE;
            }

            NNS_SndPlayerSetChannelPriority(&voice->handle, channelPrio);
            NNS_SndPlayerSetTrackPitch(&voice->handle, SND_TRACK_ALL, pitch);
        }
    }

    sndLinkVoice(voice);

    voice->id = id;
    voice->volume = volume;
    voice->x = x;
    voice->y = y;
    voice->stopOthers = stopOthers;
    voice->channelPrio = channelPrio;
    voice->playerPrio = playerPrio;
    voice->trackCount = trackCount;
    voice->duration = duration;

    sndApplyPosition(voice, x, y, duration);

    if (echoIdx == 1) {
        while (echoIdx <= 2) {
            volume = FX_Whole(volume * FX32_CONST(0.21));
            if (volume < 5) {
                break;
            }

            if (channelPrio > 1) {
                channelPrio >>= 1;
            }
            if (playerPrio > 1) {
                playerPrio >>= 1;
            }

            SndVoice * echo = sndAllocVoice(voice);
            sndScheduleVoice(echo, id, echoIdx * SND_ECHO_DELAY_FRAMES, volume, x, y, 0, echoIdx+1, (u16)(playerPrio << 8) | (u8)channelPrio, duration);

            echoIdx++;

            if (voice->chainNext != NULL) {
                echo->chainNext = voice->chainNext;
                voice->chainNext->chainPrev = echo;
            }
            voice->chainNext = echo;
            echo->chainPrev = voice;
        }
    }
    return TRUE;
}

// MATCHED 100%
s32 sndCountVoices(u32 id) {
    SndVoice * cur = sndVoiceList.head;
    u32 count = 0;

    while (cur != NULL) {
        if (NNS_SndHandleIsValid(&cur->handle) && cur->echoIdx < 2 && (cur->id & SND_ID_STREAM) == 0 && (cur->id & SND_ID_MATCH_MASK) == (id & SND_ID_MATCH_MASK)) {
            count++;
        }

        cur = cur->next;
    }

    return count;
}

// MATCHED 100%
void sndApplyVoiceStacking(SndVoice * voice, u32 id, s16 * outPitch, s8 * volume) {
    SndVoice * cur = sndVoiceList.head;

    u8 used[SND_VOICE_COUNT];
    MI_CpuFill8(&used, 0, sizeof(used));

    while (cur != NULL) {
        if (NNS_SndHandleIsValid(&cur->handle) && cur->echoIdx < 2 
                && (cur->id & SND_ID_STREAM) == 0 && (cur->id & SND_ID_MATCH_MASK) == (id & SND_ID_MATCH_MASK)) {
            used[cur->stackIdx] = 1;
        }

        cur = cur->next;
    }

    for (s32 i = 0; i < SND_VOICE_COUNT; i++) {
        if (used[i] == 0) {
            voice->stackIdx = i;
            *outPitch = i << 4;

            *volume -= sndStackVolumeDrop[i];
            if (*volume < 1) {
                *volume = 1;
            }
            break;
        }
    }
}

// MATCHED 100%
BOOL sndReserveChannels(s32 trackCount, s8 playerPrio, s8 channelPrio) {
    u8 victims[17];
    u8 * cursor = victims;

    MI_CpuFill8(victims, 0xff, sizeof(victims));

    SND_FlushCommand(SND_COMMAND_BLOCK | SND_COMMAND_IMMEDIATE);
    SND_WaitForCommandProc(SND_GetCurrentCommandTag());

    u16 activeTracks = sndCountActiveTracks();

    while (SND_TRACK_NUM - activeTracks < trackCount) {
        SndVoice * cur = sndVoiceList.head;
        SndVoice * victim = NULL;

        u8 lowestPrio = playerPrio + channelPrio;

        while (cur != NULL) {
            if (NNS_SndHandleIsValid(&cur->handle)) {
                u8 * scan = victims;
                while(*scan != 0xff && *scan != cur->idx) {
                    scan++;
                }

                if (*scan == 0xff && cur->playerPrio + cur->channelPrio <= lowestPrio) {
                    victim = cur;
                    lowestPrio = cur->playerPrio + cur->channelPrio;
                }
            }

            cur = cur->next;
        }

        if (victim == NULL) {
            return FALSE;
        }

        *cursor = victim->idx;
        cursor++;

        activeTracks -= victim->trackCount;
    }

    cursor = victims;
    while (*cursor != 0xff) {
        sndStopVoice(&sndVoices[*cursor], 0);

        NNS_SndHandleReleaseSeq(&sndVoices[*cursor].handle);

        func_0202f1b4(&sndVoices[*cursor].handle);

        cursor++;
    }

    return TRUE;
}

// MATCHED 100%
void sndLoadBank(SndLoadNode * node) {
    NNSSndSeqArc * seqArc = (NNSSndSeqArc *)node->userData;

    for (u32 i = 0; i < seqArc->count; i++) {
        NNSSndSeqArcSeqInfo * info = &seqArc->info[i];

        if (info->offset != NNS_SND_SEQ_ARC_INVALID_OFFSET) {
            const NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(info->param.bankNo);
            u16 fileId = bankInfo->fileId;

            if (NNS_SndArcGetFileAddress(fileId) == NULL) {
                u32 size = NNS_SndArcGetFileSize(fileId);
                void * mem = sndHeapAlloc(sndHeapSys.heap, size, &sndDisposeBank, &sndArc, fileId);

                void * slot = sndAllocBankSlot(bankInfo, mem, i);
                sndQueueNode(mem, size, fileId, 0, &sndLoadWaveArc, slot);
                NNS_SndArcSetFileAddress(fileId, mem);
            }
        }

        info++;
    }
}

// MATCHED 100%
void sndLoadWaveArc(SndLoadNode * node) {
    SndBankSlot * slot = (SndBankSlot *)node->userData;
    const NNSSndArcBankInfo * bankInfo = slot->bankInfo;

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        u16 waveArcNo = bankInfo->waveArcNo[i];

        if (waveArcNo != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            const NNSSndArcWaveArcInfo * waveArcInfo = NNS_SndArcGetWaveArcInfo(waveArcNo);
            u16 fileId = waveArcInfo->fileId;

            if (waveArcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                sndOpenWaveArc(fileId, sndHeapSys.heap, FALSE, TRUE, TRUE, &sndFinishWaveArcLoad, slot->seqIdx);
            } else {
                u32 size = NNS_SndArcGetFileSize(fileId);
                void * mem = sndHeapAlloc(sndHeapSys.heap, size, &sndDisposeWaveArc, &sndArc, fileId);
                sndQueueNode(mem, size, fileId, 0, NULL, NULL);
                NNS_SndArcSetFileAddress(fileId, mem);
            }
        }
    }
}

// MATCHED 100%
void sndFinishWaveArcLoad(SndLoadNode * ctx) {
    SNDWaveArc * waveArc = (SNDWaveArc *)ctx->buffer;
    u32 tableSize = ((SndLoadSlot *)ctx->userData)->tableSize;
    u32 seqIdx = ((SndLoadSlot *)ctx->userData)->userData;

    if (((SndLoadSlot *)ctx->userData)->offsetsFixed == 0) {
        MI_CpuCopy8(waveArc->waveOffset, &waveArc->waveOffset[waveArc->waveCount], tableSize);
        MI_CpuFill8(waveArc->waveOffset, 0, tableSize);
    }

    const NNSSndArcSeqArcInfo * arcInfo = NNS_SndArcGetSeqArcInfo(sndLoader.seqNo);
    const NNSSndSeqArc * seqArc = (const NNSSndSeqArc *)NNS_SndArcGetFileAddress(arcInfo->fileId);
    const NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(seqArc->info[seqIdx].param.bankNo);
    SNDBankData * bankData = (SNDBankData *)NNS_SndArcGetFileAddress(bankInfo->fileId);

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        if (bankInfo->waveArcNo[i] != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            const NNSSndArcWaveArcInfo * arcInfo = NNS_SndArcGetWaveArcInfo(bankInfo->waveArcNo[i]);
            u16 fileId = arcInfo->fileId;
            if (arcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                sndLoadInstrumentWaves(waveArc, bankData, i, fileId, sndHeapSys.heap);
            }
        }
    }
}

// MATCHED 100%
void * sndAllocBankSlot(const NNSSndArcBankInfo * bankInfo, void * bankData, u32 seqIdx) {
    SndBankSlot * slot = &sndBankSlots[0];
    s32 slotsLeft = SND_BANK_COUNT-1;

    while (slotsLeft >= 0) {
        if (slot->inUse == 0)
            break;
        slotsLeft--;
        slot++;
    }

    slot->bankInfo = bankInfo;
    slot->bankData = bankData;
    slot->seqIdx = seqIdx;
    slot->inUse = 1;
    return slot;
}

// MATCHED 100%
void sndStartAlt(SndVoice * voice, s16 id, s16 delay, s16 x, s16 y, s8 volume, s16 duration) {
    id &= SND_ID_MATCH_MASK;

    if (id != 0) {
        sndAltIdx ^= 1;

        u16 altId = sndAltVoiceInfo[sndAltIdx].voiceId;

        sndAltVoiceId[sndAltIdx] = id;

        func_0202f388(sndAltVoiceInfo[sndAltIdx].arg);

        if (delay == 0) {
            sndStartVoice(voice, altId | SND_ID_ALT, volume, x, y, 1, sndEchoMode, SND_PRIO_DEFAULT, duration);
        } else {
            sndScheduleVoice(voice, altId | SND_ID_ALT, delay, volume, x, y, 0, sndEchoMode, duration, 0);
        }
    }
}

// MATCHED 100%
void sndUpdateFades() {
    SndFade * fade = &sndFade[1];
    s8 channelPrio;
    NNSSndHandle * handle = &sndSeqHandle[1];
    s16 slot = 1;

    while (slot >= 0) {
        if (fade->state != NNS_SND_SEQ_MUTE_OFF && fade->state != NNS_SND_SEQ_MUTE_STOP) {

            BOOL hasPlayer = (sndSeqHandle[slot].player != NULL);
            if (hasPlayer) {
                switch (fade->state) {
                    case NNS_SND_SEQ_MUTE_NO_STOP: {
                        fade->volume += fade->step;

                        if (fade->volume > fade->maxVolume) {
                            fade->volume = fade->maxVolume;
                            fade->state = NNS_SND_SEQ_MUTE_OFF;
                        }

                        NNS_SndPlayerSetTrackMuteEx(handle, SND_TRACK_ALL, NNS_SND_SEQ_MUTE_OFF);
                        break;
                    }
                    case NNS_SND_SEQ_MUTE_RELEASE:
                        fade->volume -= fade->step;
                        if (fade->volume < 0) {
                            fade->volume = 0;
                            fade->state = NNS_SND_SEQ_MUTE_STOP;
                            NNS_SndPlayerSetTrackMuteEx(handle, SND_TRACK_ALL, NNS_SND_SEQ_MUTE_NO_STOP);
                        }
                        break;
                    default:
                        break;
                }

                if (fade->volume != 0) {
                    NNS_SndPlayerSetTrackVolume(handle, SND_TRACK_ALL, FX_Whole(fade->volume) & 0xfff);
                }

                const NNSSndArcSeqInfo * setInfo = NNS_SndArcGetSeqInfo(sndSeqHandle[slot].player->seqNo);
                s8 playerPrio;

                if (fade->volume == fade->maxVolume) {
                    playerPrio = setInfo->param.playerPrio;
                    channelPrio = setInfo->param.channelPrio;
                } else {
                    switch (fade->state) {
                        case NNS_SND_SEQ_MUTE_NO_STOP:
                            playerPrio = 1;
                            channelPrio = playerPrio;
                            break;
                        case NNS_SND_SEQ_MUTE_RELEASE:
                        default:
                            playerPrio = 0;
                            channelPrio = playerPrio;
                    }
                }

                NNS_SndPlayerSetPlayerPriority(handle, playerPrio);
                NNS_SndPlayerSetChannelPriority(handle, channelPrio);
            }
        }

        handle--;
        slot--;
        fade--;
    }
}

// MATCHED 100%
void sndLoadAndStartSeq(u8 slot, s16 seqNo, BOOL restart, u32 budget) {
    if (seqNo == sndSeqNo[slot]) {
        if (restart)  {
            sndStartSeqOnSlot(seqNo, slot, FALSE);
        }
    } else {
        NNSSndHeapHandle heap = slot ? sndHeapSlot1.heap : sndHeapSlot0.heap;

        sndSeqNo[slot] = seqNo;

        NNS_SndHeapClear(heap);

        BOOL hasSingleLoad = sndLoadSeqData(heap, seqNo, &sndLoadedData[slot]);
        u32 restartFlag = restart ? 1 : 0;

        sndLoadBudget = budget;

        if (hasSingleLoad) {
            sndBeginLoad(2, seqNo, slot, restartFlag, 0);
        } else {
            sndBeginLoad(restartFlag, seqNo, slot, 0, 0);
        }
    }
}

// MATCHED 100%
void sndStartSeqOnSlot(u32 seqNo, u8 slot, BOOL startMuted) {
    SndLoadedData * loaded = &sndLoadedData[slot];
    const NNSSndArcSeqInfo * seqInfo = NNS_SndArcGetSeqInfo(seqNo);
    const NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(seqInfo->param.bankNo);

    NNS_SndArcSetFileAddress(seqInfo->fileId, loaded->field00);
    NNS_SndArcSetFileAddress(bankInfo->fileId, loaded->field04);

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        if (loaded->field08[i] != NULL) {
            const NNSSndArcWaveArcInfo * waveInfo = NNS_SndArcGetWaveArcInfo(bankInfo->waveArcNo[i]);
            NNS_SndArcSetFileAddress(waveInfo->fileId, loaded->field08[i]);
        }
    }

    NNS_SndPlayerStopSeq(&sndSeqHandle[slot], 0);

    SndFade * fade = &sndFade[slot];
    fade->volume = fade->maxVolume;
    fade->state = NNS_SND_SEQ_MUTE_OFF;

    NNS_SndPlayerSetTrackMuteEx(&sndSeqHandle[slot], SND_TRACK_ALL, NNS_SND_SEQ_MUTE_OFF);

    if (startMuted) {
        sndFade[slot].volume = 0;
    }

    s32 playerNo = (slot != 0) ? 1 : 0;
    NNS_SndArcPlayerStartSeqEx(&sndSeqHandle[slot], playerNo, -1, -1, seqNo);

    NNS_SndPlayerSetTrackVolume(&sndSeqHandle[slot], SND_TRACK_ALL, (u32)FX_Whole(sndFade[slot].volume) & 0xfff);

    if (sndTempoRatio[slot] != SND_TEMPO_RATIO_UNITY) {
        NNS_SndPlayerSetTempoRatio(&sndSeqHandle[slot], sndTempoRatio[slot]);
        sndTempoRatio[slot] = SND_TEMPO_RATIO_UNITY;
    }

    if (startMuted) {
        sndFade[slot].state = NNS_SND_SEQ_MUTE_RELEASE;
        sndFade[slot].volume = sndFade[slot].maxVolume;
        sndFade[slot].step = FX_Div(sndFade[slot].maxVolume, FX32_ONE);
        sndUpdateFades();
    }

    sndCurrent.field00 = seqNo;
    sndCurrent.field04 = slot;
}

// MATCHED 100%
BOOL sndLoadSeqData(NNSSndHeapHandle heap, u32 seqNo, SndLoadedData * out) {
    const NNSSndArcSeqInfo * seqInfo;
    const NNSSndArcBankInfo * bankInfo;
    const NNSSndArcWaveArcInfo * arcInfo;
    u16 fileId;
    u32 size;
    s32 i;
    BOOL anySingleLoad = FALSE;

    out->field00 = 0;
    out->field04 = 0;

    for (i = 0; i < 4; i++) {
        out->field08[i] = 0;
    }

    seqInfo = NNS_SndArcGetSeqInfo(seqNo);
    fileId = seqInfo->fileId;
    size = NNS_SndArcGetFileSize(fileId);
    void * mem = sndHeapAlloc(heap, size, &sndDisposeSeq, &sndArc, fileId);
    sndQueueNode(mem, size, fileId, 0, NULL, NULL);
    out->field00 = mem;

    bankInfo = NNS_SndArcGetBankInfo(seqInfo->param.bankNo);
    fileId = bankInfo->fileId;
    size = NNS_SndArcGetFileSize(fileId);
    mem = sndHeapAlloc(heap, size, &sndDisposeBank, &sndArc, fileId);
    sndQueueNode(mem, size, fileId, 0, NULL, NULL);
    out->field04 = mem;

    for (i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        if (bankInfo->waveArcNo[i] != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            arcInfo = NNS_SndArcGetWaveArcInfo(bankInfo->waveArcNo[i]);
            fileId = arcInfo->fileId;

            if (arcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                out->field08[i] = sndOpenWaveArc(fileId, heap, TRUE, FALSE, TRUE, &sndFixWaveOffsets, 0);
                anySingleLoad = TRUE;
            } else {
                size = NNS_SndArcGetFileSize(fileId);
                mem = sndHeapAlloc(heap, size, &sndDisposeWaveArc, &sndArc, fileId);
                sndQueueNode(mem, size, fileId, 0, 0, 0);
                out->field08[i] = mem;
            }
        }
    }

    return anySingleLoad;
}

// MATCHED 100%
void sndLoadSeqWaves(u32 seqNo, u8 slot) {
    SndLoadedData * loaded = &sndLoadedData[slot];
    const NNSSndArcSeqInfo * seqInfo = NNS_SndArcGetSeqInfo(seqNo);
    const NNSSndArcBankInfo * bankInfo = NNS_SndArcGetBankInfo(seqInfo->param.bankNo);

    for (s32 i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM; i++) {
        if (bankInfo->waveArcNo[i] != NNS_SND_ARC_INVALID_WAVEARC_NO) {
            const NNSSndArcWaveArcInfo * waveInfo = NNS_SndArcGetWaveArcInfo(bankInfo->waveArcNo[i]);

            u16 fileId = waveInfo->fileId;
            if (waveInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD) {
                SNDWaveArc * waveArc = (SNDWaveArc *)loaded->field08[i];
                sndLoadInstrumentWaves(waveArc, (SNDBankData *)loaded->field04, i, fileId, (slot == 0) ? sndHeapSlot0.heap : sndHeapSlot1.heap);
            }
        }
    }
}

// MATCHED 100%
void * sndOpenWaveArc(u32 fileId, NNSSndHeapHandle heap, BOOL forceReload, BOOL publish, BOOL deferred, void (*onComplete)(SndLoadNode *), u32 userData) {
    SNDWaveArc * waveArc;
    u32 len;
    u32 tableSize;

    waveArc = (SNDWaveArc *)NNS_SndArcGetFileAddress(fileId);
    if (waveArc != NULL && forceReload == 0) {
        if (onComplete != NULL && deferred != 0) {
            SndLoadSlot * slot = (SndLoadSlot *)sndAllocLoadSlot(waveArc->waveCount * 4, userData);
            slot->offsetsFixed = 1;

            SndLoadNode ctx;
            ctx.buffer = waveArc;
            ctx.userData = slot;

            onComplete(&ctx);
        }

        return waveArc;
    }

    NNS_SndArcReadFile(fileId, &sndWaveArcHeader, sizeof(sndWaveArcHeader), 0);
    tableSize = sndWaveArcHeader.waveCount * sizeof(u32);
    len = tableSize * 2 + sizeof(sndWaveArcHeader);

    waveArc = (SNDWaveArc *)NNS_SndHeapAlloc(heap, len + 0x20, &sndDisposeWaveArcOnly, (u32)&sndArc, fileId);

    if (deferred == 0) {
        NNS_SndArcReadFile(fileId, waveArc, tableSize + sizeof(sndWaveArcHeader), 0);

        MI_CpuCopy8(waveArc->waveOffset, &waveArc->waveOffset[waveArc->waveCount], tableSize);
        MI_CpuFill8(waveArc->waveOffset, 0, tableSize);
        DC_StoreRange(waveArc, len);
    } else {
        sndQueueNode(waveArc, tableSize + sizeof(sndWaveArcHeader), fileId, 0, onComplete, sndAllocLoadSlot(tableSize, userData));
    }

    if (publish != 0) {
        NNS_SndArcSetFileAddress(fileId, waveArc);
    }

    return waveArc;
}

// MATCHED 100%
void sndFixWaveOffsets(SndLoadNode * ctx) {
    SndLoadSlot * slot = (SndLoadSlot *)ctx->userData;
    SNDWaveArc * waveArc = (SNDWaveArc *)ctx->buffer;
    u32 tableSize = slot->tableSize;

    if (slot->offsetsFixed == 0) {
        MI_CpuCopy8(waveArc->waveOffset, &waveArc->waveOffset[waveArc->waveCount], tableSize);
        MI_CpuFill8(waveArc->waveOffset, 0, tableSize);
    }
}

// MATCHED 100%
void * sndAllocLoadSlot(u32 tableSize, u32 userData) {
    SndLoadSlot * slot = &sndLoadSlots[0];
    s32 slotsLeft = SND_LOAD_COUNT-1;

    while (slotsLeft >= 0) {
        if (slot->inUse == 0)
            break;
        slotsLeft--;
        slot++;
    }

    slot->tableSize = tableSize;
    slot->userData = userData;
    slot->inUse = 1;
    return slot;
}

// MATCHED 100%
void sndLoadInstrumentWaves(SNDWaveArc * waveArc, SNDBankData * bank, u32 waveNo, u16 fileId, NNSSndHeapHandle heap) {
    SNDInstPos pos = SND_GetFirstInstDataPos(bank);
    SNDInstData inst;

    while (SND_GetNextInstData(bank, &inst, &pos)) {
        if (inst.type == 1 && waveNo == inst.param.wave[1]) {
            sndLoadWave(waveArc, inst.param.wave[0], fileId, heap, 1);
        }
    }
}

// MATCHED 100%  -- LoadSingleWave modified
BOOL sndLoadWave(SNDWaveArc * waveArc, s32 waveNo, u32 fileId, NNSSndHeapHandle heap, BOOL deferred) {
    if (SND_GetWaveDataAddress(waveArc, waveNo) != NULL) {
        return FALSE;
    }

    u32 waveCount = SND_GetWaveDataCount(waveArc);
    u32 begin = waveArc->waveOffset[waveArc->waveCount + waveNo];
    u32 end = (waveNo < --waveCount) ? waveArc->waveOffset[waveArc->waveCount + waveNo + 1] : waveArc->fileHeader.fileSize;

    void * mem = sndHeapAlloc(heap, end-begin, &sndDisposeWaveData, waveArc, waveNo);

    u32 offset = waveArc->waveOffset[waveArc->waveCount + waveNo];

    if (deferred) {
        sndQueueNode(mem, end-begin, fileId, offset, NULL, 0);
    } else {
        NNS_SndArcReadFile(fileId, mem, end-begin, offset);
        DC_StoreRange(mem, end-begin);
    }

    SND_SetWaveDataAddress((SNDWaveArc *)waveArc, waveNo, (const SNDWaveData *)mem);
    return TRUE;
}

// MATCHED 100%
void * sndHeapAlloc(NNSSndHeapHandle heap, u32 size, NNSSndHeapDisposeCallback callback, void * arc, u32 fileId) {
    if (size < 0x1c) {
        size = 0x1c;
    }

    return NNS_SndHeapAlloc(heap, size, callback, (u32)arc, fileId);
}

// MATCHED 100%
void sndInitNodeList() {
    sndLoadNodeHead.next = (SndLoadNode*)&sndLoadNodeTail;
    sndLoadNodeTail.prev = (SndLoadNode*)&sndLoadNodeHead;
    sndLoadNodeTail.next = (SndLoadNode*)-1;
}

// MATCHED 100%
void sndQueueNode(void * mem, u32 size, u32 fileId, u32 offset, void (*onComplete)(SndLoadNode *), void * userData) {
    SndLoadNode * node = (SndLoadNode *)mem;
    node->buffer = node;
    node->fileId = fileId;
    node->size = size;
    node->offset = offset;
    node->onComplete = onComplete;
    node->userData = userData;
    sndLoadNodeTail.prev->next = node;
    node->next = (SndLoadNode *)&sndLoadNodeTail;
    sndLoadNodeTail.prev = (SndLoadNode *)mem;
}

// MATCHED 100%
void sndBeginLoad(u8 state, u32 seqNo, u8 slot, u32 arg, u8 heapSel) {
    SndLoadNode * first = sndLoadNodeHead.next;
    SndLoadNode * next = first->next;

    sndLoader.next = next;
    sndLoader.buffer = first->buffer;
    sndLoader.fileId = first->fileId;
    sndLoader.size = first->size;
    sndLoader.offset = first->offset;
    sndLoader.onComplete = first->onComplete;
    sndLoader.userData = first->userData;
    sndLoader.current = next;
    sndLoader.state = state;
    sndLoader.seqNo = seqNo;
    sndLoader.slot = slot;
    sndLoader.readSoFar = 0;
    sndLoader.busy = 1;
    sndLoader.arg = arg;
    sndLoader.heapSel = heapSel;
}

// MATCHED 100%
void sndInitRequests() {
    for (s32 i = 15; i >= 0; i--) {
        sndRequests[i].argForm = 0xff;
    }

    sndRequestTail.tail = (SndRequest *)&sndRequestHead;
    sndRequestHead.first = (SndRequest *)&sndRequestTail;
}

// MATCHED 100%
void sndQueueRequest(SndRequest * req) {
    SndRequest * slot = sndRequests;
    s32 slotsLeft = SND_REQ_COUNT-1;

    while (slotsLeft >= 0) {
        if (slot->argForm == 0xff) {
            MI_CpuCopy8(req, slot, 0x1c);

            SndRequestHead * head = &sndRequestHead;
            SndRequest * first = head->first;

            slot->next = first;
            slot->prev = (SndRequest *)head;
            first->prev = slot;
            head->first = slot;
            return;
        }

        slot++;
        slotsLeft--;
    }
}

// MATCHED 100%
void sndRunNextRequest() {
    SndRequestHead * head = &sndRequestHead;
    SndRequest * req = sndRequestTail.tail;

    if (req == (SndRequest *)head) {
        return;
    }

    switch (req->argForm) {
        case 0:
            ((void (*)(u32, u8, u32))req->field0c)(
                req->form0.arg1,
                req->form0.arg2,
                req->form0.arg3
            );
            break;
        case 1:
            ((void (*)(u16 *, u32, u32))req->field0c)(
                req->form1.programs,
                req->form1.arg2,
                req->form1.arg3
            );
            break;
        case 2:
            ((void (*)(u32, u32))req->field0c)(
                req->form2.arg1,
                req->form2.arg2
            );
            break;
        default:
            break;
    }

    req->next->prev = req->prev;
    req->prev->next = req->next;
    req->argForm = -1;
}

// MATCHED 100%
void sndUpdateLoader() {
    if (!sndProcessLoadQueue()) {
        return;
    }

    // UnkNode * loader = &sndLoader;
    if (sndLoader.busy != 0) {
        return;
    }

    u32 state = sndLoader.state;
    if (state != 0) {
        if (state != 1) {
            if (state != 2) {
                return;
            }
        } else {
            sndHeapSaveState(&sndLoader);

            sndStartSeqOnSlot(sndLoader.seqNo, sndLoader.slot, FALSE);

            MI_CpuFill8(&sndLoader, 0, sizeof(SndLoader));

            sndInitNodeList();

            MI_CpuFill8(&sndBankSlots, 0, sizeof(sndBankSlots));
            MI_CpuFill8(&sndLoadSlots, 0, sizeof(sndLoadSlots));

            sndRunNextRequest();
            return;
        }

        u32 seqNo = sndLoader.seqNo;
        u8 slot = sndLoader.slot;
        u32 arg = sndLoader.arg;

        MI_CpuFill8(&sndLoader, 0, sizeof(SndLoader));

        sndInitNodeList();

        MI_CpuFill8(&sndBankSlots, 0, sizeof(sndBankSlots));
        MI_CpuFill8(&sndLoadSlots, 0, sizeof(sndLoadSlots));

        sndLoadSeqWaves(seqNo, slot);

        sndBeginLoad(arg, seqNo, slot, 0, (&sndLoader)->heapSel);
    } else {
        sndHeapSaveState(&sndLoader);

        MI_CpuFill8(&sndLoader, 0, sizeof(sndLoader));

        sndInitNodeList();

        MI_CpuFill8(&sndBankSlots, 0, sizeof(sndBankSlots));
        MI_CpuFill8(&sndLoadSlots, 0, sizeof(sndLoadSlots));

        sndRunNextRequest();
    }
}

// MATCHED 100%
void sndHeapSaveState(SndLoadNode* arg) {
    NNSSndHeapHandle heap = (NNSSndHeapHandle)arg;

    if (sndLoader.heapSel != 0) {
        if (sndLoader.heapSel == 1) {
            heap = sndHeapSys.heap;
        }
    } else {
        SndHeap * src;
        if (sndLoader.slot == 0) {
            src = &sndHeapSlot0;
        } else {
            src = &sndHeapSlot1;
        }
        heap = src->heap;
    }

    NNS_SndHeapSaveState(heap);
}

// MATCHED 100%
BOOL sndProcessLoadQueue() {
    SndLoadNode ** newNode;
    u32 chunk;
    u32 totalRead = 0;
    SndLoadNode * endOfList = (SndLoadNode *)-1;

    do {
        chunk = sndLoadBudget - totalRead;

        if (sndLoader.size - sndLoader.readSoFar < chunk) {
            chunk = sndLoader.size - sndLoader.readSoFar;
        }

        u8 * dest = (u8*)sndLoader.buffer;
        u32 read = sndLoader.readSoFar;
        u32 * writePtr = (u32*)(dest + read);
        NNS_SndArcReadFile(
            sndLoader.fileId,
            writePtr,
            chunk,
            read + sndLoader.offset
        );

        DC_StoreRange(writePtr, chunk);

        sndLoader.readSoFar += chunk;
        if (sndLoader.readSoFar < sndLoader.size) {
            goto false_exit;
            // break; // Early return false?
        }

        sndLoader.readSoFar = 0;

        BOOL listWasEmpty = FALSE;
        if (sndLoader.current != endOfList && sndLoader.current->next == endOfList) {
            listWasEmpty = TRUE;
            sndInitNodeList();
        }

        newNode = &sndLoader.next;
        if (sndLoader.onComplete != NULL) {
            sndLoader.onComplete(&sndLoader);
        }

        if (listWasEmpty && sndLoadNodeHead.next != endOfList) {
            sndLoader.current = sndLoadNodeHead.next;
        }

        sndLoader.next = sndLoader.current->next;
        sndLoader.buffer = sndLoader.current->buffer;
        sndLoader.fileId = sndLoader.current->fileId;
        sndLoader.size = sndLoader.current->size;
        sndLoader.offset = sndLoader.current->offset;
        sndLoader.onComplete = sndLoader.current->onComplete;
        sndLoader.userData = sndLoader.current->userData;
        sndLoader.current = sndLoader.next;

        if (*newNode == endOfList) {
            sndLoader.busy = 0;
            break;
        }

        // Check loop conditions
    } while (chunk < sndLoadBudget && (totalRead += chunk) < sndLoadBudget);

    return TRUE;

false_exit:
    return FALSE;
}

// MATCHED 100%
void sndDisposeSeq(void * mem, u32 size, u32 data1, u32 data2) {
    sndClearFileAddress(mem, data1, data2);
    SND_InvalidateSeqData(mem, (u8 *)mem + size);
}

// MATCHED 100%
void sndDisposeWaveArc(void * mem, u32 size, u32 data1, u32 data2) {
    sndClearFileAddress(mem, data1, data2);
    SND_InvalidateWaveData(mem, (u8 *)mem + size);
    SND_DestroyWaveArc((SNDWaveArc *)mem);
}

// MATCHED 100%
void sndDisposeBank(void * mem, u32 size, u32 data1, u32 data2) {
    sndClearFileAddress(mem, data1, data2);
    SND_InvalidateBankData(mem, (u8 *)mem + size);
    SND_DestroyBank((SNDBankData *)mem);
}

// MATCHED 100%
void sndDisposeWaveArcOnly(void * mem, u32 size, u32 data1, u32 data2) {
    sndClearFileAddress(mem, data1, data2);
    SND_DestroyWaveArc((SNDWaveArc *)mem);
}

// MATCHED 100%  --  modified SingleWaveDisposeCallback
void sndDisposeWaveData(void * mem, u32 size, u32 data1, u32 data2) {
    SNDWaveArc * waveArc = (SNDWaveArc *)data1;

    if (waveArc->fileHeader.headerSize != 0) {
        if (mem == SND_GetWaveDataAddress(waveArc, data2)) {
            SND_SetWaveDataAddress(waveArc, data2, NULL);
        }
    }

    SND_InvalidateWaveData(mem, (u8 *)mem + size);
}

// MATCHED 100%
void sndClearFileAddress(void * mem, u32 data1, u32 data2) {
    NNSSndArc * arc;
    OSIntrMode mode;

    mode = OS_DisableInterrupts();
    arc = NNS_SndArcSetCurrent((NNSSndArc *)data1);

    if (mem == NNS_SndArcGetFileAddress(data2)) {
        NNS_SndArcSetFileAddress(data2, NULL);
    }

    NNS_SndArcSetCurrent(arc);
    OS_RestoreInterrupts(mode);
}

// MATCHED 100%
BOOL sndReadDriverInfo() {
    if (sndDriverInfoDirty == 0) {
        while (SND_RecvCommandReply(SND_COMMAND_NOBLOCK) != NULL) {}

        if (!SND_IsFinishedCommandTag(sndCommandTag)) {
            return FALSE;
        }

        if (sndDriverInfoIdx < 0) {
            sndDriverInfoIdx = 1;
        }

        SND_ReadDriverInfo(&sndDriverInfo[sndDriverInfoIdx]);

        sndCommandTag = SND_GetCurrentCommandTag();

        if (sndDriverInfoIdx == 0) {
            sndDriverInfoIdx = 1;
        } else {
            sndDriverInfoIdx = 0;
        }

        DC_InvalidateRange(&sndDriverInfo[sndDriverInfoIdx], sizeof(SNDDriverInfo));

        SND_FlushCommand(SND_COMMAND_NOBLOCK);

        return TRUE;
    } else {
        SND_ReadDriverInfo(&sndDriverInfo[0]);

        sndCommandTag = SND_GetCurrentCommandTag();
        sndDriverInfoDirty = 0;

        return FALSE;
    }
}

#pragma optimization_level 2
// MATCHED 100%
u32 sndCountPlayerTracks(NNSSndHandle * handle) {
    if (!NNS_SndHandleIsValid(handle)) {
        return 0;
    }

    SNDDriverInfo * driverInfo = (sndDriverInfoIdx < 0) ? NULL : &sndDriverInfo[sndDriverInfoIdx];
    if (driverInfo == NULL) {
        return 0;
    }

    SNDPlayer * player = &driverInfo->work.player[handle->player->playerNo];
    if (!player->active_flag) {
        return 0;
    }

    u32 count = 0;
    for (s32 i = 0; i < SND_TRACK_NUM_PER_PLAYER; i++) {
        if (player->tracks[i] != 0xff) {
            count++;
        }
    }

    return count;
}
#pragma optimization_level reset

// MATCHED 100%
BOOL sndGetDefaultPrio(u32 id, s8 * outPlayerPrio, s8 * outChannelPrio) {
    u16 seqArcNo = (id & SND_ID_MATCH_MASK) >> SND_ID_ARC_SHIFT;
    u16 seqIdx = id & SND_ID_SEQ_MASK;

    NNSSndSeqArc * seqArc = (NNSSndSeqArc *)NNS_SndArcGetFileAddress(NNS_SndArcGetSeqArcInfo(seqArcNo)->fileId);
    if (seqArc == NULL) {
        return FALSE;
    }

    if (outPlayerPrio != NULL) {
        *outPlayerPrio = seqArc->info[seqIdx].param.playerPrio;
    }

    if (outChannelPrio != NULL) {
        *outChannelPrio = seqArc->info[seqIdx].param.channelPrio;
    }

    return TRUE;
}

// MATCHED 100%
BOOL sndGetStreamPrio(u32 strmNo, s8 * outPlayerPrio) {
    const NNSSndArcStrmInfo * strmInfo = NNS_SndArcGetStrmInfo(strmNo);

    if (strmInfo == NULL) {
        return FALSE;
    } else {
        *outPlayerPrio = strmInfo->playerPrio;
        return TRUE;
    }
}

// MATCHED 100%
u16 sndGetTrackCount(u32 id) {
    u16 seqArcNo = (id & SND_ID_MATCH_MASK) >> SND_ID_ARC_SHIFT;
    u16 seqIdx = id & SND_ID_SEQ_MASK;

    NNSSndSeqArc * seqArc = (NNSSndSeqArc *)NNS_SndArcGetFileAddress(NNS_SndArcGetSeqArcInfo(seqArcNo)->fileId);
    if (seqArc == NULL) {
        return 0;
    }

    u8 * base = (u8 *)seqArc + seqArc->baseOffset;
    u8 cmd = base[seqArc->info[seqIdx].offset];
    u8 * seqData = base + seqArc->info[seqIdx].offset;

    u16 count = 1;

    if (cmd == 0xff) {
        return 0;
    }

    if (cmd != 0xfe) {
        return 1;
    }

    u16 mask = (seqData[2] << 8) | seqData[1];
    while (mask != 0) {
        count += mask & 1;
        mask >>= 1;
    };

    return count;
}

