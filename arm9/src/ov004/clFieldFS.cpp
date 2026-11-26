// objdiff-target: build/delinks/_dsd_gap@ov004_5.o

#include "globals.h"

#include "ov004/clFieldFS.h"

#include "main/heap.h"

extern "C" {

#include <nitro.h>

// --- Variable Definitions --- //

#include "data/clFieldFS.h"

// --- Function Declarations --- //

void eventTableLoaded(FSFileReq *, s32);  // 0x0208754c

}

// --- Function Definitions --- //

// MATCHED 100%
clFieldFS::clFieldFS(u8 priority) : clGuideMapFS(priority, 13) {
    this->fieldc24 = &data_ov004_020d562c;
    this->fieldc28 = &data_ov004_020d2114;
    this->fieldc2c = &data_ov004_020e3cf8;
    this->fieldc30 = &data_ov004_020d28c8;

    this->setArchive(8, "/Treasure/TreasureInfo.dat", &treasureInfoTable, FALSE);
    this->setArchive(9, "/FRfx/FOfxTex.dat", &fofxTexTable, FALSE);
    this->setArchive(10, "/FRfx/FOfxPal.dat", &fofxPalTable, FALSE);
    this->setArchive(11, "/FRfx/FDfxTex.dat", &fdfxTexTable, FALSE);
    this->setArchive(12, "/FRfx/FDfxPal.dat", &fdfxPalTable, FALSE);

    this->baseEventTable = NULL;
}

// MATCHED 100%
clFieldFS::~clFieldFS() {}

// MATCHED 100%
u8 * clFieldFS::readStreamBelow(s16 slot, u16 chunkIdx, u8 * bufEnd, u32 * pSize) {
    FSFileReqEx * req = this->allocReq();
    if (req == NULL) {
        return NULL;
    }

    u32 size = this->readStreamHeader((FSFileReqEx *)this->setupReq(req, chunkIdx, NULL, 0, slot), -1);

    bufEnd -= size;

    if (pSize != NULL) {
        *pSize = size;
    }

    this->queueStream(req, bufEnd, NULL, 0);

    return bufEnd;
}

// MATCHED 100%
u32 clFieldFS::getStreamSize(s16 slot, u32 chunkIdx) {
    if (this->getChunkSize(slot, chunkIdx) == 0) {
        return 0;
    }

    FSFileReqEx req;
    req.slot = slot;
    req.chunkIdx = chunkIdx;

    return this->readStreamHeader(&req, -1);
}

// MATCHED 100%
u8 * clFieldFS::loadEventTable(clField * field) {
    s32 size = 0x128;
    u8 * buf = (u8 *)heapAlloc(field->unk_virtual_14(), size, NULL, FALSE);
    return this->readChunk(4, field->fieldc0*3 + 2, 0, buf, &size, eventTableLoaded, this, 0x8000, 1, FALSE);
}

// MATCHED 100%
void eventTableLoaded(FSFileReq * req, s32 unused) {
    clFieldFS * fs = (clFieldFS *)req->arg;
    u32 * offset = (u32 *)req->buf;
    u32 base = fs->tables[4]->offsets[req->chunkIdx];

    for (s32 n = req->bufSize / 4; n != 0; n--) {
        *offset += base;
        offset++;
    }

    FSChunkTable * tbl = (FSChunkTable *)req->buf;
    fs->baseEventTable = fs->tables[4];
    fs->tables[4] = tbl;
}
