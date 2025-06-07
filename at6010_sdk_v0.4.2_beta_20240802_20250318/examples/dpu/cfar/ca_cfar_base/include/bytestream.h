/**
 * Copyright (c) 2023, Airtouching Intelligence Technology.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Airtouching
 *    Intelligence Technology integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY AIRTOUCHING TECHNOLOGY "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AIRTOUCHING TECHNOLOGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __BYTESTREAM_H__
#define __BYTESTREAM_H__

#include <stdint.h>
#include <string.h>
#include "cmsis_compiler.h"

#ifndef MIN
#define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

typedef struct GetByteContext {
    const uint8_t *buffer, *buffer_end, *buffer_start;
} GetByteContext;

typedef struct PutByteContext {
    uint8_t *buffer, *buffer_end, *buffer_start;
    int eof;
} PutByteContext;

#ifndef RL32
#   define RL32(x)                                \
       (((uint32_t)((const uint8_t*)(x))[3] << 24) |    \
        ((uint32_t)((const uint8_t*)(x))[2] << 16) |    \
        ((uint32_t)((const uint8_t*)(x))[1] <<  8) |    \
        ((uint32_t)((const uint8_t*)(x))[0]))
#endif
#ifndef WL32
#   define WL32(p, val) do {                 \
        uint32_t d = (uint32_t)(val);                     \
        ((uint8_t*)(p))[0] = (uint8_t)(d);               \
        ((uint8_t*)(p))[1] = (uint8_t)((d) >> 8);            \
        ((uint8_t*)(p))[2] = (uint8_t)((d) >> 16);           \
        ((uint8_t*)(p))[3] = (uint8_t)((d) >> 24);           \
    } while(0)
#endif
#ifndef RL24
#   define RL24(x)                           \
       (((uint32_t)((const uint8_t*)(x))[2] << 16) |         \
        ((uint32_t)((const uint8_t*)(x))[1] <<  8) |         \
        ((uint32_t)((const uint8_t*)(x))[0]))
#endif
#ifndef WL24
#   define WL24(p, d) do {                   \
        ((uint8_t*)(p))[0] = (uint8_t)(d);               \
        ((uint8_t*)(p))[1] = (uint8_t)((d) >> 8);            \
        ((uint8_t*)(p))[2] = (uint8_t)((d) >> 16);           \
    } while(0)
#endif
#ifndef RL16
#   define RL16(x)                           \
        (((uint16_t)((const uint8_t*)(x))[1] << 8) |          \
         ((uint16_t)((const uint8_t*)(x))[0]))
#endif
#ifndef WL16
#   define WL16(p, val) do {                 \
        uint16_t d = (uint16_t)(val);                     \
        ((uint8_t*)(p))[0] = (uint8_t)(d);               \
        ((uint8_t*)(p))[1] = (uint8_t)((d) >> 8);            \
    } while(0)
#endif

#define RB8(x)     (((const uint8_t*)(x))[0])
#define WB8(p, d)  do { ((uint8_t*)(p))[0] = (uint8_t)(d); } while(0)

#define DEF(type, name, bytes, read, write)                                  \
__STATIC_INLINE type bytestream_get_ ## name(const uint8_t **b)        \
{                                                                              \
    (*b) += bytes;                                                             \
    return (type)read(*b - bytes);                                             \
}                                                                              \
__STATIC_INLINE void bytestream_put_ ## name(uint8_t **b,              \
                                                     const type value)         \
{                                                                              \
    write(*b, value);                                                          \
    (*b) += bytes;                                                             \
}                                                                              \
__STATIC_INLINE void bytestream2_put_ ## name ## u(PutByteContext *p,  \
                                                           const type value)   \
{                                                                              \
    bytestream_put_ ## name(&p->buffer, value);                                \
}                                                                              \
__STATIC_INLINE void bytestream2_put_ ## name(PutByteContext *p,       \
                                                      const type value)        \
{                                                                              \
    if (!p->eof && (p->buffer_end - p->buffer >= bytes)) {                     \
        write(p->buffer, value);                                               \
        p->buffer += bytes;                                                    \
    } else                                                                     \
        p->eof = 1;                                                            \
}                                                                              \
__STATIC_INLINE void bytestream2_put_ ## name ## _na(PutByteContext *p,\
                                                      const type value)        \
{                                                                              \
    if (!p->eof && (p->buffer_end - p->buffer >= bytes)) {                     \
        write(p->buffer, value);                                               \
    } else                                                                     \
        p->eof = 1;                                                            \
}                                                                              \
__STATIC_INLINE type bytestream2_get_ ## name ## u(GetByteContext *g)  \
{                                                                              \
    return bytestream_get_ ## name(&g->buffer);                                \
}                                                                              \
__STATIC_INLINE type bytestream2_get_ ## name(GetByteContext *g)       \
{                                                                              \
    if (g->buffer_end - g->buffer < bytes) {                                   \
        g->buffer = g->buffer_end;                                             \
        return 0;                                                              \
    }                                                                          \
    return bytestream2_get_ ## name ## u(g);                                   \
}                                                                              \
__STATIC_INLINE type bytestream2_peek_ ## name(GetByteContext *g)      \
{                                                                              \
    if (g->buffer_end - g->buffer < bytes)                                     \
        return 0;                                                              \
    return (type)read(g->buffer);                                              \
}

DEF(unsigned int, le32, 4, RL32, WL32)
DEF(unsigned int, le24, 3, RL24, WL24)
DEF(unsigned int, le16, 2, RL16, WL16)
DEF(unsigned int, byte, 1, RB8, WB8)

__STATIC_INLINE void bytestream2_init_writer(PutByteContext* p,
    uint8_t* buf,
    int buf_size)
{
    p->buffer = buf;
    p->buffer_start = buf;
    p->buffer_end = buf + buf_size;
    p->eof = 0;
}

__STATIC_INLINE unsigned int bytestream2_get_bytes_left(GetByteContext* g)
{
    return (unsigned int)(g->buffer_end - g->buffer);
}

__STATIC_INLINE int bytestream2_tell(GetByteContext* g)
{
    return (int)(g->buffer - g->buffer_start);
}

__STATIC_INLINE int bytestream2_tell_p(PutByteContext* p)
{
    return (int)(p->buffer - p->buffer_start);
}

__STATIC_INLINE void bytestream2_skip(GetByteContext* g,
    unsigned int size)
{
    g->buffer += MIN((unsigned int)(g->buffer_end - g->buffer), size);
}

__STATIC_INLINE unsigned int bytestream2_get_buffer(GetByteContext* g,
    uint8_t* dst,
    unsigned int size)
{
    int size2 = MIN(g->buffer_end - g->buffer, (int)size);
    memcpy(dst, g->buffer, (unsigned int)size2);
    g->buffer += size2;
    return (unsigned int)size2;
}

__STATIC_INLINE void bytestream2_skip_p(PutByteContext* p,
    unsigned int size)
{
    int size2;
    if (p->eof)
        return;
    size2 = MIN(p->buffer_end - p->buffer, (int)size);
    if (size2 != (int)size)
        p->eof = 1;
    p->buffer += size2;
}

__STATIC_INLINE unsigned int bytestream2_put_buffer(PutByteContext* p,
    const uint8_t* src,
    unsigned int size)
{
    int size2;
    if (p->eof)
        return 0;
    size2 = MIN(p->buffer_end - p->buffer, (int)size);
    if (size2 != (int)size)
        p->eof = 1;
    memcpy(p->buffer, src, (unsigned int)size2);
    p->buffer += size2;
    return (unsigned int)size2;
}

__STATIC_INLINE unsigned int bytestream2_get_eof(PutByteContext* p)
{
    return (unsigned int)p->eof;
}

#define BUFFER_SEEK_SET        0               /* seek to an absolute position */
#define BUFFER_SEEK_CUR        1               /* seek relative to current position */
#define BUFFER_SEEK_END        2               /* seek relative to end of file */
__STATIC_INLINE  int clip(int a, int amin, int amax)
{
    if (a < amin) return amin;
    else if (a > amax) return amax;
    else               return a;
}

