// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "structs/unk_02030540.h"

#include "main/unk_0200907c.h"

extern void func_0202f234(UnkGlobal_01ff94a8 * a, u32 b);
extern void func_0202f250(UnkGlobal_01ff94a8 * a, u32 b);
extern void func_0202f270(UnkGlobal_01ff94a8 * a, u32 b, u32 c);
extern void func_0202f290(UnkGlobal_01ff94a8 * a, u32 b, u32 c);
extern UnkStruct_02030540 * func_02030540(u32 a);

// MATCH 100%
void func_0200907c() {
    UnkStruct_01ff94c4 * r7 = data_01ff94c4;
    s8 r8;
    UnkGlobal_01ff94a8 * r9 = &data_01ff94a8;
    s16 r10 = 1;
    u32 r11 = 0;

    while(r10 >= 0) {
        if (r7->field0c != 0 && r7->field0c != 3) {

            u32 flag = (data_01ff94a4[r10] != NULL);
            if (flag != 0) {
                switch (r7->field0c) {
                    case 1:
                        r7->field04 += r7->field08;

                        if (r7->field04 > r7->field00) {
                            r7->field04 = r7->field00;
                            r7->field0c = r11;
                        }

                        func_0202f270(r9, 0xffff, 0);
                        break;
                    case 2:
                        r7->field04 -= r7->field08;
                        if (r7->field04 < 0) {
                            r7->field04 = 0;
                            r7->field0c = 3;
                            func_0202f270(r9, 0xffff, 1);
                        }
                        break;
                    default:
                        break;
                }

                if (r7->field04 != 0) {
                    func_0202f290(r9, 0xffff, 0xfff & (r7->field04 >> 0xc));
                }

                UnkStruct_02030540 * ret = func_02030540(data_01ff94a4[r10]->field38);
                s8 r1;

                if (r7->field04 == r7->field00) {
                    r1 = ret->field08;
                    r8 = ret->field07;
                } else {
                    switch (r7->field0c) {
                        case 1:
                            r1 = 1;
                            r8 = r1;
                            break;
                        case 2:
                        default:
                            r1 = 0;
                            r8 = r1;
                    }
                }

                func_0202f234(r9, r1);
                func_0202f250(r9, r8);
            }
        }

        r9--;
        r10--;
        r7--;
    }
}
