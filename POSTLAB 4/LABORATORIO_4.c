/*
 * NombreProgra.c
 *
 * Created: 30/03/2026
 * Author: Raúl Castañéda	
 * Description: contador de 8 bits y lectura de valor de potenciómetro por medio de lectura ADC 
 */
/****************************************/
// Encabezado (Libraries)
#include <avr/io.h>
#include <avr/interrupt.h>
// Declaramos macros con los nombres de los componentes que conforman el circuito
#define BOTON_INC (1 << PC3)  // Botón de incremento en PC3
#define BOTON_DEC (1<< PC2) // Botón de decremento en PC2
#define LOW_NIBBLE 0x3F // LOW_NIBBLE del contador y parte del HIGH en PB0->PB5 
#define HIGH_NIBBLE 0x03 // Parte del HIGH_NIBBLE del contador en PC0->PC1
#define TCNT0_valor 100; // Valor del TCNT0 donde iniciara el conteo 
#define PUERTOS_SEGMENTOS PORTD 
#define display_decenas (1<<PC4) // display de decenas ubicado en PC4
#define display_unidades (1<<PC5) // Display  de unidades ubicado en PC5
#define LED_alarma (1 << PD7) // led ubicada en PD7
volatile uint8_t contador = 0; 
volatile uint8_t value_ADC = 0; 
volatile uint8_t nibble_alto = 0; 
volatile uint8_t nibble_bajo = 0; 
volatile uint8_t display_actual = 0; 
uint8_t display[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71}; // Declaramos el arreglo para la combinación de segmentos del display
/****************************************/
// Function prototypes
void setup(void); 
void mostrar_conteo(uint8_t valor); // Se encarga de mostrar de la configuración de LEDS ya que estan repartidos en diferentes puertos
void init_ADC(); // Configuración del ISR de ADC
void init_TIMER0(); // Configuración de ISR de timer0 para mux de los displays
void mostrar_unidades(uint8_t nibble_bajo); // Mandamos el valor del ADC a la tabla del display y la desplegamos en todo el PORTD
void mostrar_decenas(uint8_t nibble_alto); // Mandamos el valor del ADC a la tabla del display y la desplegamos en todo el PORTD

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
	DDRD = 0xFF; // PD0->PD7 como salidas (ubciación de los displays) y led de la alarma
	DDRC |= ((display_decenas)| display_unidades); // PC4 Y PC5 como salidas para los displays a usar
		
// Configuración de interrupciones //	
	// Configuración de ISR pinchange en el PORTC
	PCICR |= (1<<PCIE1);
	// Habiltamos la interrupción solo en PC2 y PC3
	PCMSK1 |= (BOTON_INC | BOTON_DEC);
	// Habilitamos las interrupciones/timer0overflow
	TIMSK0 |= (1<<TOIE0);
	sei(); // Activación de interrupciones globales
}

void mostrar_conteo(uint8_t valor)
{
	PORTB = (PORTB & ~LOW_NIBBLE) | (valor & LOW_NIBBLE);
	PORTC = (PORTC & ~HIGH_NIBBLE) | ((valor >> 6) & HIGH_NIBBLE);
}

void mostrar_unidades(uint8_t nibble_bajo) // Por medio de esta función mandamos el valor del nibble_bajo a la tabla de displays y luego lo cargamos a todo el PORTD
{
	PUERTOS_SEGMENTOS = display[nibble_bajo]; 
	
}

void mostrar_decenas(uint8_t nibble_alto) // De igual manera como en void mostrar_unidades(uint8_t nibble_bajo) mandamos el valor de nibble_alto a la tabla de display y luego lo cargamos a todo el PORTD
{
	PUERTOS_SEGMENTOS = display[nibble_alto]; 

		
}
//**********************//
// Iniciamos toda las funciones de configuración para que el programa corra correctamente
int main(void)
{
	// Iniciamos todas las configuraciones del ADC, timer0 y muestreo del contador de 8 bits. 
	setup();
	mostrar_conteo(contador);
	init_ADC();
	init_TIMER0();
	
	while (1)
	{
	}
	
	return 0;
}

void init_ADC()
{
	// Configuración de ISR por ADC en pin A6
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << MUX1)|(1 << MUX2); // Seleccionamos canal donde estara el POT en este caso es en el ADC6
	ADCSRA |= (1 << ADEN); // Habilitamos el ADC
	ADCSRA |= (1 << ADIE); // Habilitamos interrupciones por ADC
	// Configuramos la justificación a la izquierda 
	ADMUX |= (1 << ADLAR); 
	ADCSRA |= (1 << ADSC); // Activamos la conversión inicial, luego la que esta en la ISR se encarga de las 
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

void init_TIMER0()
{
	// Habilitamos el modo normal para el TIMER0
	TCCR0A &= ~((1<<WGM01) | (1<< WGM00));
	TCCR0B &= ~((1<<WGM02));
	// Configuración del prescaler de 64 
	TCCR0B &= ~(1<<CS02);
	TCCR0B |= ((1<<CS01) | (1<<CS00));
	// Carga de valor inicial al TCNT0
	TCNT0 = TCNT0_valor;
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

ISR (ADC_vect)
{
	value_ADC = ADCH ; // Asignamos el ADCH a una variable global 
	nibble_alto = value_ADC >> 4 ; //  "Dividimos el nibble alto en el display de decenas"
	nibble_bajo = value_ADC & 0x0F ; // "Dividimos el nibble bajo en el display de unidades"
	ADCSRA |= (1 << ADSC) ; // Volvemos a hacer la lectura 
}

ISR (TIMER0_OVF_vect)
{
		PORTC &= ~((display_unidades) | (display_decenas)); // De primero apagamos los displays de unidades y decenas
		if (display_actual == 0) // Utilizamos "display_actual" como una bandera para apagar y encender el dislpay que necesitamos
		{
			mostrar_unidades(nibble_bajo); 	// Si "display_actual" == 0, se activa el display de unidades (PC5) mostrando el nibble bajo de la lectura del ADC 
			PORTC |= (display_unidades); // Encendemos PC5 que es donde esta conectado el transistor del display de unidades
		}
		else 
		{
			mostrar_decenas(nibble_alto); // Si "display_actual" == 1, se activa el display de decenas (PC4) mostrnado el nibble alto de la lectura del ADC
			PORTC |= (display_decenas); // Encendemos PC5 que es donde esta conectado el transistor del display de decenas 
		}
		
		TCNT0 = TCNT0_valor; // Volvemos a cargar el valor 100 para que vuelva a iniciar el conteo el timer0
		display_actual = (display_actual + 1) % 2; // Toggle de la bandera para activar display de unidades y display de decenas 
		// Proceso de alarma
		if (value_ADC > contador) // Comparamos si el valor del potenciómetro es mayor si sí, se enciende LED en PD7
		{
			PORTD |= (LED_alarma);
		}
		else // Si no, no se enciende o en el caso de que este siga incrementando de valor, se apaga automaticamente 
		{
			PORTD &= ~(LED_alarma);
		}
 }