/*
 * PWM1.h
 *
 * Created: 13/04/2026 23:20:00
 *  Author: raulc
 */ 


#ifndef PWM1_H_
#define PWM1_H_

// Encabezado (Libraries)
#include <avr/io.h>

//#define SERVO_1 (1 << PB1) // PIN del servo donde recibiria la señal PWM
#define OCR1A_valor 187
#define ICR1_valor 2499
#define ticks 8U

/****************************************/
// Function prototypes
void setup(void);
void init_timer1();
void PWM_set(uint16_t tiempo_us);

#endif /* PWM1_H_ */