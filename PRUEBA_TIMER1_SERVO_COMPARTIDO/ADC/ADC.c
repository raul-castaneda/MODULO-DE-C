/*
 * ADC.c
 *
 * Created: 03/05/2026 20:32:04
 *  Author: raulc
 */ 

#include "ADC.H"
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