// objdiff-target: build/delinks/_dsd_gap@main_7.o

extern "C" {

#include "globals.h"

#include "main/fs.h"

#include <nitro/fs.h>
#include <nitro/hw/common/lcd.h>
#include <nitro/math.h>

#include "main/heap.h"
#include "main/system.h"

// --- Struct Definitions --- //

// --- Extern Data --- //

// --- Extern Functions --- //

extern void func_01ff8000(u8 ** param1, u32 * param2, u32 * param3);
extern void func_01ff80a4(u8 ** param1, void ** param2);
extern void func_01ff8150(u8 ** param1, void ** param2);
extern void func_01ff8228(void * param1, u32 param2, u32 param3);

// --- Function Declarations --- //

u32 func_0201adbc(FSChunkTable * tbl); // 0x0201adbc
u8 * func_0201add4(FSChunkTable * tbl, u32 idx); // 0x0201add4
u32 func_0201ade4(FSChunkTable * tbl, u32 idx); // 0x0201ade4
BOOL func_0201b180(FSOverlayID id, MIProcessor target); // 0x0201b180
BOOL func_0201b274(FSOverlayID id, MIProcessor target); // 0x0201b274
BOOL func_0201b28c(u32 param1, void * p_mem, u32 size); // 0x0201b28c
BOOL func_0201b2e4(void * p_mem, u32 size); // 0x0201b2e4

// --- Function Definitions --- //

// MATCHED 100%
clFS::clFS(u8 param1, u32 param2, u16 param3, u32 param4, u32 param5) : clTask(param1, param2, param5) {
    this->fieldb0 = NULL;
    this->fieldac = NULL;
    this->fieldd4 = NULL;
    this->fieldd0 = NULL;

    this->fieldc0 = 0xa8;
    this->fieldc4 = 5;
    this->fieldc6 = 2;
    this->fieldc8 = 0;
    this->fieldca_07 = 0;

    this->fielda8 = 0;

    if (param4 != 0xffff7f7f) {
        data_0205ba8c.dmaNo = param4;
        FS_SetDefaultDMA(param4);
    }

    this->fieldca_00 = param3;

    UnkHeader_0201a058 * header = GET_HEADER(this);
    u16 heap = header->field08_heap;

    this->fieldb4 = (FSFile *)func_02019eac(heap, param3 * sizeof(FSFile), NULL, 0);
    this->fieldbc = (FSChunkTable **)func_02019eac(heap, param3 * sizeof(FSChunkTable **), NULL, 0);
    this->fieldb8 = (const char **)func_02019eac(heap, param3 * sizeof(const char *), NULL, 0);
    this->fieldcc = (UnkStruct_FieldCC *)func_02019eac(heap, param3 * sizeof(UnkStruct_FieldCC), NULL, 0);

    for (s16 i = 0; param3 > i; i++) {
        FS_InitFile(&this->fieldb4[i]);
        this->fieldb8[i] = NULL;
        this->fieldbc[i] = NULL;
    }

    FS_InitFile(&this->field28);
}

// MATCHED 100%
clFS::~clFS() {
    func_0201b390();

    func_0201a058(this->fieldb4);
    func_0201a058(this->fieldbc);
    func_0201a058(this->fieldb8);
    func_0201a058(this->fieldcc);
}

// MATCHED 100%
void clFS::run() {
    bool flag = false;

    if (data_0205b7a8.field26 == 0) {
        bool ready = true;

        if (reg_GX_VCOUNT >= this->fieldc0 && reg_GX_VCOUNT <= (s16)HW_LCD_HEIGHT) {
            ready = false;
        }

        if (ready) {
            flag = true;
        }
    }

    if (flag) {
        this->fieldc8 = 0;
    } else if (this->fieldc8 >= this->fieldc6) {
        this->fieldc8 = this->fieldc4;
    } else if (++this->fieldc8 != this->fieldc4+1) {
        return;
    }

    bool run = true;
    while (run) {
        BOOL flag = (this->fieldd0 == NULL);

        if (!flag) {
            this->pumpTableLoads();
        } else if (!(flag = (this->fieldac == NULL))) {
            this->pumpReads();
        } else if (this->fielda8 != 0) {
            this->pumpOverlay();
        } else {
            this->fieldc8 = 0;
            return;
        }

        run = false;

        if (data_0205b7a8.field26 == 0) {
            bool ready = true;

            if (reg_GX_VCOUNT >= this->fieldc0 && reg_GX_VCOUNT <= (s16)HW_LCD_HEIGHT) {
                ready = false;
            }

            if (ready) {
                run = true;
            }
        }
    }
}

// MATCHED 100%
void clFS::func_0201ada4() {
    func_0201b390();
    this->func_0201a5f4();
}

// MATCHED 100%
u32 func_0201adbc(FSChunkTable * tbl) {
    return tbl->sizesArrOffset ? (tbl->sizesArrOffset - 8) / 4 : 0;
}

// MATCHED 100%
u8 * func_0201add4(FSChunkTable * tbl, u32 idx) {
    return (u8 *)tbl + CHUNK_OFFSET(tbl, idx);
}

// MATCHED 100%
u32 func_0201ade4(FSChunkTable * tbl, u32 idx) {
    return CHUNK_OFFSET(tbl, idx+1) - CHUNK_OFFSET(tbl, idx);
}

// MATCHED 100%
u32 clFS::func_0201adf8(s16 param1, const char * param2, u32 param3) {
    this->func_0201b330(param1, param2);

    u32 size;
    FS_ReadFile(&this->fieldb4[param1], &size, 4);

    u32 bytes;
    if (param3 == 1) {
            bytes = ((size != 0) ? (size - 8) >> 2 : 0) << 1;
    } else {
        bytes = 0;
    }

    return size + bytes;
}

// MATCHED 100%
FSChunkTable * clFS::func_0201ae68(u32 param1) {
    if (FS_IsFile(&this->fieldb4[(u16)param1])) {
        FS_CloseFile(&this->fieldb4[(u16)param1]);
    }

    FSChunkTable * ptr = this->fieldbc[param1];

    this->fieldb8[param1] = NULL;
    this->fieldbc[param1] = NULL;

    return ptr;
}

// MATCHED 100%
void clFS::func_0201aecc(u32 param1) {
    UnkStruct_FieldCC * val = &this->fieldcc[param1];
    while ((val->field1e & 0x3) != 0) {
        this->pumpTableLoads();
    }
}

// MATCHED 100%
u32 clFS::func_0201af08(u32 param1, u32 param2) {
    FSChunkTable * tbl = this->fieldbc[param1];
    return CHUNK_OFFSET(tbl, param2+1) - CHUNK_OFFSET(tbl, param2);
}

// MATCHED 100%
u32 clFS::func_0201af24(u32 param1, u32 param2) {
    FSChunkTable * tbl = this->fieldbc[param1];
    return (CHUNK_OFFSET(tbl, param2 + 1) - CHUNK_OFFSET(tbl, param2)) - CHUNK_SIZE(tbl, param2);
}

// MATCHED 100%
FSFileReq * clFS::func_0201af54(FSFileReq * param1, u32 param2, u8 * param3, u32 param4, s16 param5) {
    if (param5 >= 0) {
        param1->field24 = param5;
    }

    param1->field26 = param2;
    param1->field10 = param3;
    param1->field14 = param4;

    return param1;
}

// MATCHED 100%
void clFS::func_0201af78(FSFileReq * param1) {
    FSFileReq * r0 = this->fieldac;

    while (r0 != NULL) {
        if (r0 == param1) {
            BOOL run = (this->fieldd0 == NULL);
            while (!run) {
                this->pumpTableLoads();
                run = (this->fieldd0 == NULL);
            }

            while ((param1->field28 & 3) != 0) {
                this->pumpReads();
            }
            break;
        }

        r0 = r0->field00;
    }
}

// MATCHED 100%
void clFS::func_0201b01c(FSFileReq * param1) {
    if ((param1->field28 & 3) != 1) {
        if ((param1->field28 & 3) == 2) {
            FS_CancelFile(&this->fieldb4[param1->field24]);

            param1->field18 = 0;
            param1->field28 |= 0x10;
        }
    } else {
        FSFileReq * r2 = this->fieldac;

        if (r2 == param1) {
            FSFileReq * next = param1->field00;
            this->fieldac = next;
            if (next == NULL) {
                this->fieldb0 = NULL;
            }
        } else {
            while (r2 != NULL) {
                FSFileReq * next = r2->field00;

                if (r2->field00 == param1) {
                    FSFileReq * newNext = param1->field00;
                    r2->field00 = param1->field00;
                    if (newNext == NULL) {
                        this->fieldb0 = r2;
                    }
                    break;
                }

                r2 = next;
            }
        }

        if (param1->fn != NULL) {
            param1->fn(param1, false);
        }

        param1->field28 = 0x10;
    }
}

// MATCHED 100%
void clFS::func_0201b0e4() {
    BOOL run = (this->fieldd0 == NULL);
    while (!run) {
        this->pumpTableLoads();
        run = (this->fieldd0 == NULL);
    }

    run = (this->fieldac == 0);
    while (!run) {
        this->pumpReads();
        run = (this->fieldac == 0);
    }
}

// MATCHED 100%
BOOL func_0201b180(FSOverlayID id, MIProcessor target) {
    return FS_LoadOverlay(target, id);
}

// MATCHED 100%
BOOL clFS::func_0201b198(FSOverlayID id, MIProcessor target) {
    BOOL success = FS_LoadOverlayInfo(&this->field70, target, id);
    if (!success) {

        return FALSE;
    }

    FS_ClearOverlayImage(&this->field70);

    if (FS_IsFile(&this->field28)) {
        FS_CloseFile(&this->field28);
    }

    if (!FS_OpenFileFast(&this->field28, FS_GetOverlayFileID(&this->field70))) {
        return FALSE;
    }

    FS_ClearOverlayImage(&this->field70);

    this->field9c = this->field70.header.ram_address;

    u32 size = this->field70.header.ram_size;
    this->fielda0 = this->field70.header.ram_size;
    this->fielda4 = MATH_MIN(size, CARD_ROM_PAGE_SIZE);
    this->fielda8 = 1;

    return TRUE;
}

// MATCHED 100%
u32 clFS::func_0201b250() {
    if (this->fielda8 != 0) {
        return 2;
    } else {
        FS_StartOverlay(&this->field70);
        return 0;
    }
}

// MATCHED 100%
BOOL func_0201b274(FSOverlayID id, MIProcessor target) {
    return FS_UnloadOverlay(target, id);
}

// MATCHED 100%
BOOL func_0201b28c(u32 dmaNo, void * p_mem, u32 size) {
    data_0205ba8c.dmaNo = dmaNo;

    FS_Init(dmaNo);

    if (p_mem != NULL) {
        BOOL flag = (FS_TryLoadTable(p_mem, size) <= size);
        if (flag) {
            data_0205ba8c.table = p_mem;
        }
    }

    return FS_TryLoadTable(NULL, 0);
}

// MATCHED 100%
BOOL func_0201b2e4(void * p_mem, u32 size) {
    if (data_0205ba8c.table != NULL) {
        return FALSE;
    }

    BOOL flag = (FS_TryLoadTable(p_mem, size) <= size);
    if (!flag) {
        return FALSE;
    } else {
        data_0205ba8c.table = p_mem;
        return TRUE;
    }
}

// MATCHED 100%
bool clFS::func_0201b330(u16 param1, const char * param2) {
    if (FS_IsFile(&this->fieldb4[param1])) {
        FS_CloseFile(&this->fieldb4[param1]);
    }

    FS_OpenFile(&this->fieldb4[param1], param2);

    this->fieldb8[param1] = param2;

    return true;
}

// MATCHED 100%
void clFS::func_0201b390() {
    s32 r7 = this->fieldca_00;
    for (s16 i = 0; i < r7; i++) {
        FSFile * file = &this->fieldb4[(u16)i];
        if (FS_IsFile(file)) {
            FS_CloseFile(file);
        }
    }

    if (FS_IsFile(&this->field28)) {
        FS_CloseFile(&this->field28);
    }
}

// MATCHED 100%
bool clFS::queueTableLoad(u32 param1, FSChunkTable * param2, u32 param3, u32 param4, void (*param5)(UnkStruct_FieldCC *, bool), u32 param6, u32 param7) {
    UnkStruct_FieldCC * r12 = this->fieldcc;
    UnkStruct_FieldCC * r14 = &r12[param1];

    r14->fn = param5;
    r14->field08 = param6;

    this->fieldbc[param1] = param2;

    r14->field0c = (u32 *)this->fieldbc[param1];
    r14->field1c = param1;
    r14->field10 = param3;
    r14->field18 = param4;
    r14->field1e = param7 | 1;
    r12[param1].field00 = 0;

    if (param7 != 0) {
        param3 = MATH_MIN(param3, CARD_ROM_PAGE_SIZE);
    }

    r14->field14 = param3;

    if (this->fieldd4 == NULL) {
        this->fieldd0 = r14;
        this->fieldd4 = r14;
    } else {
        this->fieldd4->field00 = r14;
        this->fieldd4 = this->fieldd4->field00;
    }

    return true;
}

// MATCHED 100%
bool clFS::pumpTableLoads() {
    UnkStruct_FieldCC * r4 = this->fieldd0;
    FSFile * file = &this->fieldb4[r4->field1c];

    if (FS_IsBusy(file)) {
        return false;
    }

    if ((r4->field1e & 3) == 2) {
        if ((r4->field1e & 0x8000) && r4->field10 > CARD_ROM_PAGE_SIZE) {
            r4->field0c += CARD_ROM_PAGE_SIZE / sizeof(u32);
            r4->field18 += CARD_ROM_PAGE_SIZE;
            r4->field10 -= CARD_ROM_PAGE_SIZE;
            if (r4->field10 < CARD_ROM_PAGE_SIZE) {
                r4->field14 = r4->field10;
            }
        } else {
            if (r4->fn != NULL) {
                this->fieldca_07 = 1;
                r4->fn(r4, (r4->field1e & 0x10) == 0);
                this->fieldca_07 = 0;
            }

            r4->field1e &= 0x10;
            this->fieldd0 = r4->field00;
            r4->field00 = NULL;

            r4 = this->fieldd0;
            if (r4 == NULL) {
                this->fieldd4 = NULL;
                return true;
            }

            file = &this->fieldb4[r4->field1c];
            goto start;
        }

        if (!FS_IsFile(file)) {
            this->func_0201b330(r4->field1c, this->fieldb8[r4->field1c]);
            FS_SeekFile(file, r4->field18, FS_SEEK_SET);
        }
    } else {
start:
        r4->field1e = (r4->field1e & ~3) | 2;
        if (!FS_IsFile(file)) {
            this->func_0201b330(r4->field1c, this->fieldb8[r4->field1c]);
        }
        FS_SeekFile(file, r4->field18, FS_SEEK_SET);
    }
    
    FS_ReadFile(file, r4->field0c, r4->field14);
    return true;
}

// MATCHED 100%
bool clFS::queueRead(FSFileReq * param1, u32 param2, s32 param3, void (*param4)(FSFileReq *, bool), u32 param5, u32 param6) {
    if (param3 >= 0) {
        param1->field24 = param3;
    }

    param1->fn = param4;
    param1->field08 = param5;
    param1->field28 = param6 | 1;
    param1->field20 = param2;
    param1->field0c = param1->field10;
    param1->field00 = NULL;

    s32 size = this->func_0201af08((s16)param1->field24, param1->field26) - param2;
    if (param1->field14 != 0) {
        size = param1->field14;
    }

    param1->field18 = size;
    if (size == 0) {
        return false;
    }

    if (param6 != 0) {
        size = MATH_MIN(size, CARD_ROM_PAGE_SIZE);
    }

    param1->field1c = size;

    if (this->fieldb0 == NULL) {
        this->fieldac = param1;
        this->fieldb0 = param1;
    } else {
        this->fieldb0->field00 = param1;
        this->fieldb0 = this->fieldb0->field00;
    }

    return true;
}

// MATCHED 100%
bool clFS::pumpReads() {
    FSFileReq * req = this->fieldac;
    FSFile * file = &this->fieldb4[req->field24];

    if (FS_IsBusy(file)) {
        return false;
    }

    if ((req->field28 & 3) == 2) { 
        if ((req->field28 & 0x8000) && req->field18 > CARD_ROM_PAGE_SIZE) {
            req->field0c += CARD_ROM_PAGE_SIZE;
            req->field20 += CARD_ROM_PAGE_SIZE;
            req->field18 -= CARD_ROM_PAGE_SIZE;
            if (req->field18 < CARD_ROM_PAGE_SIZE) {
                req->field1c = req->field18;
            }
        } else {
            if (req->fn != NULL) {
                this->fieldca_07 = 1;
                req->fn(req, (req->field28 & 0x10) == 0);
                this->fieldca_07 = 0;
            }

            req->field28 &= 0x10;
            this->fieldac = req->field00;
            req->field00 = NULL;

            req = this->fieldac;
            if (req == NULL) {
                this->fieldb0 = NULL;
                return true;
            }

            file = &this->fieldb4[req->field24];
            goto start;
        }

        if (!FS_IsFile(file)) {
            this->func_0201b330(req->field24, this->fieldb8[req->field24]);
            FS_SeekFile(file, req->field20 + CHUNK_OFFSET(this->fieldbc[req->field24], req->field26), FS_SEEK_SET);
        }
    } else {
start:
        req->field28 = (req->field28 & ~3) | 2;

        if (!FS_IsFile(file)) {
            this->func_0201b330(req->field24, this->fieldb8[req->field24]);
        }
        FS_SeekFile(file, req->field20 + CHUNK_OFFSET(this->fieldbc[req->field24], req->field26), FS_SEEK_SET);
    }

    FS_ReadFile(file, req->field0c, req->field1c);
    return true;
}

// MATCHED 100%
bool clFS::pumpOverlay() {
    if (FS_IsBusy(&this->field28)) {
        return false;
    }

    if (this->fielda8 == 2) {
        if (this->fielda0 <= CARD_ROM_PAGE_SIZE) {
            if (FS_IsFile(&this->field28)) {
                FS_CloseFile(&this->field28);
            }

            this->fielda8 = 0;

            return true;
        }

        this->field9c += CARD_ROM_PAGE_SIZE;
        this->fielda0 -= CARD_ROM_PAGE_SIZE;
        if (this->fielda0 < CARD_ROM_PAGE_SIZE) {
            this->fielda4 = this->fielda0;
        }
    } else {
        this->fielda8 = 2;
    }

    if (!FS_IsFile(&this->field28)) {
        if (!FS_OpenFileFast(&this->field28, FS_GetOverlayFileID(&this->field70))) {
            return false;
        }

        FS_SeekFile(&this->field28, this->field70.header.ram_size - this->fielda0, FS_SEEK_SET);
    }

    FS_ReadFile(&this->field28, this->field9c, this->fielda4);

    return true;
}

// MATCHED 100%
clFSEx::clFSEx(u8 param1, u32 param2, u16 param3, u16 param4, u32 param5, u32 param6) : clDPP::clDPP(param1, param2, param4, param5, param6) {
    u32 heap = GET_HEADER(this)->field08_heap;
    this->fieldc10 = (FSFileReqEx *)func_02019eac(heap, param3 * sizeof(FSFileReqEx), NULL, 0);
    this->fieldc0c = (ChunkSizeFn *)func_02019eac(heap, param4 * sizeof(ChunkSizeFn), NULL, 0);;
    this->fieldc14 = NULL;
    this->fieldc18 = (u8*)this->fieldc10 + param3 * sizeof(FSFileReqEx) * sizeof(FSFileReqEx);

    func_01ff8228(this->fieldc10, 0, param3 * sizeof(FSFileReqEx));
    func_01ff8228(&this->fieldc1c, 0, 2 * sizeof(u32));

    FSFileReqEx * ptr = this->fieldc10;
    while (param3 != 0) {
        ptr->field00 = this->fieldc14;
        this->fieldc14 = ptr;
        param3--;
        ptr++;
    }
}

// MATCHED 100%
clFSEx::~clFSEx() {
    s16 size = this->fieldca_00;
    for (s16 r7 = 0; r7 < size; r7++) {
        if ((this->fieldc1c[r7 >> 5] & (1 << (r7 & 0x1f))) != 0) {
            this->func_0201ae68(r7);
            this->fieldc1c[r7 >> 5] &= ~(1 << (r7 & 0x1f));
        } else {
            func_0201a058(this->func_0201ae68(r7));
        }
    }

    func_0201a058(this->fieldc10);
    func_0201a058(this->fieldc0c);
}

// MATCHED 100%
bool clFSEx::func_0201bdfc(s16 param1, const char * param2, void (*param3)(UnkStruct_FieldCC *, bool), u32 param4, u32 param5, u32 param6, u32 param7, u32 param8) {
    if (this->fieldbc[param1] != NULL) {
        if ((this->fieldc1c[param1 >> 5] & 1 << (param1 & 0x1f)) != 0) {
            this->func_0201ae68(param1);
            this->fieldc1c[param1 >> 5] &= ~(1 << (param1 & 0x1f));
        } else {
            func_0201a058(this->func_0201ae68(param1));
        }
        this->fieldbc[param1] = NULL;
    }

    u32 size = this->func_0201adf8(param1, param2, param5);
    this->fieldc0c[param1] = (param5 == 0) ? data_02054c94[2] : data_02054c94[1];
    
    FSChunkTable * tbl = (FSChunkTable *)func_02019eac(param6, size, NULL, param8);
    this->queueTableLoad(param1, tbl, size, 0, param3, param4, param7);

    return true;
}

// MATCHED 100%
void clFSEx::func_0201bf34(s16 param1, const char * param2, FSChunkTable * param3, u32 param4) {
    if (this->fieldbc[param1] != NULL) {
        if ((this->fieldc1c[param1 >> 5] & 1 << (param1 & 0x1f)) != 0) {
            this->func_0201ae68(param1);
            this->fieldc1c[param1 >> 5] &= ~(1 << (param1 & 0x1f));
        } else {
            func_0201a058(this->func_0201ae68(param1));
        }
        this->fieldbc[param1] = NULL;
    }

    this->func_0201b330(param1, param2);

    this->fieldc0c[param1] = (param4 == 0) ? data_02054c94[0] : data_02054c94[3];
    this->fieldbc[param1] = param3;
    this->fieldc1c[param1 >> 5] |= (1 << (param1 & 0x1f));
}

// MATCHED 100%
u8 * clFSEx::func_0201c044(s16 param1, u32 param2, u32 param3, u8 * param4, s32 * param5, void (*param6)(FSFileReq *, bool), u32 param7, u32 param8, u32 param9, u32 param10) {
    u32 size = (this->*this->fieldc0c[param1])(param1, param2);
    if (size == 0) {
        if (param5 != NULL) {
            *param5 = 0;
        }
        return NULL;
    }

    FSFileReqEx * res = this->func_0201c344();
    if (res == NULL) {
        if (param5 != NULL) {
            *param5 = -1;
        }
        return NULL;
    }

    u32 oldSize = size;
    if (param4 == 0) {
        param4 = (u8 *)func_02019eac(param9, size, NULL, param10);
    } else if (param5 != NULL && *param5 != 0) {
        size = *param5;
    }

    if (param5 != NULL) {
        *param5 = oldSize;
    }

    if (param4 == NULL) {
        return NULL;
    }

    res->field2c = param4;

    FSFileReq * req = this->func_0201af54(res, param2, param4, size, -1);
    this->clDPP::queueRead(req, param3, param1, param6, param7, param8);

    return param4;
}

// MATCHED 100%
void clFSEx::func_0201c194() {
    FSFileReq * req = this->fieldac;
    while (req != NULL) {
        this->func_0201c890(req);

        BOOL flag = (req->field28 & 3) == 0;
        if (!flag) {
            req = req->field00;
        } else {
            FSFileReq * oldReq = req;
            req = req->field00;
            oldReq->field00 = this->fieldc14;
            this->fieldc14 = (FSFileReqEx *)oldReq;
        }
    }
}

// MATCHED 100%
u8 * clFSEx::func_0201c1f8(s16 param1, u32 param2, u8 * param3, u32 param4, u8 * param5, s32 * param6, void (*param7)(FSFileReq *, bool), u32 param8, s32 param9, u32 param10) {
    if (this->func_0201af08(param1, param2) == 0) {
        if (param6 != NULL) {
            *param6 = 0;
        }
        return NULL;
    }

    FSFileReqEx * req = this->func_0201c344();
    if (req == NULL) {
        if (param6 != NULL) {
            *param6 = -1;
        }
        return NULL;
    }

    param4 = this->func_0201c700((FSFileReqEx *)this->func_0201af54(req, param2, param3, param4, param1), -1);

    if (param6 != NULL) {
        *param6 = param4;
    }

    if (param5 == NULL) {
        param5 = (u8 *)func_02019eac(param9, param4, NULL, param10);
    }

    if (param5 == NULL) {
        return NULL;
    }

    this->func_0201c7a0(req, (u32*)param5, param7, param8);

    return param5;
}

// MATCHED 100%
bool clFSEx::pumpReads() {
    FSFileReqEx * req = (FSFileReqEx *)this->fieldac;
    if (!this->clDPP::pumpReads()) {
        return false;
    }

    if (req != this->fieldac && req >= this->fieldc10 && req < this->fieldc18) {
        req->field00 = this->fieldc14;
        this->fieldc14 = req;
    }

    return true;
}

// MATCHED 100%
FSFileReqEx * clFSEx::func_0201c344() {
    FSFileReqEx * req = this->fieldc14;
    if (req == NULL) {
        return NULL;
    } else {
        this->fieldc14 = (FSFileReqEx *)req->field00;
        return req;
    }
}

// MATCHED 100%
clDAP::clDAP(u8 param1, u32 param2, u32 param3) : clTask::clTask(param1, param2, param3) {
    this->field2c = NULL;
    this->field28 = NULL;
    this->field30 = 0xa8;
    this->field34 = 5;
    this->field36 = 2;
    this->field38 = 0;
}

// MATCHED 100%
void clDAP::run() {
    bool flag = false;
    if (data_0205b7a8.field26 == 0) {
        bool ready = true;

        if (reg_GX_VCOUNT >= this->field30 && reg_GX_VCOUNT <= (s16)HW_LCD_HEIGHT) {
            ready = false;
        }

        if (ready) {
            flag = true;
        }
    }

    if (flag) {
        this->field38 = 0;
    } else if (this->field38 >= this->field36) {
        this->field38 = this->field34;
    } else if (++this->field38 != this->field34+1) {
        return;
    }

    bool run = true;
    while (run) {
        BOOL flag = (this->field28 == NULL) ? TRUE : FALSE;
        if (flag) {
            this->field38 = 0;
            break;
        }

        this->func_0201c5c8();

        run = false;

        if (data_0205b7a8.field26 == 0) {
            bool ready = true;

            if (reg_GX_VCOUNT >= this->field30 && reg_GX_VCOUNT <= (s16)HW_LCD_HEIGHT) {
                ready = false;
            }

            if (ready) {
                run = true;
            }
        }
    }
}

// MATCHED 100%
u32 clDAP::func_0201c4f4(FSFileReqEx * param1) {
    param1->field0c = param1->field10;

    u32 local;
    func_01ff8000(&param1->field0c, &local, &param1->field34);

    param1->field38 = local;
    param1->field2a = 1;
    return param1->field34;
}

// MATCHED 100%
void clDAP::func_0201c534(FSFileReqEx * param1, u32 * param2) {
    param1->field2c = param2;
    param1->field30 = param2;
    param1->field2a = 2;
    param1->field00 = NULL;

    if (this->field2c != NULL) {
        this->field2c->field00 = param1;
        this->field2c = (FSFileReqEx *)this->field2c->field00;
    } else {
        this->field28 = param1;
        this->field2c = param1;
    } 
}

// MATCHED 100%
void clDAP::func_0201c574(FSFileReq * param1) {
    FSFileReq * req = this->field28;
    while (req != NULL) {
        if (req == param1) {
            while (param1->field2a != 0) {
                this->func_0201c5c8();
            }
            break;
        }

        req = req->field00;
    }
}

// MATCHED 100%
void clDAP::func_0201c5c8() {
    this->field28->field0c += 2;

    if (this->field28->field30 >= (void *)HW_PLTT && this->field28->field30 < (void *)HW_OAM_END) {
        func_01ff8150(&this->field28->field0c, &this->field28->field30);
    } else {
        func_01ff80a4(&this->field28->field0c, &this->field28->field30);
    }

    this->field28->field2a = 3;

    if (--this->field28->field38 == 0) {
        FSFileReqEx * req = this->field28;
        req->field2a = 0;
        this->field28 = (FSFileReqEx *)req->field00;
        req->field00 = NULL;
        if (this->field28 == NULL) {
            this->field2c = NULL;
        }
    }
}

// MATCHED 100%
clDPP::clDPP(u8 param1, u32 param2, u16 param3, u32 param4, u32 param5) : clFS::clFS(param1, param2, param3, param4, param5) {
    this->alignedBuf = (u8*)((u32)(this->buffer + 0x1f) & ~0x1f);
    this->bufIdx = 0;
}

// MATCHED 100%
u32 clDPP::func_0201c700(FSFileReqEx * param1, s16 param2) {
    if (param2 >= 0) {
        param1->field24 = param2;
    }

    u32 res = this->func_0201af08((s16)param1->field24, param1->field26);
    if (res == 0) {
        return 0;
    }

    u8 arr[0x8];
    param1->field0c = arr;
    param1->field1c = 8;
    param1->field20 = 0;

    this->func_0201c8d8(param1, 0);

    u32 local;
    func_01ff8000(&param1->field0c, &local, &param1->field34);

    param1->field20 = param1->field0c - (u8*)arr;
    param1->field18 = res - param1->field20;
    param1->field2a = 1;
    param1->field38 = local;
    return param1->field34;
}

// MATCHED 100%
void clDPP::func_0201c7a0(FSFileReqEx * param1, u32 * param2, void (*param3)(FSFileReq *, bool), u32 param4) {
    param1->fn = param3;
    param1->field08 = param4;
    param1->field2c = param2;
    param1->field30 = param2;
    param1->field28 = 1;
    param1->field2a = 2;
    param1->field00 = NULL;

    if (param1->field10 == NULL) {
        param1->field10 = this->alignedBuf;
        param1->field14 = 0xb00;
        param1->field1c = CARD_ROM_PAGE_SIZE;
    } else {
        if (data_0205ba8c.dmaNo < 4) {
            u32 alignment = ((u32)param1->field10 & 0x1f);
            if (((u32)param1->field10 & 0x1f) != 0) {
                param1->field14 -= 32 - alignment;
            }
            param1->field10 = (u8*)(((u32)param1->field10 + 0x1f) & ~0x1f);
            param1->field1c = -CARD_ROM_PAGE_SIZE & (param1->field14 / 6);
        } else {
            param1->field1c = param1->field14 / 6;
        }
    }

    if (this->fieldb0 != NULL) {
        this->fieldb0->field00 = param1;
        this->fieldb0 = this->fieldb0->field00;
    } else {
        this->fieldac = param1;
        this->fieldb0 = param1;
    }
}

// MATCHED 100%
void clDPP::func_0201c890(FSFileReq * param1) {
    param1->field2a = 0;
    this->func_0201b01c(param1);
}

// MATCHED 100%
bool clDPP::queueRead(FSFileReq * param1, u32 param2, s32 param3, void (*param4)(FSFileReq *, bool), u32 param5, u32 param6) {
    param1->field2a = 0;
    return clFS::queueRead(param1, param2, param3, param4, param5, param6);
}

// MATCHED 100%
void clDPP::func_0201c8d8(FSFileReqEx * param1, u32 param2) {
    FSFile * file = &this->fieldb4[param1->field24];

    if (FS_IsBusy(file)) {
        FS_WaitAsync(file);
    }

    if (!FS_IsFile(file)) {
        this->func_0201b330(param1->field24, this->fieldb8[param1->field24]);
    }

    FSFile newFile;
    FS_InitFile(&newFile);
    FS_CreateFileFromRom(&newFile, file->prop.file.top, file->prop.file.bottom - file->prop.file.top);
    FS_SeekFile(&newFile, param1->field20 + CHUNK_OFFSET(this->fieldbc[param1->field24], param1->field26), FS_SEEK_SET);
    FS_ReadFile(&newFile, param1->field0c, param1->field1c);
    FS_CloseFile(&newFile);
}

// MATCHED 100%
bool clDPP::pumpReads() {
    FSFileReqEx * doneReq;
    FSFile * file;
    FSFileReqEx * req;
    s32 remaining;
    u8 * ptr;
    u8 * cursor;
    
    req = (FSFileReqEx *)this->fieldac;
    cursor = NULL;
    file = &this->fieldb4[req->field24];

    if (FS_IsBusy(file)) {
        return false;
    }

    if ((req->field28 & 3) == 2) {
        if (data_0205ba8c.dmaNo < 4 &&
            (req->field2a != 0 || ((req->field20 + CHUNK_OFFSET(this->fieldbc[req->field24], req->field26)) & 0x1ff) == 0) &&
            ((u32)req->field0c & 0x1f) == 0 && 
            (req->field1c & 0x1ff) == 0) {
                    DC_InvalidateRange(req->field0c, req->field1c);
        }

        if (req->field2a != 0) {
            this->func_0201ced8(req);
            doneReq = req;
            cursor = this->cursor;
            remaining = this->remaining;
            if (req->field18 > 0) {
                req->field0c = ((u8*)req->field10 + req->field1c * this->bufIdx);
                req->field18 -= req->field1c;
                req->field20 += req->field1c;
                if (++this->bufIdx >= 4) {
                    this->bufIdx = 0;
                }
            } else {
                goto pop;
            }
        } else if ((req->field28 & 0x8000) && req->field18 > CARD_ROM_PAGE_SIZE) {
            req->field0c += CARD_ROM_PAGE_SIZE;
            req->field20 += CARD_ROM_PAGE_SIZE;
            req->field18 -= CARD_ROM_PAGE_SIZE;
            if (req->field18 < CARD_ROM_PAGE_SIZE) {
                req->field1c = req->field18;
            }
        } else {
            if (req->fn != NULL) {
                this->fieldca_07 = 1;
                req->fn(req, (req->field28 & 0x10) == 0);
                this->fieldca_07 = 0;
            }
pop:
            this->fieldac = req->field00;
            req->field00 = NULL;
            req->field28 &= 0x10;
            req = (FSFileReqEx *)this->fieldac;
            if (req == NULL) {
                this->fieldb0 = NULL;
                goto drain;
            }
            file = &this->fieldb4[req->field24];
            goto start;
        }

        if (!FS_IsFile(file)) {
            this->func_0201b330(req->field24, this->fieldb8[req->field24]);
            FS_SeekFile(file, req->field20 + CHUNK_OFFSET(this->fieldbc[req->field24], req->field26), FS_SEEK_SET);
        }
    } else {
start:
        if (req->field2a != 0) {
            u32 align = (req->field20 + CHUNK_OFFSET(this->fieldbc[req->field24], req->field26)) & 0x1ff;
            this->remaining = req->field1c - align;
            req->field18 -= this->remaining;
            ptr = req->field10;
            req->field0c = ptr + req->field1c * this->bufIdx;
            req->field20 -= align;
            this->cursor = req->field0c + align;
            this->bufIdx++;
            if (this->bufIdx >= 4) {
                this->bufIdx = 0;
            }
        }

        req->field28 = (req->field28 & ~3) | 2;

        if (!FS_IsFile(file)) {
            this->func_0201b330(req->field24, this->fieldb8[req->field24]);
        }
        FS_SeekFile(file, req->field20 + CHUNK_OFFSET(this->fieldbc[req->field24], req->field26), FS_SEEK_SET);
    }
    FS_ReadFileAsync(file, req->field0c, req->field1c);

drain:
    if (cursor != NULL && doneReq->field38 != 0) {
        ptr = &doneReq->field10[doneReq->field1c * 4];
        if (cursor == ptr) {
            cursor = doneReq->field10;
        }

        u16 r4 = (cursor[0] | cursor[1] << 8) + 2;
        if (&cursor[2] > ptr) {
            r4 = (remaining >= 2) ? (cursor[0] | doneReq->field10[0] << 8) + 2 : 0xffff;
        }

        u8 * local;
        while (remaining > 1 && r4 <= remaining) {
            local = cursor + 2;
            if (cursor + r4 > ptr) {
                u32 sz = (u32)(cursor + ((u8*)r4 - ptr));
                MI_CpuCopy8(doneReq->field10, ptr, sz);
                cursor = doneReq->field10 + sz;
            } else {
                cursor += (s32)r4;
            }

            this->fn_d8(&local, &doneReq->field30);
            if (--doneReq->field38 == 0) {
                if (doneReq->fn != NULL) {
                    this->fieldca_07 = 1;
                    doneReq->fn(doneReq, (doneReq->field28 & 0x10) == 0);
                    this->fieldca_07 = 0;
                }
                doneReq->field2a = 0;
                goto done;
            }

            remaining -= r4;

            r4 = (cursor[0] | (u16)(cursor[1] << 8)) + 2;
            if (&cursor[2] > ptr) {
                r4 = (remaining >= 2) ? (cursor[0] | (u16)(doneReq->field10[0] << 8)) + 2 : 0xffff;
            }
        }

        this->cursor = cursor;
        this->remaining = remaining + doneReq->field1c;
    }
done:

    if (FS_IsBusy(file)) {
        FS_WaitAsync(file);
    }

    return true;
}

// MATCHED 100%
void clDPP::func_0201ced8(FSFileReqEx * param1) {
    bool inGraphicsRAM = false;
    if (param1->field30 >= (void *)HW_PLTT && param1->field30 < (void *)HW_OAM_END) {
        inGraphicsRAM = true;
    }

    this->fn_d8 = (inGraphicsRAM) ? func_01ff8150 : func_01ff80a4;
    param1->field2a = 3;
}

} // extern "C"