__STATIC_INLINE int bytestream2_seek(GetByteContext* g,
    int offset,
    int whence)
{
    switch (whence) {
    case BUFFER_SEEK_CUR:
        offset = clip(offset, -(g->buffer - g->buffer_start),
            g->buffer_end - g->buffer);
        g->buffer += offset;
        break;
    case BUFFER_SEEK_END:
        offset = clip(offset, -(g->buffer_end - g->buffer_start), 0);
        g->buffer = g->buffer_end + offset;
        break;
    case BUFFER_SEEK_SET:
        offset = clip(offset, 0, g->buffer_end - g->buffer_start);
        g->buffer = g->buffer_start + offset;
        break;
    default:
        return 0;
    }
    return bytestream2_tell(g);
}

__STATIC_INLINE int bytestream2_seek_p(PutByteContext* p,
    int offset,
    int whence)
{
    p->eof = 0;
    switch (whence) {
    case BUFFER_SEEK_CUR:
        if (p->buffer_end - p->buffer < offset)
            p->eof = 1;
        offset = clip(offset, -(p->buffer - p->buffer_start),
            p->buffer_end - p->buffer);
        p->buffer += offset;
        break;
    case BUFFER_SEEK_END:
        if (offset > 0)
            p->eof = 1;
        offset = clip(offset, -(p->buffer_end - p->buffer_start), 0);
        p->buffer = p->buffer_end + offset;
        break;
    case BUFFER_SEEK_SET:
        if (p->buffer_end - p->buffer_start < offset)
            p->eof = 1;
        offset = clip(offset, 0, p->buffer_end - p->buffer_start);
        p->buffer = p->buffer_start + offset;
        break;
    default:
        return 0;
    }
    return bytestream2_tell_p(p);
}
#endif
