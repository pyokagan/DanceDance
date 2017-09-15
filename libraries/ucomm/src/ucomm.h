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

typedef uint8_t ucomm_type_t;
typedef uint8_t ucomm_id_t;

#define UCOMM_MESSAGE_HEADER \
    ucomm_id_t id; \
    ucomm_type_t type;

typedef enum {
    // Message types for sending a sample
    UCOMM_MESSAGE_ACC1,
    UCOMM_MESSAGE_ACC2
} ucomm_MessageType;

typedef struct ucomm_MessageHeader {
    UCOMM_MESSAGE_HEADER
} ucomm_MessageHeader;
ASSERT_SIZE(ucomm_MessageHeader, 2);

typedef struct ucomm_MessageAcc {
    UCOMM_MESSAGE_HEADER // type = UCOMM_MESSAGE_ACC1 or UCOMM_MESSAGE_ACC2
    int16_t x, y, z;
    uint16_t __dummy_crc16;
} ucomm_MessageAcc;
ASSERT_SIZE(ucomm_MessageAcc, 10);

typedef union ucomm_Message {
    ucomm_MessageHeader header;
    ucomm_MessageAcc acc;
} ucomm_Message;

typedef struct ucomm_Sample {
    ucomm_id_t id; // sample ID.
    struct {
        int16_t x, y, z;
    } acc1;
    struct {
        int16_t x, y, z;
    } acc2;
} ucomm_Sample;

void ucomm_init(void);

void ucomm_read(ucomm_Message *);

void ucomm_write(const ucomm_Message *);

void ucomm_writeSample(const ucomm_Sample *);

void ucomm_writeSampleAcc1(const ucomm_Sample *);

void ucomm_writeSampleAcc2(const ucomm_Sample *);

#undef ASSERT_SIZE
#undef ASSERT_STRINGIFY

#ifdef __cplusplus
}
#endif

#endif // _UCOMM_H
