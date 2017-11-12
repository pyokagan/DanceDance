#ifndef _ARDUINO_UART1_H
#define _ARDUINO_UART1_H
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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
bool uart1_read(uint8_t *);

#ifdef __cplusplus
}
#endif

#endif // _ARDUINO_UART1_H
