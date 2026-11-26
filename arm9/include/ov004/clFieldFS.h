#ifndef OV004_CL_FIELD_FS_H
#define OV004_CL_FIELD_FS_H

#include "types.h"

#ifdef __cplusplus

#include "ov003/clGuideMapFS.h"
#include "ov004/clField.h"

class clFieldFS : public clGuideMapFS {
public:
    const FSChunkTable * fieldc24;
    const FSChunkTable * fieldc28;
    const FSChunkTable * fieldc2c;
    const FSChunkTable * fieldc30;
    FSChunkTable * baseEventTable;  // +0xc34

    clFieldFS(u8);  // C1 0x020872a0
    virtual ~clFieldFS();  // D1 0x020873b8 D0 0x020873cc

    u8 * readStreamBelow(s16, u16, u8 *, u32 *);  // 0x020873e8
    u32 getStreamSize(s16, u32);  // 0x02087470
    u8 * loadEventTable(clField *);  // 0x020874b8
};

#endif

#endif
