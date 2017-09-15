#ifndef _ARDUINO_TASKCOMM_H_
#define _ARDUINO_TASKCOMM_H_
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include <ucomm.h>

typedef enum {
    TASKCOMM_COMMAND_SEND
} taskComm_CommandType;

typedef struct taskComm_CommandSend {
    taskComm_CommandType type; // TASKCOMM_COMMAND_SEND
    ucomm_Message msg;
} taskComm_CommandSend;

typedef union taskComm_Command {
    taskComm_CommandType type;
    taskComm_CommandSend send;
} taskComm_Command;

extern QueueHandle_t taskComm_queue;

void taskComm_setup(void);

void taskComm(void *);

#endif // _ARDUINO_TASKCOMM_H_
