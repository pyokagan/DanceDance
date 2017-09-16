#include "taskComm.h"
#include "taskRecv.h"
#include "taskI2C.h"

void setup() {
    Serial.begin(115200); // serial for debug messages
    taskComm_setup();
    taskI2C_setup();
    xTaskCreate(taskComm, "Comm", 128, NULL, 1, NULL);
    xTaskCreate(taskRecv, "Recv", 128, NULL, 1, NULL);
    xTaskCreate(taskI2C, "I2C", 128, NULL, 2, NULL);
}

void loop() {
    // do nothing
}
