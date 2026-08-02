// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/playerStatus.h"

#include "main/backup.h"
#include "main/clGameMain.h"

// --- Extern Data Variables --- //

extern LevelStats * sLevelTable[];  // 0x020526ec
extern BadgeInfo sBadgeInfo[];  // 0x020526f8
extern BadgeComboInfo sBadgeCombo[];  // 0x02052778
extern ItemInfo sItemInfo[];  // 0x020527f8
extern ItemInfo sKeyItemInfo[];  // 0x02052a68
extern GearInfo sGearInfo[];  // 0x02053b30

extern s8 data_020526bc[];
extern s8 data_020526c0[];
extern s16 sStartLvBowser[];  // 0x020526c4
extern s16 sStartGearSlotsLuigi[];  // 0x020526c6
extern s16 sStartGearSlotsBowser[];  // 0x020526c8
extern u16 sStartBadgeLuigi[];  // 0x020526ca
extern s16 sStartGearSlotsMario[];  // 0x020526cc
extern u16 sStartBadgeMario[];  // 0x020526ce
extern s16 sStartLvLuigi[];  // 0x020526d0
extern s16 sStartLvMario[];  // 0x020526d2
extern u16 sStartGearMario[][4];  // 0x020526d4
extern u16 sStartGearLuigi[][4];  // 0x020526dc
extern u16 sStartGearBowser[][4];  // 0x020526e4

extern s8 data_0205af84[];
extern s8 data_0205af88[];
extern s8 data_0205af8c[];

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

// --- Variable Definitions --- // (replace with real data later)

const s8 sGearSlotsBowser[4] = { 1, 2, 3, 3 };  // 0x0204ac40
const s8 sGearSlotsBros[8] = { 1, 2, 3, 3, 3, 3 };  // 0x0204ac44

// --- Function Declarations --- //

void setLevel(u8, u8);  // 0x020184a8
ItemInfo * getKeyItemInfo(u32);  // 0x020185d0
ItemInfo * getItemInfo(u32);  // 0x02018600
BadgeInfo * getBadgeInfo(u32);  // 0x02018630
GearInfo * getGearInfo(u16);  // 0x02018650
BadgeComboInfo * getBadgeCombo(u32);  // 0x02018670
u32 getBadgeComboEffect(u32, u32 *, u32 *);  // 0x02018688
s16 badgeComboIdx(s32, s32);  // 0x020187dc
void calcStats(PlayerStatus *, u32, s16, u16);  // 0x020189b4
BOOL findGearEffect(u8, u8, u8 *, u8 *);  // 0x020190a4
u16 getEquippedBadge(u8);  // 0x02019154
BOOL keyItemFlagSet(u32);  // 0x02019178
s32 keyItemToFlag(u32);  // 0x020191c0
void expCopy(ExpCounter *, ExpCounter *);  // 0x020193a0
void expAdd(ExpCounter *, s32);  // 0x020193c4
s32 expAddStep(ExpCounter *, s32);  // 0x020193ec
s8 getRank(u8, u8, s8);  // 0x020194f4
s32 getMaxLevelIdx(void);  // 0x0201957c
s32 expGetNextLevel(ExpCounter *);  // 0x02019584

// --- Function Definitions --- //

