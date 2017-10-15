#ifndef _DIE_H_
#define _DIE_H_

void die(const char *fmt, ...)
    __attribute__((noreturn, format(printf, 1, 2)));

#endif // _DIE_H_
