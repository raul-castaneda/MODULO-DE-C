/*
 * PWM1.c
 *
 * Created: 13/04/2026 23:20:59
 *  Author: raulc
 */ 

#include "PWM1.h"

// Definimos como salida PB1
void setup()
{
	// Configuramos como salida, PB1 donde estara ubicado el servo del prelab y es donde esta conectado el OCR1A
	DDRB |= servo_1; // Servo 1 (OCR1A) PB1	-> como salida 
	DDRB |= servo_2; // Servo 2 (OCR2A) PB2 -> como salida 
}
void init_timer1()
{
	OCR1A = 0; 
	ICR1 = 0; 
	// Habilitamos la opción de la señal PWM no invertida del timer1
	TCCR1A &= ~((1<<COM1A1) | (1<<COM1A0) | (1<<COM1B1) | (1<<COM1B0)); // Apagamos todos los registros para el modo no invertido de la señal y solo tener encendido el COM1A1
	TCCR1A |= (1<<COM1A1) | (1<<COM1B1);
	// Configuramos modo fast del timer1
	TCCR1A &= ~((1<<WGM11) | (1<<WGM10));	// Apagamos todos los bits para evitar errores en la configurción fast
	TCCR1B &= ~((1<<WGM13) | (1<<WGM12)); // Apagamos todos los bits para evitar errores en la configurción fast
	TCCR1A |= (1<<WGM11); // Encendemos WGM11 en el TCCR1A
	TCCR1B |= ((1<<WGM13) | (1<<WGM12)); // Encendemos WGM13 y WGM12 en TCCR1B
	// Configuramos el I/O presclaer
	TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));
	TCCR1B |= (1<<CS11); // Prescaler de 8
	
	// Caegamos valores de los registros OCR1A y ICR1
	OCR1A = OCR1A_valor;
	OCR1B = OCR1B_valor;
	ICR1 = ICR1_valor;
	
	
}

void PWM_set(uint16_t tiempo_us)
{
	// Añadimos programación defensiva para evitar errores, cuidando el rango apto para el movimiento del servo y evitar daños en el mismo. 
	
	if (tiempo_us < 500)
	{
		tiempo_us = 500;
	}
	else if (tiempo_us >2500)
	{
		tiempo_us = 2500;
	}
	// Conversiones para modificar el valor de OCR1A
	OCR1A = tiempo_us * 2;
}

void PWM_set_B(uint16_t tiempo_us_B)
{
	if(tiempo_us_B < 500)
	{
		tiempo_us_B = 500; 
	}
	else if (tiempo_us_B > 2500)
	{
		tiempo_us_B = 2500;
	}
	// Conversión para modificar el valor de OCR1B
	OCR1B = tiempo_us_B * 2; 
}