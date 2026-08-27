#ifndef MAIN_YANA_CTRL_H
#define MAIN_YANA_CTRL_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaUnitSys.h"

class inYanaUnit {
public:
    inYanaUnit * next;  // +0x4
    inYanaUnit * prev;  // +0x8

    virtual ~inYanaUnit() {}
    virtual void func_0202066c(void) = 0;

    void operator delete(void *);  // 0x020204a4
    void moveToList(inYanaUnit **);  // 0x02020508
    void detach(void);  // 0x02020554
};

class clYanaCtrlBase : public inYanaUnit {
public:

    virtual ~clYanaCtrlBase();  // D1 0x0202057c D0 0x020205a4 D2 0x020205e0
};

class clYanaCtrl: public clYanaCtrlBase {
public:
    void (*handler)(clYanaCtrl *);

    virtual ~clYanaCtrl() {};  // D1 0x020206cc D0 0x020206ac
    virtual void func_0202066c(void);  // 0x0202066c
};

void * operator new(size_t, clYanaUnitSys *);  // 0x02020494

#endif
#endif
