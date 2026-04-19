/*
 * PWM1.c
 *
 * Created: 13/04/2026 23:20:59
 *  Author: raulc
 */ 

#include "PWM1.h"
static uint16_t timer_valor = 0;
//#define SERVO_1 (1 << PB1)
void init_timer1()
{
	OCR1A = 0; 
	ICR1 = 0; 
	// Habilitamos la opción de la señal PWM no invertida del timer1
	TCCR1A &= ~((1<<COM1A1) | (1<<COM1A0)); // Apagamos todos los registros para el modo no invertido de la señal y solo tener encendido el COM1A1
	TCCR1A |= (1<<COM1A1);
	// Configuramos modo fast del timer1
	TCCR1A &= ~((1<<WGM11) | (1<<WGM10));	// Apagamos todos los bits para evitar errores en la configurción fast
	TCCR1B &= ~((1<<WGM13) | (1<<WGM12)); // Apagamos todos los bits para evitar errores en la configurción fast
	TCCR1A |= (1<<WGM11); // Encendemos WGM11 en el TCCR1A
	TCCR1B |= ((1<<WGM13) | (1<<WGM12)); // Encendemos WGM13 y WGM12 en TCCR1B
	// Configuramos el I/O presclaer
	TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
	TCCR1B |= (1<<CS11);
	
	// Caegamos valores de los registros OCR1A y ICR1
	OCR1A= OCR1A_valor;
	ICR1 = ICR1_valor;
	
}

void PWM_set(uint16_t tiempo_us)
{
	// Añadimos programación defensiva para evitar errores
	
	if (tiempo_us < 500)
	{
		tiempo_us = 500;
	}
	else if (tiempo_us >2500)
	{
		tiempo_us = 2500;
	}
	// Conversiones para modificar el valor de OCR1A
	
	timer_valor = (tiempo_us/ticks); // Conversión de us a ticks
	OCR1A = tiempo_us * 2;
}
