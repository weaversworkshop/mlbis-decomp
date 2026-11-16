#ifndef ITCM_MAIN_LOOP_H
#define ITCM_MAIN_LOOP_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void vblankIntr(void);  // 0x01ff85a4
void vcountIntr(void);  // 0x01ff8a38
void bootGame(void);  // 0x01ff8b50
void enterSleep(void);  // 0x01ff8c70

#ifdef __cplusplus
} // extern "C"
#endif

#endif
