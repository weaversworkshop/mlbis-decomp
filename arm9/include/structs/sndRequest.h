#ifndef STRUCTS_SND_REQUEST_H
    #define STRUCTS_SND_REQUEST_H

    #include "types.h"

    typedef struct SndRequest {
        struct SndRequest * next;
        struct SndRequest * prev;
        u8 argForm;
        u8 pad_09[0x3];
        void (*field0c)(void);
        union {
            struct {
                u32 arg1;
                u8 arg2;
                u32 arg3;
            } form0;
            struct {
                u16 * programs;
                u32 arg2;
                u32 arg3;
            } form1;
            struct {
                u32 arg1;
                u32 arg2;
            } form2;
        };
    } SndRequest;

    typedef struct {
        SndRequest * first;
    } SndRequestHead;

    typedef struct {
        u8 pad_00[0x4];
        SndRequest * tail;
    } SndRequestTail;
#endif
