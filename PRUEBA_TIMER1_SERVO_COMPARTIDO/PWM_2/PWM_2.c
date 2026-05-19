/*
 * PWM_2.c
 *
 * Created: 18/04/2026 15:53:31
 *  Author: raulc
 */ 
#include "PWM_2.h"

void setup_2(void)
{
	DDRB |= servo_3; // Servo 1 (OCR1B) PB3	-> como salida 
	DDRD |= servo_4; // Servo 1 (OCR2B) PD3	-> como salida 
}
void init_TIMER2(void)
{
	OCR2A = 0;
	TCCR2A &= ~((1<<COM2A1) | (1<<COM2A0) | (1<<COM2B1) | (1<<COM2B0)); // Apagamos los bits del registro para evitar errores
	TCCR2A |= (1<<COM2A1) | (1<<COM2B1); // Encendemos COM2A1/COM2B1 para activar el modo fast_pwm_mode usandO OCR2A y OCR2B
	
	// Habilitamos la configuración del timer2 para modo fast pwm con top = 0xff
	TCCR2A &= ~((1<<WGM21) | (1<<WGM20)); // WGM22/21/20 == 0, para evitar errores
	TCCR2B &= ~(1<<WGM22); 				  // WGM22/21/20 == 0, para evitar errores WGM22 esta en TCCR2B
	TCCR2A |= ((1<<WGM21) | (1<<WGM20)); // FAST_PWM; top == 0xff; WGM21 == 1; WGM20 == 1.
	
	// Configuramos el prescaler del timer2 a 1024
	TCCR2B &= ~((1<<CS22) | (1<<CS21) | (1<<CS20)); // C22/21/20 == 0 para evitar errores
	TCCR2B |= ((1<<CS22) | (1<<CS21) | (1<<CS20));
	
	// Iniciamos el valor inicial de OCR2A/OCR2B al centro del servo == 22
	OCR2A = OCR2A_centro;
	OCR2B = OCR2B_centro;
}

void PWM_set2(uint8_t OCR2A_valor)
{
	if(OCR2A_valor < OCR2A_valormin)
	{
		OCR2A_valor = OCR2A_valormin;
	}
	
	if(OCR2A_valor > OCR2A_valormax)
	{
		OCR2A_valor = OCR2A_valormax;
	}
	
	OCR2A = OCR2A_valor;
	
}

void PWM_set2_B(uint8_t OCR2B_valor)
{
	if(OCR2B_valor < OCR2B_valormin)
	{
		OCR2B_valor = OCR2B_valormin;
	}
	
	if(OCR2B_valor > OCR2B_valormax)
	{
		OCR2B_valor = OCR2B_valormax;
	}
	
	OCR2B = OCR2B_valor;
}