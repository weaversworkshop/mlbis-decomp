// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/sndEmitter.h"

#include "main/sound.h"

extern "C" {

// --- Struct Definitions --- //

typedef struct {
    s32 id;  // +0x0
    SndEmitter * emitter;  // +0x4
    union { s16 * xSrc; s32 x; };
    union { s16 * ySrc; s32 y; };
    s16 mode;  // +0x10
    u16 repeats : 15;  // +0x12
    u16 isFixed : 1;
    u8 slot;  // +0x14
} SndEmitterRequest;

// --- Extern Functions --- //

extern void func_01ff82ac(void *, void *, u32);
extern void func_01ff8380(void *, void *, u32);

// --- Variable Definitions --- //

u8 sndEmitterNext;  // 0x02058928
SndEmitterRequest sndEmitterRequests[16];  // 0x0205892c

SndEmitterSlot sndEmitters[16];  // 0x02058aac

// --- Function Declarations --- //

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

