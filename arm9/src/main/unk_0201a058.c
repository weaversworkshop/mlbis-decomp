// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/unk_0201a058.h"
#include "main/unk_0201a180.h"

// MATCHED 100%
void func_0201a058(UnkStruct_0201a058 * param1) {
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
