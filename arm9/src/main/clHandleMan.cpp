// objdiff-target: build/delinks/_dsd_gap@main_7.o

#include "globals.h"

#include "main/clHandleMan.h"

extern "C" {

#include <nitro.h>

#include "main/heap.h"

// --- Variable Definitions --- //

nsObjSys::clHandleMan * handleMan;  // 0x0205c1ac

nsObjSys::clVramManBase * vramManTable[2][4][4];  // 0x0205c1b0

// --- Function Declarations --- //

nsObjSys::clVramManBase * getVramManOfHandle(VramHandle *);  // 0x020287e0

} // extern "C"

// --- Function Definitions --- //

// MATCHED 100%
nsObjSys::clVramManBase * getVramManOfHandle(VramHandle * param1) {
    u8 index = param1->manIndex;
    u8 type = param1->manType;
    u8 bank = param1->manBank;

    if (param1->manIndex >= 4) {
        return NULL;
    }

    return vramManTable[bank][type][index];
}

namespace nsObjSys {

// MATCHED 100%
clHandleMan::clHandleMan(u32 heap, u32 count) {
    handleMan = this;

    VramHandle * pool;
    if (count == 0) {
        pool = NULL;
    } else {
        pool = (VramHandle *)::operator new[](count * sizeof(VramHandle), heap, NULL, FALSE);
    }

    this->pool = pool;
    this->freeList = 0;
    this->used = 0;

}

// MATCHED 100%
clHandleMan::~clHandleMan() {
    handleMan = NULL;

    if (this->pool != NULL) {
        delete[] this->pool;
        this->pool = NULL;
    }
}

// MATCHED 100%
VramHandle * clHandleMan::alloc() {
    VramHandle * handle = this->freeList;
    if (this->freeList != NULL) {
        this->freeList = handle->next;
        return handle;
    }

    VramHandle * pool = this->pool;
    return pool + (this->used++);
}

// MATCHED 100%
void clHandleMan::free(VramHandle * handle) {
    handle->next = this->freeList;
    this->freeList = handle;
}


inline VramHandle * clVramManByteBase::insert(VramHandle * next, VramHandle * prev, u32 offset, u32 size, u32 id, u16 attr) {
    VramHandle * handle = handleMan->alloc();
    handle->next = next;
    handle->prev = prev;
    handle->offset = offset;
    handle->size = size;
    handle->id = id;
    handle->refCount = 1;
    handle->attr = attr;

    if (prev == NULL) {
        this->head[0] = handle;
    } else {
        prev->next = handle;
    }

    if (next == NULL) {
        this->head[1] = handle;
    } else {
        next->prev = handle;
    }

    return handle;
}

// MATCHED 100%
VramHandle * clVramManByteBase::alloc(u32 size, u32 id, u32 attr) {
    u32 base;
    u32 off;

    u32 manBits = (this->manBank << 2) | (this->manType << 3)  | (this->manIndex << 6);
    u32 side = (attr & 1);
    BOOL share = (attr & 4);

    attr = (attr & ~4) | manBits;

    if (share) {
        VramHandle * shared = this->findShared(id, attr, 3, 0, 1);
        if (shared != NULL) {
            return shared;
        }
    }

    VramHandle * cur = this->head[side];
    if (cur == NULL) {
        if (side == 0) {
            side = this->base;
        } else {
            side = this->base + this->size - size;
        }

        return this->insert(NULL, NULL, side, size, id, attr);
    }

    if (side == 0) {
        VramHandle * last = NULL;
        base = this->base;
        off = base;

        while (cur != NULL) {
            u32 curOffset = cur->offset;
            if (curOffset - off >= size) {
                return this->insert(cur, last, off, size, id, attr);
            }

            last = cur;
            off = curOffset + cur->size;
            cur = cur->next;
        }

        if (last == this->head[1]) {
            off = last->offset + last->size;
            if (base + this->size - off >= size) {
                return this->insert(cur, last, off, size, id, attr);
            }
        }
    } else {
        VramHandle * last = NULL;
        base = this->base;
        off = base + this->size;

        while (cur != NULL) {
            u32 curOffset = cur->offset;
            if (off - (curOffset + cur->size) >= size) {
                return this->insert(last, cur, off - size, size, id, attr);
            }

            last = cur;
            cur = cur->prev;
            off = curOffset;
        }

        if (last == this->head[0]) {
            if (off - base >= size) {
                return this->insert(last, cur, off - size, size, id, attr);
            }
        }
    }

    return NULL;
}

// MATCHED 100%
VramHandle * clVramManByteBase::allocFrom(u32 size, u32 id, u32 attr, u32 hint) {
    u32 gapStart;
    VramHandle * cur;
    u32 gapSize;
    u32 off;

    u32 manBits = (this->manBank << 2) | (this->manType << 3) | (this->manIndex << 6);
    u32 side = (attr & 1);
    BOOL share = attr & 4;

    attr = (attr & ~4) | manBits;

    if (share) {
        VramHandle * shared = this->findShared(id, attr, 3, hint, 1);
        if (shared != NULL) {
            return shared;
        }
    }

    u32 base = this->base;
    BOOL hasHint = (hint != 0);

    cur = this->head[side];
    hint += base;
    if (cur == NULL) {
        if (side != 0) {
            if (hint != 0) {
                hint = hint - size;
            } else {
                hint = base + this->size - size;
            }
        }
        return this->insert(NULL, NULL, hint, size, id, attr);
    }

    VramHandle * last = NULL;
    if (side == 0) {
        off = base;

        if (hasHint) {
            while (cur != NULL) {
                u32 curOffset = cur->offset;
                gapStart = off;
                gapSize = curOffset - off;

                if (off + gapSize > hint) {
                    break;
                }

                last = cur;
                off = curOffset + cur->size;
                cur = cur->next;
            }

            if (last == this->head[1]) {
                gapStart = off;
                gapSize = base + this->size - off;
            }

            if (gapStart < hint) {
                gapSize -= hint - gapStart;
                gapStart = hint;
            }

            if (gapSize >= size) {
                return this->insert(cur, last, gapStart, size, id, attr);
            }

            if (cur != NULL) {
                last = cur;
                cur = cur->next;

                if (last != NULL) {
                    off = last->offset + last->size;
                }
            } else {
                return NULL;
            }
        }

        while (cur != NULL) {
            u32 curOffset = cur->offset;
            if (curOffset - off >= size) {
                return this->insert(cur, last, off, size, id, attr);
            }

            last = cur;
            off = curOffset + cur->size;
            cur = cur->next;
        }

        if (last == this->head[1]) {
            off = last->offset + last->size;
            if (base + this->size - off >= size) {
                return this->insert(cur, last, off, size, id, attr);
            }
        }
    } else {
        off = base + this->size;
        gapStart = off;

        if (hasHint) {
            u32 hintStart = hint - size;
            while (cur != NULL) {
                u32 curOffset = cur->offset;
                gapStart = curOffset + cur->size;
                gapSize = off - gapStart;

                if (gapStart <= hintStart) {
                    break;
                }

                last = cur;
                cur = cur->prev;
                off = curOffset;
            }

            if (last == this->head[0]) {
                off = last->offset;
                gapStart = base;
                gapSize = off - base;
            }

            if (gapStart + gapSize > hint) {
                gapSize = hint - gapStart;
            }

            if (gapSize >= size) {
                return this->insert(last, cur, gapStart + gapSize - size, size, id, attr);
            }

            if (cur != NULL) {
                last = cur;
                cur = cur->prev;

                if (last != NULL) {
                    off = last->offset;
                }
            } else {
                return NULL;
            }
        }

        while (cur != NULL) {
            u32 curOffset = cur->offset;
            if (off - (curOffset + cur->size) >= size) {
                return this->insert(last, cur, off - size, size, id, attr);
            }

            last = cur;
            cur = cur->prev;
            off = curOffset;
        }

        if (last == this->head[0]) {
            if (off - base >= size) {
                return this->insert(last, cur, off - size, size, id, attr);
            }
        }
    }

    return NULL;
}

// MATCHED 100%
VramHandle * clVramManByteBase::allocAt(u32 size, u32 id, u32 attr, u32 offset) {
    VramHandle * last;

    u32 manBits = (this->manBank << 2) | (this->manType << 3) | (this->manIndex << 6);

    attr &= ~4;

    u32 side = (attr & 1);
    VramHandle * cur = this->head[side];

    attr |= manBits;

    u32 base = this->base;
    offset += base;
    u32 end = offset + size;

    if (cur == NULL) {
        if (offset < base || end > base + this->size) {
            return NULL;
        }
        return this->insert(NULL, NULL, offset, size, id, attr);
    }

    last = NULL;
    if (side == 0) {
        u32 off = base;

        while (cur != NULL) {
            if (off <= offset && end <= cur->offset) {
                return this->insert(cur, last, offset, size, id, attr);
            }

            last = cur;
            off = cur->offset + cur->size;
            cur = cur->next;
        }

        if (last == this->head[1]) {
            if (off <= offset && end <= base + this->size) {
                return this->insert(cur, last, offset, size, id, attr);
            }
        }
    } else {
        u32 off = base + this->size;

        while (cur != NULL) {
            u32 curOffset = cur->offset;
            if (curOffset + cur->size <= offset && end <= off) {
                return this->insert(last, cur, offset, size, id, attr);
            }

            last = cur;
            cur = cur->prev;
            off = curOffset;
        }

        if (last == this->head[0]) {
            if (base <= offset && end <= off) {
                return this->insert(last, cur, offset, size, id, attr);
            }
        }
    }

    return NULL;
}

// MATCHED 100%
BOOL clVramManByteBase::release(VramHandle * handle, s32 threshold, u32 delta) {
    handle->refCount += delta;
    if (handle->refCount > threshold) {
        return FALSE;
    }

    if (this->head[0] == handle) {
        this->head[0] = handle->next;
    }
    if (this->head[1] == handle) {
        this->head[1] = handle->prev;
    }

    if (handle->prev != NULL) {
        handle->prev->next = handle->next;
    }
    if (handle->next != NULL) {
        handle->next->prev = handle->prev;
    }

    handleMan->free(handle);

    return TRUE;
}

// MATCHED 100%
VramHandle * clVramManByteBase::findShared(u32 id, u16 attr, u32 mask, u32 fromOffset, s16 refDelta) {
    u32 key = attr & mask;

    VramHandle * r0 = this->head[key & 1];

    fromOffset += this->base;

    if ((key & 1) == 0) {

        if (fromOffset != 0) {
            while (r0 != NULL) {
                if (r0->offset >= fromOffset) {
                    break;
                }

                r0 = r0->next;
            }
        }

        while (r0 != NULL) {
            if (r0->id == id && key == (r0->attr & mask)) {
                r0->refCount += refDelta;
                return r0;
            }

            r0 = r0->next;
        }
    } else {
        if (fromOffset != 0) {
            while (r0 != NULL) {
                if (r0->offset + r0->size <= fromOffset) {
                    break;
                }

                r0 = r0->next;
            }
        }

        while (r0 != NULL) {
            if (r0->id == id && key == (r0->attr & mask)) {
                r0->refCount += refDelta;
                return r0;
            }

            r0 = r0->prev;
        }
    }

    return NULL;
}

// MATCHED 100%
u32 clVramManByteBase::compact(u32 kind) {
    u32 moved = 0;

    if ((kind & 1) == 0) {
        u32 freeStart = this->base;
        VramHandle * cur = this->head[0];

        while (cur != NULL && (cur->attr & 3) == kind) {
            moved += cur->offset - freeStart;
            freeStart = cur->offset + cur->size;
            cur->offset -= moved;

            if (moved != 0) {
                cur->moved = 1;
            }

            cur = cur->next;
        }
    } else {
        VramHandle * cur = this->head[1];
        u32 freeEnd = this->base + this->size;

        while (cur != NULL && (cur->attr & 3) == kind) {
            moved += freeEnd - (cur->offset + cur->size);
            freeEnd = cur->offset;
            cur->offset += moved;

            if (moved != 0) {
                cur->moved = 1;
            }

            cur = cur->prev;
        }
    }

    return moved;
}

}