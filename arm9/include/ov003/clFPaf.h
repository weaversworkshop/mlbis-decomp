#ifndef OV003_CL_F_PAF_H
#define OV003_CL_F_PAF_H

#include "types.h"

#ifdef __cplusplus

#include "main/clPaf.h"

class clFPaf : public clPaf {
public:

    // ~clFPaf();  // implicit; D1 0x02069d94 D0 0x02069d64 Thn12_D1 0x02069dcc Thn12_D0 0x02069dbc
    virtual void func_ov003_02069d60(void);  // 0x02069d60
};

#endif

#endif
