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
    while (Serial1.available() <= 0);
    return Serial1.read();
}

#endif // ARDUINO
