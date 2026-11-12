// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

// --- Struct Definitions --- //

struct Struct02054bfc {
    void (*field00)(void);
    void (*field04)(void);
    u32 field08[3];
};

extern "C" {

// --- Extern Functions --- //

extern void func_01ff8d18(void);
extern void func_ov139_020647c0(void);

} // extern "C"

// --- Variable Definitions --- //

Struct02054bfc data_02054bfc = { func_01ff8d18, func_ov139_020647c0 };

struct UnkStruct_0205afa4 {
    UnkStruct_0205afa4() {
        #ifndef IDE_PARSE
        asm {
            ldr lr, =data_02054bfc
            ldr r0, [lr]
            ldmia r0, {r0, r1, r2, r3, r4}
            stmia lr, {r0, r1, r2, r3, r4}
        }
        #endif
    }
};

static UnkStruct_0205afa4 data_0205afa4;
