#ifndef _USERIAL_H
#define _USERIAL_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void userial_init(void);

/** Writes `byte` to the UART */
void userial_write(uint8_t byte);

/* Flushes the write buffer */
void userial_flush(void);

/**
 * Reads a byte from the UART.
 * Will block if there are no characters in the UART receive buffer
 */
uint8_t userial_read(void);

#ifdef __cplusplus
}
#endif

#endif // _USERIAL_H
