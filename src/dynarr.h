#ifndef DYN_ARR
#define DYN_ARR

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifndef DYN_NO_SHOT_NAMES
#define arr_push        dyn_arr_push
#define arr_puts        dyn_arr_puts
#define arr_append      dyn_arr_concat

#define arr_pop         dyn_arr_pop
#define try_arr_pop     dyn_arr_try_pop

#define arr_dup         dyn_arr_dup
#define arr_free        dyn_arr_free

#define arr_last        dyn_arr_last
#define arr_reserve     dyn_arr_reserve

#define arr_len         dyn_arr_len
#define arr_cap         dyn_arr_cap
#endif // !DYN_NO_SHOT_NAMES

#define CHECK_TYPE_COMP(x, y) \
    _Static_assert( \
        __builtin_types_compatible_p(typeof(x), typeof(y)), \
        "Must be a comparable type")

#define DYN_INITIAL_CAP 16
#define DYN_GROW_FACTOR 1.5f

extern void* dyn_grow(void *arr, size_t cap, size_t elem_size);
extern void* dyn_dup(void *arr, size_t elem_size);

#define dyn_header(t)       ((t != NULL) ? ((dyn_arr_header *)(t) - 1) : NULL)  // from array to header 
#define dyn_array(h)        ((void *)(((dyn_arr_header *)(h)) + 1))             // from header to array
#define dyn_arr_cap(a)      ((a) ? dyn_header(a)->cap : 0)
#define dyn_arr_len(a)      ((a) ? dyn_header(a)->len : 0)

#define dyn_arr_push(arr, value)                \
    (                                           \
        dyn_arr_maybe_grow(arr),                \
        (arr)[dyn_header(arr)->len++] = (value) \
    )

#define dyn_arr_pop(arr)            \
    (                               \
        (dyn_header(arr)->len--),   \
        (arr)[dyn_header(arr)->len] \
    )

#define dyn_arr_try_pop(arr, out)          \
    (                                      \
        dyn_arr_len(arr) > 0 ?             \
        (*(out) = dyn_arr_pop(arr), 1) : 0 \
    )

#define dyn_arr_concat_no_type_check(arr, arr_src, count)                                \
    do {                                                                                 \
        dyn_arr_reserve(arr, dyn_arr_len(arr) + (count));                                \
        memcpy(arr + dyn_arr_len(arr),(arr_src), (count) * sizeof(*(arr_src)));          \
        dyn_header(arr)->len += (count);                                                 \
    } while(0)

#define dyn_arr_puts(arr, ...)                                         \
    do {                                                               \
        typeof(*(arr)) tmp[] = { __VA_ARGS__ };                        \
        size_t count = sizeof(tmp)/sizeof(tmp[0]);                     \
        dyn_arr_concat_no_type_check(arr, tmp, count);                 \
    } while(0)

#define dyn_arr_concat(arr, arr_src, count)                                \
    do {                                                                   \
        CHECK_TYPE_COMP(*arr, *arr_src);                                   \
        dyn_arr_concat_no_type_check(arr, arr_src, count);                 \
    } while(0)


#define dyn_arr_dup(arr) \
    (dyn_array(dyn_dup(dyn_header(arr), sizeof(*arr))))

#define dyn_arr_free(arr) \
    do { \
        free(dyn_header(arr)); \
        arr = NULL; \
    } while(0)

#define dyn_arr_init_grow(arr)                          \
        ((arr == NULL) ? dyn_arr_grow(arr, DYN_INITIAL_CAP) : 0)

#define dyn_arr_re_grow(arr)                            \
        ((dyn_header(arr)->len == dyn_header(arr)->cap) \
                       ? dyn_arr_grow(arr, (int)dyn_header(arr)->cap * DYN_GROW_FACTOR) : 0)

#define dyn_arr_maybe_grow(arr) \
    (dyn_arr_init_grow(arr) || dyn_arr_re_grow(arr))

#define dyn_arr_grow(arr, cap) \
    ((arr) = dyn_array(dyn_grow(dyn_header(arr), cap, sizeof(*(arr)))))

#define dyn_arr_reserve(arr, cap) \
    ((dyn_arr_cap(arr) < (cap)) ? \
        dyn_arr_grow(arr, cap) : 0)

#define dyn_arr_last(arr) \
    ((arr)[dyn_arr_len(arr)-1])

typedef struct {
    size_t len;
    size_t cap;
}dyn_arr_header;

#ifdef ENABLE_DYN_STR
typedef char* string;

#define CHECK_TYPE_STR(x) \
    _Static_assert( \
        __builtin_types_compatible_p(typeof(x), const char*), \
        "Must be a string const char *")

#define str_len         dyn_arr_len
#define str_cap         dyn_arr_cap

#define str_dup         dyn_arr_dup
#define str_free        dyn_arr_free

#define str_concat      dyn_str_concat

#define dyn_str_concat(str, str1)                                          \
    do {                                                                   \
        CHECK_TYPE_STR(str1);                                              \
        size_t count = strlen(str1) + 1;                                   \
        dyn_arr_concat_no_type_check(str, str1, count);                    \
    } while(0)

#endif // ENABLE_DYN_STR

#ifdef DYN_ARR_IMPLEMENTATION
#include<stdio.h>

void *dyn_grow(void* arr, size_t cap, size_t elem_size) {
    size_t new_size = elem_size * cap + sizeof(dyn_arr_header);

    if (arr == NULL) {
        arr = malloc(new_size);
        if (!arr) {
            perror("Cant Malloc");
            exit(EXIT_FAILURE);
        }

        memset(arr, 0, new_size);
        dyn_arr_header *header = (dyn_arr_header *)arr;

        header->cap = cap;
        header->len = 0;
        return arr;
    }
    
    dyn_arr_header *header = (dyn_arr_header *)arr;
    size_t old_cap = header->cap;
    if (old_cap >= cap) return arr;

    size_t old_size = elem_size * old_cap + sizeof(dyn_arr_header);

    void *tmp = realloc(arr, new_size);
    if (tmp == NULL) {
        perror("Cant realloc");
        exit(EXIT_FAILURE);
    }
    
    memset((char *)tmp + old_size, 0, new_size - old_size);
    arr = tmp;
    header = (dyn_arr_header *)arr;
    header->cap = cap;
    return arr;
}

void *dyn_dup(void* arr, size_t elem_size) {
    if (arr == NULL) return NULL;

    dyn_arr_header *header = (dyn_arr_header *)arr;
    size_t mem_len = header->cap * elem_size + sizeof(dyn_arr_header);

    void *new_arr = malloc(mem_len);
    memcpy(new_arr,arr, mem_len);
    return new_arr;
}
#endif // DYN_ARR_IMPLEMENTATION

#endif // !DYN_ARR
