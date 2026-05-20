// Common primitive type defs
#ifndef NITRO_TYPES_H
    #define NITRO_TYPES_H
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned int u32;
    typedef unsigned long u64;

    typedef signed char s8;
    typedef signed short s16;
    typedef signed int s32;
    typedef signed long s64;

    typedef int bool;
    #define TRUE  1
    #define FALSE 0
    #define true  1
    #define false 0

    #ifndef NULL
        #define NULL ((void *)0)
    #endif
#endif