/*
 * PWM_0.c
 *
 * Created: 19/04/2026 12:57:48
 *  Author: raulc
 */ 
#include "PWM_0.h"
void init_timer0(void)
{
	// Inicializamos el OCR0A
	OCR0A = 0;
	// Apagamos los bits que nos serrviran para configurar el timer0 en modo CTC
	TCCR0A &= ~((1<<WGM01) | (1<<WGM00));
	TCCR0B &= ~((1<<WGM02));
	
	TCCR0A |= (1<<WGM01); // Configuramos timer0 en modo ctc con top OCR0A
	
	// Configuramos prescaler del clk i/o
	
	TCCR0B &= ~((1<<CS02) | (1<<CS01) | (1<<CS00)); // De igual manera apagamos los bits para evitar errores
	TCCR0B |= (1<<CS01); // Prescaler de 8
	
	//Asignamos el valor del OCR0A
	OCR0A = OCR0A_valor;
	// Habilitamos la interrupcion por compare match A
	TIMSK0 |= (1<<OCIE0A);
}

void PWM_set0(uint8_t pulso_nuevo)
{
	if(pulso_nuevo < 5)
	{
		pulso_nuevo = 5;
	}
	if(pulso_nuevo > 25)
	{
		pulso_nuevo = 25;
	}
	
	pulso_ms = pulso_nuevo;
}
void setup_timer0(void)
{
	// Configuramos como salida PD7
	DDRD |= led_timer0;
}

