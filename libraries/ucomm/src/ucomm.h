#ifndef _UCOMM_H
#define _UCOMM_H
#include "userial.h"
#include "uframe.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UCOMM_MESSAGE_SAMPLE
} ucomm_MessageType;

typedef struct ucomm_MessageHeader {
    uint8_t type;
    uint8_t id;
} ucomm_MessageHeader;

typedef struct ucomm_MessageSample {
    uint8_t type; // UCOMM_MESSAGE_SAMPLE
    uint8_t id;
    int16_t acc1x;
    int16_t acc1y;
    int16_t acc1z;
    int16_t acc2x;
    int16_t acc2y;
    int16_t acc2z;
    uint16_t __dummy_crc16;
} ucomm_MessageSample;

typedef union ucomm_Message {
    ucomm_MessageHeader header;
    ucomm_MessageSample sample;
} ucomm_Message;

void ucomm_init(void);

void ucomm_read(ucomm_Message *);

void ucomm_write(const ucomm_Message *);

#ifdef __cplusplus
}
#endif

#endif // _UCOMM_H
