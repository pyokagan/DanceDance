#ifndef _ARDUINO_TASKCOMM_H_
#define _ARDUINO_TASKCOMM_H_
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <ucomm.h>

extern uint8_t RECV_FLAG;

extern SemaphoreHandle_t lock;

typedef enum {
    TASKCOMM_COMMAND_SEND_SAMPLE,
    TASKCOMM_COMMAND_RESEND_SAMPLE,
    TASKCOMM_COMMAND_SEND_POW,
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

typedef struct taskComm_CommandSendPow {
    taskComm_CommandType type; // TASKCOMM_COMMAND_SEND_POW
    ucomm_Pow pow;
} taskComm_CommandSendPow;

typedef union taskComm_Command {
    taskComm_CommandType type;
    taskComm_CommandSendSample sendSample;
    taskComm_CommandResendSample resendSample;
    taskComm_CommandSendPow sendPow;
} taskComm_Command;

extern QueueHandle_t taskComm_queue;

void taskComm_setup(void);

void taskComm(void *);

#endif // _ARDUINO_TASKCOMM_H_
