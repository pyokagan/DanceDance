#ifndef _ARDUINO_UART1_H
#define _ARDUINO_UART1_H
#include <stdint.h>
#include <HardwareSerial.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#ifdef __cplusplus
extern "C" {
#endif

extern SemaphoreHandle_t lock_read;
extern HardwareSerial UART1;

/**
 * Initializes the arduino's UART 1
 */
void uart1_init(void);

/**
 * Writes `byte` to the UART.
 * Blocks if the UART transmit buffer is full.
 */
void uart1_write(uint8_t byte);

/**
 * Reads a byte from the UART.
 * Blocks if there are no characters in the receive buffer.
 */
uint8_t uart1_read(void);

#ifdef __cplusplus
}
#endif

#endif // _ARDUINO_UART1_H
