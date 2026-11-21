#ifndef OV003_CL_GUIDE_MAP_FS_H
#define OV003_CL_GUIDE_MAP_FS_H

#include "types.h"

#ifdef __cplusplus

#include "main/fs.h"

class clGuideMapFS : public clFSEx {
public:

    clGuideMapFS(u8, u16);  // C2 0x0206b3ec C1 0x0206b598
    virtual ~clGuideMapFS();  // D1 0x0206b744 D0 0x0206b758 D2 0x0206b774
    void run(void);  // 0x0206b788
};

#endif

#endif
