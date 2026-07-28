// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/clGlobalEvent.h"
#include "main/playerStatus.h"
#include "main/rng.h"

#include "structs/unk_020563e4.h"

#include <nitro.h>

// --- Struct Definitions --- //

typedef struct {
    u32 field00;
    u8 pad_04[0x18];
} UnkGlobal_02056504;

// --- Extern Data --- //

extern UnkGlobal_02056504 data_02056504;

// --- Extern Functions --- //

extern void func_01ff8228(u32 *, u32, u32);

extern s32 func_020058bc(u16, clGlobalEvent *, EventContext *);
extern void func_02005a78(u16, s32, clGlobalEvent *, EventContext *);
extern void func_02005da0(UnkGlobal_02056504 *, s32);
extern s64 func_0204536c(s32, s32);

// --- Variable Definitions --- //

const u8 argSizes[12] = { 1, 2, 4, 1, 2, 4, 2, 4, 4, 2, 0, 0 };  // 0x0204ac2c

// --- Function Declarations --- //

void fetchInstruction(clGlobalEvent *, EventContext *, EventInstruction *);  // 0x02015f0c
u32 rewindInstruction(clGlobalEvent *, EventContext *, EventInstruction *);  // 0x020160d4
u32 stepScript(clGlobalEvent *, EventContext *);  // 0x020161d4
u32 execInstruction(clGlobalEvent *, EventContext *, EventInstruction *);  // 0x02016254
bool evalCompare(s32, s32, s32);  // 0x02017634
void initContext(EventContext *, u8 *);  // 0x02017728

// --- Function Definitions --- //

// MATCHED 100%
clScriptSystem::clScriptSystem() {
    func_01ff8228(&this->vars[6], 0, 2 * sizeof(u32));
}

// MATCHED 100%
clScriptSystem::~clScriptSystem() {}

// MATCHED 100%
void clScriptSystem::setOpInfo(EventOpInfo * param) {
    this->opInfo = param;
}

