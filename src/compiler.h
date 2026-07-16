#ifndef QOL_COMPILER
#define QOL_COMPILER

#include <stdio.h>
#include <stdlib.h>

#define LOG(fmt, ...)           \
    fprintf(stderr,             \
        "[%s:%d] " fmt "\n",    \
        __FILE__, __LINE__,     \
        ##__VA_ARGS__           \
    ) 

#define TODO(str) do {      \
    LOG("TODO:- %s", str);  \
    exit(EXIT_FAILURE);     \
    } while(0)

#if defined(__GNUC__) || defined(__clang__)
    #define likely(x)   __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
#endif 

#if defined(__GNUC__) || defined(__clang__)
    #define FORCE_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
    #define FORCE_INLINE __forceinline
#else
    #define FORCE_INLINE inline
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
    #define NORETURN __declspec(noreturn)
#else
    #define NORETURN
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define PACKED __attribute__((packed))
#elif defined(_MSC_VER)
    #warning "Use pragma pack(push, 1) and pragma pack(pop) explicitly"
#else
    #define PACKED
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define ASSUME(x) \
        do { if (!(x)) __builtin_unreachable(); } while (0)
#elif defined(_MSC_VER)
    #define ASSUME(x) __assume(x)
#else
    #define ASSUME(x)
#endif

#if defined(__GNUC__) || defined(__clang__)
    #define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
    #define UNREACHABLE() __assume(0)
#else
    #define UNREACHABLE() ((void)0)
#endif

#define TYPECHECK(x, y) \
    _Static_assert( \
        __builtin_types_compatible_p(typeof(x), typeof(y)), \
        "incompatible types")

#define ___PASTE(a,b) a##b
#define __PASTE(a,b) ___PASTE(a,b)

#define __UNIQUE_ID(prefix) \
    __PASTE(__PASTE(__UNIQUE_ID_, prefix), __COUNTER__)

#define __max(t1, t2, max1, max2, x, y)     \
    ({                                      \
        t1 max1 = (x);					    \
        t2 max2 = (y);					    \
        (void) (&max1 == &max2);			\
        max1 > max2 ? max1 : max2;          \
    })

#define MAX_ST(x, y)					\
	__max(                              \
        typeof(x), typeof(y),			\
	    __UNIQUE_ID(max1_),             \
        __UNIQUE_ID(max2_),	            \
	      x, y                          \
    )

#define __min(t1, t2, min1, min2, x, y)     \
    ({                                      \
        t1 min1 = (x);					    \
        t2 min2 = (y);					    \
        (void) (&min1 == &min2);			\
        min1 < min2 ? min1 : min2;          \
    })

#define MIN_ST(x, y)					\
	__min(                              \
        typeof(x), typeof(y),			\
	    __UNIQUE_ID(min1_),             \
        __UNIQUE_ID(min2_),	            \
	      x, y                          \
    )

#endif // !QOL_COMPILER
