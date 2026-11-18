#ifndef MAIN_YANA_CTRL_H
#define MAIN_YANA_CTRL_H

#include "types.h"

#ifdef __cplusplus

#include "main/clYanaUnitSys.h"

extern "C" {
    #include <nitro.h>
}

class clYanaCtrlBase;

class inYanaUnit {
public:
    clYanaCtrlBase * next;  // +0x0
    clYanaCtrlBase * prev;  // +0x4

    void operator delete(void *);  // 0x020204a4
};

class clYanaCtrlSys {
public:
    clYanaCtrlBase * field00;
    clYanaUnitSys * field04;

    void func_02020608(void);  // 0x02020608
    void func_02020634(void);  // 0x02020634
};

class clYanaCtrlBase : public inYanaUnit {
public:
    clYanaCtrlBase() { this->next = NULL; this->prev = NULL; }
    virtual ~clYanaCtrlBase();  // D1 0x0202057c D0 0x020205a4 D2 0x020205e0
    virtual void update(void) = 0;

    void moveToList(clYanaCtrlSys *);  // 0x02020508
    void detach(void);  // 0x02020554
};

class clYanaCtrl: public clYanaCtrlBase {
public:
    void (*handler)(clYanaCtrl *);

    virtual ~clYanaCtrl() {};  // D1 0x020206cc D0 0x020206ac
    virtual void update(void);  // 0x0202066c
};

void * operator new(size_t, clYanaUnitSys *);  // 0x02020494

#endif
#endif