// MATCHED 100%
void fetchInstruction(clGlobalEvent * sys, EventContext * ctx, EventInstruction * inst) {
    u8 * head;
    u32 * arg;
    s32 i;
    EventOpInfo * info;
    s32 argCount;
    u8 * p;
    u32 mask;
    

    head = ctx->pc;
    p = &ctx->pc[6];
    inst->opcode = head[0] | (head[1] << 8);

    info = &sys->opInfo[inst->opcode];

    mask = head[2] | (head[3] << 8) | (head[4] << 16) | (head[5] << 24);
    inst->varArgMask = mask;

    if (EV_HAS_DEST(info->argInfo)) {
        inst->destVar = p[0] | (p[1] << 8);
        p += 2;
    }

    argCount = EV_ARG_COUNT(info->argInfo);

    if (argCount != 0) {

        arg = (u32 *)inst->args;

        for (i = 0; i < argCount; i++) {

            if ((mask >> i & 1) != 0) {
                arg[0] = sys->getVar(ctx, p[0] | (p[1] << 8));
                p += 2;
            } else {
                switch ((info->argTypes[i/2] >> (i & 1) * 4) & 0xf) {
                    case 0:
                        arg[0] = p[0];
                        p++;
                        break;
                    case 3:
                        arg[0] = (s8)p[0];
                        p++;
                        break;
                    case 1:
                        arg[0] = (u16)(p[0] | (p[1] << 8));
                        p += 2;
                        break;
                    case 4:
                    case 6:
                        arg[0] = (s16)(p[0] | (p[1] << 8));
                        p += 2;
                        break;
                    case 2:
                        arg[0] = (p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
                        p += 4;
                        break;
                    case 5:
                    case 7:
                        arg[0] = (s32)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
                        p += 4;
                        break;
                    default:
                        break;
                }
            }

            arg++;
        }
    }

    ctx->pc = p;
}

// MATCHED 100%
u32 rewindInstruction(clGlobalEvent * sys, EventContext * ctx, EventInstruction * inst) {
    s32 idx;

    EventOpInfo * entry = &sys->opInfo[inst->opcode];
    u8 header = entry->argInfo;
    s32 count = EV_ARG_COUNT(header);

    u32 r0 = 6;

    if (EV_HAS_DEST(header)) {
        r0 += 2;
    }

    for (idx = 0; idx < count; idx++) {
        if (((inst->varArgMask >> idx) & 1) != 0) {
            r0 += 2;
        } else {
            r0 += argSizes[entry->argTypes[idx / 2] >> (idx & 1) * 4 & 0xf];
        }
    }

    ctx->pc -= r0;
    return r0;
}

// MATCHED 100%
clGlobalEvent::clGlobalEvent() {
    func_01ff8228(this->vars, 0, 32 * sizeof(u32));
}

// MATCHED 100%
clGlobalEvent::~clGlobalEvent() {}

// MATCHED 100%
u32 stepScript(clGlobalEvent * sys, EventContext * ctx) {
    u32 ret = 1;

    if (ctx->pc != NULL) {
        if (ctx->waitFrames != 0) {
            if (--ctx->waitFrames != 0) {
                return 3;
            }
        }

        EventInstruction r4;
        ret = 0;
        while (ret == 0) {
            fetchInstruction(sys, ctx, &r4);
            ret = execInstruction(sys, ctx, &r4);
        }
    }

    return ret;
}

// MATCHED 100%
u32 execInstruction(clGlobalEvent * sys, EventContext * ctx, EventInstruction * inst) {
    s32 val;

    switch (inst->opcode) {
        case END:
            ctx->pc = NULL;
            return 1;
        case RET:
            if (ctx->sp == 0) {
                ctx->pc = NULL;
                return 2;
            }
            ctx->sp--;
            ctx->pc = ctx->stack[ctx->sp];
            return 0;
        case JUMP:
            if (inst->args[0] == 1) {
                ctx->stack[ctx->sp] = ctx->pc;
                ctx->sp++;
            }
            ctx->pc += inst->args[1];
            return 0;
        case WAIT:
            ctx->waitFrames = inst->args[0];
            return 3;
        case BRANCH_IF:
            val = evalCompare(inst->args[0], inst->args[1], inst->args[2]);
            if (inst->args[3] == val) {
                ctx->pc += inst->args[4];
            }
            return 0;
        case NOOP59:
        case NOOP60:
        case NOOP61:
        case NOOP62:
        case NOOP63:
            return 0;
        case PUSH:
            ctx->stack[ctx->sp] = (u8 *)inst->args[0];
            ctx->sp++;
            return 0;
        case POP_TO_VAR:
            ctx->sp--;
            func_02005a78(inst->destVar, (s32)ctx->stack[ctx->sp], sys, ctx);
            return 0;
        case LOOP: {
            u32 sp = ctx->sp;
            u8 ** stackTop = ctx->stackTop;
            u32 mode = inst->args[0] & 0xf;

            if (mode == 3) {
                stackTop[sp]++;
            } else if (mode == 4) {
                stackTop[sp]--;
            }

            val = evalCompare(inst->args[1], inst->args[2], (s32)stackTop[sp]);

            if (val != 0) {
                ctx->pc += inst->args[3];
                if ((inst->args[0] & 0x30) == 0x10) {
                    ctx->sp--;
                    return 0;
                }
            } else if ((inst->args[0] & 0x30) == 0x20) {
                ctx->sp--;
                return 0;
            }

            if (mode == 1) {
                stackTop[sp]++;
            } else if (mode == 2) {
                stackTop[sp]--;
            }
            return 0;
        }
        case TABLE_GET: {
            u8 * rec = &ctx->pc[inst->args[0]];
            u16 header = rec[0] | (rec[1] << 8);
            s32 type = (header & 0xf00) >> 8;
            u8 * elem = (rec + 2) + argSizes[type] * (inst->args[1] & 0xff);

            switch (type) {
                case 0:
                    val = elem[0];
                    break;
                case 3:
                    val = (s8)elem[0];
                    break;
                case 1:
                    val = (u16)(elem[0] | (elem[1] << 8));
                    break;
                case 4:
                case 6:
                    val = (s16)(elem[0] | (elem[1] << 8));
                    break;
                case 2:
                    val = (elem[0] | (elem[1] << 8) | (elem[2] << 16) | (elem[3] << 24));
                    break;
                case 5:
                case 7:
                    val = (s32)(elem[0] | (elem[1] << 8) | (elem[2] << 16) | (elem[3] << 24));
                    break;
                default:
                    val = 0;
                    break;
            }

            func_02005a78(inst->destVar, val, sys, ctx);
            return 0;
        }
        case BYTE_GET: {
            u8 * ptr = &ctx->pc[inst->args[0]];
            func_02005a78(inst->destVar, (u16)(*ptr | (*(ptr+1) << 8)) & 0xff, sys, ctx);
            return 0;
        }
        case MOV:
            func_02005a78(inst->destVar, inst->args[0], sys, ctx);
            return 0;
        case ADD:
            func_02005a78(inst->destVar, inst->args[0] + inst->args[1], sys, ctx);
            return 0;
        case SUB:
            func_02005a78(inst->destVar, inst->args[0] - inst->args[1], sys, ctx);
            return 0;
        case MUL:
            func_02005a78(inst->destVar, inst->args[0] * inst->args[1], sys, ctx);
            return 0;
        case DIV: {
            s64 divmod_val = func_0204536c(inst->args[0], inst->args[1]);
            func_02005a78(inst->destVar, (s32)divmod_val, sys, ctx);
            return 0;
        }
        case MOD: {
            s64 divmod_val = func_0204536c(inst->args[0], inst->args[1]);
            func_02005a78(inst->destVar, (s32)(divmod_val >> 32), sys, ctx);
            return 0;
        }
        case SHL:
            func_02005a78(inst->destVar, inst->args[0] << inst->args[1], sys, ctx);
            return 0;
        case SHR:
            func_02005a78(inst->destVar, inst->args[0] >> inst->args[1], sys, ctx);
            return 0;
        case AND:
            func_02005a78(inst->destVar, inst->args[0] & inst->args[1], sys, ctx);
            return 0;
        case OR:
            func_02005a78(inst->destVar, inst->args[0] | inst->args[1], sys, ctx);
            return 0;
        case XOR:
            func_02005a78(inst->destVar, inst->args[0] ^ inst->args[1], sys, ctx);
            return 0;
        case NEG:
            func_02005a78(inst->destVar, -inst->args[0], sys, ctx);
            return 0;
        case LNOT:
            func_02005a78(inst->destVar, (inst->args[0] == 0), sys, ctx);
            return 0;
        case BNOT:
            func_02005a78(inst->destVar, ~inst->args[0], sys, ctx);
            return 0;
        case INC:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, val+1, sys, ctx);
            return 0;
        case DEC:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, val-1, sys, ctx);
            return 0;
        case ADD_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, inst->args[0]+val, sys, ctx);
            return 0;
        case SUB_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, val-inst->args[0], sys, ctx);
            return 0;
        case MUL_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, inst->args[0]*val, sys, ctx);
            return 0;
        case DIV_EQ: {
            val = func_020058bc(inst->destVar, sys, ctx);
            s64 divmod_val = func_0204536c(val, inst->args[0]);
            func_02005a78(inst->destVar, (s32)divmod_val, sys, ctx);
            return 0;
        }
        case MOD_EQ: {
            val = func_020058bc(inst->destVar, sys, ctx);
            s64 divmod_val = func_0204536c(val, inst->args[0]);
            func_02005a78(inst->destVar, (s32)(divmod_val >> 32), sys, ctx);
            return 0;
        }
        case SHL_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, val << inst->args[0], sys, ctx);
            return 0;
        case SHR_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, val >> inst->args[0], sys, ctx);
            return 0;
        case AND_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, inst->args[0] & val, sys, ctx);
            return 0;
        case OR_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, inst->args[0] | val, sys, ctx);
            return 0;
        case XOR_EQ:
            val = func_020058bc(inst->destVar, sys, ctx);
            func_02005a78(inst->destVar, inst->args[0] ^ val, sys, ctx);
            return 0;
        case SQRT:
            val = FX_Sqrt(inst->args[0] << FX32_SHIFT);
            func_02005a78(inst->destVar, val / FX32_ONE, sys, ctx);
            return 0;
        case INVSQRT:
            val = FX_InvSqrt(inst->args[0] << FX32_SHIFT);
            func_02005a78(inst->destVar, val / FX32_ONE, sys, ctx);
            return 0;
        case INV:
            val = FX_Inv(inst->args[0] << FX32_SHIFT);
            func_02005a78(inst->destVar, val / FX32_ONE, sys, ctx);
            return 0;
        case SIN:
            func_02005a78(inst->destVar, FX_SinIdx((u16)((inst->args[0] << 16) / 360)), sys, ctx);
            return 0;
        case COS:
            func_02005a78(inst->destVar, FX_CosIdx((u16)((inst->args[0] << 16) / 360)), sys, ctx);
            return 0;
        case ATAN:
            func_02005a78(inst->destVar, FX_AtanIdx(inst->args[0]) * 360 / (1 << 16), sys, ctx);
            return 0;
        case ATAN2:
            func_02005a78(inst->destVar, FX_Atan2Idx(inst->args[0], inst->args[1]) * 360 / (1 << 16), sys, ctx);
            return 0;
        case RAND:
            val = rand_range(inst->args[0]);
            func_02005a78(inst->destVar, val, sys, ctx);
            return 0;
        case MOV_FX:
            func_02005a78(inst->destVar, inst->args[0], sys, ctx);
            return 0;
        case FX_TRUNC:
            func_02005a78(inst->destVar, FX_Whole(inst->args[0]), sys, ctx);
            return 0;
        case FX_FLOOR:
            func_02005a78(inst->destVar, FX_Floor(inst->args[0]), sys, ctx);
            return 0;
        case FX_SQRT:
            func_02005a78(inst->destVar, FX_Sqrt(inst->args[0]), sys, ctx);
            return 0;
        case FX_INVSQRT:
            func_02005a78(inst->destVar, FX_InvSqrt(inst->args[0]), sys, ctx);
            return 0;
        case FX_INV:
            func_02005a78(inst->destVar, FX_Inv(inst->args[0]), sys, ctx);
            return 0;
        case FX_SIN:
            func_02005a78(inst->destVar, FX_SinIdx((u16)((inst->args[0] << 4) / 360)), sys, ctx);
            return 0;
        case FX_COS:
            func_02005a78(inst->destVar, FX_CosIdx((u16)((inst->args[0] << 4) / 360)), sys, ctx);
            return 0;
        case FX_ATAN: 
            func_02005a78(inst->destVar, FX_AtanIdx(inst->args[0]) * 360 / (1 << 4), sys, ctx);
            return 0;
        case FX_ADD:
            func_02005a78(inst->destVar, inst->args[0] + inst->args[1], sys, ctx);
            return 0;
        case FX_SUB:
            func_02005a78(inst->destVar, inst->args[0] - inst->args[1], sys, ctx);
            return 0;
        case FX_MUL:
            func_02005a78(inst->destVar, FX_MUL(inst->args[0], inst->args[1]), sys, ctx);
            return 0;
        case FX_DIV:
            func_02005a78(inst->destVar, FX_Div(inst->args[0], inst->args[1]), sys, ctx);
            return 0;
        case FX_MOD:
            func_02005a78(inst->destVar, FX_ModS32(inst->args[0], inst->args[1]), sys, ctx);
            return 0;
        case FX_ATAN2: 
            func_02005a78(inst->destVar, FX_Atan2Idx(inst->args[0], inst->args[1]) * 360 / (1 << 4), sys, ctx);
            return 0;
        case OP64:
            func_02005da0(&data_02056504, -1);
            data_02056504.field00 |= 2;
            return 0;
        case OP65:
            val = addCoins(inst->args[0]);
            func_02005a78(inst->destVar, val, sys, ctx);
            return 0;
        case ADD_COINS: {
            u16 old = data_020563e4.field20;
            u32 sum = old + inst->args[0];

            if (sum > 999) {
                data_020563e4.field20 = 999;
            } else {
                data_020563e4.field20 = sum;
            }

            func_02005a78(inst->destVar, data_020563e4.field20 - old, sys, ctx);
            return 0;
        }
        case GET_FLAG:
            if ((inst->args[0] & 0x3000) == 0x3000) {
                val = (data_02055fe4.field4c3 >> (inst->args[0] & 0xfff)) & 1;
            } else {
                val = getItemCount(inst->args[0]);
            }

            func_02005a78(inst->destVar, val, sys, ctx);
            return 0;
        case OP68:
            val = addItem(inst->args[0], inst->args[1]);
            func_02005a78(inst->destVar, val, sys, ctx);
            return 0;
        case ACTOR_GET: {
            PlayerStatus *player = &gParty.party[inst->args[0]];
            ExpCounter local;

            switch (inst->args[1]) {
                case 0:
                    val = player->stats[0];
                    break;
                case 1:
                    val = player->stats[1];
                    break;
                case 2:
                    val = player->stats[2];
                    break;
                case 3:
                    val = player->stats[3];
                    break;
                case 4:
                    val = player->stats[4];
                    break;
                case 5:
                    val = player->stats[5];
                    break;
                case 6:
                    val = player->stats[6];
                    break;
                case 7:
                    val = player->stats[7];
                    break;
                case 8:
                    val = player->stats[8];
                    break;
                case 9:
                    val = player->stats[9];
                    break;
                case 10:
                    val = player->stats[10];
                    break;
                case 11:
                    val = player->stats[11];
                    break;
                case 12:
                    val = player->stats[12];
                    break;
                case 13:
                    val = player->stats[13];
                    break;
                case 14:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetRank(&local);
                    break;
                case 15:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetLevel(&local);
                    break;
                case 16:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetLevelIdx(&local);
                    break;
                case 17:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetIntoLevel(&local);
                    break;
                case 18:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetExp(&local);
                    break;
                case 19:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetLevelCost(&local);
                    break;
                case 20:
                    expInit(&local, inst->args[0], player->exp);
                    val = expGetNext(&local);
                    break;
                case 21:
                    val = player->gearSlots;
                    break;
                case 22:
                    val = player->gear[0] | ID_GEAR;
                    break;
                case 23:
                    val = player->gear[1] | ID_GEAR;
                    break;
                case 24:
                    val = player->gear[2] | ID_GEAR;
                    break;
                case 25:
                    val = player->badge | ID_BADGE;
                    break;
            }

            func_02005a78(inst->destVar, val, sys, ctx);
            return 0;
        }
        case ACTOR_SET: {
            s32 id = inst->args[0];
            s32 val = inst->args[2];
            PlayerStatus *player = &gParty.party[id];
            ExpCounter local;

            switch (inst->args[1]) {
                case 0:
                    player->stats[0] = val;
                    refreshStats(id);
                    break;
                case 1:
                    player->stats[1] = val;
                    refreshStats(id);
                    break;
                case 2:
                    player->stats[2] = val;
                    refreshStats(id);
                    break;
                case 3:
                    player->stats[3] = val;
                    refreshStats(id);
                    break;
                case 4:
                    player->stats[4] = val;
                    refreshStats(id);
                    break;
                case 5:
                    player->stats[5] = val;
                    refreshStats(id);
                    break;
                case 7:
                    player->stats[7] = val;
                    refreshStats(id);
                    break;
                case 9:
                    player->stats[9] = val;
                    refreshStats(id);
                    break;
                case 17: {
                    player->exp = val;
                    expInit(&local, id, player->exp);

                    player->level = expGetLevel(&local);
                    refreshStats(id);
                    break;
                }
                case 21:
                    player->gearSlots = val;
                    break;
                case 22:
                    equipGear(id, val, 0);
                    break;
                case 23:
                    equipGear(id, val, 1);
                    break;
                case 24:
                    equipGear(id, val, 2);
                    break;
                case 25:
                    equipBadge(id, val);
                    break;
            }
            return 0;
        }
        default:
            break;

    }

    return sys->execCustom(ctx, inst);
}

