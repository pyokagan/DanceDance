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

static QueueHandle_t taskComm_queue;

static void taskComm(void *);
static void taskI2C(void *);

void setup() {
    ucomm_init();
    Serial.begin(9600);
    taskComm_queue = xQueueCreate(5, sizeof(taskComm_Command));
    xTaskCreate(taskComm, "Comm", 128, NULL, 1, NULL);
    xTaskCreate(taskI2C, "I2C", 128, NULL, 2, NULL);
}

void loop() {
    // do nothing
}

void taskComm(void *pvParameters) {
    uint8_t msgid = 0;

    for (;;) {
        taskComm_Command cmd;

        while (!xQueueReceive(taskComm_queue, &cmd, portMAX_DELAY));
        switch (cmd.type) {
        case TASKCOMM_COMMAND_SEND:
            cmd.send.msg.header.id = msgid++;
            ucomm_write(&cmd.send.msg);
            break;
        }
    }
}

void taskI2C(void *pvParameters) {
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;) {
        taskComm_Command cmd;
        cmd.type = TASKCOMM_COMMAND_SEND;
        cmd.send.msg.header.type = UCOMM_MESSAGE_SAMPLE;
        cmd.send.msg.sample.acc1x = 0;
        cmd.send.msg.sample.acc1y = 1;
        cmd.send.msg.sample.acc1z = 2;
        cmd.send.msg.sample.acc2x = 3;
        cmd.send.msg.sample.acc2y = 4;
        cmd.send.msg.sample.acc2z = 5;

        while (!xQueueSendToBack(taskComm_queue, &cmd, portMAX_DELAY));

        vTaskDelayUntil(&lastWakeTime, 23 / portTICK_PERIOD_MS);
    }
}
