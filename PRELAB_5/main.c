/*
 * NombreProgra.c
 *
 * Created: 13/04/2026
 * Author: Raúl Castañeda 24085
 * Description: Control de servo motor por medio de un potenciómetro 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
#include "PWM1/PWM1.h"
volatile uint8_t ADC_valor = 0;
uint16_t tiempo_us = 1500;

/****************************************/
// Function prototypes
void init_ADC(void);
/****************************************/
// Main Function


void setup()
{
		// Configuración reloj principal a 1Mhz
		CLKPR = (1 << CLKPCE);
		CLKPR = (1 << CLKPS2);
		
		DDRB |= (1<<DDB1);
}
int main(void)
{
	// Iniciamos todas las configuraciones del ADC, timer0 y muestreo del contador de 8 bits.
	
	cli();
	setup();
	init_timer1();
	init_ADC();
	PWM_set(tiempo_us);
	sei();
	
	while (1)
	{
		PWM_set(1000);
	}
	
	return 0;
}
/****************************************/
// NON-Interrupt subroutines

void init_ADC(void)
{
	// Configuración de ISR por ADC en pin A6
	//ADMUX |= (1 << REFS0);
	//ADMUX |= (1 << MUX1)|(1 << MUX2); // Seleccionamos canal donde estara el POT en este caso es en el ADC6
	ADMUX = (1 << REFS0) | (1 << ADLAR) | (1 << MUX2) | (1 << MUX1);
	ADCSRA |= (1 << ADEN); // Habilitamos el ADC
	ADCSRA |= (1 << ADIE); // Habilitamos interrupciones por ADC
	// Configuramos la justificación a la izquierda
	ADMUX |= (1 << ADLAR);
	ADCSRA |= (1 << ADSC); // Activamos la conversión inicial, luego la que esta en la ISR se encarga de las
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

ISR (ADC_vect)
{
	ADC_valor = ADCH;
	//uint16_t tiempo_us;
	
	tiempo_us = 2000 + ((uint32_t)ADC_valor * 2000)/255;
	PWM_set(tiempo_us);
	ADCSRA |= (1 <<ADSC);
}


/****************************************/
// Interrupt routines