#ifndef _ARDUINO_TASKCOMM_H_
#define _ARDUINO_TASKCOMM_H_
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <ucomm.h>

typedef enum {
    TASKCOMM_COMMAND_SEND_SAMPLE,
    TASKCOMM_COMMAND_RESEND_SAMPLE
} taskComm_CommandType;

typedef struct taskComm_CommandSendSample {
    taskComm_CommandType type; // TASKCOMM_COMMAND_SEND_SAMPLE
    ucomm_Sample sample;
} taskComm_CommandSendSample;

typedef struct taskComm_CommandResendSample {
    taskComm_CommandType type; // TASKCOMM_COMMAND_RESEND_SAMPLE
    uint8_t packetTypes;
    ucomm_id_t id;
} taskComm_CommandResend;

typedef union taskComm_Command {
    taskComm_CommandType type;
    taskComm_CommandSendSample sendSample;
    taskComm_CommandResendSample resendSample;
} taskComm_Command;

extern QueueHandle_t taskComm_queue;

void taskComm_setup(void);

void taskComm(void *);

#endif // _ARDUINO_TASKCOMM_H_
