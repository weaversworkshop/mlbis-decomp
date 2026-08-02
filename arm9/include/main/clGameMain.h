#ifndef MAIN_CL_GAME_MAIN_H
#define MAIN_CL_GAME_MAIN_H

#include "types.h"

#ifdef __cplusplus

#include "main/task.h"

class clGameMain : public clTask {
public:
    u8 pad_18[0x10];
    u16 mode;
    u16 prevMode;

    clGameMain();
    virtual ~clGameMain() {};  // D1 0x020057cc D0 0x020057e0
    virtual void run();  // 0x02004ce8

    void setMode(u16);  // 0x020057ac
};

extern clGameMain * gameMain;

extern "C" {
#endif // __cplusplus

#include "main/playerStatus.h"

#define MSG_BANK_KEYITEM  0
#define MSG_BANK_ITEM     1
#define MSG_BANK_BADGE    2
#define MSG_BANK_GEAR     3
#define MSG_BANK_COUNT    4

typedef struct {
    u32 field000[16];
    u32 field040[5];
    u32 field054[2];
    u32 field05c[34];
    u32 field0e4[128];
    u8 field2e4[152];
    u8 field37c[2][7];
    u8 pad_38a[0x2];
    PartyStatus party;
    u8 : 2;
    u8 field4cc_2 : 1;
    u8 field4cc_3 : 2; // current save slot (0/1) ?? 
    u8 : 3;
    u8 field4cd;
    u8 field4ce;
    u8 pad_4cf[0x1];
    u32 field4d0;
    u32 field4d4;
    u32 * messageBanks[MSG_BANK_COUNT];
    u16 field4e8[10];
    u16 field4fc;
    u16 field4fe;
    u16 field500;
    u16 field502;
    union {
        struct {
            u32 field504_00 : 2;
            u32 field504_02 : 11;
            u32 field504_13 : 2;
        };
        struct {
            u32 field504_0b : 1;
            u32 field504_1b : 1;
        };
    };
    s16 field508[6];
} GameState;

extern GameState gameState;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
