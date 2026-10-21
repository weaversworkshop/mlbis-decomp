#ifndef MAIN_CL_GLOBAL_EVENT_H
#define MAIN_CL_GLOBAL_EVENT_H

#include "types.h"

#ifdef __cplusplus

typedef struct {
    u8 argInfo;  // +0x0
    u8 argTypes[0xf];  // +0x1
} EventOpInfo;

#define EV_ARG_COUNT(x) ((x) & 0x7f)
#define EV_HAS_DEST(x)  ((x) & 0x80)

typedef struct EventContext {
    u8 * pc;  // +0x0
    u32 args[8];  // +0x4
    u32 locals[16];  // +0x24
    union {
        u8 * stackTop[17];  // +0x64
        u32 words[17];  // +0x64
        struct {
            u32  flags;  // +0x64
            u8 * stack[16];  // +0x68
        };
    };
    u16 sp;  // +0xa8
    u16 waitFrames;  // +0xaa
} EventContext;

enum Opcode {
    END, RET,
    BRANCH_IF, JUMP,
    WAIT,
    PUSH, POP_TO_VAR,
    LOOP,
    MOV,
    ADD, SUB, MUL, DIV, MOD,
    SHL, SHR,
    AND, OR, XOR, NEG,
    LNOT, BNOT,
    INC, DEC,
    ADD_EQ, SUB_EQ, MUL_EQ, DIV_EQ, MOD_EQ,
    SHL_EQ, SHR_EQ,
    AND_EQ, OR_EQ, XOR_EQ,
    SQRT, INVSQRT, INV,
    SIN, COS, ATAN, ATAN2,
    RAND,
    MOV_FX,
    FX_ADD, FX_SUB, FX_MUL, FX_DIV, FX_MOD,
    FX_TRUNC, FX_FLOOR,
    FX_SQRT, FX_INVSQRT, FX_INV,
    FX_SIN, FX_COS, FX_ATAN, FX_ATAN2,
    TABLE_GET, BYTE_GET,
    NOOP59, NOOP60, NOOP61, NOOP62, NOOP63, // op 59-63 are noops??
    OP64, OP65, // unknown opcode
    ADD_COINS,
    GET_FLAG,
    OP68,   // unknown opcode
    ACTOR_GET, ACTOR_SET,
};

typedef struct EventInstruction {
    u16 opcode;  // +0x0
    u16 destVar;  // +0x2
    u32 varArgMask;  // +0x4
    s32 args[24];  // +0x8
} EventInstruction;

class clScriptSystem {
public:
    EventOpInfo * opInfo;  // +0x4
    u32 vars[32];  // +0x8

    clScriptSystem();  // C2 0x02015ebc
    virtual ~clScriptSystem();  // D1 0x02015ee8 D0 0x02015eec D2 0x02015f00

    virtual void setOpInfo(EventOpInfo *);  // 0x02015f04
};

class clGlobalEvent : public clScriptSystem {
public:    
    clGlobalEvent();  // C1 0x02016160
    virtual ~clGlobalEvent();  // D1 0x02016190 D0 0x020161a4 D2 0x020161c0
    
    virtual s32  getVar(EventContext *, u32);  // 0x02017614
    virtual u32  execCustom(EventContext *, EventInstruction *);  // 0x02017600
    virtual s32  getCustomVariable(EventContext *, u32);  // 0x02017608
    virtual void setCustomVariable(EventContext *, u32, s32);  // 0x02017610
};

#endif // __cplusplus

#endif
