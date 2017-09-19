#ifndef _UCOMM_POWREADER_H
#define _UCOMM_POWREADER_H
#include "ucomm.h"

#ifndef ARDUINO
#include <string.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ucomm_PowReader {
    ucomm_Pow pow;
    struct timespec lastTime; // Time when last packet was received.
    bool firstPacket;
    bool disconnect;
    bool ready;
} ucomm_PowReader;

void ucomm_PowReader_init(ucomm_PowReader *);

void ucomm_PowReader_release(ucomm_PowReader *);

void ucomm_PowReader_reset(ucomm_PowReader *);

bool ucomm_PowReader_feed(ucomm_PowReader *, const ucomm_Message *);

#ifdef __cplusplus
}
#endif

#endif // ARDUINO
#endif // _UCOMM_POWREADER_H
