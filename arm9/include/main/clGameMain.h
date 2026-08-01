#ifndef MAIN_CL_GAME_MAIN_H
#define MAIN_CL_GAME_MAIN_H

#include "types.h"

#ifdef __cplusplus

#include "main/task.h"

class clGameMain : public clTask {
public:
    u8 pad_18[0x10];
    u16 mode;
    u16 prevMode;

    clGameMain();
    virtual ~clGameMain() {};  // D1 0x020057cc D0 0x020057e0
    virtual void run();  // 0x02004ce8

    void setMode(u16);  // 0x020057ac
};

extern clGameMain * gGameMain;

#endif // __cplusplus

// #ifdef __cplusplus
// extern "C" {
// #endif

// #ifdef __cplusplus
// } // extern "C"
// #endif

#endif
