#ifndef MAIN_MENU_AI_H
#define MAIN_MENU_AI_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaAnim.h"

namespace nsMenuAI {

class clBtlMoveBase : public clYanaAnim {
public:
    virtual ~clBtlMoveBase() {}

    virtual void fn_10();  // 0x020199b0
    
    virtual void m08();
    
    virtual void fn_14();
    virtual void fn_18();
};

class clBtlMove : public clBtlMoveBase {
public:
    virtual ~clBtlMove() {}
    virtual void fn_10();  // 0x020199b4
};

}

#endif
#endif
