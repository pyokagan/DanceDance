#ifndef _USERIAL_H
#define _USERIAL_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void userial_initRaspi(void);

/** Writes `byte` to the UART */
typedef void (*userial_write_t)(uint8_t byte);
extern userial_write_t userial_write;

/* Flushes the write buffer */
typedef void (*userial_flush_t)(void);
extern userial_flush_t userial_flush;

/**
 * Reads a byte from the UART.
 * Will block if there are no characters in the UART receive buffer
 */
typedef uint8_t (*userial_read_t)(void);
extern userial_read_t userial_read;

#ifdef __cplusplus
}
#endif

#endif // _USERIAL_H
