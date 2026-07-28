#ifndef STRUCTS_UNK_02056370_H
    #define STRUCTS_UNK_02056370_H

    #include "types.h"

    typedef struct {
        u16 chr;
        union {
            u16 stats[14];
            struct {
                u16 base[6];
                u16 maxHp, hp;
                u16 maxSp, sp;
                u16 pow, def, speed, stache;    // effective stats (gear applied)
            };
        };
        u8 pad_1e[0x2];
        union {
            u32 levelExp;
            struct {
                u32 level : 8;
                u32 exp : 24;
            };
        };
        union {
            u32 expSlots;
            struct {
                u32 expToNext : 24;
                u32 gearSlots : 8;
            };
        };
        u8 gear[3];
        u8 pad_2b[0x1];
        u8 badge;
        u8 pad_2d[0x3];
    } PlayerStatus;

    typedef struct {
        PlayerStatus party[3];
        u8 pad_90[0x6];
        s8 itemCount[0x20];
        s8 gearCount[0x81];
        u8 badgeOwned;
        u8 pad_138[0x8];
    } PartyStatus;
#endif
