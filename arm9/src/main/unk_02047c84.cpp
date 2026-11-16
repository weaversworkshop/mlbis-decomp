// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "main/unk_02047c84.h"

#include "main/sound.h"

// --- Struct Definitions --- //

extern "C" {

// --- Extern Functions --- //

extern void func_ov139_020647c0(void);

} // extern "C"

// --- Variable Definitions --- //

UnkStruct_02054bfc data_02054bfc = { sndTick, func_ov139_020647c0 };  // 0x02054bfc

typedef struct UnkStruct_0205afa4 {
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
} UnkStruct_0205afa4;

static UnkStruct_0205afa4 data_0205afa4;
