/*
 * NombreProgra.c
 *
 * Created: 30/03/2026
 * Author: Raúl Castañéda	
 * Description: Contador de 8 bits con implementación de botones de incremento y decremento 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>

#define BOTON_INC (1 << PC3)  // Botón de incremento en PC3
#define BOTON_DEC (1<< PC2) // Botón de decremento en PC2
#define LOW_NIBBLE 0x3F // LOW_NIBBLE del contador y parte del HIGH en PB0->PB5 
#define HIGH_NIBBLE 0x03 // Parte del HIGH_NIBBLE del contador en PC0->PC1

volatile uint8_t contador = 0; 
/****************************************/
// Function prototypes
void setup(void); 
void mostrar_conteo(uint8_t valor); 

/****************************************/
// Main Function

void setup()
{
	// Configuración del clock a 1MHz
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	// Configuración de puertos
	DDRB |= LOW_NIBBLE; // PBO->PB5 como salidas
	DDRC |= HIGH_NIBBLE; // PC0->PC1 como salidas
	DDRC &= ~(BOTON_INC | BOTON_DEC); // PC2->PC3 como entradas
	PORTC |= (BOTON_INC | BOTON_DEC);
	
	
	// Configuración de ISR pinchange en el PORTC
	PCICR |= (1<<PCIE1);
	// Habiltamos la interrupción solo en PC2 y PC3
	PCMSK1 |= (BOTON_INC | BOTON_DEC);
	
	sei(); // Activación de interrupciones globales
}

void mostrar_conteo(uint8_t valor)
{
	PORTB = (PORTB & ~LOW_NIBBLE) | (valor & LOW_NIBBLE);
	PORTC = (PORTC & ~HIGH_NIBBLE) | ((valor >> 6) & HIGH_NIBBLE);
}
//**********************//

int main(void)
{
	setup();
	mostrar_conteo(contador);
	
	while (1)
	{
	}
	
	return 0;
}

/****************************************/
// NON-Interrupt subroutines

/****************************************/
// Interrupt routines

ISR (PCINT1_vect)
{
	if (!(PINC & BOTON_INC)) // Si PC3 = 0 (presionado) el contador incrementa  
	{
		contador ++; 
		mostrar_conteo(contador); 
	}
	else if (!(PINC & BOTON_DEC)) // Si PC2 = 0 (presionado) el contador decrementa 
	{
		contador --; 
		mostrar_conteo(contador);
	}
}