/****************************************************************************
*                                                                           *
* simpleppdef.h -- Basic simple++ library Definitions                       *
*                                                                           *
* Copyright (c) X.D. Guo. All rights reserved.                              *
*                                                                           *
****************************************************************************/

#ifndef _POLITEDEF_H_
#define _POLITEDEF_H_

#ifndef _POLITE_VERSION
#define _POLITE_VERSION "1.0.2012.9"
#endif

#if  _MSC_VER >= 1600
#  ifdef __cplusplus
#    define __cpp0x       201109L
#    define __cpp11       __cpp0x
#    define __cxx0x       __cpp11
#    define __cxx11       __cxx0x
#  endif
#endif

#ifndef __cxx11
#  define nullptr         0
#endif

/*
 * _POLAPI_ Definitions
 */
#ifndef POLITEAPI
#define POLITEAPI __stdcall
#endif

/*
 * _POL_BASETYPES_ Definitions
 */

#ifndef _POL_BASETYPES
#define _POL_BASETYPES
#ifndef INTTYPES_DEFINED
#define INTTYPES_DEFINED
#define u_int uint32_t
typedef unsigned long u_long;
typedef long long llong;
typedef unsigned long long u_llong;
#if defined(__cxx11) || defined(__GNUC__)
#include <stdint.h>
#else
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef signed __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#endif
typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;
typedef uint64_t qword_t;
#endif
#endif  /* _POL_BASETYPES */

typedef int _initialization_t;

/*
 * _POL_CONSTANTS_ Definitions
 */
#ifndef _POL_CONSTANTS
#define _POL_CONSTANTS
static const uint64_t __B =  (1);
static const uint64_t __KB = (1024 * __B) ;
static const uint64_t __MB = (1024 * __KB);
static const uint64_t __GB = (1024 * __MB);
static const uint64_t __TB = (1024 * __GB);
static const uint64_t __EB = (1024 * __TB);
#endif  /* _POL_CONSTANTS */
#include <memory>
/*
 * TEMPLATE TYPE structs Definition
 */
#ifndef _POL_BOOL
#define _POL_BOOL
template<typename _Int>
struct Bool
{
    static const _Int True = 1;
    static const _Int False = 0;
} ;
#endif /* _POL_BOOL */

/*
 * size in bytes constant: SZ Definition
 */
#ifndef _POL_SZ
#define _POL_SZ
#define __b __B
#define __k __K
#define __m __M
#define __g __G
#define __t __T
#define __e __E
#define __K __KB
#define __M __MB
#define __G __GB
#define __T __TB
#define __E __EB
#define SZ(n, u) ( (n) * __##u )
static const uint8_t __MAX_UINT8 = SZ(256, B) - 1;
static const uint16_t __MAX_UINT16 = SZ(64, KB) - 1;
static const uint32_t __MAX_UINT32 = SZ(4, GB) - 1;
static const uint64_t __MAX_UINT64 = SZ(16383, EB) + (SZ(1, EB) - 1);
#endif   /* _POL_SZ */

/*
* Polite Micro _towcb Definition
*/
#ifndef _towcb
#define __towcb(quote) L##quote
#define _towcb(quote) __towcb(quote)
#endif /* _towcb */

#ifndef _POL_RDTSC
#define _POL_RDTSC
#if defined(__GNUC__)
#define _naked_mark
static __inline int64_t rdtsc(void)
{
    int64_t ts;
    uint32_t ts1, ts2;
    __asm__ __volatile__("rdtsc\n\t":"=a"(ts1), "=d"(ts2));
    ts = ((uint64_t) ts2 << 32) | ((uint64_t) ts1);
    return ts;
}
#elif !defined(_WIN64) /* win32 */
#define _naked_mark __declspec(naked)
_naked_mark
static inline int64_t rdtsc(void)
{
    __asm
    {
        rdtsc;
        ret;
    }
}
#else
#define _naked_mark
#endif
#endif  /* _POL_RDTSC_ */

#ifndef _ISNULLPTR
#define _ISNULLPTR
#define IsNullptr(ptr) ( nullptr == (ptr) )
#endif /* _ISNULLPTR */

#ifndef _POLITE_STRING
#define _POLITE_STRING
#include <string>
namespace simplepp_1_0_2012_9 {
#ifdef _UNICODE
    typedef std::wstring string;
#else
    typedef std::string string;
#endif
};
#endif /* _POLITE_STRING */

#ifndef _POLITE_ALIAS
#define _POLITE_ALIAS
namespace simplepp = simplepp_1_0_2012_9;
namespace xsimple = simplepp;
namespace libsimple = simplepp;
#endif /* _POLITE_ALIAS */

#endif /* _POLITEDEF_H_ */
/*
* Copyright (c) 2012-2019 by X.D. Guo ALL RIGHTS RESERVED.
* Consult your license regarding permissions and restrictions.
***/

