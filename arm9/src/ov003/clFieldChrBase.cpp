// objdiff-target: build/delinks/_dsd_gap@ov003_3.o

#include "ov003/clFieldChrBase.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

#include "itcm/cpu_mem.h"

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
clFieldChrBase::clFieldChrBase(clFieldBase * field, u8 param2, UnkStruct_0206b0f4 * param3, u32 param4) {
    cpu_fill32(&this->field10, 0, sizeof(this->field10));
    this->field10.fieldbc = this;

    this->field10.fieldac_0 = 1;

    this->field = field;
    this->fieldd0 = param2;
    this->fieldd3 = 0;

    this->fieldd3_4 = (field != NULL) ? field->unk_virtual_04() : 0; 

    if (param3 == NULL) {
        return;
    }

    this->fieldd3_0 = (param3->field10_1 & param4);
}

// MATCHED 100%
clFieldChrBase::~clFieldChrBase() {}

// MATCHED 100%
void clFieldChrBase::func_ov003_0206b290() {}

// MATCHED 100%
void clFieldChrBase::func_ov003_0206b294() {
    this->field10.fieldac_1 = 0;
    this->field10.fieldb8 = 0;
}

// MATCHED 100%
void clFieldChrBase::func_ov003_0206b2ac() {
    this->field10.fieldac_2 = 1;
}

// MATCHED 100%
void clFieldChrBase::func_ov003_0206b2bc() {
    this->field10.fieldac_2 = 0;
}

// MATCHED 100%
void clFieldChrBase::func_ov003_0206b2cc() {
    this->field08 = this;
}

// MATCHED 100%
void clFieldChrBase::func_ov003_0206b2d4() {}

// MATCHED 100%
u32 clFieldChrBase::func_ov003_0206b2d8() { return 10; }