// MATCHED 100%
void initParty(u32 preset) {
    u16 startBadge;
    s32 chr;
    u32 slot;
    PlayerStatus * p;

    func_01ff8228(&gameState.party, 0, sizeof(PartyStatus));

    startBadge = 0;
    u16 startGear[4] = { 0, 0, 0, 0 };
    p = gameState.party.members;

    for (chr = 0; chr < 3; chr++) {
        LevelStats * lvl = sLevelTable[chr];

        switch (chr) {
            case CHR_MARIO:
                p->chr = CHR_MARIO;
                p->level = sStartLvMario[preset];
                p->gearSlots = sStartGearSlotsMario[preset];

                startBadge = sStartBadgeMario[preset];

                for (s32 i = 0; i < 4; i++) {
                    startGear[i] = sStartGearMario[preset][i];
                }
                break;
            case CHR_LUIGI:
                p->chr = CHR_LUIGI;
                p->level = sStartLvLuigi[preset];
                p->gearSlots = sStartGearSlotsLuigi[preset];

                startBadge = sStartBadgeLuigi[preset];

                for (s32 i = 0; i < 4; i++) {
                    startGear[i] = sStartGearLuigi[preset][i];
                }
                break;
            case CHR_BOWSER:
                p->chr = CHR_BOWSER;
                p->level = sStartLvBowser[preset];
                p->gearSlots = sStartGearSlotsBowser[preset];

                for (s32 i = 0; i < 4; i++) {
                    startGear[i] = sStartGearBowser[preset][i];
                }
        }

        p->stats[0] = lvl[p->level-1].hp;
        p->stats[1] = lvl[p->level-1].sp;
        p->stats[2] = lvl[p->level-1].pow;
        p->stats[3] = lvl[p->level-1].def;
        p->stats[4] = lvl[p->level-1].speed;
        p->stats[5] = lvl[p->level-1].stache;
        p->expToNext = lvl[p->level].expNext;

        for (u32 lvlIdx = 1; lvlIdx < p->level; lvlIdx++) {
            p->exp += lvl[lvlIdx-1].expNext;
        }

        p->stats[6] = p->stats[0];
        p->stats[8] = p->stats[1];
        p->stats[10] = p->stats[2];
        p->stats[11] = p->stats[3];
        p->stats[12] = p->stats[4];
        p->stats[13] = p->stats[5];

        for (slot = 0; slot < p->gearSlots; slot++) {
            addItem(startGear[slot], 1);
            equipGear(chr, startGear[slot], slot);
        }

        if (startBadge != 0) {
            addItem(startBadge, 1);
            equipBadge(chr, startBadge);
        }

        p->stats[7] = p->stats[6];
        p->stats[9] = p->stats[8];
        
        p++;
    }

    gameState.party.coins = 0;

    gameState.party.field94 = 0;

    gameState.party.itemCount[0] = data_020526c0[preset];
    gameState.party.itemCount[0x18] = data_0205af8c[preset];
    gameState.party.itemCount[1] = data_0205af88[preset];
    gameState.party.itemCount[0x10] = data_020526bc[preset];
    gameState.party.itemCount[0xb] = data_0205af84[preset];
}

// MATCHED 100%
void setLevel(u8 chr, u8 level) {
    PlayerStatus * p = &gameState.party.members[chr];
    p->level = level;
    
    if (chr < 2) {
        s8 rank = getRank(chr, level-1, -1);
        p->gearSlots = sGearSlotsBros[rank-1];
    } else {
        s8 rank = getRank(chr, level-1, -1);
        p->gearSlots = sGearSlotsBowser[rank-1];
    }
}

// MATCHED 100%
u16 * getNameIdPtr(s32 id) {
    u32 idx = id & ID_INDEX_MASK;

    switch (id & ID_SPACE_MASK) {
        case ID_KEYITEM:
            return &sKeyItemInfo[idx].nameId;
        case ID_ITEM:
            return &sItemInfo[idx].nameId;
        case ID_BADGE:
            return &sBadgeInfo[idx].nameId;
        case ID_GEAR:
            return &sGearInfo[idx].nameId;
        default:
            return NULL;
    }
}

// MATCHED 100%
ItemInfo * getKeyItemInfo(u32 id) {
    if ((id & ID_KEYITEM) != ID_KEYITEM) {
        return NULL;
    }

    return &sKeyItemInfo[id & ID_INDEX_MASK];
}

// MATCHED 100%
ItemInfo * getItemInfo(u32 id) {
    if ((id & ID_ITEM) != ID_ITEM) {
        return NULL;
    }

    return &sItemInfo[id & ID_INDEX_MASK];
}

// MATCHED 100%;
BadgeInfo * getBadgeInfo(u32 id) {
    if ((id & ID_BADGE) != ID_BADGE) {
        return NULL;
    }
    
    return &sBadgeInfo[id & ID_INDEX_MASK];
}

// MATCHED 100%
GearInfo * getGearInfo(u16 id) {
    if ((id & ID_SPACE_MASK) != ID_GEAR) {
        return NULL;
    }
    
    return &sGearInfo[id & ID_INDEX_MASK];
}

// MATCHED 100%
BadgeComboInfo * getBadgeCombo(u32 idx) {
    if (idx >= 0x10) {
        return NULL;
    }

    return &sBadgeCombo[idx];
}

