// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clFieldBaseSystem.h"

#include "main/clGameMain.h"

// --- Function Definitions --- //

// MATCHED 100%
clFieldBaseSystem::clFieldBaseSystem() : clTask(8, 0, 0) {
    this->field5c = 0;

    this->field34 = 0;
    this->field38 = 0;

    this->field68 = 0;

    gameWork.field4cc_0 = 0;
}

// MATCHED 100%
clFieldBaseSystem::~clFieldBaseSystem() {}

// MATCHED 100%
void clFieldBaseSystem::run() {
    (this->*field28)();
}
