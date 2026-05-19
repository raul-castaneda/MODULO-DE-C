/*
 * UART.c
 *
 * Created: 16/05/2026 15:13:31
 *  Author: raulc
 */ 

#include "UART.h"

void initUART()
{
	// Inicialización
	UCSR0A = 0;
	UCSR0B = 0;
	UCSR0C = 0;
	// Configuraciones de pines PD0->PD1
	DDRD &= ~(1<<DDD0);  // D0 = RX -> entrada
	DDRD |=  (1<<DDD1);  // D1 = TX -> salida
	// Velocidad normal
	UCSR0A = 0;
	// Habilitación de interrupciones para recepción y transmisión
	UCSR0B = (1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0);
	// Configuración de modo asíncrono, sin paridad, 1 stop bit, 8 bits de datos
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
	// Carga de UBRR0
	UBRR0  = 103;
}
