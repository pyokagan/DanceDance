#ifndef _UDEBUG_H
#define _UDEBUG_H
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void udebug_write(const void *data, size_t len);

void udebug_writeString(const char *str);

#ifdef __cplusplus
}
#endif

#endif // _UDEBUG_H
