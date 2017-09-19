#ifndef ARDUINO
#include "ucomm_PowReader.h"

static void
timespec_diff(const struct timespec *start, const struct timespec *stop,
        struct timespec *result)
{
    if ((stop->tv_nsec - start->tv_nsec) < 0) {
        result->tv_sec = stop->tv_sec - start->tv_sec - 1;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec + 1000000000;
    } else {
        result->tv_sec = stop->tv_sec - start->tv_sec;
        result->tv_nsec = stop->tv_nsec - start->tv_nsec;
    }
}

void
ucomm_PowReader_init(ucomm_PowReader *self)
{
    self->firstPacket = true;
    self->disconnect = false;
    self->ready = false;
}

void
ucomm_PowReader_release(ucomm_PowReader *self)
{
    // do nothing
}

void
ucomm_PowReader_reset(ucomm_PowReader *self)
{
    self->disconnect = true;
    self->ready = false;
    self->firstPacket = true;
}

static bool
isPowType(ucomm_type_t type)
{
    return type == UCOMM_MESSAGE_POW;
}

bool
ucomm_PowReader_feed(ucomm_PowReader *self, const ucomm_Message *msg)
{
    if (!isPowType(msg->header.type))
        return false;

    self->disconnect = false;
    if (!self->firstPacket) {
        struct timespec currentTime, diff;

        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        timespec_diff(&self->lastTime, &currentTime, &diff);
        if (diff.tv_sec >= 1)
            ucomm_PowReader_reset(self);
        self->lastTime = currentTime;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &self->lastTime);
    }
    self->firstPacket = false;

    self->pow.id = msg->pow.id;
    self->pow.voltage = msg->pow.voltage;
    self->pow.current = msg->pow.current;
    self->ready = true;

    return true;
}

#endif // ARDUINO
