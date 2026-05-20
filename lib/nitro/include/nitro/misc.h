#ifndef NITRO_MISC_H_
#define NITRO_MISC_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
    #define SDK_IFDEFCPP extern "C" {
    #define SDK_ENDIFCPP }
#else
    #define SDK_IFDEFCPP
    #define SDK_ENDIFCPP
#endif

#define SDK_ASSERT(exp)                       ((void) 0)
#define SDK_ASSERTMSG(exp, ...)               ((void) 0)
#define SDK_TASSERTMSG(exp, ...)              ((void) 0)
#define SDK_WARNING(exp, ...)                 ((void) 0)
#define SDK_TWARNING(exp, ...)                ((void) 0)
#define SDK_NULL_ASSERT(exp)                  ((void) 0)
#define SDK_MIN_ASSERT(exp, min)              ((void) 0)
#define SDK_MAX_ASSERT(exp, max)              ((void) 0)
#define SDK_MINMAX_ASSERT(exp, min, max)      ((void) 0)
#define SDK_FATAL_ERROR(...)                  ((void) 0)
#define SDK_TFATAL_ERROR(...)                 ((void) 0)
#define SDK_INTERNAL_ERROR(...)               ((void) 0)
#define SDK_TINTERNAL_ERROR(...)              ((void) 0)
#define SDK_ALIGN4_ASSERT(exp)                ((void) 0)
#define SDK_ALIGN2_ASSERT(exp)                ((void) 0)

#define SDK_COMPILER_ASSERT(expr) \
    extern void sdk_compiler_assert ## __LINE__ (char is[(expr) ? +1 : -1])

void OSi_ReferSymbol(void * symbol);

#define SDK_REFER_SYMBOL(symbol) OSi_ReferSymbol((void *)(symbol))
#define SDK_MIDDLEWARE_STRING(vender, module) "[SDK+" vender ":" module "]"
#define SDK_DEFINE_MIDDLEWARE(id, vender, module) static char id [] = SDK_MIDDLEWARE_STRING(vender, module)
#define SDK_USING_MIDDLEWARE(id) SDK_REFER_SYMBOL(id)

static inline void * OSi_AbortByUnsupportedRegister (const char * regname, const char * file, int line)
{
    (void)regname;
    (void)file;
    (void)line;
    return 0;
}

#define OS_UNSUPPORTED_REGADDR(reg) OSi_AbortByUnsupportedRegister(#reg, __FILE__, __LINE__)
#define OS_UNSUPPORTED_REG8(reg)    *(REGType8 *)OSi_AbortByUnsupportedRegister(#reg, __FILE__, __LINE__)
#define OS_UNSUPPORTED_REG16(reg)   *(REGType16 *)OSi_AbortByUnsupportedRegister(#reg, __FILE__, __LINE__)
#define OS_UNSUPPORTED_REG32(reg)   *(REGType32 *)OSi_AbortByUnsupportedRegister(#reg, __FILE__, __LINE__)
#define OS_UNSUPPORTED_REG64(reg)   *(REGType64 *)OSi_AbortByUnsupportedRegister(#reg, __FILE__, __LINE__)

#ifdef __cplusplus
}
#endif

#endif
