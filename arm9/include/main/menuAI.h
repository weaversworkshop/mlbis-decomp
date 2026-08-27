#ifndef MAIN_MENU_AI_H
#define MAIN_MENU_AI_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaAnim.h"

namespace nsMenuAI {

class clBtlMoveBase : public clYanaAnim {
public:
    virtual ~clBtlMoveBase() {}  // D1 0x020199b8 D0 0x020199cc

    virtual void fn_10(void);  // 0x020199b0
    virtual void onEnd(void);  // ov122 0x0206db30  // ??
    virtual void start(u16 length);  // ov122 0x0206d9c4  // ??
    virtual void stop(void);  // ov122 0x0206d9b4  // ??
};

class clBtlMove : public clBtlMoveBase {
public:
    virtual ~clBtlMove() {}  // D1 0x020199e8 D0 0x020199fc
    
    virtual void fn_10(void);  // 0x020199b4
};

}

#endif
#endif
