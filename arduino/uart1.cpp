#include "uart1.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

void
uart1_init(void)
{
    Serial1.begin(115200);
}

void
uart1_write(uint8_t byte)
{
    Serial1.write(byte);
}

uint8_t
uart1_read(void)
{
    while (Serial1.available() <= 0);
    return Serial1.read();
}
