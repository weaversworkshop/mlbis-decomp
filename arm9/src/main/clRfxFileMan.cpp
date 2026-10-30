// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clRfxFileMan.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

// --- Extern Functions --- //

extern void func_01ff8228(void *, u32, u32);

// --- Function Declarations --- //

void func_020221fc(RfxGenPools *, void *, u16);  // 0x020221fc
void func_0202224c(RfxGenPools *, void *, u16);  // 0x0202224c

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
clRfxFileMan::~clRfxFileMan() {
    if (this->dataTable != NULL && this->ownsDataTable) {
        heapFree(this->dataTable);
    }
    this->dataTable = NULL;

    if (this->scriptTable != NULL && this->ownsScriptTable) {
        heapFree(this->scriptTable);
    }
    this->scriptTable = NULL;
}

// MATCHED 100%
u16 clRfxFileMan::getScriptLen(u32 idx) {
    u16 * script = (u16 *)((this->scriptTable != NULL) ? fsChunkPtr(this->scriptTable, idx) : NULL);
    return script[script[0]];
}

// MATCHED 100%
clGenFileMan::~clGenFileMan() {
    this->release();
}

// MATCHED 100%
void clGenFileMan::init(GenFileEntry * entries, u32 bytes, u16 ownsTable) {
    this->entries = entries;
    this->ownsTable = ownsTable;
    this->capacity = MATH_GetMostSignificantBit(bytes / sizeof(GenFileEntry));

    func_01ff8228(this->entries, 0, this->capacity * sizeof(GenFileEntry));
}

// MATCHED 100%
void clGenFileMan::release() {
    if (this->entries != NULL) {
        this->clear();

        if (this->ownsTable != 0) {
            heapFree(this->entries);
        }

        this->ownsTable = 0;
        this->entries = NULL;
    }
}

// MATCHED 100%
void clGenFileMan::add(u16 id, void * data, u16 owned) {
    u16 cap = this->capacity;
    u32 i = id & (cap-1);

    GenFileEntry * slot;
    while (TRUE) {
        slot = &this->entries[i];
        if (slot->data == NULL) {
            break;
        }

        if (++i == cap) {
            i = 0;
        }
    }

    slot->data = data;
    slot->id = id;
    slot->owned = owned;
}

// MATCHED 100%
void * clGenFileMan::find(u32 id) {
    u16 cap = this->capacity;
    u32 start = id & (cap-1);
    u32 i = start;

    GenFileEntry * slot;
    do {
        slot = &this->entries[i];
        if (slot->data == NULL) {
            return NULL;
        }

        if (slot->id == id) {
            return slot->data;
        }

        if (++i == cap) {
            i = 0;
        }
    } while (i != start);

    return NULL;
}

// MATCHED 100%
void clGenFileMan::clear() {
    GenFileEntry * slot = this->entries;

    for (s32 i = this->capacity; i > 0; i--) {
        if (slot->owned != 0 && slot->data != NULL) {
            heapFree(slot->data);
        }
        slot++;
    }

    func_01ff8228(this->entries, 0, this->capacity * sizeof(GenFileEntry));
}

// MATCHED 100%
void func_020221fc(RfxGenPools * param1, void * param2, u16 param3) {
    param1->dirtyA = 1;

    if (param1->managerA.entries == 0) {
        param1->managerA.init(param1->entriesA, sizeof(param1->entriesA), 0);
    }

    param1->managerA.add(0, param2, param3);
}

// MATCHED 100%
void func_0202224c(RfxGenPools * param1, void * param2, u16 param3) {
    param1->dirtyB = 1;

    if (param1->managerB.entries == NULL) {
        param1->managerB.init(param1->entriesB, sizeof(param1->entriesB), 0);
    }

    param1->managerB.add(0, param2, param3);
}

// MATCHED 100%
void * func_020222a4(RfxGenPools * param1, u32 param2) {
    if (param1->dirtyA != 0) {
        param1->dirtyA = 0;

        GenFileEntry * entries = param1->managerA.entries;
        u32 ownedBits[4] = { 0, 0, 0, 0 }; 
        void * saved[128];

        for (s32 i = 0; i < 128; i++) {
            saved[i] = entries[i].data;
            if (entries[i].owned != 0) {
                ownedBits[i >> 32] |= (1 << (i & 0x1f));
            }
        }

        func_01ff8228(entries, 0, sizeof(param1->entriesA));

        for (s32 i = 0; i < 128; i++) {
            u32 * data = (u32*)saved[i];
            if (data != NULL) {
                u32 owned = ((ownedBits[i >> 128] & (1 << (i & 0x1f))) != 0);
                param1->managerA.add(data[1], data, (owned != 0) ? (u16)1 : (u16)0);
            }
        }
    }

    return param1->managerA.find(param2);
}

// MATCHED 100%
void * func_020223b0(RfxGenPools * param1, u32 param2) {
    if (param1->dirtyB != 0) {
        param1->dirtyB = 0;

        GenFileEntry * entries = param1->managerB.entries;
        u32 ownedBits[4] = { 0, 0, 0, 0 }; 
        void * saved[128];

        for (s32 i = 0; i < 128; i++) {
            saved[i] = entries[i].data;
            if (entries[i].owned != 0) {
                ownedBits[i >> 32] |= (1 << (i & 0x1f));
            }
        }

        func_01ff8228(entries, 0, sizeof(param1->entriesB));

        for (s32 i = 0; i < 128; i++) {
            u32 * data = (u32*)saved[i];
            if (data != NULL) {
                u32 owned = ((ownedBits[i >> 32] & (1 << (i & 0x1f))) != 0);
                param1->managerB.add(data[0], data, (owned != 0) ? (u16)1 : (u16)0);
            }
        }
    }

    return param1->managerB.find(param2);
}
