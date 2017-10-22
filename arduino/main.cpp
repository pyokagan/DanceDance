#include "taskComm.h"
#include "taskRecv.h"
#include "taskI2C.h"
#include "taskPow.h"

#include "uart1.h"

void setup() {
    taskComm_setup();
    taskI2C_setup();

    xTaskCreate(taskComm, "Comm", 256, NULL, 1, NULL);
    xTaskCreate(taskRecv, "Recv", 256, NULL, 1, NULL);
    xTaskCreate(taskI2C, "I2C", 512, NULL, 2, NULL);
    xTaskCreate(taskPow, "Pow", 128, NULL, 3, NULL);
}

void loop() {
}
