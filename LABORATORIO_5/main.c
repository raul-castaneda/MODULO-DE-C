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
#include "PWM_2/PWM_2.h"
#include "PWM_0/PWM_0.h"

// Variables globales // 
uint16_t tiempo_us = 1500;
volatile uint8_t ADC_valor = 0;
volatile uint8_t pulso_ms = 0;
volatile uint8_t contador_ms = 0;

/****************************************/
// Function prototypes
void init_ADC(void);
/****************************************/
// Main Function


void setup()
{		
		DDRB |= (1<<DDB1);
		
}
int main(void)
{
	// Iniciamos todas las configuraciones del ADC, timer0 y muestreo del contador de 8 bits.
	
	cli();
	setup();
	setup_2();
	setup_timer0();
	init_timer0();
	init_timer1();
	init_TIMER2(); 
	init_ADC();
	PWM_set(tiempo_us);
	sei();
	
	while (1)
	{
		
	}
	
	return 0;
}
/****************************************/
// NON-Interrupt subroutines

void init_ADC(void)
{
	// Configuración de ISR por ADC en pin A7
	ADMUX |= (1 << REFS0);
	ADMUX &= ~((1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (1<<MUX0));
	ADMUX |= ((1 << MUX2)|(1 << MUX1) | (1<< MUX0)); // Seleccionamos canal donde estara el POT en este caso es en el ADC7
	//ADMUX |= ((1<<MUX2) | (1<<MUX1)); // Seleccionamos el canal donde estara el pot para el servo por timer2 que en este caso esl ADC6 
	ADCSRA |= (1 << ADEN); // Habilitamos el ADC
	ADCSRA |= (1 << ADIE); // Habilitamos interrupciones por ADC
	// Configuramos la justificación a la izquierda
	ADMUX |= (1 << ADLAR);
	ADCSRA |= (1 << ADSC); // Activamos la conversión inicial, luego la que esta en la ISR se encarga de las
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}




/****************************************/
// Interrupt routines

ISR (ADC_vect)
{
	
	ADC_valor = ADCH;
	uint8_t canal = ADMUX & 0x0F; 
	
	if (canal == 7)
	{
		// Esto es para el ADC7
		uint16_t tiempo_us;
		
		tiempo_us = 500 + ((uint32_t)ADC_valor * 2000)/255;
		PWM_set(tiempo_us);
		
		ADMUX = (ADMUX & 0xF0) | 0x06; 
	}
	
	else if (canal == 6)
	{
		// Mapeo por el ADC6
		uint8_t OCR2A_valor = OCR2A_valormin + ((uint16_t) ADC_valor * 32) / 255 ;  // Operación de actualización del OCR2A = 6 + (adc_valor2 * 32)/ 255 = valor actualizado del ocr2a_valor
		PWM_set2(OCR2A_valor);
		ADMUX = (ADMUX & 0xF0) | 0x05; 
	}
	
	else if (canal == 5)
	{
		uint8_t pulso_nuevo = 5 + ((uint16_t)ADC_valor * 20)/255;
		PWM_set0(pulso_nuevo);
		ADMUX = (ADMUX & 0xF0) | 0x07; 
	}
	
	ADCSRA |= (1<<ADSC); // volver a hacer lectura de adc
}

ISR (TIMER0_COMPA_vect)
{
	contador_ms ++;
	if (contador_ms >= periodo_ms)
	{
		contador_ms = 0;
	}
	
	if (contador_ms <= pulso_ms)
	{
		PORTD |= led_timer0;
	}
	else
	{
		PORTD &= ~(led_timer0);
	}
}