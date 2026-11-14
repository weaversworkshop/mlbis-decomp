// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/sndEmitter.h"

#include "main/sound.h"

extern "C" {

// --- Struct Definitions --- //

typedef struct {
    SndEmitterCmd ** cmdTable;  // +0x0
    SndEmitter * head;  // +0x4
} SndEmitterList;

// --- Extern Data Variables --- //

extern SndEmitterCmd * data_ov000_02069468;

extern SndEmitterCmd * data_ov011_02070018;

// --- Variable Definitions --- //

u8 sndFadeTarget = 0xff;  // 0x020500ac

SndEmitterList sndEmitterList;  // 0x02058920

// --- Function Declarations --- //

void sndEmitSetCmdTable(void);  // 0x0200a40c
void sndEmitUpdate(void);  // 0x0200a61c
void sndEmitTickScript(SndEmitter *);  // 0x0200a814
void sndEmitArmEnd(SndEmitter *);  // 0x0200a98c
void sndEmitLoopEnd(SndEmitter *);  // 0x0200aa20
void sndEmitArmTimed(SndEmitter *);  // 0x0200aad8
void sndEmitLoopTimed(SndEmitter *);  // 0x0200ab6c
void sndEmitCrossFade(s32);  // 0x0200acc8
void sndEmitCrossFadeBack(s32);  // 0x0200ad68

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
