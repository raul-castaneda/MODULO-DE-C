/*
 * PWM_2.h
 *
 * Created: 18/04/2026 15:53:19
 *  Author: raulc
 */ 


#ifndef PWM_2_H_
#define PWM_2_H_

#include <avr/io.h>
/****************************************/
// Definiciones de macros // 
#define servo_3 (1<<PB3) // Servo 3 usando OCR2A ubicado físicamente en PB3
#define servo_4 (1<<PD3) // Servo 4 usando OCR2B ubicado físicamente en PD3 
#define	OCR2A_centro 22 // 90 grados
#define	OCR2A_valormin 6 // 0 grados
#define	OCR2A_valormax 38 // 180 grados
/****************************************/
// Definición de valores del OCR2B para pinza de la garra // 
#define OCR2B_centro 22 // 90 grados
#define OCR2B_valormin 32 // 0 grados
#define OCR2B_valormax 36 // 180 grados
/****************************************/
// Función de prototipos 
void setup_2(void);
void init_TIMER2(void);
void init_ADC2(void);
void PWM_set2(uint8_t OCR2A_valor);
void PWM_set2_B(uint8_t OCR2B_valor);

#endif /* PWM_2_H_ */