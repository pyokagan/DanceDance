#ifndef _USERIAL_H
#define _USERIAL_H
#include <stdint.h>
#include <stdbool.h>

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
 * Returns true if a byte was successfully read,
 * false if an error occurred while reading.
 */
typedef bool (*userial_read_t)(uint8_t *);
extern userial_read_t userial_read;

#ifdef __cplusplus
}
#endif

#endif // _USERIAL_H
