#ifndef _STRBUF_H_
#define _STRBUF_H_
#include <string.h>
#include <stdarg.h>

typedef struct StrBuf {
    char *buf;
    size_t len, alloc;
} StrBuf;

#define STRBUF_INIT { NULL, 0, 0 }

void StrBuf_init(StrBuf *);

void StrBuf_release(StrBuf *);

void StrBuf_reset(StrBuf *);

void StrBuf_alloc(StrBuf *, size_t);

void StrBuf_grow(StrBuf *, size_t);

void StrBuf_vaddf(StrBuf *, const char *fmt, va_list);

void StrBuf_addf(StrBuf *, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));

#endif // _STRBUF_H_
