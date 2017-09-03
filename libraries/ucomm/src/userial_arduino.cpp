#ifdef ARDUINO
#include "userial.h"
#include <Arduino.h>

void
userial_init(void)
{
    Serial1.begin(115200);
}

void
userial_write(uint8_t byte)
{
    Serial1.write(byte);
}

void
userial_flush(void)
{
    // do nothing since we do not do software write buffering
}

uint8_t
userial_read(void)
{
    uint8_t out;
    do {
        out = Serial1.read();
    } while (out < 0);
    return out;
}

#endif // ARDUINO
