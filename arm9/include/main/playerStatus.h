#ifndef MAIN_PLAYER_STATUS_H
#define MAIN_PLAYER_STATUS_H

#include "types.h"

#define CHR_MARIO   0
#define CHR_LUIGI   1
#define CHR_BOWSER  2

#define ID_SPACE_MASK   0xf000
#define ID_INDEX_MASK   0x0fff
#define ID_KEYITEM      0x1000
#define ID_ITEM         0x2000
#define ID_BADGE        0x3000
#define ID_GEAR         0x4000

#define STAT_MAX    999
#define ITEM_MAX    99
#define GEAR_MAX    9
#define COIN_MAX    999999
#define EXP_MAX     9999999
#define LEVEL_IDX_MAX   98

typedef struct {
    u32 hp : 10;  // +0x0
    u32 sp : 10;
    u32 pow : 10;
    u32 pad_00_1e : 2;
    u32 def : 10;  // +0x4
    u32 speed : 10;
    u32 stache : 10;
    u32 pad_04_1e : 2;
    u16 expNext;  // +0x8
    u8 pad_0a[0x2];
} LevelStats;

typedef struct {
    u16 nameId;  // +0x0
    u8 pad_02[0x16];
} ItemInfo;

typedef struct {
    u16 nameId;  // +0x0
    u8 pad_02[0xe];
} BadgeInfo;

typedef struct {
    u16 nameId;  // +0x0
    u8 pad_02[0xd];
    u8 bowserOnly : 1;  // +0xf
    u8 category : 4;
    u8 reqRank : 3;
    s16 statBonus[6];  // +0x10
    u8 effect;  // +0x1c
    u8 effectValue;  // +0x1d
    u8 effectValue2;  // +0x1e
    u8 pad_1f[0x1];
} GearInfo;

typedef struct {
    u16 nameId;  // +0x0
    u8 effect;  // +0x2
    u8 pad_03[0x1];
    u16 value;  // +0x4
    u8 pad_06[0x2];
} BadgeComboInfo;

typedef struct {
    s16 chr;  // +0x0
    u16 levelIdx;  // +0x2
    u32 exp;  // +0x4
    u32 expNext;  // +0x8
    u8 leveledUp : 1;  // +0xc
    u8 pad_0c_1 : 7;
    u8 pad_0d[0x3];
} ExpCounter;

typedef struct {
    u16 chr;  // +0x0
    u16 stats[14];  // +0x2  baseHP, baseSp, basePow, baseDef, baseSpeed, baseStache, maxHp, hp, maxSp, sp, pow, def, speed, stache
    u8 pad_1e[0x2];
    u32 level : 8;  // +0x20
    u32 exp : 24;
    u32 expToNext : 24;  // +0x24
    u32 gearSlots : 8;
    u8 gear[3];  // +0x28
    u8 pad_2b[0x1];
    u8 badge;  // +0x2c
    u8 pad_2d[0x3];
} PlayerStatus;

typedef struct {
    PlayerStatus members[3];  // +0x0
    u32 coins;  // +0x90
    u16 field94;
    s8 itemCount[0x20];  // +0x96
    s8 gearCount[0x81];  // +0xb6
    u8 badgeOwned;  // +0x137
    u8 field138;
    u8 pad_139[0x1];
    s16 field13a;
    s16 field13c;
    u8 pad_13e[0x2];
} PartyStatus;

void initParty(u32);  // 0x02018048
u16 * getNameIdPtr(s32);  // 0x02018538
s32 getItemCount(u16);  // 0x02018824
s8 addItem(u16, s8);  // 0x02018878
s32 addCoins(s32);  // 0x02018974
BOOL equipGear(u8, u16, u8);  // 0x02018ec4
BOOL refreshStats(u8);  // 0x02019028
void equipBadge(u8, u16);  // 0x02019058
void expInit(ExpCounter *, s32, u32);  // 0x02019360
s8 expGetRank(ExpCounter *);  // 0x020194c4
s32 expGetLevel(ExpCounter *);  // 0x02019564
s32 expGetLevelIdx(ExpCounter *);  // 0x02019574
s32 expGetIntoLevel(ExpCounter *);  // 0x020195a4
s32 expGetLevelCost(ExpCounter *);  // 0x020195d4
s32 expGetExp(ExpCounter *);  // 0x02019618
s32 expGetNext(ExpCounter *);  // 0x02019620

#endif
