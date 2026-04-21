/*
 * NombreProgra.c
 *
 * Created: 20/04/2026 19:24:22
 * Author: raulc	
 * Description: Comunicación entre computadora y micro 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL 
#include <util/delay.h>
 
/****************************************/
// Function prototypes
void initUART();
void writeChar(char c);
void writeString(char*string);
/****************************************/
// Main Function
int main(void)
{
	cli();
	DDRB |= (1<<DDB5);
	PORTB &= ~(1<<PORTB5);
	initUART();
	writeString("sisas");
	sei();
	while (1)
	{
		writeString("Sisas\n");
		_delay_ms(500);           // ? espera 500ms y repite
	}
}
/****************************************/
// NON-Interrupt subroutines
void initUART()
{
	// Configuración de pines 
	DDRD &= ~(1<<DDD0); // D0 = rx -> entrada 
	DDRD |= (1<<DDD1); // D1 = tx -> salida 
	// Normal speed
	UCSR0A = 0; 
	// Habilitar interrupciones para recepción y transmisión
	UCSR0B |= ((1<<RXCIE0) | (1<<RXEN0) | (1<<TXEN0));
	// Configuración de modo asincrono, sin paridad, 1 stop bit, 8 bits de datos
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00);
	// Carga de UBRR0
	UBRR0 = 103; 
}
void writeChar(char c)
{
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c; 
}
void writeString(char*string)
{
	for(uint8_t i=0; *(string + i) != '\0'; i++)
	{
		writeChar(string[i]);
	}
}
/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	uint8_t bufferRX = UDR0;
	writeChar(bufferRX);
	
	if (bufferRX == 'a')
	{
		PORTB |= (1<<PORTB5);
		
	}
	
	else if (bufferRX != 'a')
	{
		PORTB &= ~(1<<PORTB5);
	}
}





