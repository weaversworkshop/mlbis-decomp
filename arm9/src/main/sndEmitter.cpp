// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/sndEmitter.h"

#include "main/sound.h"

extern "C" {

// --- Struct Definitions --- //

typedef struct {
    SndEmitterCmd ** cmdTable;
    SndEmitter * head;
} SndEmitterList;

typedef struct {
    s32 id;
    SndEmitter * emitter;
    union { s16 * xSrc; s32 x; };
    union { s16 * ySrc; s32 y; };
    s16 mode;
    u16 repeats : 15;
    u16 isFixed : 1;
    u8 slot;
} SndEmitterRequest;

// --- Extern Data Variables --- //

extern SndEmitterCmd * data_ov000_02069468;

extern SndEmitterCmd * data_ov011_02070018;

// --- Extern Functions --- //

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

// --- Variable Definitions --- //

extern u8 sndFadeTarget;  // 0x020500ac

extern SndEmitterList sndEmitterList;  // 0x02058920
extern u8 sndEmitterNext;  // 0x02058928
extern SndEmitterRequest sndEmitterRequests[16];  // 0x0205892c

SndEmitterSlot sndEmitters[16];  // 0x02058aac

// --- Function Declarations --- //

void sndEmitSetCmdTable(void);  // 0x0200a40c
void sndEmitInit(void);  // 0x0200a600
void sndEmitUpdate(void);  // 0x0200a61c
void sndEmitTickScript(SndEmitter *);  // 0x0200a814
void sndEmitArmEnd(SndEmitter *);  // 0x0200a98c
void sndEmitLoopEnd(SndEmitter *);  // 0x0200aa20
void sndEmitArmTimed(SndEmitter *);  // 0x0200aad8
void sndEmitLoopTimed(SndEmitter *);  // 0x0200ab6c
void sndEmitCrossFade(s32);  // 0x0200acc8
void sndEmitCrossFadeBack(s32);  // 0x0200ad68
void sndEmitUseSlot1(void);  // 0x0200aebc
void sndEmitUseSlot0(void);  // 0x0200aef8
void sndEmitFadeSlot1(void);  // 0x0200af34
void sndEmitFadeSlot0(void);  // 0x0200af80
void sndEmitStopSlot1(void);  // 0x0200afc8
void sndEmitStopSlot0(void);  // 0x0200afd8
SndEmitter * sndEmitPlayTracked(s32, s16 *, s16 *, s16, s16, s16, u8, s16);  // 0x0200afe8
SndEmitter * sndEmitPlay(s32, s16, s16, s16, s16, s16, u8, s16);  // 0x0200b094
void sndEmitStop(u32);  // 0x0200b13c
void sndEmitStopAll(u32);  // 0x0200b2a8
void sndEmitReset(u32);  // 0x0200b30c
void sndEmitStopId(s32);  // 0x0200b340
void sndEmitStopFaded(u32, BOOL);  // 0x0200b3e4
void sndEmitStopSlot(u32);  // 0x0200b4b8
void sndEmitClearQueue(void);  // 0x0200b57c
void sndEmitFlushQueue(u32);  // 0x0200b5a8
void sndEmitReboot(void);  // 0x0200b718
SndEmitterSlot * sndEmitAlloc(s32);  // 0x0200b75c

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void sndEmitSetCmdTable() {
    sndEmitterList.cmdTable = &data_ov011_02070018;
}

// MATCHED 100%
SndEmitter::SndEmitter() {
    this->reset();

    this->xSrc = NULL;
    this->ySrc = NULL;
}

// MATCHED 100%
void SndEmitter::start(s32 id, s16 delay, s16 mode, s16 repeats, s16 x, s16 y, s16 volume, s16 duration) {
    if (id & SND_ID_SCRIPT) {
        this->step = 0;
        this->repeatCount = 0;
        this->id = id;
        this->noStopOnEnd = 0;

        SndEmitterCmd * ret = this->resolveCmd();

        this->volume = ret->volume;
        this->timer = ret->life;

        if (this->timer != -1) {
            this->timer += ret->arg;
        }

        this->tick = sndEmitTickScript;
        this->x = x;
        this->y = y;

        BOOL linked = (this->next != 0) || (this->prev != 0);
        if (!linked) {
            this->link();
        }

        this->playVoice(ret->id, ret->arg, TRUE, ret->duration);
    } else {
        this->step = 0xff;

        if (mode == 0) {
            this->tick = sndEmitTickScript;
            repeats = 0;
        } else if (mode < 0) {
            this->tick = sndEmitArmEnd;
        } else {
            this->tick = sndEmitArmTimed;
        }

        this->id = id;
        this->timer = mode;
        this->period = mode;
        this->repeats = repeats;
        this->volume = volume;
        this->x = x;
        this->y = y;

        BOOL linked = (this->next != 0) || (this->prev != 0);
        if (!linked) {
            this->link();
        }

        this->playVoice(this->id, delay, TRUE, duration);
    }
}

// MATCHED 100%
void SndEmitter::stop(u32 fadeFrames) {
    sndStopOwned(&this->voices[this->voiceIdx], fadeFrames);

    this->unlink();
}

// MATCHED 100%
void sndEmitInit() {
    sndEmitterList.head = NULL;
    sndEmitterList.cmdTable = &data_ov000_02069468;
}

// MATCHED 100%
void sndEmitUpdate() {
    SndEmitter * emitter = sndEmitterList.head;
    s16 x, y;

    while (emitter != NULL) {
        if (emitter->noPosUpdate == 0) {
            emitter->getPosition(&x, &y);
            sndSetListPosition(&emitter->voices[emitter->voiceIdx], x, y);
        }

        SndEmitter * next = emitter->next;
        emitter->tick(emitter);
        emitter = next;
    }
}

// MATCHED 100%
void SndEmitter::getPosition(s16 * x, s16 * y) {
    *x = (this->xSrc != NULL) ? this->xSrc[0] : this->x;
    *y = (this->ySrc != NULL) ? this->ySrc[0] : this->y;
}

// MATCHED 100%
void SndEmitter::playVoice(u32 id, s16 delay, BOOL forceDelay, s16 duration) {
    s16 x, y;

    this->getPosition(&x, &y);

    if (delay == 0 && forceDelay) {
        delay = 1;
    }

    this->voiceIdx = (voiceIdx + 1) & 7;

    if ((id & SND_ID_STREAM) && sndEchoMode != 0) {
        if ((id & 0xff0fff) >= 0x31) {
            id += 500;
        }
    }

    u32 idMatch = id & 0xff0fff;
    if (idMatch == 0x2014f || idMatch == 0x20150 || idMatch == 0x20153 || idMatch == 0x20154 || idMatch - 0x2014d <= 1) {
        id |= 0x4000;
    } else if (idMatch + 0xfffdfe83 <= 1) {
        id |= 0x8000;
    }

    if (duration < 0) {
        if ((duration & 0x8000) && (duration & 0x1000)) {
            x = duration;
        } else {
            x = duration & ~0x8000;
        }

        duration = 0;

        this->xSrc = NULL;
    }

    sndPlayByIdOwned(&this->voices[this->voiceIdx], id, delay, x, y, this->volume, duration);
}

// MATCHED 100%
void sndEmitTickScript(SndEmitter * emitter) {
    if (emitter->timer != -1 && emitter->step != 0xff) {
        emitter->timer--;
        if (emitter->timer <= 0) {
            BOOL playing = (emitter->voices[emitter->voiceIdx].head != NULL);
            if (playing && emitter->noStopOnEnd == 0) {
                sndStopOwned(&emitter->voices[emitter->voiceIdx], 2);
            }
            emitter->advance();
        }
    } else {
        BOOL playing = (emitter->voices[emitter->voiceIdx].head != NULL);
        if (!playing) {
            emitter->advance();
        }
    }
}

// MATCHED 100%
void SndEmitter::advance() {
    if (this->step == 0xff) {
        this->unlink();
    } else {
        this->step++;

        SndEmitterCmd * cmd = this->resolveCmd();

        if (cmd->id == -1) {
            this->unlink();
        } else {
            if (cmd->id == -2) {
                this->step = cmd->arg;
                cmd = this->resolveCmd();
            }

            this->volume = cmd->volume;

            this->timer = cmd->life;
            if (this->timer != -1) {
                this->timer += cmd->arg;
            }

            this->playVoice(cmd->id, cmd->arg, FALSE, cmd->duration);
        }
    }
}

// MATCHED 100%
void sndEmitArmEnd(SndEmitter * param1) {
    BOOL playing = (param1->voices[param1->voiceIdx].head != NULL);
    if (!playing) {
        param1->unlink();
    }

    SndVoice * r0 = param1->voices[param1->voiceIdx].head;
    playing = (r0 != NULL);
    playing = (playing && NNS_SndHandleIsValid(&r0->handle));

    if (playing) {
        param1->tick = sndEmitLoopEnd;
    }
}

// MATCHED 100%
void sndEmitLoopEnd(SndEmitter * param1) {
    SndVoice * r0 = param1->voices[param1->voiceIdx].head;
    BOOL playing = (r0 != NULL);
    playing = (playing && NNS_SndHandleIsValid(&r0->handle));

    if (!playing) {
        if (param1->repeats != 0 && --param1->repeats == 0) {
            param1->unlink();
        }

        if (param1->tick != NULL) {
            param1->playVoice(param1->id, 0, FALSE, 0);
        }
    }
}

// MATCHED 100%
void sndEmitArmTimed(SndEmitter * param1) {
    BOOL playing = (param1->voices[param1->voiceIdx].head != NULL);
    if (!playing) {
        param1->unlink();
    }

    SndVoice * r0 = param1->voices[param1->voiceIdx].head;
    playing = (r0 != NULL);
    playing = (playing && NNS_SndHandleIsValid(&r0->handle));

    if (playing) {
        param1->tick = sndEmitLoopTimed;
    }
}

// MATCHED 100%
void sndEmitLoopTimed(SndEmitter * param1) {
    if (--param1->timer <= 0) {
        param1->timer = param1->period;

        if (param1->repeats != 0 && --param1->repeats == 0) {
            sndStopOwned(&param1->voices[param1->voiceIdx], 2);
            param1->unlink();
        }

        if (param1->tick != NULL) {
            param1->playVoice(param1->id, 0, FALSE, 0);
        }
    }
}

// MATCHED 100%
void SndEmitter::link() {
    if (sndEmitterList.head != NULL) {
        this->next = sndEmitterList.head;
        sndEmitterList.head->prev = this;
    }

    sndEmitterList.head = this;
    this->prev = (SndEmitter *)&sndEmitterList.head;
}

// MATCHED 100%
void SndEmitter::unlink() {
    if (this->next != NULL || this->prev != NULL) {
        this->prev->next = this->next;

        if (this->next != NULL) {
            this->next->prev = this->prev;
        }

        this->reset();
    }
}

// MATCHED 100%
void SndEmitter::reset() {
    this->voices[0].head = NULL;
    this->voices[1].head = NULL;
    this->voices[2].head = NULL;
    this->voices[3].head = NULL;

    this->next = NULL;
    this->prev = NULL;
    this->tick = NULL;
    this->id = 0;

    this->x = 0x7f;
    this->y = 0x60;

    this->volume = 0x7f;

    this->timer = 0;
    this->period = 0;
    this->repeats = 0;
    this->field43 = 0;
}

// MATCHED 100%
void sndEmitCrossFade(s32 fadeFrames) {
    u32 status0 = sndGetSlotStatus(0);
    u32 status1 = sndGetSlotStatus(1);

    if ((status0 != 0 || status1 != 0) && (status0 != 3 || status1 != 3)) {
        if ((status0 == 3 || status0 == 0) && status1 != 0) {
            sndStartFadeSlot(1, fadeFrames, 1);
            sndFadeTarget = 1;
        }

        if ((status1 == 3 || status1 == 0) && status0 != 0) {
            sndStartFadeSlot(1, fadeFrames, 0);
            sndFadeTarget = 0;
        }
    }
}

// MATCHED 100%
void sndEmitCrossFadeBack(s32 fadeFrames) {
    sndStartFadeSlot(0, fadeFrames, sndFadeTarget);
    sndFadeTarget = 0xff;
}

// MATCHED 100%
SndEmitterCmd * SndEmitter::resolveCmd() {
    while (TRUE) {
        SndEmitterCmd * cmd = &sndEmitterList.cmdTable[this->id & 0xff0fff][this->step];

        if (cmd->id == -2) {
            if (this->repeatCount == 0) {
                if (cmd->volume != 0) {
                    this->repeatCount = cmd->volume-1;
                }
                return cmd;
            } else if (--this->repeatCount != 0) {
                return cmd;
            } else {
                this->step++;
            }
        } else if (cmd->id == -3) {
            this->noStopOnEnd = 1;
            this->step++;
        } else if (cmd->id == -4) {
            sndEmitCrossFade(cmd->arg);
            this->step++;
        } else if (cmd->id == -5) {
            sndEmitCrossFadeBack(cmd->arg);
            this->step++;
        } else if (cmd->id == -6) {
            this->noStopOnEnd = 0;
            this->step++;
        } else {
            return cmd;
        }
    }

    return NULL;
}

// MATCHED 100%
void sndEmitUseSlot1() {
    sndRestartSlot(0, FALSE, TRUE);
    sndRestartSlot(1, FALSE, FALSE);
    sndEchoMode = 1;
    sndEmitFlushQueue(1);
}

// MATCHED 100%
void sndEmitUseSlot0() {
    sndRestartSlot(0, FALSE, FALSE);
    sndRestartSlot(1, FALSE, TRUE);
    sndEchoMode = 0;
    sndEmitFlushQueue(0);
}

// MATCHED 100%
void sndEmitFadeSlot1() {
    sndStartFadeSlot(1, 0x3c, FALSE);
    sndStartFadeSlot(0, 0x3c, TRUE);
    sndEchoMode = 1;
    sndEmitStopFaded(0, FALSE);
    sndEmitFlushQueue(1);
}

// MATCHED 100%
void sndEmitFadeSlot0() {
    sndStartFadeSlot(0, 0x3c, FALSE);
    sndStartFadeSlot(1, 0x3c, TRUE);
    sndEchoMode = 0;
    sndEmitStopFaded(1, FALSE);
    sndEmitFlushQueue(0);
}

// MATCHED 100%
void sndEmitStopSlot1() {
    sndEmitStopSlot(1);
}

// MATCHED 100%
void sndEmitStopSlot0() {
    sndEmitStopSlot(0);
}

// MATCHED 100%
SndEmitter * sndEmitPlayTracked(s32 id, s16 * xSrc, s16 * ySrc, s16 delay, s16 mode, s16 repeats, u8 slot, s16 duration) {
    SndEmitterSlot * emitter = sndEmitAlloc(id);
    if (emitter != NULL) {
        emitter->xSrc = xSrc;
        emitter->ySrc = ySrc;

        emitter->start(id, delay, mode, repeats, 0x7f, 0x60, 0x7f, duration);

        emitter->slot = slot;

        if (id & SND_ID_REQUEUE) {
            emitter->enqueue(id, (u32)xSrc, (u32)ySrc, mode, repeats, FALSE, slot);
        }
    }

    return emitter;
}

// MATCHED 100%
SndEmitter * sndEmitPlay(s32 id, s16 x, s16 y, s16 delay, s16 mode, s16 repeats, u8 slot, s16 duration) {
    SndEmitterSlot * emitter = sndEmitAlloc(id);
    if (emitter != NULL) {
        emitter->xSrc = NULL;
        emitter->ySrc = NULL;

        emitter->start(id, delay, mode, repeats, x, y, 0x7f, duration);

        emitter->slot = slot;

        if (id & SND_ID_REQUEUE) {
            emitter->enqueue(id, x, y, mode, repeats, TRUE, slot);
        }
    }

    return emitter;
}

// MATCHED 100%
void sndEmitStop(u32 fadeFrames) {
    for (s32 i = 15; i >= 0; i--) {
        SndEmitter * emitter = &sndEmitters[i];

        switch (emitter->id) {
            case 0x2000019:
            case 0x200001a:
            case 0x2000042:
            case 0x2000088:
            case 0x2000089:
            case 0x200008a:
            case 0x200008e:
            case 0x2000090:
            case 0x20000a1:
            case 0x20000a2:
            case 0x20000a3:
                break;

            default: {
                BOOL linked = (emitter->next != NULL || emitter->prev != NULL);
                if (linked) {
                    emitter->stop(fadeFrames);

                    emitter->xSrc = NULL;
                    emitter->ySrc = NULL;
                }
            }
        }
    }
}

// MATCHED 100%
void sndEmitStopAll(u32 fadeFrames) {
    for (s32 i = 15; i >= 0; i--) {
        SndEmitter * emitter = &sndEmitters[i];
        BOOL linked = (emitter->next != NULL || emitter->prev != NULL);
        if (linked) {
            emitter->stop(fadeFrames);

            emitter->xSrc = NULL;
            emitter->ySrc = NULL;
        }
    }
}

// MATCHED 100%
void sndEmitReset(u32 fadeFrames) {
    sndEmitStopAll(fadeFrames);

    for (s32 i = 15; i >= 0; i--) {
        sndEmitterRequests[i].id = -1;
        sndEmitterRequests[i].emitter = NULL;
    }
}

// MATCHED 100%
void sndEmitStopId(s32 id) {
    for (s32 i = 15; i >= 0; i--) {
        SndEmitter * emitter = &sndEmitters[i];
        BOOL linked = (emitter->next != NULL || emitter->prev != NULL);
        if (linked && id == emitter->id) {
            emitter->stop(4);

            emitter->xSrc = NULL;
            emitter->ySrc = NULL;
        }
    }

    for (s32 i = 15; i >= 0; i--) {
        if (id == sndEmitterRequests[i].id) {
            sndEmitterRequests[i].id = -1;
            sndEmitterRequests[i].emitter = 0;
        }
    }
}

// MATCHED 100%
void sndEmitStopFaded(u32 slot, BOOL queuedOnly) {
    SndEmitterSlot * emitter = sndEmitters;

    for (s32 i = 15; i >= 0; i--, emitter++) {
        BOOL linked = (emitter->next != NULL || emitter->prev != NULL);

        if (linked && emitter->slot == slot && (!queuedOnly || (emitter->id & SND_ID_REQUEUE))) {
            u32 r1 = emitter->id & 0xc0ff0fff;

            if (r1 != 0x40000150 && r1 != 0x40000344 && r1 != 0x4000015e && r1 != 0x40000352 && r1 != 0x4000015f && r1 != 0x40000353) {
                emitter->stop(60);

                emitter->xSrc = NULL;
                emitter->ySrc = NULL;
            }
        }
    }
}

// MATCHED 100%
void sndEmitStopSlot(u32 slot) {
    SndEmitterSlot * emitter = sndEmitters;
    for (s32 i = 15; i >= 0; i--, emitter++) {
        BOOL linked = (emitter->next != NULL || emitter->prev != NULL);
        if (linked && emitter->slot == slot && (emitter->id & 0xc0ff0fff) != 0x40000150 && (emitter->id & 0xc0ff0fff) != 0x40000344) {
            emitter->stop(0x3c);

            emitter->xSrc = NULL;
            emitter->ySrc = NULL;
        }
    }

    for (s32 i = 15; i >= 0; i--) {
        if (slot == sndEmitterRequests[i].slot) {
            sndEmitterRequests[i].id = -1;
            sndEmitterRequests[i].emitter = 0;
        }
    }
}

// MATCHED 100%
void sndEmitClearQueue() {
    for (s32 i = 15; i >= 0; i--) {
        sndEmitterRequests[i].id = -1;
        sndEmitterRequests[i].emitter = 0;
    }
}

// MATCHED 100%
void sndEmitFlushQueue(u32 slot) {
    SndEmitterRequest * req = sndEmitterRequests;
    SndEmitterRequest copy;
    s32 i;
    SndEmitterRequest * copyPtr = &copy;

    for (i = 15; i >= 0; i--, req++) {
        if (req->id != -1 && (req->slot == slot || slot == 0xff)) {
            SndEmitterSlot * emitter = sndEmitters;
            s32 j = 15;
            while (j >= 0) {
                if (req->emitter == emitter) {
                    BOOL linked = (emitter->next != NULL || emitter->prev != NULL);
                    if (linked && req->id == emitter->id) {
                        break;
                    }
                }

                j--;
                emitter++;
            }

            if (j <= 0) {
                if (req < copyPtr) {
                    func_01ff8380(req, &copy, sizeof(copy));
                } else {
                    func_01ff82ac(req, &copy, sizeof(copy));
                }

                req->id = -1;
                req->emitter = NULL;

                if (req->isFixed == 0) {
                    sndEmitPlayTracked(copy.id, copy.xSrc, copy.ySrc, 0, copy.mode, copy.repeats, copy.slot, 0x3c);
                } else {
                    sndEmitPlay(copy.id, copy.x, copy.y, 0, copy.mode, copy.repeats, copy.slot, 0x3c);
                }
            }
        }
    }
}

// MATCHED 100%
void sndEmitReboot() {
    sndShutdown(TRUE);
    sndInit(FALSE, 0xa0000, 0x8ace0, 0x80);
    sndLoadGroup(0x47, 0xc00);
    sndPlaySeqNoStop(0x5f, 0, 0xc00);
}

// MATCHED 100%
SndEmitterSlot * sndEmitAlloc(s32 id) {
    s32 i;

    if (id & SND_ID_NO_DUP) {
        u8 idx = sndEmitterNext;
        for (i = 15; i >= 0; i--) {
            if (id == sndEmitters[++idx & 0xf].id) {
                return NULL;
            }
        }
    }

    for (i = 15; i >= 0; i--) {
        SndEmitterSlot * emitter = &sndEmitters[++sndEmitterNext & 0xf];
        BOOL linked = (emitter->next != NULL || emitter->prev != NULL);
        if (!linked) {
            return emitter;
        }
    }

    return NULL;
}

// MATCHED 100%
void SndEmitter::enqueue(s32 id, u32 x, u32 y, s16 mode, s16 repeats, s8 isFixed, u8 slot) {
    SndEmitterRequest * req = sndEmitterRequests;
    for (s32 i = 15; i >= 0; i--) {
        if (req->id == -1) {
            req->emitter = this;
            req->id = id;

            if (isFixed) {
                req->x = x;
                req->y = y;
            } else {
                req->xSrc = (s16 *)x;
                req->ySrc = (s16 *)y;
            }

            req->mode = mode;
            req->repeats = repeats;
            req->isFixed = isFixed;
            req->slot = slot;
            return;
        }

        req++;
    }
}

