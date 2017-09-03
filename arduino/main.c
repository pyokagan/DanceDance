#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <ucomm.h>

static void taskBlink(void *);
static void taskComm(void *);

void setup() {
    ucomm_init();
    xTaskCreate(taskBlink, "Blink", 128, NULL, 2, NULL);
    xTaskCreate(taskComm, "Comm", 128, NULL, 1, NULL);
}

void loop() {
    // do nothing
}

void taskBlink(void *pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT);

    for (;;) {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void taskComm(void *pvParameters) {
    for (;;) {
        char data[] = "Hello World!";
        uframe_write(data, sizeof(data) - 1);
    }
}
