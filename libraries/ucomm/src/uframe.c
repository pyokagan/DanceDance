#include "uframe.h"
#include "userial.h"
#include <stdbool.h>
#ifdef ARDUINO
#include <util/crc16.h>
#endif

static uint16_t
uframe_crc16(uint16_t crc, uint8_t byte)
{
#ifdef ARDUINO
    return _crc16_update(crc, byte);
#else
    crc ^= byte;
    for (int i = 0; i < 8; ++i) {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }

    return crc;
#endif
}

static void
uframe_writeByte(uint8_t b)
{
    switch (b) {
    case UFRAME_ESCAPE_BYTE:
    case UFRAME_FLAG_BYTE:
        userial_write(UFRAME_ESCAPE_BYTE);
        userial_write(b ^ 0x20);
        break;
    default:
        userial_write(b);
        break;
    }
}

void
uframe_write(const void *data, uint8_t len)
{
    const uint8_t *ptr = data;
    uint16_t checksum = 0;

    userial_write(UFRAME_FLAG_BYTE);
    for (uint8_t i = 0; i < len; i++) {
        uframe_writeByte(ptr[i]);
        checksum = uframe_crc16(checksum, ptr[i]);
    }
    uframe_writeByte(checksum >> 8);
    uframe_writeByte(checksum & 0xff);
    userial_write(UFRAME_FLAG_BYTE);
    userial_flush();
}

uint8_t
uframe_read(void *data, uint8_t maxLen)
{
    enum {
        STATE_INIT,
        STATE_START,
        STATE_BODY
    } state = STATE_INIT;
    uint8_t *ptr = data;
    uint8_t bytesRead, byte;
    bool escape;

#define RESET() (bytesRead = 0, escape = false)

    for (;;) {
        switch (state) {
        case STATE_INIT:
            RESET();
            state = STATE_START;
            break;
        case STATE_START: // looking for starting UFRAME_FLAG_BYTE
            byte = userial_read();
            if (byte == UFRAME_FLAG_BYTE)
                state = STATE_BODY;
            break;
        case STATE_BODY:
            byte = userial_read();
            if (bytesRead >= maxLen) {
                state = STATE_INIT;
            } else if (escape) {
                ptr[bytesRead++] = byte ^ 0x20;
                escape = false;
            } else if (byte == UFRAME_ESCAPE_BYTE) {
                escape = true;
            } else if (byte == UFRAME_FLAG_BYTE) {
                if (bytesRead < 2) {
                    RESET();
                    continue;
                }

                // Calculate crc
                uint16_t crc = 0;
                for (uint8_t i = 0; i < bytesRead - 2; i++)
                    crc = uframe_crc16(crc, ptr[i]);
                uint16_t expectedCrc = (ptr[bytesRead - 2] << 8) | ptr[bytesRead - 1];
                if (crc == expectedCrc)
                    return bytesRead - 2;
                else
                    RESET();
            } else {
                ptr[bytesRead++] = byte;
            }
            break;
        }
    }

#undef RESET
}