// MATCHED 100%
u32 clGlobalEvent::execCustom(EventContext * param1, EventInstruction * param2) {
    return 1;
}

// MATCHED 100%
s32 clGlobalEvent::getCustomVariable(EventContext * ctx, u32 id) {
    return 0;
}

// MATCHED 100%
void clGlobalEvent::setCustomVariable(EventContext * ctx, u32 id, s32 value) {}

// MATCHED 100%
s32 clGlobalEvent::getVar(EventContext * param2, u32 param3) {
    return func_020058bc(param3, this, param2);
}

// MATCHED 100%
bool evalCompare(s32 op, s32 lhs, s32 rhs) {
    switch (op) {
        case 0:
            return (lhs == rhs);
        case 1:
            return (lhs != rhs);
        case 2:
            return (lhs < rhs);
        case 3:
            return (lhs > rhs);
        case 4:
            return (lhs <= rhs);
        case 5:
            return (lhs >= rhs);
        case 6:
            return ((lhs & rhs) != 0);
        case 7:
            return ((lhs | rhs) != 0);
        case 8:
            return ((lhs ^ rhs) != 0);
        case 9:
            return (lhs == 0);
        case 10:
            return (lhs != -1);
    }

    return 0;
}

// MATCHED 100%
void initContext(EventContext * ctx, u8 * code) {
    ctx->pc = code;

    func_01ff8228(ctx->locals, 0, sizeof(ctx->locals));

    ctx->flags = 0;
    ctx->sp = 0;
    ctx->waitFrames = 0;
}

} // extern "C"
