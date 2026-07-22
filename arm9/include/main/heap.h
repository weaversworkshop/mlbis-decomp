#ifndef MAIN_HEAP_H
#define MAIN_HEAP_H

#include "types.h"

typedef struct UnkHeader_0201a058 {
    struct UnkHeader_0201a058 * field00;
    struct UnkHeader_0201a058 * field04;
    union {
        struct {
            u32 field08_00 : 25;
            u32 field08_25 : 1;
            u32 field08_heap : 6;
        };
        struct {
            u32 field08_size : 26;
            u32 : 6;
        };
    };
} UnkHeader_0201a058;

#define GET_HEADER(ptr) ((UnkHeader_0201a058*)((u8*)ptr - 0xc))

typedef struct {
    UnkHeader_0201a058 * field00[2][2];
    u32 field10_00 : 27;
    u32 field10_27 : 5;
} UnkGlobal_0205b7f8;

extern UnkGlobal_0205b7f8 data_0205b7f0[32];

#ifdef __cplusplus
extern "C" {
#endif

void func_02019c90(void);
void * func_02019eac(s32 heap, u32 size, void * ptr, u32 flag);
void func_0201a058(void * param1);
void func_0201a174(void * param1);

#ifdef __cplusplus
}
#endif

#endif
