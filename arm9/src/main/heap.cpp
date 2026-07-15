// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/heap.h"

#include <nitro/os.h>

extern "C" {

#include "main/cpu_mem.h"

// --- Struct Definitions --- //

struct UnkHeader_0201a058 {
    struct UnkHeader_0201a058 * field00;
    struct UnkHeader_0201a058 * field04;
    union {
        struct {
            u32 field08_00 : 25;
            u32 field08_25 : 1;
            u32 field08_26 : 6;
        };
        struct {
            u32 field08_size : 26;
            u32 : 6;
        };
    };
};

// --- Extern Data --- //

extern void SDK_MAIN_ARENA_LO(void);
extern void SDK_IRQ_STACKSIZE(void);
extern void SDK_SYS_STACKSIZE(void);
extern void SDK_AUTOLOAD_ITCM_END(void);
extern void SDK_AUTOLOAD_DTCM_END(void);

// --- Function Declarations --- //

void func_02019c90(void);
void func_02019e10(UnkGlobal_0205b7f8 * param1);
void * func_02019eac(s32 heap, u32 size, void * ptr, u32 flag);
void func_0201a058(void * param1);
void func_0201a180(UnkHeader_0201a058 * param1, UnkHeader_0201a058 * param2);
s32 func_0201a1c0(s32 param1, u32 param2, void * param3, u32 param4);
void func_0201a258(s32 param1);
void func_0201a2bc(void * param1);
void func_0201a2e0(u32 param1, void * param2);

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
void func_02019c90() {
    cpu_clear32(0, &data_0205b7f0, 0x280);

    u32 base0 = ((u32)SDK_MAIN_ARENA_LO + 0x1f) & ~0x1f;
    u32 end0 = HW_MAIN_MEM_MAIN_END - 0xc;
    data_0205b7f0[0].field00[0][0] = (UnkHeader_0201a058 *)base0;
    data_0205b7f0[0].field00[0][1] = (UnkHeader_0201a058 *)end0;
    
    u32 itcmBase = (u32)SDK_AUTOLOAD_ITCM_END;
    u32 itcmTop = HW_ITCM_END - 0x18;
    u32 dtcmFloor = HW_DTCM + 0x3f80 - (u32)SDK_IRQ_STACKSIZE - (u32)SDK_SYS_STACKSIZE;
    u32 size1 = itcmTop - itcmBase;
    u32 size0 = end0 - 0xc - base0;
    data_0205b7f0[1].field00[0][0] = (UnkHeader_0201a058 *)itcmBase;
    u32 end1 = HW_ITCM_END - 0xc;
    data_0205b7f0[1].field00[0][1] = (UnkHeader_0201a058 *)end1;

    u32 dtcmBase = (u32)SDK_AUTOLOAD_DTCM_END;
    u32 end2 = dtcmFloor - 0x100c;
    u32 size2 = dtcmFloor - 0x1000 - dtcmBase;
    data_0205b7f0[2].field00[0][0] = (UnkHeader_0201a058 *)dtcmBase;
    data_0205b7f0[2].field00[0][1] = (UnkHeader_0201a058 *)end2;

    u32 sharedBase = HW_MAIN_MEM_SHARED;
    u32 sharedEnd = HW_MAIN_MEM_SYSTEM - 0xc;
    data_0205b7f0[3].field00[0][0] = (UnkHeader_0201a058 *)sharedBase;
    data_0205b7f0[3].field00[0][1] = (UnkHeader_0201a058 *)sharedEnd;

    data_0205b7f0[0].field00[1][0] = (UnkHeader_0201a058 *)base0;
    data_0205b7f0[0].field00[1][1] = (UnkHeader_0201a058 *)end0;
    data_0205b7f0[1].field00[1][0] = (UnkHeader_0201a058 *)itcmBase;
    data_0205b7f0[1].field00[1][1] = (UnkHeader_0201a058 *)end1;
    data_0205b7f0[2].field00[1][0] = (UnkHeader_0201a058 *)dtcmBase;
    data_0205b7f0[2].field00[1][1] = (UnkHeader_0201a058 *)end2;
    data_0205b7f0[3].field00[1][0] = (UnkHeader_0201a058 *)sharedBase;
    data_0205b7f0[3].field00[1][1] = (UnkHeader_0201a058 *)sharedEnd;

    data_0205b7f0[0].field10_00 = size0;
    data_0205b7f0[1].field10_00 = size1;
    data_0205b7f0[2].field10_00 = size2 - 0x18;
    data_0205b7f0[3].field10_00 = 0xbe8;

    data_0205b7f0[0].field10_27 = 0;
    data_0205b7f0[1].field10_27 = 1;
    data_0205b7f0[2].field10_27 = 2;
    data_0205b7f0[3].field10_27 = 3;

    UnkGlobal_0205b7f8 * d = data_0205b7f0;
    for (s32 i = 4; i > 0; i--) {
        func_02019e10(d);
        d++;
    }
}

// MATCHED 100%
void func_02019e10(UnkGlobal_0205b7f8 * param1) {
    UnkHeader_0201a058 * node = param1->field00[0][0];
    node->field00 = NULL;
    node->field04 = param1->field00[0][1];
    node->field08_size = param1->field10_00 | 0x2000000;
    node->field08_26 = param1->field10_27 * 2;

    node = param1->field00[0][1];
    node->field00 = param1->field00[0][0];
    node->field04 = NULL;
    node->field08_size = param1->field10_00;
    node->field08_26 = param1->field10_27 * 2 + 1;
}

// MATCHED 100%
void * func_02019eac(s32 heap, u32 size, void * ptr, u32 flag) {
    s32 idx;
    u32 sz;
    s32 offset;
    u32 r6;
    UnkHeader_0201a058 * r14;

    if (size != 0) {
        offset = heap / 2;
        idx = heap & 1;
        if (flag == 1) {
            r14 = data_0205b7f0[offset].field00[1][idx];
        } else {
            r14 = data_0205b7f0[offset].field00[0][idx];
        }

        if (idx != 0) {
            r14 = r14->field00;
        }

        size = (size + 3) & ~3;

        r6 = (idx == 0);
        do {
            sz = r14->field08_00;
            if (r14->field08_25 != 0 && sz >= size) {
                offset = size + 0xc;
                if (sz > offset) {
                    if (idx == 0) {

                        UnkHeader_0201a058 * r2 = (UnkHeader_0201a058 *)((u8 *)r14 + 0xc + size);
                        r2->field08_size = (sz - offset) | 0x2000000;
                        r2->field00 = r14;

                        UnkHeader_0201a058 * f4 = r14->field04;
                        r2->field04 = f4;

                        if (f4 != NULL) {
                            f4->field00 = r2;
                        }

                        r14->field08_size = size;
                        r14->field04 = r2;

                        if (data_0205b7f0[heap / 2].field00[1][0] < r2) {
                            data_0205b7f0[heap / 2].field00[1][0] = r2;
                        }
                    } else {
                        UnkHeader_0201a058 * old = r14;
                        r14 = (UnkHeader_0201a058 *)((u8 *)r14 + (sz - size));
                        r14->field08_size = size;
                        r14->field00 = old;

                        UnkHeader_0201a058 * neighbor = old->field04;
                        r14->field04 = neighbor;

                        if (neighbor != NULL) {
                            neighbor->field00 = r14;
                        }

                        size = sz - (size + 0xc);

                        old->field08_size = size | 0x2000000;
                        old->field04 = r14;

                        if (data_0205b7f0[heap / 2].field00[1][1] > r14) {
                            data_0205b7f0[heap / 2].field00[1][1] = r14;
                        }
                    }
                } else {
                    r14->field08_size = sz;
                }

                r14->field08_25 = 0;
                r14->field08_26 = heap;

                return (void *)((u8 *)r14 + 0xc);
            }
            r14 = (r6 != 0) ? r14->field04 : r14->field00;
        } while (r14 != NULL);
    }

    return NULL;
}

// MATCHED 100%
void func_0201a058(void * param1) {
    if (param1 == NULL) {
        return;
    }

    UnkHeader_0201a058 * header = (UnkHeader_0201a058 *)((u8*)param1 - 0xc);
    UnkHeader_0201a058 * r5 = header->field00;
    UnkHeader_0201a058 * r6 = header->field04;

    s32 r3 = header->field08_26;

    header->field08_25 = 1;

    s32 r7 = r3 / 2;
    s32 r8 = r3 & 1;

    if (r6 != NULL && r6->field08_25 != 0) {
        func_0201a180(header, r6);

        if (r8 == 0) {
            if (data_0205b7f0[r7].field00[1][0] == r6) {
                data_0205b7f0[r7].field00[1][0] = header;
            }
        }
    }

    if (r5 != NULL && r5->field08_25 != 0) {
        func_0201a180(r5, header);

        if (r8 == 0) {
            if (data_0205b7f0[r7].field00[1][0] == header) {
                data_0205b7f0[r7].field00[1][0] = r5;
            }
        } else {
            if (data_0205b7f0[r7].field00[1][1] == header) {
                data_0205b7f0[r7].field00[1][1] = header->field04;
            }
        }
    }
}

// MATCHED 100%
void * operator new(size_t size, u32 heap, void * ptr, u32 flag) {
    return func_02019eac(heap, size, ptr, flag);
}

// MATCHED 100%
void * operator new[](size_t size, u32 heap, void * ptr, u32 flag) {
    return func_02019eac(heap, size, ptr, flag);
}

// MATCHED 100%
void operator delete(void * param1) {
    func_0201a058(param1);
}

// MATCHED 100%
void operator delete[](void * param1) {
    func_0201a058(param1);
}

// MATCHED 100%
void func_0201a180(UnkHeader_0201a058 * param1, UnkHeader_0201a058 * param2) {
    param1->field08_00 += param2->field08_00 + 0xc;

    UnkHeader_0201a058 * node = param2->field04;
    param1->field04 = node;
    if (node != NULL) {
        node->field00 = param1;
    }
}

// MATCHED 100%
s32 func_0201a1c0(s32 param1, u32 param2, void * param3, u32 param4) {
    void * addr = func_02019eac(param1, param2 + 0x18, NULL, param4);
    if (addr == NULL) {
        return -1;
    }

    UnkGlobal_0205b7f8 * ptr = &data_0205b7f0[4];
    for (s32 i = 4; i < 32; i++) {
        if (ptr->field00[0][0] == NULL) {
            ptr->field00[0][0] = (UnkHeader_0201a058 *)addr;
            ptr->field00[1][0] = (UnkHeader_0201a058 *)addr;
            ptr->field00[0][1] = (UnkHeader_0201a058 *)((u32)addr + param2 + 0xc);
            ptr->field00[1][1] = (UnkHeader_0201a058 *)((u32)addr + param2 + 0xc);

            ptr->field10_00 = param2;
            ptr->field10_27 = i;

            func_02019e10(ptr);

            return i*2;
        }

        ptr++;
    }

    func_0201a058(addr);
    return -1;
}

// MATCHED 100%
void func_0201a258(s32 param1) {
    func_0201a058(data_0205b7f0[param1 / 2].field00[0][0]);

    data_0205b7f0[param1 / 2].field00[0][0] = NULL;
    data_0205b7f0[param1 / 2].field00[0][1] = NULL;
    data_0205b7f0[param1 / 2].field00[1][0] = NULL;
    data_0205b7f0[param1 / 2].field00[1][1] = NULL;

    data_0205b7f0[param1 / 2].field10_00 = 0;
}

// MATCHED 100%
void func_0201a2bc(void * param1) {
    if (param1 == NULL) {
        param1 = (void *)(((u32)SDK_MAIN_ARENA_LO + 0x1f) & ~0x1f);
    }

    func_0201a2e0(0, param1);
}

// MATCHED 100%
void func_0201a2e0(u32 param1, void * param2) {
    u32 r7 = ((u32)param2 + 0x1f) & ~0x1f;
    UnkHeader_0201a058 * r6 = data_0205b7f0[param1].field00[0][0];
    
    UnkHeader_0201a058 r5 = *r6;

    data_0205b7f0[param1].field00[0][0] = (UnkHeader_0201a058 *)r7;
    data_0205b7f0[param1].field00[1][0] = (UnkHeader_0201a058 *)r7;

    data_0205b7f0[param1].field10_00 += (u32)r6 - r7;

    *data_0205b7f0[param1].field00[0][0] = r5;

    data_0205b7f0[param1].field00[0][0]->field08_00 += (u32)r6 - r7;
    data_0205b7f0[param1].field00[1][1]->field00 = data_0205b7f0[param1].field00[0][0];
}
