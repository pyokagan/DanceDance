#include "uart1.h"
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <HardwareSerial.h>
#include <HardwareSerial_private.h>

HardwareSerial UART1(&UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1);
SemaphoreHandle_t lock_read = xSemaphoreCreateBinary();

#if defined(UART1_RX_vect)
ISR(UART1_RX_vect)
#elif defined(USART1_RX_vect)
ISR(USART1_RX_vect)
#else
#error "Don't know what the Data Register Empty vector is called for Serial1"
#endif
{
	UART1._rx_complete_irq();
	xSemaphoreGiveFromISR(lock_read, NULL);
}

#if defined(UART1_UDRE_vect)
ISR(UART1_UDRE_vect)
#elif defined(USART1_UDRE_vect)
ISR(USART1_UDRE_vect)
#else
#error "Don't know what the Data Register Empty vector is called for Serial1"
#endif
{
	UART1._tx_udr_empty_irq();
}

void uart1_init(void)
{
	UART1.begin(115200);
}

void uart1_write(uint8_t byte)
{
	UART1.write(byte);
}

uint8_t uart1_read(void)
{
	if (xSemaphoreTake(lock_read, portMAX_DELAY) == pdTRUE) {
		return UART1.read();
	}
}
