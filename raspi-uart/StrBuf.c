#include "StrBuf.h"
#include "die.h"
#include <stdlib.h>
#include <stdio.h>

void
StrBuf_init(StrBuf *self)
{
    self->buf = NULL;
    self->len = 0;
    self->alloc = 0;
}

void
StrBuf_release(StrBuf *self)
{
    free(self->buf);
    StrBuf_init(self);
}

void
StrBuf_reset(StrBuf *self)
{
    self->len = 0;
}

void
StrBuf_alloc(StrBuf *self, size_t nbytes)
{
    if (self->alloc >= nbytes)
        return;

    size_t newAllocBytes;
    if (self->alloc >= 64U)
        newAllocBytes = (nbytes * 3U) / 2U;
    else
        newAllocBytes = 64U;

    if (newAllocBytes < nbytes)
        newAllocBytes = nbytes;

    self->buf = realloc(self->buf, newAllocBytes);
    if (!self->buf)
        die("out of memory");
    self->alloc = newAllocBytes;
}

void
StrBuf_grow(StrBuf *self, size_t nbytes)
{
    StrBuf_alloc(self, self->alloc + nbytes);
}

void
StrBuf_vaddf(StrBuf *self, const char *fmt, va_list ap)
{
    va_list cp;
    int len;

    size_t avail = self->alloc - self->len;
    va_copy(cp, ap);
    len = vsnprintf(self->buf + self->len, avail, fmt, cp);
    va_end(cp);
    if (len < 0)
        die("vsnprintf returned unexpected len: %d", len);
    if (len > avail) {
        size_t required = (size_t)len - avail;
        StrBuf_grow(self, required);
        avail = self->alloc - self->len;
        len = vsnprintf(self->buf + self->len, avail, fmt, ap);
        if (len < 0 || len > avail)
            die("vsnprintf returned unexpected len: %d", len);
    }
    self->len += len;
}

void
StrBuf_addf(StrBuf *self, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    StrBuf_vaddf(self, fmt, ap);
    va_end(ap);
}
