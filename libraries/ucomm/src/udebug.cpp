#include "udebug.h"
#include <stdio.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

void
udebug_print(const void *data, size_t len)
{
#ifdef ARDUINO
    Serial.write((const char *)data, len);
#else
    fwrite(data, 1, len, stdout);
#endif
}

void
udebug_writeString(const char *str)
{
    udebug_print(str, strlen(str));
}
