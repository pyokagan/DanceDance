#ifndef _UFRAME_H
#define _UFRAME_H
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UFRAME_FLAG_BYTE 0xFF
#define UFRAME_ESCAPE_BYTE 0x7D

void uframe_write(const void *data, uint8_t len);

/**
 * Reads a frame into the buffer `data` of size `len`.
 * Returns the size of the frame (number of bytes read) in `nbytes`.
 * Returns true if the frame was read successfully,
 * false if an error occurred while reading.
 */
bool uframe_read(void *data, uint8_t len, uint8_t *nbytes);

#ifdef __cplusplus
}
#endif

#endif // _UFRAME_H
