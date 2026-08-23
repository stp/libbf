/*
 * C utilities
 * 
 * Copyright (c) 2017 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _CUTILS_H
#define _CUTILS_H

#include <inttypes.h>

/* [STP] MSVC compatibility: cl.exe has neither the GNU builtins nor
   __attribute__. clang-cl defines _MSC_VER too but supports both, so it
   keeps the upstream branch. One of STP's own changes, which are what
   this repository's stp branch adds to the verbatim upstream on master. */
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
#define likely(x)       (x)
#define unlikely(x)     (x)
#define force_inline __forceinline
#define no_inline __declspec(noinline)
#define __maybe_unused
#define CUTILS_FORMAT_PRINTF(a, b)
#else
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define force_inline inline __attribute__((always_inline))
#define no_inline __attribute__((noinline))
#define __maybe_unused __attribute__((unused))
#define CUTILS_FORMAT_PRINTF(a, b) __attribute__((format(printf, a, b)))
#endif

#define xglue(x, y) x ## y
#define glue(x, y) xglue(x, y)
#define stringify(s)    tostring(s)
#define tostring(s)     #s

#ifndef offsetof
#define offsetof(type, field) ((size_t) &((type *)0)->field)
#endif
#define countof(x) (sizeof(x) / sizeof(x[0]))

/* return the pointer of type 'type *' containing 'ptr' as field 'member' */
#define container_of(ptr, type, member) ((type *)((uint8_t *)(ptr) - offsetof(type, member)))

typedef int BOOL;

#ifndef FALSE
enum {
    FALSE = 0,
    TRUE = 1,
};
#endif

void pstrcpy(char *buf, int buf_size, const char *str);
char *pstrcat(char *buf, int buf_size, const char *s);
int strstart(const char *str, const char *val, const char **ptr);

static inline int max_int(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

static inline int min_int(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

static inline int clamp_int(int val, int min_val, int max_val)
{
    if (val < min_val)
        return min_val;
    else if (val > max_val)
        return max_val;
    else
        return val;
}

#if defined(_MSC_VER) && !defined(__clang__)

/* [STP] The _BitScan* mapping of the GNU bit-scan builtins; 31 ^ idx is
   31 - idx for idx in [0, 31]. The a = 0 precondition below makes the
   "scanned nothing" return value unreachable. */

/* WARNING: undefined if a = 0 */
static inline int clz32(unsigned int a)
{
    unsigned long idx;
    _BitScanReverse(&idx, a);
    return 31 ^ (int)idx;
}

/* WARNING: undefined if a = 0 */
static inline int clz64(uint64_t a)
{
#if defined(_M_X64) || defined(_M_ARM64)
    unsigned long idx;
    _BitScanReverse64(&idx, a);
    return 63 ^ (int)idx;
#else
    if (a >> 32)
        return clz32((unsigned int)(a >> 32));
    return 32 + clz32((unsigned int)a);
#endif
}

/* WARNING: undefined if a = 0 */
static inline int ctz32(unsigned int a)
{
    unsigned long idx;
    _BitScanForward(&idx, a);
    return (int)idx;
}

/* WARNING: undefined if a = 0 */
static inline int ctz64(uint64_t a)
{
#if defined(_M_X64) || defined(_M_ARM64)
    unsigned long idx;
    _BitScanForward64(&idx, a);
    return (int)idx;
#else
    if ((unsigned int)a)
        return ctz32((unsigned int)a);
    return 32 + ctz32((unsigned int)(a >> 32));
#endif
}

#pragma pack(push, 1)
struct packed_u32 {
    uint32_t v;
};

struct packed_u16 {
    uint16_t v;
};
#pragma pack(pop)

#else

/* WARNING: undefined if a = 0 */
static inline int clz32(unsigned int a)
{
    return __builtin_clz(a);
}

/* WARNING: undefined if a = 0 */
static inline int clz64(uint64_t a)
{
    return __builtin_clzll(a);
}

/* WARNING: undefined if a = 0 */
static inline int ctz32(unsigned int a)
{
    return __builtin_ctz(a);
}

/* WARNING: undefined if a = 0 */
static inline int ctz64(uint64_t a)
{
    return __builtin_ctzll(a);
}

struct __attribute__((packed)) packed_u32 {
    uint32_t v;
};

struct __attribute__((packed)) packed_u16 {
    uint16_t v;
};

#endif

static inline uint32_t get_u32(const uint8_t *tab)
{
    return ((struct packed_u32 *)tab)->v;
}

static inline void put_u32(uint8_t *tab, uint32_t val)
{
    ((struct packed_u32 *)tab)->v = val;
}

static inline uint32_t get_u16(const uint8_t *tab)
{
    return ((struct packed_u16 *)tab)->v;
}

static inline void put_u16(uint8_t *tab, uint16_t val)
{
    ((struct packed_u16 *)tab)->v = val;
}

typedef void *DynBufReallocFunc(void *opaque, void *ptr, size_t size);

typedef struct {
    uint8_t *buf;
    size_t size;
    size_t allocated_size;
    BOOL error; /* true if a memory allocation error occurred */
    DynBufReallocFunc *realloc_func;
    void *opaque; /* for realloc_func */
} DynBuf;

void dbuf_init(DynBuf *s);
void dbuf_init2(DynBuf *s, void *opaque, DynBufReallocFunc *realloc_func);
int dbuf_realloc(DynBuf *s, size_t new_size);
int dbuf_write(DynBuf *s, size_t offset, const uint8_t *data, size_t len);
int dbuf_put(DynBuf *s, const uint8_t *data, size_t len);
int dbuf_putc(DynBuf *s, uint8_t c);
int dbuf_putstr(DynBuf *s, const char *str);
static inline int dbuf_put_u32(DynBuf *s, uint32_t val)
{
    return dbuf_put(s, (uint8_t *)&val, 4);
}
static inline int dbuf_put_u16(DynBuf *s, uint16_t val)
{
    return dbuf_put(s, (uint8_t *)&val, 2);
}
int CUTILS_FORMAT_PRINTF(2, 3) dbuf_printf(DynBuf *s,
                                           const char *fmt, ...);
void dbuf_free(DynBuf *s);
static inline BOOL dbuf_error(DynBuf *s) {
    return s->error;
}


#define UTF8_CHAR_LEN_MAX 4

int unicode_to_utf8(uint8_t *buf, unsigned int c);
int unicode_from_utf8(const uint8_t *p, int max_len, const uint8_t **pp);

#endif
