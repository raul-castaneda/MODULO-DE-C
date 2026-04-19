/*
 * PWM_0.h
 *
 * Created: 19/04/2026 12:58:01
 *  Author: raulc
 */ 


#ifndef PWM_0_H_
#define PWM_0_H_

// Encabezado (Libraries)
#include <avr/io.h>
extern volatile uint8_t pulso_ms; 
#define OCR0A_valor 200 // Valor del OCR0A
#define periodo_ms 200 // Periodo total de la señal PWM = 0,02s
#define led_timer0 (1 << PD7) // Ubicación del led en PD7
// Function prototypes
void init_timer0(void);
void init_ADC0(void);
void setup_timer0(void);
void PWM_set0(uint8_t pulso_nuevo);
#endif /* PWM_0_H_ */