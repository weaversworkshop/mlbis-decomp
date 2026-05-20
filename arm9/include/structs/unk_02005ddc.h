#ifndef STRUCTS_UNK_02005DDC_H
    #define STRUCTS_UNK_02005DDC_H

    #include "types.h"

    typedef struct {
      union {
          u32 field00;
          struct {
              u32 bit0 : 1;
              u32 bit1 : 1;
              u32 bit2 : 1;
              u32 rest : 29;
          };
      };
    } UnkStruct_02005ddc_00;
#endif
