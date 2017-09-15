#ifndef _ARDUINO_TASKI2C_H_
#define _ARDUINO_TASKI2C_H_
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>

void taskI2C_setup(void);

void taskI2C(void *);

#endif // _ARDUINO_TASKI2C_H_
