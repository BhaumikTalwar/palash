#ifndef QOL_TYPES_
#define QOL_TYPES_

#include <stddef.h>
#include <stdint.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef size_t    usize;
typedef ptrdiff_t isize;

typedef uintptr_t uptr;
typedef intptr_t  iptr;

#define ARRAY_COUNT(x) \
    (sizeof(x) / sizeof((x)[0]))

#define KB(x) ((x)   * 1024ULL)
#define MB(x) (KB(x) * 1024ULL)
#define GB(x) (MB(x) * 1024ULL)

#define UNUSED(x) ((void)(x))

#define STATIC_ASSERT(cond, msg) \
    _Static_assert(cond, msg)

#endif // !QOL_TYPES_
