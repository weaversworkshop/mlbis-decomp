#ifndef __ansi_prefix__
#define __ansi_prefix__

#include <os_enum.h>

#define __dest_os __arm_bare

#define _MSL_CONSOLE_SUPPORT          1
#define _MSL_OS_DISK_FILE_SUPPORT     0
#define _MSL_OS_TIME_SUPPORT          1
#define _MSL_CLOCK_T_AVAILABLE        1
#define _MSL_TIME_T_AVAILABLE         1
#define _MSL_THREADSAFE               0
#define _MSL_PTHREADS                 0
#define _MSL_LOCALDATA_AVAILABLE      1
#define _MSL_OS_ALLOC_SUPPORT         0
#define _MSL_LONGLONG                 1
#define _MSL_FLOATING_POINT           1
#define _MSL_WIDE_CHAR                1
#define _MSL_USES_SUN_MATH_LIB        1
#define _MSL_POSIX                    0
#define _MSL_NEEDS_EXTRAS             0
#define _MSL_STRERROR_KNOWS_ERROR_NAMES 1
#define _MSL_ASSERT_DISPLAYS_FUNC     1
#define _MSL_C_LOCALE_ONLY            1
#define _MSL_C99                      1
#define _MSL_FLOATING_POINT_IO        1
#define _MSL_USES_EXIT_PROCESS        1
#define _MSL_FLT_EVAL_METHOD          0

#define __ANSI_OVERLOAD__
#define _MSL_INTEGRAL_MATH

#ifdef SDK_CW
    #undef _MSL_OS_TIME_SUPPORT
    #define _MSL_OS_TIME_SUPPORT 0
#endif

#endif
