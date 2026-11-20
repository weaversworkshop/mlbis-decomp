// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clGlobalEvent.h"

#include "main/clGameMain.h"
#include "main/softReset.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"
#include "main/rng.h"

// --- Variable Definitions --- //

const u8 argTypeSizeTbl[12] = { 1, 2, 4, 1, 2, 4, 2, 4, 4, 2, 0, 0 };  // 0x0204ac2c

// --- Function Declarations --- //

bool cmp(s32, s32, s32);  // 0x02017634
void initContext(clLocalEvent *, u8 *);  // 0x02017728

// --- Function Definitions --- //

// MATCHED 100%
clScriptSystem::clScriptSystem() {
    cpu_fill32(&this->vars[6], 0, 2 * sizeof(u32));
}

// MATCHED 100%
clScriptSystem::~clScriptSystem() {}

// MATCHED 100%
void clScriptSystem::setOpInfo(EventOpInfo * param) {
    this->opInfo = param;
}

// MATCHED 100%
void clGlobalEvent::fetchInstruction(clLocalEvent * ctx, ScriptCode * inst) {
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

    info = &this->opInfo[inst->opcode];

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
                arg[0] = this->getRegister(ctx, p[0] | (p[1] << 8));
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
u32 clGlobalEvent::rewind(clLocalEvent * ctx, ScriptCode * inst) {
    s32 idx;

    EventOpInfo * entry = &this->opInfo[inst->opcode];
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
            r0 += argTypeSizeTbl[entry->argTypes[idx / 2] >> (idx & 1) * 4 & 0xf];
        }
    }

    ctx->pc -= r0;
    return r0;
}

// MATCHED 100%
clGlobalEvent::clGlobalEvent() {
    cpu_fill32(this->vars, 0, sizeof(this->vars));
}

// MATCHED 100%
clGlobalEvent::~clGlobalEvent() {}

// MATCHED 100%
u32 clGlobalEvent::execLocalEvent(clLocalEvent * ctx) {
    u32 ret = 1;

    if (ctx->pc != NULL) {
        if (ctx->waitFrames != 0) {
            if (--ctx->waitFrames != 0) {
                return 3;
            }
        }

        ScriptCode r4;
        ret = 0;
        while (ret == 0) {
            this->fetchInstruction(ctx, &r4);
            ret = this->execInstruction(ctx, &r4);
        }
    }

    return ret;
}