// MATCHED 100%
u32 getBadgeComboEffect(u32 idx, u32 * outValue, u32 * outExtra) {
    BadgeComboInfo * combo = getBadgeCombo(idx);

    switch (combo->effect) {
        case 0:
            if (outValue != NULL) {
                *outValue = combo->value & ~0x8000;
            }

            if (outExtra != NULL && (combo->value & 0x8000) != 0) {
                *outExtra = 1;
            } else {
                *outExtra = 0;
            }
            break;
        case 1:
            if (outValue != NULL) {
                *outValue = combo->value << 8;
            }
            
            if (outExtra != NULL) {
                *outExtra = 0;
            }
            break;
        case 2:
            if (outValue != NULL) {
                *outValue = (u32)combo->value << 0x18 >> 0x10;
            }

            if (outExtra != NULL) {
                *outExtra = combo->value & 0xff00;
            }
            break;
        case 3:
            if (outValue != NULL) {
                *outValue = combo->value;
            }
            
            if (outExtra != NULL) {
                *outExtra = 0;
            }
            break;
        case 4:
            if (outValue != NULL) {
                *outValue = 0;
            }

            if (outExtra != NULL) {
                *outExtra = 0;
            }
            break;
        case 5:
            if (outValue != NULL) {
                *outValue = combo->value;
            }
            
            if (outExtra != NULL) {
                *outExtra = 0;
            }
            break;
        case 6:
            if (outValue != NULL) {
                *outValue = combo->value & ~0x8000;
            }

            if (outExtra != NULL && (combo->value & 0x8000) != 0) {
                *outExtra = 1;
            } else {
                *outExtra = 0;
            }
            break;
    }

    return combo->effect;
}

// MATCHED 100%
s16 badgeComboIdx(s32 marioBadge, s32 luigiBadge) {
    if ((marioBadge & ID_INDEX_MASK) >= 4) {
        return -1;
    }

    s32 luigi = luigiBadge & ID_INDEX_MASK;
    if (luigi < 4 || luigi >= 8) {
        return -1;
    }

    return luigi - 4 + (marioBadge & ID_INDEX_MASK) * 4;
}

// MATCHED 100%
s32 getItemCount(u16 id) {
    s32 count = 0;

    switch ((id & ID_SPACE_MASK)) {
        case ID_ITEM:
            count = gameState.party.itemCount[id & ID_INDEX_MASK];
            break;
        case ID_GEAR:
            count = gameState.party.gearCount[id & ID_INDEX_MASK];
    }
    return count;
}

// MATCHED 100%
s8 addItem(u16 id, s8 delta) {
    s8 ret = 0;
    u16 idx = (id & ID_INDEX_MASK);
    
    PartyStatus * party = &gameState.party;

    switch ((u16)(id & ID_SPACE_MASK)) {
        case ID_BADGE: {
            if (delta < 0) {
                if (party->badgeOwned & (1 << idx)) {
                    ret = -1;
                }

                party->badgeOwned &= ~(1 << idx);

                return ret;
            } else {
                if (!(party->badgeOwned & (1 << idx))) {
                    ret = 1;
                }

                party->badgeOwned |= (1 << idx);

                return ret;
            }
        }
        case ID_GEAR: {
            s8 old = party->gearCount[idx];
            s16 val = delta + old;
            if (val < 0) {
                val = 0;
            }
            if (val > 9) {
                val = 9;
            }

            party->gearCount[idx] = val;

            return (s8)(val - old);
        }
        case ID_ITEM: {
            s8 old = party->itemCount[idx];
            s16 val = delta + old;
            if (val < 0) {
                val = 0;
            }
            if (val > 99) {
                val = 99;
            }

            party->itemCount[idx] = val;

            return (s8)(val - old);
        }
    }

    return 0;
}

// MATCHED 100%
s32 addCoins(s32 delta) {
    u32 old = gameState.party.coins;
    s32 coins = old + delta;
    if (coins < 0) {
        gameState.party.coins = 0;
    } else {
        if (coins > COIN_MAX) {
            gameState.party.coins = COIN_MAX;
        } else {
            gameState.party.coins = coins;
        }
    }

    return (gameState.party.coins - old);
}

