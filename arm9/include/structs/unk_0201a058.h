#ifndef STRUCTS_UNK_0201A058_H
    #define STRUCTS_UNK_0201A058_H

    #include "types.h"

    typedef struct UnkHeader_0201a058{
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
    } UnkHeader_0201a058;

    typedef struct {
        u8 pad_00;
    } UnkStruct_0201a058;
#endif