// MATCHED 100%
u32 clGlobalEvent::execInstruction(clLocalEvent * ctx, ScriptCode * inst) {
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
            ctx->pc = (u8 *)ctx->stack[ctx->sp];
            return 0;
        case JUMP:
            if (inst->args[0] == 1) {
                ctx->stack[ctx->sp] = (s32)ctx->pc;
                ctx->sp++;
            }
            ctx->pc += inst->args[1];
            return 0;
        case WAIT:
            ctx->waitFrames = inst->args[0];
            return 3;
        case BRANCH_IF:
            val = cmp(inst->args[0], inst->args[1], inst->args[2]);
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
            ctx->stack[ctx->sp] = inst->args[0];
            ctx->sp++;
            return 0;
        case POP_TO_VAR:
            ctx->sp--;
            putRegFlag(inst->destVar, ctx->stack[ctx->sp], this, ctx);
            return 0;
        case LOOP: {
            u32 sp = ctx->sp;
            s32 * stackTop = &ctx->stack[-1];
            u32 mode = inst->args[0] & 0xf;

            if (mode == 3) {
                stackTop[sp]++;
            } else if (mode == 4) {
                stackTop[sp]--;
            }

            val = cmp(inst->args[1], inst->args[2], stackTop[sp]);

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
            u8 * elem = (rec + 2) + argTypeSizeTbl[type] * (inst->args[1] & 0xff);

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

            putRegFlag(inst->destVar, val, this, ctx);
            return 0;
        }
        case BYTE_GET: {
            u8 * ptr = &ctx->pc[inst->args[0]];
            putRegFlag(inst->destVar, (u16)(*ptr | (*(ptr+1) << 8)) & 0xff, this, ctx);
            return 0;
        }
        case MOV:
            putRegFlag(inst->destVar, inst->args[0], this, ctx);
            return 0;
        case ADD:
            putRegFlag(inst->destVar, inst->args[0] + inst->args[1], this, ctx);
            return 0;
        case SUB:
            putRegFlag(inst->destVar, inst->args[0] - inst->args[1], this, ctx);
            return 0;
        case MUL:
            putRegFlag(inst->destVar, inst->args[0] * inst->args[1], this, ctx);
            return 0;
        case DIV:
            putRegFlag(inst->destVar, inst->args[0] / inst->args[1], this, ctx);
            return 0;
        case MOD:
            putRegFlag(inst->destVar, inst->args[0] % inst->args[1], this, ctx);
            return 0;
        case SHL:
            putRegFlag(inst->destVar, inst->args[0] << inst->args[1], this, ctx);
            return 0;
        case SHR:
            putRegFlag(inst->destVar, inst->args[0] >> inst->args[1], this, ctx);
            return 0;
        case AND:
            putRegFlag(inst->destVar, inst->args[0] & inst->args[1], this, ctx);
            return 0;
        case OR:
            putRegFlag(inst->destVar, inst->args[0] | inst->args[1], this, ctx);
            return 0;
        case XOR:
            putRegFlag(inst->destVar, inst->args[0] ^ inst->args[1], this, ctx);
            return 0;
        case NEG:
            putRegFlag(inst->destVar, -inst->args[0], this, ctx);
            return 0;
        case LNOT:
            putRegFlag(inst->destVar, (inst->args[0] == 0), this, ctx);
            return 0;
        case BNOT:
            putRegFlag(inst->destVar, ~inst->args[0], this, ctx);
            return 0;
        case INC:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val+1, this, ctx);
            return 0;
        case DEC:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val-1, this, ctx);
            return 0;
        case ADD_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, inst->args[0]+val, this, ctx);
            return 0;
        case SUB_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val-inst->args[0], this, ctx);
            return 0;
        case MUL_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, inst->args[0]*val, this, ctx);
            return 0;
        case DIV_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val / inst->args[0], this, ctx);
            return 0;
        case MOD_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val % inst->args[0], this, ctx);
            return 0;
        case SHL_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val << inst->args[0], this, ctx);
            return 0;
        case SHR_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, val >> inst->args[0], this, ctx);
            return 0;
        case AND_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, inst->args[0] & val, this, ctx);
            return 0;
        case OR_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, inst->args[0] | val, this, ctx);
            return 0;
        case XOR_EQ:
            val = getRegFlag(inst->destVar, this, ctx);
            putRegFlag(inst->destVar, inst->args[0] ^ val, this, ctx);
            return 0;
        case SQRT:
            val = FX_Sqrt(inst->args[0] << FX32_SHIFT);
            putRegFlag(inst->destVar, val / FX32_ONE, this, ctx);
            return 0;
        case INVSQRT:
            val = FX_InvSqrt(inst->args[0] << FX32_SHIFT);
            putRegFlag(inst->destVar, val / FX32_ONE, this, ctx);
            return 0;
        case INV:
            val = FX_Inv(inst->args[0] << FX32_SHIFT);
            putRegFlag(inst->destVar, val / FX32_ONE, this, ctx);
            return 0;
        case SIN:
            putRegFlag(inst->destVar, FX_SinIdx((u16)((inst->args[0] << 16) / 360)), this, ctx);
            return 0;
        case COS:
            putRegFlag(inst->destVar, FX_CosIdx((u16)((inst->args[0] << 16) / 360)), this, ctx);
            return 0;
        case ATAN:
            putRegFlag(inst->destVar, FX_AtanIdx(inst->args[0]) * 360 / (1 << 16), this, ctx);
            return 0;
        case ATAN2:
            putRegFlag(inst->destVar, FX_Atan2Idx(inst->args[0], inst->args[1]) * 360 / (1 << 16), this, ctx);
            return 0;
        case RAND:
            val = rand_range(inst->args[0]);
            putRegFlag(inst->destVar, val, this, ctx);
            return 0;
        case MOV_FX:
            putRegFlag(inst->destVar, inst->args[0], this, ctx);
            return 0;
        case FX_TRUNC:
            putRegFlag(inst->destVar, FX_Whole(inst->args[0]), this, ctx);
            return 0;
        case FX_FLOOR:
            putRegFlag(inst->destVar, FX_Floor(inst->args[0]), this, ctx);
            return 0;
        case FX_SQRT:
            putRegFlag(inst->destVar, FX_Sqrt(inst->args[0]), this, ctx);
            return 0;
        case FX_INVSQRT:
            putRegFlag(inst->destVar, FX_InvSqrt(inst->args[0]), this, ctx);
            return 0;
        case FX_INV:
            putRegFlag(inst->destVar, FX_Inv(inst->args[0]), this, ctx);
            return 0;
        case FX_SIN:
            putRegFlag(inst->destVar, FX_SinIdx((u16)((inst->args[0] << 4) / 360)), this, ctx);
            return 0;
        case FX_COS:
            putRegFlag(inst->destVar, FX_CosIdx((u16)((inst->args[0] << 4) / 360)), this, ctx);
            return 0;
        case FX_ATAN: 
            putRegFlag(inst->destVar, FX_AtanIdx(inst->args[0]) * 360 / (1 << 4), this, ctx);
            return 0;
        case FX_ADD:
            putRegFlag(inst->destVar, inst->args[0] + inst->args[1], this, ctx);
            return 0;
        case FX_SUB:
            putRegFlag(inst->destVar, inst->args[0] - inst->args[1], this, ctx);
            return 0;
        case FX_MUL:
            putRegFlag(inst->destVar, FX_MUL(inst->args[0], inst->args[1]), this, ctx);
            return 0;
        case FX_DIV:
            putRegFlag(inst->destVar, FX_Div(inst->args[0], inst->args[1]), this, ctx);
            return 0;
        case FX_MOD:
            putRegFlag(inst->destVar, FX_ModS32(inst->args[0], inst->args[1]), this, ctx);
            return 0;
        case FX_ATAN2: 
            putRegFlag(inst->destVar, FX_Atan2Idx(inst->args[0], inst->args[1]) * 360 / (1 << 4), this, ctx);
            return 0;
        case OP64:
            setSoftResetFade(&softResetState, -1);
            softResetState.pending = 1;
            return 0;
        case OP65:
            val = addCoins(inst->args[0]);
            putRegFlag(inst->destVar, val, this, ctx);
            return 0;
        case ADD_COINS: {
            u16 old = gameWork.party.field94;
            u32 sum = old + inst->args[0];

            if (sum > 999) {
                gameWork.party.field94 = 999;
            } else {
                gameWork.party.field94 = sum;
            }

            putRegFlag(inst->destVar, gameWork.party.field94 - old, this, ctx);
            return 0;
        }
        case GET_FLAG:
            if ((inst->args[0] & 0x3000) == 0x3000) {
                val = (gameWork.party.badgeOwned >> (inst->args[0] & 0xfff)) & 1;
            } else {
                val = getItemNum(inst->args[0]);
            }

            putRegFlag(inst->destVar, val, this, ctx);
            return 0;
        case OP68:
            val = addItem(inst->args[0], inst->args[1]);
            putRegFlag(inst->destVar, val, this, ctx);
            return 0;
        case ACTOR_GET: {
            PlayerStatus *player = &gameWork.party.members[inst->args[0]];
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

            putRegFlag(inst->destVar, val, this, ctx);
            return 0;
        }
        case ACTOR_SET: {
            s32 id = inst->args[0];
            s32 val = inst->args[2];
            PlayerStatus *player = &gameWork.party.members[id];
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

    return this->execEx(ctx, inst);
}

// MATCHED 100%
u32 clGlobalEvent::execEx(clLocalEvent * param1, ScriptCode * param2) {
    return 1;
}

// MATCHED 100%
s32 clGlobalEvent::getStatusReg(clLocalEvent * ctx, u32 id) {
    return 0;
}

// MATCHED 100%
void clGlobalEvent::setStatusReg(clLocalEvent * ctx, u32 id, s32 value) {}

// MATCHED 100%
s32 clGlobalEvent::getRegister(clLocalEvent * param2, u32 param3) {
    return getRegFlag(param3, this, param2);
}

// MATCHED 100%
bool cmp(s32 op, s32 lhs, s32 rhs) {
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
void initContext(clLocalEvent * ctx, u8 * code) {
    ctx->pc = code;

    cpu_fill32(ctx->workRegs, 0, sizeof(ctx->workRegs));

    ctx->workBitRegs[0] = 0;
    ctx->sp = 0;
    ctx->waitFrames = 0;
}

} // extern "C"