// MATCHED 100%
void calcStats(PlayerStatus * dst, u32 chr, s16 slot, u16 gearId) {
    PlayerStatus * src = &gameState.party.members[chr];

    s32 hp = src->stats[0];
    dst->stats[0] = hp;

    s32 sp = src->stats[1];
    dst->stats[1] = sp;

    s32 pow = src->stats[2];
    dst->stats[2] = pow;

    s32 def = src->stats[3];
    dst->stats[3] = def;

    s32 speed = src->stats[4];
    dst->stats[4] = speed;

    s32 stache = src->stats[5];
    dst->stats[5] = stache;

    for (u32 i = 0; i < src->gearSlots; i++) {
        GearInfo * gear;
        if (slot == i) {
            gear = getGearInfo(gearId);
        } else {
            gear = getGearInfo(src->gear[i] | ID_GEAR);
        }

        hp += gear->statBonus[0];
        sp += gear->statBonus[1];
        pow += gear->statBonus[2];
        def += gear->statBonus[3];
        speed += gear->statBonus[4];
        stache += gear->statBonus[5];
    }

    s32 val = STAT_MAX;
    if (hp <= STAT_MAX) {
        val = (hp < 0) ? 0 : hp;
    }
    if (val == 0) {
        hp = 1;
        dst->stats[6] = hp;
    } else {
        if (hp > STAT_MAX) {
            hp = STAT_MAX;
        } else if (hp < 0) {
            hp = 0;
        }
        dst->stats[6] = hp;
    }

    val = STAT_MAX;
    if (sp > val) {
        sp = val;
    } else if (sp < 0) {
        sp = 0;
    }
    dst->stats[8] = sp;

    if (pow > STAT_MAX) {
        pow = STAT_MAX;
    } else if (pow < 0) {
        pow = 0;
    }
    dst->stats[10] = pow;

    val = STAT_MAX;
    if (def <= STAT_MAX) {
        val = (def < 0) ? 0 : def;
    }

    if (val == 0) {
        def = 1;
        dst->stats[11] = def;
    } else {
        if (def > STAT_MAX) {
            def = STAT_MAX;
        } else if (def < 0) {
            def = 0;
        }
        dst->stats[11] = def;
    }

    if (speed > STAT_MAX) {
        speed = STAT_MAX;
    } else if (speed < 0) {
        speed = 0;
    }
    dst->stats[12] = speed;

    if (stache > STAT_MAX) {
        stache = STAT_MAX;
    } else if (stache < 0) {
        stache = 0;
    }
    dst->stats[13] = stache;

    for (u32 i = 0; i < src->gearSlots; i++) {
        GearInfo * gear;
        if (slot == i) {
            gear = getGearInfo(gearId);
        } else {
            gear = getGearInfo(src->gear[i] | ID_GEAR);
        }

        switch (gear->effect) {
            case 1: {
                s32 v = hp * (gear->effectValue + 100);
                hp = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 2: {
                s32 v = sp * (gear->effectValue + 100);
                sp = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 3: {
                s32 v = pow * (gear->effectValue + 100);
                pow = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 0x46: { // 70
                s32 v = def * (gear->effectValue + 100);
                def = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
            case 4: {
                s32 v = speed * (gear->effectValue + 100);
                speed = (v % 100 < 50) ? v / 100 : v / 100 + 1;
                break;
            }
        }
    }

    val = STAT_MAX;
    if (hp <= STAT_MAX) {
        val = (hp < 0)  ? 0 : hp;
    }

    if (val == 0) {
        dst->stats[6] = 1;
    } else {
        if (hp > STAT_MAX) {
            hp = STAT_MAX;
        } else if (hp < 0) {
            hp = 0;
        }
        dst->stats[6] = hp;
    }

    if (sp > STAT_MAX) {
        sp = STAT_MAX;
    } else if (sp < 0) {
        sp = 0;
    }
    dst->stats[8] = sp;

    if (pow > STAT_MAX) {
        pow = STAT_MAX;
    } else if (pow < 0) {
        pow = 0;
    }
    dst->stats[10] = pow;

    val = STAT_MAX;
    if (def <= STAT_MAX) {
        val = (def < 0) ? 0 : def;
    }

    if (val == 0) {
        dst->stats[11] = 1;
    } else {
        if (def > STAT_MAX) {
            def = STAT_MAX;
        } else if (def < 0) {
            def = 0;
        }
        dst->stats[11] = def;
    }

    if (speed > STAT_MAX) {
        speed = STAT_MAX;
    } else if (speed < 0) {
        speed = 0;
    }
    dst->stats[12] = speed;

    if (stache > STAT_MAX) {
        stache = STAT_MAX;
    } else if (stache < 0) {
        stache = 0;
    }
    dst->stats[13] = stache;
}

// MATCHED 100%
BOOL equipGear(u8 chr, u16 gearId, u8 slot) {
    GearInfo * gear;
    u32 i;

    PlayerStatus * p = &gameState.party.members[chr];

    gear = getGearInfo(gearId);

    if (gear->category != 0) {
        for (i = 0; i < p->gearSlots; i++) {
            GearInfo * other = getGearInfo(p->gear[i] | ID_GEAR);
            if (slot != i && gear->category == other->category) {
                return FALSE;
            }
        }

        if (gear->bowserOnly != 0 && chr != CHR_BOWSER) {
            return FALSE;
        }

        if (gear->bowserOnly == 0 && chr == CHR_BOWSER) {
            return FALSE;
        }

        if (slot >= p->gearSlots) {
            return FALSE;
        }

        gear = getGearInfo(gearId);

        if (getRank(chr, p->level-1, 0) < gear->reqRank) {
            return FALSE;
        }
    }

    calcStats(p, chr, slot, gearId);

    p->gear[slot] = gearId & ID_INDEX_MASK;

    if (p->stats[7] > p->stats[6]) {
        p->stats[7] = p->stats[6];
    }

    if (p->stats[9] > p->stats[8]) {
        p->stats[9] = p->stats[8];
    }

    return TRUE;
}

// MATCHED 100%
BOOL refreshStats(u8 chr) {
    return equipGear(chr, gameState.party.members[chr].gear[0] | ID_GEAR, 0);
}

// MATCHED 100%
void equipBadge(u8 chr, u16 badgeId) {
    gameState.party.members[chr].badge = badgeId & ID_INDEX_MASK;

    s16 combo = badgeComboIdx(gameState.party.members[0].badge, gameState.party.members[1].badge);
    if (combo != -1) {
        gameState.party.field138 = combo;
    }
}

// MATCHED 100%
BOOL findGearEffect(u8 chr, u8 effect, u8 * outValue, u8 * outValue2) {

    PlayerStatus * p = &gameState.party.members[chr];
    BOOL found = FALSE;
    u8 bestValue = 0;
    u8 bestValue2 = 0;

    for (u32 i = 0; i < p->gearSlots; i++) {
        GearInfo * gear = getGearInfo(p->gear[i] | ID_GEAR);

        if (gear->effect == effect) {
            if (gear->effectValue > bestValue || (gear->effectValue == bestValue && gear->effectValue2 > bestValue2)) {
                bestValue2 = gear->effectValue2;
                bestValue = gear->effectValue;

                if (outValue != NULL) {
                    *outValue = gear->effectValue;
                }

                if (outValue2 != NULL) {
                    *outValue2 = bestValue2;
                }
            }

            found = TRUE;
        }
    }

    return found;
}

// MATCHED 100%
u16 getEquippedBadge(u8 chr) {
    return gameState.party.members[chr].badge | ID_BADGE;
}

// MATCHED 100%
BOOL keyItemFlagSet(u32 keyItemId) {
    s32 flag = keyItemToFlag(keyItemId);
    if (flag == 0) {
        return 0;
    }

    u32 val = gameState.field054[(flag & 0xfff) >> 5] >> (flag & 0x1f);
    return ((val & 1) != 0);
}

// MATCHED 100%
s32 keyItemToFlag(u32 keyItemId) {
    switch (keyItemId - 0x1000) {
        case 20:
            return 0;
        case 21:
            return 0;
        case 22:
            return 0;
        case 6:
            return 0x2011;
        case 3:
            return 0x2012;
        case 13:
            return 0x201d;
        case 10:
            return 0x201c;
        case 11:
            return 0x201e;
        case 0:
            return 0x2010;
        case 23:
            return 0;
        case 12:
            return 0x201f;
        case 24:
            return 0;
        case 25:
            return 0;
        case 2:
            return 0x2016;
        case 4:
            return 0x2017;
        case 26:
            return 0;
        case 5:
            return 0x2018;
        case 1:
            return 0x2019;
        case 27:
            return 0;
        case 8:
            return 0x201b;
        case 14:
            return 0x2021;
        case 7:
            return 0x201a;
        case 9:
            return 0;
        case 15:
            return 0x2022;
        case 16:
            return 0x2013;
        case 17:
            return 0;
        case 18:
            return 0;
        case 19:
            return 0;
    }
    return keyItemId;
}

// MATCHED 100%
void expInit(ExpCounter * ctr, s32 chr, u32 exp) {
    ctr->chr = chr;
    ctr->levelIdx = 0;
    ctr->exp = 0;
    ctr->expNext = expGetLevelCost(ctr);
    ctr->leveledUp = 0;

    expAdd(ctr, exp);
}

// MATCHED 100%
void expCopy(ExpCounter * dst, ExpCounter * src) {
    dst->chr = src->chr;
    dst->levelIdx = src->levelIdx;
    dst->exp = src->exp;
    dst->expNext = src->expNext;
}

// MATCHED 100%
void expAdd(ExpCounter * ctr, s32 amount) {
    while (amount > 0) {
        amount = expAddStep(ctr, amount);
    }
}

// MATCHED 100%
s32 expAddStep(ExpCounter * ctr, s32 amount) {
    s32 need = expGetNext(ctr) - expGetExp(ctr);

    if (amount - need < 0 || getMaxLevelIdx() <= expGetLevelIdx(ctr)) {
        s32 cur = expGetExp(ctr);
        if (cur + amount < 0) {
            ctr->exp = 0;
        } else if (cur + amount > EXP_MAX) {
            ctr->exp = EXP_MAX;
        } else {
            ctr->exp = cur + amount;
        }
        return 0;
    }

    ctr->leveledUp = 1;
    ctr->levelIdx++;
    ctr->exp = expGetNext(ctr);

    s32 val = expGetNext(ctr) + expGetLevelCost(ctr);
    if (val < 0) {
        ctr->expNext = 0;
    } else if (val > EXP_MAX) {
        ctr->expNext = EXP_MAX;
    } else {
        ctr->expNext = val;
    }

    return amount - need;
}

// MATCHED 100%
s8 expGetRank(ExpCounter * ctr) {
    return getRank(ctr->chr, ctr->levelIdx, ctr->leveledUp);
}

// MATCHED 100%
s8 getRank(u8 chr, u8 levelIdx, s8 param3) {
    if (chr == 2) {
        if (levelIdx < 9) {
            return 1;
        } else if (levelIdx < 19) {
            return 2;
        } else if (levelIdx < 39) {
            return 3;
        } else {
            return 4;
        }
    } else {
        if (levelIdx < 5) {
            return 1;
        } else if (levelIdx < 11) {
            return 2;
        } else if (levelIdx < 17) {
            return 3;
        } else if (levelIdx < 24) {
            return 4;
        } else if (levelIdx < 39) {
            return 5;
        } else {
            return 6;
        }
    }
}

// MATCHED 100%
s32 expGetLevel(ExpCounter * ctr) {
    return expGetLevelIdx(ctr) + 1;
}

// MATCHED 100%
s32 expGetLevelIdx(ExpCounter * ctr) {
    return ctr->levelIdx;
}

// MATCHED 100%
s32 getMaxLevelIdx() {
    return LEVEL_IDX_MAX;
}

// MATCHED 100%
s32 expGetNextLevel(ExpCounter * ctr) {
    s32 next = expGetLevelIdx(ctr);
    next++;

    s32 max = getMaxLevelIdx();
    if (max < next) {
        next = max;
    }

    return next + 1;
}

// MATCHED 100%
s32 expGetIntoLevel(ExpCounter * ctr) {
    return expGetExp(ctr) - (expGetNext(ctr) - expGetLevelCost(ctr));
}

// MATCHED 100%
s32 expGetLevelCost(ExpCounter * ctr) {
    if (expGetLevelIdx(ctr) >= getMaxLevelIdx()) {
        return 0;
    }

    return sLevelTable[ctr->chr][ctr->levelIdx].expNext;
}

// MATCHED 100%
s32 expGetExp(ExpCounter * ctr) {
    return ctr->exp;
}

// MATCHED 100%
s32 expGetNext(ExpCounter * ctr) {
    return ctr->expNext;
}
