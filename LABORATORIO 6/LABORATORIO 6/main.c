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
void initADC();
uint16_t leer_ADC(); 
void writeChar(char c);
void writeString(char*string);
void writeInt(uint16_t numero);
volatile uint8_t modo = 0; // Modos que va a tener el menu en la terminal 
/****************************************/
// Main Function
int main(void)
{
	cli();
	DDRB |= 0x3F; // PB0->PB5 como salidas
	PORTB &= ~0x3F;// Apagamos PB0->PB5
	DDRD |= (1<<DDD6) | (1<<DDD7); // PD6 y PD7 como salidas 
	PORTD &= ~((1<<DDD6) | (1<<DDD7)); // Apagamos PD6 y PD7  
	initUART();
	initADC();
	sei();
		writeString("\r\n ------Menu-------\r\n ");
		writeString(" Opción 1: leer potenciómetro\r\n ");
		writeString(" Opción 2: escribir caracteres\r\n ");
		writeString(" Opción 3: volver a menu principal \r\n");
		writeString("================\r\n");
	while (1)
	{
		
		if (modo == 1)
		{
			uint16_t potenciometro = leer_ADC();
			writeString("Potenciometro:"); 
			writeInt(potenciometro);
			writeString("\r\n");
			_delay_ms(500);// 500ms de delay para mandar continuamente las frases planteadas anteriormente
		}
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
void initADC(void)
{
	// Configuración de ISR por ADC en pin A7
	ADMUX |= (1 << REFS0);
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= ((1 << MUX2)| (1<< MUX0)); // Seleccionamos canal donde estara el POT en este caso es en el ADC5
	ADCSRA |= (1 << ADEN); // Habilitamos el ADC
	// Configuramos la justificación a la izquierda
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}
uint16_t leer_ADC()
{
	ADCSRA |= (1 << ADSC); // Activamos la conversión inicial, luego la que esta en la ISR se encarga de las
	while (ADCSRA & (1<<ADSC)); // Esperamos a que la conversión termine es decir ADSC == 0 
	return ADC; // Retorna el resultado de la conversión anterior de un valor entre 0 y 1023
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

void writeInt(uint16_t numero)
{
	char buf[6]; // Creamos un arreglo de 6 caracteres para guardar temporalmente el valor del ADC 
	uint8_t i = 0; 
	
	if (numero == 0)
	{
		writeChar('0');
		return;
	}
	// Extraemos los numeros de derecha a izquierda y hacemos la conversión del numero a su valor en ASCII
	while (numero > 0)
	{
		buf[i++] = '0' + (numero % 10);
		numero /= 10;
	}
	// Hacemos "print" del valor del potenciómetro
	for (int8_t j = i -1; j>= 0; j--)
	{
		writeChar(buf[j]);
	}
}
/****************************************/
// Interrupt routines
ISR(USART_RX_vect)
{
	uint8_t bufferRX = UDR0;
	writeChar(bufferRX);
	
	if (modo == 0) // Estamos en el menu 
	{
		if(bufferRX == '1') // Modo lectura de potenciómetro 
		{
			modo = 1; 
			writeString("\r\n --- Modo lectura de potenciómetro ---\r\n");
			writeString("\r\n --- Presiona 3 para volver al menu principal");
			
		}
		else if(bufferRX == '2')
		{
			modo = 2;
			writeString("\r\n --- Modo esritura de caracteres ---\r\n");
			writeString("\r\n --- Presiona 3 para volver al menu principal");
		}
	}
	
	else if (bufferRX == '3')
	{
		modo = 0; 
		writeString("\r\n ------Menu-------\r\n ");
		writeString(" Opción 1: leer potenciómetro\r\n ");
		writeString(" Opción 2: escribir caracteres\r\n ");
		writeString(" Opción 3: volver a menu principal \r\n");
		writeString("================\r\n");
		
	}
	
	else if (modo == 2)
	{
		// Bits 0->1 van a PD6 y PD7
		PORTD = (PORTD & 0x3F) | ((bufferRX & 0x03) << 6);
		
		// Bits 2->7 van a PB0->PB5
		PORTB = (PORTB & 0xC0) | ((bufferRX >> 2) & 0x3F);
	}
}





