#ifndef _UCOMM_H
#define _UCOMM_H
#include "userial.h"
#include "uframe.h"
#include <stdint.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT_STRINGIFY(x) #x
#if (__cplusplus >= 201103L) || defined(static_assert)
#define ASSERT_SIZE(st, s) static_assert(sizeof(st) == s, ASSERT_STRINGIFY(st) " does not have size " ASSERT_STRINGIFY(s))
#else
#define ASSERT_SIZE(st, s) typedef char static_assert_ ## st ## _does_not_have_size_ ## s [(!!(sizeof(st) == s)) * 2 - 1]
#endif

typedef enum {
    UCOMM_MESSAGE_SAMPLE
} ucomm_MessageType;

typedef struct ucomm_MessageHeader {
    uint8_t type;
    uint8_t id;
} ucomm_MessageHeader;
ASSERT_SIZE(ucomm_MessageHeader, 2);

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
ASSERT_SIZE(ucomm_MessageSample, 16);

typedef union ucomm_Message {
    ucomm_MessageHeader header;
    ucomm_MessageSample sample;
} ucomm_Message;

void ucomm_init(void);

void ucomm_read(ucomm_Message *);

void ucomm_write(const ucomm_Message *);

#undef ASSERT_SIZE
#undef ASSERT_STRINGIFY

#ifdef __cplusplus
}
#endif

#endif // _UCOMM_H
