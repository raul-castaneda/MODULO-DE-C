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
/****************************************/
// Macros pertenecientes a la libreria 
#define servo_1 (1<<PB1)	// Servo 1 usando OCR1A ubicado físicamente en PB1
#define servo_2 (1<<PB2)	// Servo 1 usando OCR1B ubicado físicamente en PB2
#define OCR1A_valor 39999
#define OCR1B_valor 39999
#define ICR1_valor 40000
#define ticks 5U

/****************************************/
// Function prototypes
void setup(void);
void init_timer1();
void PWM_set(uint16_t tiempo_us);
void PWM_set_B(uint16_t tiempo_us_B);

#endif /* PWM1_H_ */